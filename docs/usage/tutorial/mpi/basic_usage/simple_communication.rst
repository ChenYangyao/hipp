Basic Point-to-point and Collective Communication
===============================================================

.. include:: /global.rst

Point-to-point Communication
--------------------------------------------------------------

We demonstrate how to use HIPP's MPI module to perform basic point-to-point communication
among processes in the below. Here we use the :func:`send() <HIPP::MPI::Comm::send>` and 
:func:`recv() <HIPP::MPI::Comm::recv>` methods of the :class:`Comm <HIPP::MPI::Comm>` class,
which are counterparts of the **Standard** MPI calls ``MPI_Send()`` and ``MPI_Recv()``.

:download:`mpi/basic-p2p.cpp </../example/tutorial/mpi/basic-p2p.cpp>`

.. include:: /../example/tutorial/mpi/basic-p2p.cpp 
    :code: cpp

To use any definition in HIPP MPI, include the header ``<hippmpi.h>`` and initialize the MPI environment 
by defining a :class:`Env <HIPP::MPI::Env>` instance. The detailed conventions and compiling options can be
found in the :ref:`API Reference <api-mpi-usage>`.

The :func:`rank() <HIPP::MPI::Comm::rank>` method of the communicator tells the rank of the current process 
in the groups of processes in the communicator (``[2]`` in the example). We use this rank determine the identity
of each process (here, we will send a message from process 0 to process 1).
Similarily, the :func:`size() <HIPP::MPI::Comm::size>` method tells the total number of processes.

To send a message (a vector containing 10 double-precision values in the example), we need to specify two things:

(1) The message envelop (target process ``rank``, and the ``tag`` to match).
(2) The message buffer. In HIPP, you can directly use a single ``std::vector`` of numeric elements as the buffer. We will discuss
    other ways of specifying buffer in the below.

Therefore, a call of ``comm.send(dest, tag, sendbuf)`` (``[3]`` in the example) sends the buffer ``sendbuf`` 
to the process ranked ``dest`` with a ``tag``, in the context of the communicator.

The message-receiving call ``comm.recv(src, tag, recv_buff)`` (``[4]`` in the example) does things inversely, 
i.e., receive a message 
from process ranked ``src`` with a matched ``tag``, and put the message into the buffer ``recv_buff``. It is 
required that the size of ``recv_buff`` is larger than the message. To tell the actual number of elements 
received, you use the :func:`count() <HIPP::MPI::Status::count>` method of the return status of the receiving call.
In the example, although the receiving buffer size is 20, only 10 elements are put into the beginning of it.

The output is 

.. code-block:: text 

    Received 10 values: 0,0,0,0,0,0,0,0,0,0

Note that HIPP's objects are printable. For example, a call of ``cout << comm`` prints out the basic 
information of the communicator, which might be useful for debugging.

.. code-block:: text 

    HIPP::MPI::Comm instance [loc=0x7ffed841c690, size=16, align=8]
    ----------
    Process group
        rank/size:        1/4
    Topology: undefined


The Communication Buffer
""""""""""""""""""""""""""

In the above example of point-to-pointcommunication, 
we use a ``std::vector`` of numeric type as the communication buffer. 
Thanks to the meta-programming support of C++, we can pass different combinations of arguments
to the same send/recv function call. 
Depending on the types of the arguments, the function call gives different semantics.

HIPP supports many forms of arguments passed to send/recv calls. See API reference for 
:class:`HIPP::MPI::Datapacket` for the details.

.. _tutor-mpi-buff-spec:

Therefore, the following calls for send (or recv) are valid and equivalent when using a ``std::vector`` as the buffer::

    vector<int> send_buff(10), recv_buff(10);

    comm.send(dest, tag, send_buff);
    comm.send(dest, tag, &send_buff[0], 10, HIPP::MPI::INT);
    comm.send(dest, tag, &send_buff[0], 10, "int");

    comm.recv(src, tag, recv_buff);
    comm.recv(src, tag, &recv_buff[0], 10, HIPP::MPI::INT);
    comm.recv(src, tag, &recv_buff[0], 10, "int");

