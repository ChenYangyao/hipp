Get-started with MPI
=======================

.. include:: /global.rst

Applications
-----------------

.. _tutor-app-distributed-sum:

Distributed Summation
""""""""""""""""""""""""""

To demonstrate the usage of MPI collective calls, we implement the parallel summation of:

.. math::

    S = \sum_{i=0}^{N-1} i.

This is an over-simplied example, but its computation pattern (i.e., task decomposition and result reduction)
is frequently used in parallel computation. 
The code can be downloaded at :download:`app-distributed-sum.cpp </../example/tutorial/mpi/app-distributed-sum.cpp>`.

We start with defining the "tasks" - each process is responsible for the summation in 
a subrange of ``[0, N)``. We compute the subrange boundaries at the root process, 
and ``scatter`` them to all processes::

    typedef std::array<int, 2> range_t;
    range_t range;
    vector<range_t> ranges;
    if( rank == 0 ) {
        int N = 10000, N_each = N / n_procs;
        for(int i=0; i<n_procs; ++i) 
            ranges.push_back({i*N_each, (i+1)*N_each});
    }
    comm.scatter(ranges.data(), range, 0);

Here, each range boundary is represented by a pair of integers. In more general 
applications, task-specification may be described by structural object. The 
communication calls may correspondingly 
involve sequence of ``char`` or user-defined datatypes.

After knowing its task, each process perform the local summation. The local 
results are then sum- ``reduce`` to the root process and get printed::

    auto [b,e] = range;
    int local_sum = 0, sum;
    for(int i=b; i<e; ++i) local_sum += i;
    comm.reduce(local_sum, sum, SUM, 0);

    if( rank == 0 )
        pout << "Sum = ", sum, endl;

The output is:

.. code-block:: text

    Sum = 49995000

.. _tutor-app-pi-computation:

Computing PI by Numerical Integration
""""""""""""""""""""""""""""""""""""""

The mathematical constant :math:`\pi` can be approximately evaluated by a numerical
integration:

.. math::

    \pi = \int_0^1 \frac{4}{1+x^2} \, \mathrm{d} x .

We use trapezoid method to approximate it, i.e., the integration interval :math:`[0,\,1]` is divided
into :math:`N` sub intervals with equal length :math:`h=1/N`. 
The location of :math:`i`-th interval is :math:`x_i = (i+0.5)h` (0-indexed).
The approximated integration in this interval is
:math:`f(x_i)h`, where :math:`f=4/(1+x^2)` is the integrand.

This is an typical embarassing parallel computation task. We implement it in the following.
This example is taken from Ch-3.3 of [GroppW-UMPIv3]_. The code can be downloaded at
:download:`app-pi-computation.cpp </../example/tutorial/mpi/app-pi-computation.cpp>`.

The process ranked ``rank`` is responsible for the intervals indexed ``rank, rank+n_procs, rank+2*n_procs, ...``.
The local integration value is simply evaluated by::

    int N = 10000;
    double h = 1.0 / N, sum = 0.;
    for(int i=rank; i<N; i+=n_procs){
        double x = h * (i+0.5);
        sum += 1. / ( 1. + x*x );
    }
    double my_pi = 4.0 * h * sum;

All local results are reduced by ``SUM`` operator to a single final value, which 
is printed finally::

    double pi;
    comm.reduce(my_pi, pi, SUM, 0);
    if( rank == 0 )
        pout << "Find PI = ", pi, endl;

The output is:

.. code-block:: text

    Find PI = 3.14159

.. _tutor-app-matrix-vec-dot:

A Self-Scheduling Example: Matrix-Vector Multiplication
"""""""""""""""""""""""""""""""""""""""""""""""""""""""""

In the following example, we use multiple processes to compute the multiplication of a matrix :math:`A` and a vector :math:`b`,
whose shapes are :math:`(n_{\rm rows},\, n_{\rm cols})` and :math:`(n_{\rm cols},\,1)`. 
The result is a vector :math:`c` of shape :math:`(n_{\rm rows},\,1)`.
This example can be found in Ch3.6 of [GroppW-UMPIv3]_. 
The codes can be found at :download:`app-matrix-vec-dot.cpp </../example/tutorial/mpi/app-matrix-vec-dot.cpp>`.

The matrix-vector multiplication is defined as, for each row :math:`i`,

.. math::

    c_i = \sum_{j=0}^{n_{\rm cols}} A_{ij} b_j.

The simplest task-decomposition scheme is to distribute rows of :math:`A` 
among processes and let each process compute on a subset of rows.

In real applications, sub-tasks may consum various amount of times. To balance 
the works on processes, self-scheduling algorithm is necessary. In this example, 
we use a single master process (also called root process, ranked ``root=n_procs-1``) 
to control and assign tasks to other worker processes (ranked in ``[0, n_procs-1)``).

The steps for this algorithm are 

- The master broadcasts all shared data to workers.
- The master assigns each worker a initial task.
- The worker waits for a job, finishes it, and replies the answer to the master.
- The master waits for reply. Once he gets a reply, stores the answer and 
  sends another task to the worker.
- If all tasks finished, the master sends a stop signal to each worker.

Here, we use HIPP's :class:`~HIPP::NUMERICAL::DArray` to describe these numerical arrays::

    #include <hippnumerical.h>
    using HIPP::NUMERICAL::DArray;
    typedef DArray<double, 2> arr_t;    // matrix
    typedef DArray<double, 1> vec_t;    // vector

To implement the algorithm, we begin with defining the algorithm of a worker::

    // if rank != root
    vec_t a({n_cols}), b({n_cols});
    comm.bcast(b, root);
    while(1) {
        int tag = comm.recv(root, ANY_TAG, a).tag();
        if( tag == n_rows ) break;
        double res = (a*b).sum();
        comm.send(root, tag, res);
    }

Here, the worker first gets the shared data, i.e., :math:`b`, then waits 
the assignment repeatedly. Each assignment is described by a row :math:`a` of 
:math:`A`. The row number is described by the communication tag for clarity.
The stop signal comes simply when the row number approaches the actual number 
of rows of :math:`A`. Once get the tasks specification, the worker finishes
the job (i.e., by a vector-vector inner product ``(a*b).sum()``) and sends 
back the result.

The master process has to first initialize the shared data, :math:`b`, broadcasts
them to workers. The matrix :math:`A` is also initialized, but gets sent later.
Here we simply set them to constant ``1.0``::

    // if rank == root
    arr_t A({n_rows, n_cols}, 1.0);
    vec_t b({n_cols}, 1.0), c({n_rows});
    comm.bcast(b, root);

The master assigns initial tasks to workers by sending the first ``n_workers``
rows to them, respectively::

    int row = 0;
    while(row < n_workers && row < n_rows) {
        comm.send(row, row, &A(row, 0), n_cols);    // send initial rows to workers
        ++ row;
    }

Then, the master waits for reply, stores the replied answer, and sends the 
next available row to worker. Finally, empty massages with ``tag = n_rows``
are sent as stop signal::

    double res;
    for(int i=0; i<n_rows; ++i){
        Status st = comm.recv(ANY_SOURCE, ANY_TAG, res);
        int src = st.source(), tag = st.tag();
        c[tag] = res;
        if( row != n_rows ){                        // send another row of A
            comm.send(src, row, &A(row,0), n_cols); 
            ++row;
        }else {
            comm.send(src, row);                    // an empty message
        }
    }
    pout << "c = A * b = ", c, endl;

Taking a small :math:`8 \times 16` matrix :math:`A` as example, 
the printed answer is:

.. code-block:: text

    c = A * b = DArray{16,16,16,16,16,16,16,16}

