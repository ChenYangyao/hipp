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

HIPP supports **four** types of arguments passed to send/recv calls. They are listed below:

.. _tutor-mpi-buff-spec:

========================================================== ===================================================================================
Arguments                                                   Semantics
========================================================== ===================================================================================
``const vector<T, A> &buff``                                a vector of any numeric type T and allocator A.
``const string &buff``                                      a ``std::string`` (only valid for send call).
``const void *buff, int n, Datatype dtype``                 raw buffer, starting at ``buff``, |br| consisting of ``n`` elements of type ``dtype``. 
``const void *buff, int n, const string &dtype``            describe the element type by a string.
========================================================== ===================================================================================

Therefore, the following calls are valid::

    vector<int> send_buff(10), recv_buff(10);
    string send_str = "content to send";
    vector<char> recv_str(128);

    comm.send(dest, tag, send_buff);
    comm.send(dest, tag, send_str);
    comm.send(dest, tag, &send_buff[0], 10, HIPP::MPI::INT);
    comm.send(dest, tag, &send_buff[0], 10, "int");

    comm.recv(src, tag, recv_buff);
    comm.recv(src, tag, recv_str);
    comm.recv(src, tag, &recv_buff[0], 10, HIPP::MPI::INT);
    comm.recv(src, tag, &recv_buff[0], 10, "int");


Collective Communication
--------------------------------------------------------------

:download:`mpi/basic-collective.cpp </../example/tutorial/mpi/basic-collective.cpp>`

Collective communication is useful when all of the processes in a communicator 
are involved in the communication. In the below, we show how to use HIPP MPI 
collective calls to calculate the summation 

.. math::

    S = \sum_{i=0}^{N-1} i.

We will use :func:`scatter() <HIPP::MPI::Comm::scatter>` method to spread task to 
each process, let each process finish its own part of task, and then use :func:`reduce() <HIPP::MPI::Comm::reduce>`
method to collectively sum the results from all parts.

Let's first initialize the MPI environment and define some helpful variables::

    HIPP::MPI::Env env;                                 
    auto comm = env.world();                            
    int rank = comm.rank(), n_procs = comm.size();

Each process gets its rank in the communicator and the number of processes.

Now, let process 0 prepares the tasks for all processes and spreads it into other process by 
:func:`scatter() <HIPP::MPI::Comm::scatter>`::

    vector<int> local_edges(2);                 // Each process will get its task edges.
    if( rank == 0 ){
        int total_num = 10000, num_per_proc = total_num/n_procs;
        vector<pair<int, int>> edges; 
        for (int i = 0; i < n_procs; i++) 
            edges.emplace_back(i*num_per_proc, (i+1)*num_per_proc);
        edges.back().second = total_num;
        comm.scatter(&edges[0], local_edges, 0); // [1] Scatter "edges" into "local_edges"
    }else{
        comm.scatter(NULL, local_edges, 0);                   
    }

We distribute the tasks in such a way: each process gets a contiguous chunk to sum. The edges
for each chunk can be represented by two integers, so we use a ``std::pair`` to describe 
the task for each processes. Process 0 then uses a ``std::vector`` to hold all tasks. 
After the call of :func:`scatter() <HIPP::MPI::Comm::scatter>` (``[1]`` in the code), each process gets its local 
task. 

Again, :func:`scatter() <HIPP::MPI::Comm::scatter>` has sevaral overloads. The simplest 
arguments are: ``(const void *sendbuf, const Datapacket &recv_dpacket, int root)``. The ``sendbuf``
is the starting address of the sending buffer, the ``recv_dpacket`` can be constructed by 
ways like in the :ref:`Point-to-point Communication <tutor-mpi-buff-spec>`. We use a single 
vector in the ``[1]`` of the code. Equivalently, we may use ``comm.scatter(&edges[0], {&local_edges[0], 2, "int"}, 0)``.
The ``root`` argument specifies the source of the data 
(not significant at destination processes, therefore set as ``NULL``).

Then, each process finishes its task by simply adding all numbers between the edges::

    int local_sum = 0;
    for(int i=local_edges[0]; i<local_edges[1]; ++i)
        local_sum += i;

Then a :func:`reduce() <HIPP::MPI::Comm::reduce>` call (``[2]`` in the code) gathers the results in all processes, 
makes summation, and put it in the ``sum`` of the root process (process 0)::

    if( rank == 0 ){
        int sum = 0;
        comm.reduce({&local_sum, 1, "int"}, 
            &sum, "+", 0);                      // [2] Reduce from "local_sum" into "sum" 

        HIPP::pout << "Result of sum is ", sum, 
            " (found by ", n_procs, " processes)", endl; 
    }else{
        comm.reduce({&local_sum, 1, "int"}, NULL, "+", 0);
    }

The simplest arguments of :func:`reduce() <HIPP::MPI::Comm::reduce>` is ``(const Datapacket &send_dpacket, void *recvbuf,
const Oppacket &op, int root)``. Where we use the ``op``-code ``"+"`` to make summation. ``root`` specifies 
the target process in which the result is put (process 0 here).

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
           spread tasks to workder processes. */
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

The result using 4 processes with :math:`N=100000` is 

.. code-block:: text

    Enter the no. of intervals: 100000
    Find pi=3.141592653598117, error=8.323564060219724e-12