Note that using the triple ``(buffer, size, datatype)`` is the most universal way in MPI to specify a data buffer.
But in HIPP, you have different ways to implement the same functional, much more convenient in some cases.
Internally, HIPP converts all these forms into an unifield :class:`HIPP::MPI::Datapacket` type which hosts the standard triplet
Then, data described by the data packet are used in send/recv.

Strings, arithmetic scalars and arrays of arithmetic scalars can be directly used as arguments::

    string send_str = "content to send";
    vector<char> recv_str(128);

    comm.send(dest, tag, send_str);
    comm.recv(src, tag, recv_str);

    int x,
        arr[3],
        *buff = new int [3];

    comm.send(dest, tag, x);
    comm.send(dest, tag, arr);
    comm.send(dest, tag, buff, 3);

Collective Communication
--------------------------

Collective communication is useful when all of the processes in a communicator 
are involved in the communication. In the below, we show how to use HIPP MPI 
collective calls to calculate the summation 

.. math::

    S = \sum_{i=0}^{N-1} i.

The example code can be found at :download:`mpi/basic-collective.cpp </../example/tutorial/mpi/basic-collective.cpp>`.
In this example, we 

- Use :func:`scatter() <HIPP::MPI::Comm::scatter>` method to spread task to 
  each process
- Let each process finish its own part of task. 
- Then use :func:`reduce() <HIPP::MPI::Comm::reduce>`
  method to collectively sum the results from all parts.

Let's first initialize the MPI environment and define some helpful variables::

    HIPP::MPI::Env env;                                 
    auto comm = env.world();                            
    int rank = comm.rank(), n_procs = comm.size();

Each process gets its rank in the communicator and the number of processes. Now, 
let process 0 prepares the tasks for all processes and spreads it into other process by 
:func:`scatter() <HIPP::MPI::Comm::scatter>`::

    int local_edges[2];
    if( rank == 0 ){
        int total_num = 10000, num_per_proc = total_num/n_procs;
        vector<int> edges;
        for (int i = 0; i < n_procs; i++){
            edges.push_back(i*num_per_proc);
            edges.push_back((i+1)*num_per_proc);
        } 
        edges.back() = total_num;
        comm.scatter(edges.data(), local_edges, 0);
    }else{
        comm.scatter(NULL, local_edges, 0);                   
    }

We distribute the tasks in such a way: each process gets a contiguous chunk to sum. 
The edges for each chunk can be represented by two integers. 
Process 0 first uses a ``std::vector`` to hold all task edges and then 
:func:`scatter() <HIPP::MPI::Comm::scatter>` each pair of edges to other 
processes.

Note that :func:`scatter() <HIPP::MPI::Comm::scatter>` has three arguments: the 
starting address of the sending buffer, the receiving buffer (two integers here),
and the rank of root process which provides the data to spread.
The receiving buffer is a data packet instance, which can be constructed like 
in :ref:`Point-to-point Communication <tutor-mpi-buff-spec>`.

Each process then finishes its task by simply adding all numbers between the edges::

    int local_sum = 0;
    for(int i=local_edges[0]; i<local_edges[1]; ++i)
        local_sum += i;

Finally, a :func:`reduce() <HIPP::MPI::Comm::reduce>` call gathers the results in all processes, 
makes summation, and put it in the ``sum`` of the root process::

    if( rank == 0 ){
        int sum = 0;
        comm.reduce(local_sum, &sum, "+", 0);
        HIPP::pout << "Result of sum is ", sum, 
            " (found by ", n_procs, " processes)", endl; 
    }else{
        comm.reduce(local_sum, NULL, "+", 0);
    }

The :func:`reduce() <HIPP::MPI::Comm::reduce>` call has four arguments:

The simplest form of :func:`reduce() <HIPP::MPI::Comm::reduce>` has four arguments: 
the sending buffer, the receiving buffer, the operation ("+" here), and the root process.


The output of the program (run with 4 processes) is 

.. code-block:: text 

    Result of sum is 49995000 (found by 4 processes)

Although we did a extremely simple task using MPI, the pattern can be applied to
any other kinds of tasks::

    struct TaskSpecification{
        /* Specifies the task, e.g., 
           source of data and computation method. */
    };

    if( rank == master ){
        vector<TaskSpecification> tasks;

        /* Use collective call or simply send call to 
           spread tasks to worker processes. */
        comm.scatter(...); 
        for(...) comm.send(...); 
    }else {
        /* The worker receives its task by collective 
           call or recv call. */
    }

    /* Do tasks. */

    /* Collect results by collective call or send/recv call. */

