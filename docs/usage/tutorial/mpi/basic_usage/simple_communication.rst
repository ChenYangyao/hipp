Get-started with MPI
=======================

.. include:: /global.rst

.. _tutor-mpi-env:

The MPI Environment
---------------------

HIPP provides the set of MPI interface which corresponds to the MPI standard. User familiar 
with the Standard MPI interface would find easy to get started with HIPP's OOP interface.

In this tutorial, we assume a inclusion of the header file and declarations of 
namespaces as following::

    #include <hippmpi.h>

    using namespace HIPP;
    using namespace HIPP::MPI;
    using namespace std;

which import all the HIPP MPI definitions and expose them to the current namespace.

To start the MPI environment, define a named :class:`~HIPP::MPI::Env` object at the 
beginning of the ``main()`` function. The global "world" communicator typed :class:`~HIPP::MPI::Comm` 
can be access from :func:`~HIPP::MPI::Env::world` method of the environment object::

    int main(int argc, char const *argv[]) {
        Env env;
        Comm comm = env.world();

        // The remaining codes ....

        return 0;
    }

The host name can be obtained by :func:`~HIPP::MPI::Env::processor_name` method 
of the environment object. The number of processes and the rank of the current 
process can be obtained from the communicator::

    string name = env.processor_name();
    int rank = comm.rank(), n_procs = comm.size();

    pout << "Host name ", name, '\n',
        "Rank ", rank, ", no. of processes ", n_procs, endl;

To compile the program, link with the HIPP MPI library and CNTL library. The MPI 
program can be run by ``mpirun`` or ``mpiexec``, depending on the implementation you used:

.. code-block:: bash

    mpicxx  -std=c++17 -O3 -Wall -o get-started.out get-started.cpp -lhippmpi -lhippcntl
    mpirun -n 4 ./get-started.out

Output:

.. code-block:: text

    Host name local-HP
    Rank 2, no. of processes 4
    Host name local-HP
    Rank 0, no. of processes 4
    Host name local-HP
    Rank 3, no. of processes 4
    Host name local-HP
    Rank 1, no. of processes 4

More details can be found in the :ref:`API-Ref <api-mpi-usage>`.

HIPP's objects are printable so that we can quickly get some knownledge 
about them, particularly useful for loggin and debug. For example::

    if(rank == 0) pout << comm;

Output:

.. code-block:: text

  HIPP::MPI::Comm instance [loc=0x7ffce560d790, size=16, align=8]
  ----------
    Process group
      rank/size:        0/4
    Topology: undefined

.. _tutor-mpi-p2p:

Point-to-point Communication
--------------------------------------------------------------

To send a message from one process to another, call :func:`~HIPP::MPI::Comm::send` and 
:func:`~HIPP::MPI::Comm::recv` on the communicator in the sender and receiver, respectively.

For example, int the following we send 5 double-precision floating-point values from 
process 0 to 1::

    double outbuf[5] = {}, inbuf[5];
    int tag = 0;
    if( rank == 0 ) {
        comm.send(1, tag, outbuf);
    }else if( rank == 1 ) {
        comm.recv(0, tag, inbuf);
        pout << "Got {", pout(inbuf, inbuf+5), "}", endl;
    }

Here the ``tag`` is for message matching if there are multiple communications between 
a pair of processes. The output from process 1 is 

.. code-block:: text

    Got {0,0,0,0,0}

Note that the receiver's buffer must be sufficiently large to hold the message.
To find the actual number of items received, use :func:`HIPP::MPI::Status::count`
on the returned status object from ``recv``::

    auto status = comm.recv(0, tag, inbuf);
    pout << "Got ", status.count(DOUBLE), " items", endl;

.. _tutor-mpi-buff-spec:

The Communication Buffer
""""""""""""""""""""""""""

The design strategy of HIPP is to provide both the most general interface 
and the interfaces specific to special tasks. 
In terms of the communication calls,
you may use the Standard MPI style to specify the buffer, i.e., use the 
``{address, count, datatype}`` triplet::

    double outbuf[5] = {};
    comm.send(1, tag, outbuf, 5, DOUBLE);

Or, equivalently, use the name string for any numerical type::

    comm.send(1, tag, outbuf, 5, "double");

Or, omit the datatype and let the library infer it from the pointer type passed::

    comm.send(1, tag, outbuf, 5);

Note that the last one is preferred since you will have no chance of mistake.

For a single numerical variable, raw-array, ``std::string`` and ``std::vector``, 
you may even drop the count since they are self-contained::
    
    double x; 
    short a[5];
    string s = "hello"; 
    vector<int> v(5);

    comm.send(1, tag, x);
    comm.send(1, tag, a);
    comm.send(1, tag, s);
    comm.send(1, tag, v);

Note that HIPP does not resize the containers, so the count is inferred from 
their ``size()``.

The full list of supported buffer specification is documented in the API-Ref 
of :class:`~HIPP::MPI::Datapacket`. The actual implementation is to first 
construct a unified Datapacket object from any of these calls, and then pass it
to the underlying MPI calls. For ``recv()``, the only limitation is that 
the object or buffer passed must be writable (hence, ``std::string`` 
cannot be used) and sufficiently large.


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
