Basic Point-to-point and Collective Communication
===============================================================

Point-to-point communication
--------------------------------------------------------------

We demonstrate how to use HIPP's MPI module to perform basic point-to-point communication
among processes in the below. Here we use the :func:`send <HIPP::MPI::Comm::send>` and 
:func:`recv <HIPP::MPI::Comm::recv>` methods of the :class:`Comm <HIPP::MPI::Comm>` class,
which are counterparts of the **Standard** MPI calls ``MPI_Send`` and ``MPI_Recv``.

:download:`mpi/basic-p2p.cpp </../example/tutorial/mpi/basic-p2p.cpp>`

.. include:: /../example/tutorial/mpi/basic-p2p.cpp 
    :code: cpp

To use any definition in HIPP MPI, include the header ``<hippmpi.h>`` and initialize the MPI environment 
by defining a :class:`Env <HIPP::MPI::Env>` instance. The detailed conventions and compiling options can be
found in the `API Reference <api-mpi-usage>`_.

The :func:`rank <HIPP::MPI::Comm::rank>` method of the communicator tells the rank of the current process 
in the groups of processes in the communicator (``[2]`` in the example). We use this rank determine the identity
of each process (here, we will send a message from process 0 to process 1).
Similarily, the :func:`size <HIPP::MPI::Comm::size>` method tells the total number of processes.

To send a message (a vector containing 10 double-precision values in the example), we need to specify two things:

(1) The message envelop (target process ``rank``, and the ``tag`` to match).
(2) The message buffer. In HIPP, you can directly use a single vector of numeric elements as the buffer. We will discuss
    other ways of specifying buffer in the below.

Therefore, a call of ``comm.send(dest, tag, send_buff)`` (``[3]`` in the example) sends the buffer ``send_buff`` 
to the process ranked ``dest`` with a ``tag``, in the context of the communicator.

The message-receiving call ``comm.recv(src, tag, recv_buff)`` (``[4]`` in the example) does things inversely, 
i.e., receive a message 
from process ranked ``src`` with a matched ``tag``, and put the message into the buffer ``recv_buff``. It is 
required that the size of ``recv_buff`` is larger than the message. To tell the actual number of elements 
received, you use the :func:`count <HIPP::MPI::Status::count>` method of the return status of the receiving call.
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

Collective communication
--------------------------------------------------------------

The collective communication is also easy to implement. For example, if we want
to calculate 1 + 2 + ... + 10000 using 10 processes.::

    using HIPP::MPI::INT;
    HIPP::MPI::Env env;
    auto world = env.world();
    // distribute the task
    int local_edges[2], sum = 0;
    if (world.rank() == 0) {
        int tot_num = 10;
        vector<int> edges{0};
        for (int i_proc = 0; i_proc < world.size(); ++i_proc)
            edges.push_back((i_proc + 1) * ceil(tot_num / world.size()));
        edges.back() = tot_num + 1; // modify the last element for un-uniform distribution
        for (int i_proc = 1; i_proc < world.size(); ++i_proc)
            world.send(i_proc, 0, &edges[i_proc], 2, INT);
        local_edges[0] = edges[0];
        local_edges[1] = edges[1];
    } else {
        world.recv(0, 0, &local_edges[0], 2, INT);
    }
    // do computation on each process
    for (int i = local_edges[0]; i < local_edges[1]; ++i)
        sum += i;
    // collect the result
    vector<int> res(world.size());
    world.gather(&sum, 1, INT, res.data(), 1, INT, 0);
    // print the summation
    if (world.rank() == 0) {
        int tot_sum = 0;
        for (auto v: res)
            tot_sum += v;
        cout << "The summation is " << tot_sum << ", calculated with " << world.size() << " processes.";
    }

Here we employ the rank-0 process to distribute the task and collect the
results after all the processses finished the tasks. For rank-0 process, we
should not distribute the task though MPI. Otherwise, the send will block the
program and rank-0 process cannot receive.

Although we did a extremely simple task using MPI, the pattern can be applied to
any other kinds of tasks.::

    struct TaskInfo{
        // information about a task
    }

    HIPP::MPI::Env env;
    auto world = env.world();
    TaskInfo local_task;
    if (world.rank() == 0){
        vector<TaskInfo> tasks;
        // init all the tasks
        for (int i_proc = 1; i_proc < world.size(); ++i_proc)
            world.send(i_proc, 0, &takss[i_proc], sizeof(TaskInfo) / sizeof(char), HIPP::MPI::CHAR);
        local_task = tasks[0];
    } else {
        world.recv(0, 0, &local_task, sizeof(TaskInfo) / sizeof(char), HIPP::MPI::CHAR);
    }

    // do tasks

    // collect the result

Here we use `char` datatype to send and recv the data, which can only be
applied to homegeneous computer systems. Otherwise, you should mannually create
a mpi datatype to send these data or send the struct one-by-one.
