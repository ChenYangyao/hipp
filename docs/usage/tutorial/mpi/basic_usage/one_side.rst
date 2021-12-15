One-Side Communications 
==========================

MPI-2 introduced and MPI-3 enhanced the remote memory access (RMA) subroutines. 
RMA does not fall into the message-passing communication models, but they have 
advantages in some cases: 

- RMA does not always require the remote process explicitly participating 
  the communication, and therefore, it is easier to use and more expressive. 
  Due to this feature, RMA is also called **one-side communication**.
- RMA has better performance if supported by specific hardware. The traditional
  message-passing communication requires message matching, which possibly introduces 
  unnecessary time-ordering. 

Due to these advantages, MPI decides to support RMA operations. The key developing
strategy is to make the RMA subroutines as similar as the message-passing 
subroutines if possible. MPI uses communicators, datatypes in the RMA operations, just 
like in the message-passing operations. MPI standard requires the RMA subroutines available
on any hardware condition, not matter whether RMA is directly supported by the hardware,
and no matter whether the coherent memory is used in the system. So, if you already
have a MPI-3 compliant library, just try RMA.

Active-target RMA with Fence
------------------------------

RMA operations are managed by a "window" object typed :class:`HIPP::MPI::Win`. A RMA 
window is created from an existing communicator. As usual, we may get the global 
"world" communicator from the MPI environmental object::

    HIPP::MPI::Env env;
    auto comm = env.world();
    int rank = comm.rank(), n_procs = comm.size();

If not all of the processes would participate RMA, just create a sub communicator 
from the global one.

To create a RMA window, each process in the communicator needs to attach a memory buffer 
to it. The RMA operations are then allowed to be posted on these buffers. The method 
:func:`win_create <HIPP::MPI::Comm::win_create>` of the communicator accepts the 
base address and size in bytes of the buffer and a displacement unit which is used 
in the RMA operations. It returns the window object created for RMA. In the following, 
we create a RMA window with each process attaching a buffer of two integers::

    vector<int> buff(2);
    void *base = buff.data();
    int disp_unit = sizeof(int), buff_size = disp_unit*buff.size();
    auto win = comm.win_create(base, buff_size, disp_unit);

Once we have a window object, methods :func:`get <HIPP::MPI::Win::get>`, :func:`put <HIPP::MPI::Win::put>`, 
:func:`accumulate <HIPP::MPI::Win::accumulate>`, and other variants are allowed to call. These 
RMA operations get, put or accumulate data from/to remote buffer. In MPI, RMA operations 
are not finished automatically. So, a pair of :func:`fence <HIPP::MPI::Win::fence>` calls 
are necessary to signal the begining and the end of RMA operations between them. In the following,
we put the rank of each process to its two neighbors in a cyclic way::

    int prev = (rank != 0) ? (rank-1) : (n_procs-1),
        next = (rank != n_procs-1) ? (rank+1) : 0;
    win.fence();
    win.put(next, rank, 0);
    win.put(prev, rank, 1);
    win.fence();

    {
        HIPP::MPI::SeqBlock seq(comm);
        HIPP::pout << "rank=", rank, ", buff=(", buff, ")", endl;
    }

Note that we print the content of the buffer in a sequential block guarded by 
a :class:`HIPP::MPI::SeqBlock` instance. This sequentializes the printings 
from different processes.
The use of :func:`fence <HIPP::MPI::Win::fence>` is the easiest way to synthesize the 
RMA operations. We will explore more features of RMA in the following section.

The output of above codes is

.. code-block:: text

    rank=0, buff=(3,1)
    rank=1, buff=(0,2)
    rank=2, buff=(1,3)
    rank=3, buff=(2,0)

We see that every process gets the ranks of its two neighbor processes. Full 
code sample can be found at 
:download:`mpi/rma-win-creation.cpp </../example/tutorial/mpi/rma-win-creation.cpp>`.



Passive-target RMA with Lock/Unlock 
-------------------------------------
Passive-target RMA operations are real "one-side", because they do 
not require the participation of remote process. Passive-target RMA uses 
the same data access subroutines as active-target RMA, 
like :func:`get <HIPP::MPI::Win::get>`, :func:`put <HIPP::MPI::Win::put>` and 
:func:`accumulate <HIPP::MPI::Win::accumulate>`. The difference is the synchronization - 
passive-target RMA uses lock/unlock operation to synthesize the data access.

In the following example, we use passive-target RMA to transfer data multiple 
times from one process to another. The code can be found at 
:download:`mpi/rma-passive-target.cpp </../example/tutorial/mpi/rma-passive-target.cpp>`.

We start with initialization of MPI environment, get the rank of self. We will 
pass 5 pieces of data from procece 0 to procece 1 (``n_RMAs=5``)::

    HIPP::MPI::Env env;
    auto comm = env.world();
    int rank = comm.rank(), n_RMAs = 5;

Process 0 and process 1 have different things to do, we separate them into 
two parts. In process 0, we just create the window by 
:func:`win_create <HIPP::MPI::Comm::win_create>` method of the communicator, 
where the memory of one integer is attached. Then we repeatedly assign a value 
to the local window, each protected with a pair of 
:func:`lock <HIPP::MPI::Win::lock>` and :func:`unlock <HIPP::MPI::Win::unlock>`
methods of the window object.
The lock/unlock operations ensure the content in private and public memories
are synchronized. After the first barrier, process 1 will get the data, 
and process 0 does not need to do anything. After the second barrier, 
the current loop of RMA ends and the next begins. The codes process 0 are::

    int val, buff_size = sizeof(int), disp_unit = 1;
    auto win = comm.win_create(&val, buff_size, disp_unit);

    for(int i=0; i<n_RMAs; ++i){
        win.lock(win.LOCK_EXCLUSIVE, 0);
        val = i;
        win.unlock(0);
        comm.barrier();
        comm.barrier();
    }

In process 1, we create the window but attach no data, because procece 1 just 
visits the memory of process 1 but does not share any of its own data. 
Then, it repeatedly get a integer from process 0 
(i.e., from process ranked ``src_rank=0``, 
memory address started at ``offset=0``).
Each :func:`get <HIPP::MPI::Win::get>` must be called after the first barrier to 
ensure the new value has been set by process 0. The :func:`get <HIPP::MPI::Win::get>`
call should also be protected by a pair of lock and unlock calls which start 
and end the access epoch, respectively, to the remote window. 
The value got by process 1 is printed at each loop.
The codes for process 1 are::

    auto win = comm.win_create(NULL, 0, 1);
    int val, src_rank = 0, offset = 0;
    for(int i=0; i<n_RMAs; ++i){
        comm.barrier();
        win.lock(win.LOCK_EXCLUSIVE, src_rank);
        win.get(src_rank, val, offset);
        win.unlock(src_rank);
        comm.barrier();
        HIPP::pout << "Get ", val, endl;

The output of the code is:

.. code-block:: text 

    Get 0
    Get 1
    Get 2
    Get 3
    Get 4