Depending on the detail type of ``TaskSpecification``, you may use different 
buffer specifications. If ``TaskSpecification`` has different types of 
members, you may simply sends/receives it as a sequence of ``char`` 
(valid in homegeneous system), or you construct new MPI datatypes using 
the methods that we will introduce in later chapters.


More on Collective Data-moving
"""""""""""""""""""""""""""""""
We give more examples of data-moving collective calls in this section. Those source codes
can be found at :download:`mpi/more-collective-data-moving.cpp </../example/tutorial/mpi/more-collective-data-moving.cpp>`.

In the above example of calculating :math:`\pi`, 
"scatter" operation moves data from one "root" process to all other procecess
(includes the root itself). "Gather" operation, on the other hand, collects data 
from all procecess and concatenates them into a buffer in the root process::

    double x = rank;
    if( rank == 0 ){
        vector<double> all_x(n_procs);
        comm.gather(x, all_x.data(), 0);
        HIPP::pout << "Process 0 gathered (", all_x, ")", endl;
    }else{
        comm.gather(x, NULL, 0);
    }

Here, ecah process has a single floating-point value "x". 
They are gathered into one buffer "all_x" by calling :func:`gather() <HIPP::MPI::Comm::gather>`.
The output is:

.. code-block:: text

    Process 0 gathered (0,1,2,3)

Applications
-----------------

Computing PI by Numerical Integration
""""""""""""""""""""""""""""""""""""""

In the following example, we use a numerical integration to approximately compute the value of :math:`\pi`.
This example is taken from Ch-3.3 of [GroppW-UMPIv3]_.
From calculus we know

.. math::

    \pi = \int_0^1 \frac{4}{1+x^2} \, \mathrm{d} x .

We use trapezoid method to approximate this integration, i.e., the integration interval :math:`[0,\,1]` is divided
into :math:`N` sub intervals with length :math:`h=1/N`. The integration value in the :math:`i`-th interval (0-indexed) is
approximated by :math:`f( (i+0.5)h )h`, where :math:`f` is the integrand.

This task is an ideal example for parallel computation. The following codes implement the algorithm.

:download:`mpi/app-pi-computation.cpp </../example/tutorial/mpi/app-pi-computation.cpp>`

.. include:: /../example/tutorial/mpi/app-pi-computation.cpp 
    :code: cpp

Note that for the :func:`reduce <HIPP::MPI::Comm::reduce>` call, the first argument is a data packet type which describes 
the sending buffer. You can construct it by many ways like just using a scalar ``my_pi``. The second argument is 
the starting address of the receiving buffer.

The result using 4 processes with :math:`N=100000` is 

.. code-block:: text

    Enter the no. of intervals: 100000
    Find pi=3.141592653598117, error=8.323564060219724e-12


A Self-Scheduling Example: Matrix-Vector Multiplication
"""""""""""""""""""""""""""""""""""""""""""""""""""""""""

In the following example, we use multiple processes to compute the multiplication of a matrix :math:`A` and a vector :math:`b`,
whose shapes are :math:`(n_{\rm row},\, n_{\rm col})` and :math:`(n_{\rm col},\,1)`. The result is a vector :math:`c` of shape :math:`(n_{\rm row},\,1)`.
This example can be found in Ch3.6 of [GroppW-UMPIv3]_. 
The codes can be found at :download:`mpi/app-matrix-vec-dot.cpp </../example/tutorial/mpi/app-matrix-vec-dot.cpp>`.

The matrix-vector multiplication is defined as 

.. math::

    c_i = \sum_{j=0}^{n_{\rm col}} A_{ij} b_j.

The simplest parallel algorithm is to split :math:`A` into rows. Each process is responsible for some rows.
Because the actual computation and communication time may be different for rows, self-scheduling algorithm 
may be necessary for balacing the tasks of procecess.

The following codes show how to use a master-slave algorithm for this purpose. We have one master process,
which splits and sends tasks to other slave procecess. Once all slaves have their tasks, the master waits
the answer from any slave, and assigns a new task to it.
Hence, we write the codes for the master as::

    void master_do(int n_rows, int n_cols, 
        const double *A, const double *b, double *c,
        HIPP::MPI::Comm &comm)
    {
        const int root = 0, n_procs = comm.size(); 

        comm.bcast({b, n_cols, "double"}, root);

        /* Assign task to each slave. */
        int n_sent = 0;
        for(int i=0; i<std::min(n_rows, n_procs-1); ++i){
            comm.send(i+1, n_sent, A+n_sent*n_cols, n_cols, "double");
            ++n_sent;
        }

        /* Wait reply from any slave, and assign a new task to it. */
        for(int i=0; i<n_rows; ++i){
            double ans;
            auto status = comm.recv(HIPP::MPI::ANY_SOURCE, HIPP::MPI::ANY_TAG, 
                &ans, 1, "double");
            c[status.tag()] = ans;

            const double *buff = A + n_sent*n_cols; 
            int count = n_cols;
            if( n_sent == n_rows ){    // All tasks were finished. Send stop signal.
                buff = NULL; count = 0; 
            }
            comm.send(status.source(), n_sent, buff, count, "double");
            if( n_sent < n_rows ) ++n_sent;
        }
    }

First, the master broadcasts :math:`b` to all workers. Then, the master assigns one row of :math:`A` to 
each slave. Finally, the master waits using :func:`recv <HIPP::MPI::Comm::recv>`, and send back 
a new task using :func:`send <HIPP::MPI::Comm::send>`. Note that here we use the communication ``tag``
to represent the row index, with ``tag=n_rows`` indicating a stop signal. 
This may fail for large tasks because the valid tag values have an upper bound.
The MPI standard requires :math:`[0,\, 32767]` to be a valid range. The actual upper bound can be 
found by :func:`tag_ub <HIPP::MPI::Env::tag_ub>` method of :class:`HIPP::MPI::Env`.

The slave just waits for the task, finishes it and sends the answer back to the master::

    void slave_do(int n_rows, int n_cols, HIPP::MPI::Comm &comm){
        const int root = 0;
        vector<double> row(n_cols), b(n_cols);
        comm.bcast(b, root);

        if( comm.rank() > n_rows ) return;

        /* Wait for assigned task, finish it, and send back the answer. */
        while(true){
            int tag = comm.recv(root, HIPP::MPI::ANY_TAG, row).tag();
            if( tag == n_rows ) break;  // Stop working on a stop signal.
            
            double ans = 0.;
            for(int i=0; i<n_cols; ++i) ans += row[i]*b[i];
            comm.send(root, tag, &ans, 1, "double");
        }
    }

For following codes show how to call these two subroutines::

    int main(int argc, char const *argv[]) {    
        HIPP::MPI::Env env;
        auto comm = env.world();
        int rank = comm.rank();

        const int n_rows = 5, n_cols = 8, n_elems = n_rows*n_cols;
        if( rank == 0 ){
            vector<double> A(n_elems), b(n_cols, 1.0), c(n_rows);
            HIPP::ALGORITHM::LinSpaced(0., n_elems) >> A.data();
            master_do(n_rows, n_cols, A.data(), b.data(), c.data(), comm);

            HIPP::pout << "A =[\n", 
                HIPP::PrtArray(A.begin(), A.end()).ncol(n_cols).width(3), "]\n",
                "b = [", b, "]\n",
                "c = [", c, "]\n"; 
        }else{
            slave_do(n_rows, n_cols, comm);
        }

        return 0;
    }

The master (``rank == 0``) creates a :math:`5\times 8` matrix :math:`A` filled with 
linear spaced data by :class:`HIPP::ALGORITHM::LinSpaced`. The whole task :math:`c=A b`
is finished by all procecess in the commnicator ``comm``. Then, the master prints the 
results into the standard output using the pretty stream :var:`HIPP::pout`. The output 
is 

.. code-block:: text 

    A =[
      0,  1,  2,  3,  4,  5,  6,  7,
      8,  9, 10, 11, 12, 13, 14, 15,
     16, 17, 18, 19, 20, 21, 22, 23,
     24, 25, 26, 27, 28, 29, 30, 31,
     32, 33, 34, 35, 36, 37, 38, 39]
    b = [1,1,1,1,1,1,1,1]
    c = [28,92,156,220,284]
