Collective Communications
====================================================

.. include:: /global.rst

.. namespace:: HIPP::MPI

.. _tutor-mpi-collective:

Basic Collective Communications
----------------------------------

In a collective communication, all processes in the group underlying the communicator
must call the method with consistent arguments.

The simplest collective communication call is :expr:`Comm::barrier()`, where each 
process in the communicator waits until all other processes arrives at this 
execution point. The following codes demonstrate how to serialize the execution 
on processes, i.e., let processes execute their codes one by one::

    for(int i=0; i<n_procs; ++i){
        if( rank == i )
            pout << "Process ", rank, endl;
        comm.barrier();
    }

In each iteration, only one process executes its codes. 
Because of the barrier at the end of the iteration, 
no process proceeds to the next step until all arrives there.
Hence, barriers serve as the synchronization points that separate 
and serialize the operations.

Code serialization appears so commonly that HIPP provides a guard type :class:`SeqBlock`
for it::

    {
        mpi::SeqBlock _{comm};
        pout << "Process ", rank, endl;
    }

The named guard object ensures the serialization of codes in the same block, 
with more efficient implementation than barriers do.

The output from both should be:

.. code-block:: text

    Process 0
    Process 1
    Process 2
    Process 3

However, depending on the cache mechanism of your system, the output may still 
get mixed.

Broadcast 
""""""""""

Another frequently used collective call is :expr:`Comm::bcast()`. 
Initially, the data object is in a "root" process. After the broadcast 
operation, all processes in the communicator get a copy of it::

    int root = 0, buf1[5];
    comm.bcast(buf1, root);

Like in the point-to-point case, HIPP defines overloading methods that 
enable you to fall back to the standard-compliant API 
when general data buffers are used::

    float *buf2 = new float [10];
    comm.bcast(buf2, 10, mpi::FLOAT, root);

Gather and Scatter
""""""""""""""""""""

:expr:`Comm::gather()` collects the same amount of data from each process 
and puts them contiguously into a buffer in the "root" process. 
For example, in the following codes, each process sends 8 integers to the 
root::

    vector<int> send_buf(8), recv_buf(8*n_procs);
    comm.gather(send_buf, recv_buf, root);

The receive buffers are not significant in all non-root processes. Hence, it is 
valid to pass an empty receive buffer in any non-root process and rewrite 
the above codes by::

    if( rank == root ) {
        comm.gather(send_buf, recv_buf, root);
    } else {
        comm.gather(send_buf, {}, root);
    }

In above gather calls, the root process itself also sends data to the receive 
buffer. You may pass a special variable :var:`IN_PLACE` as the send buffer of 
the root to avoid this::

    if( rank == root ) {
        comm.gather(mpi::IN_PLACE, recv_buf.data(), 
            8, mpi::INT, root);
    } else {
        comm.gather(send_buf, {}, root);
    }

Note that in this case, you must fall back to the standard-compliant overload.

In contrast, :expr:`Comm::scatter()` method takes a data buffer from the root, 
splits it into chunks of the same size, and sends them out, one chunk to one process.
For example, in the following codes, each process gets 8 integers from the root::

    vector<int> send_buf(8*n_procs), recv_buf(8);
    comm.scatter(send_buf, recv_buf, root);


The variants :expr:`Comm::gatherv` and :expr:`Comm::scatterv` further allow 
different amount of data to be gathered from/scattered to different processes.
The root process must provide two additional arguments, 
``counts`` and ``displs``, to specify the number of data items from/to each 
processes, and the displacements in the buffer at which the 
chunks are taken/put.

For example, in the following codes, each process sends one integer to the root, 
while the root puts the gathered chunks with a stride 2::

    int send_buf;
    vector<int> recv_buf(2*n_procs), counts(n_procs, 1), 
        displs = ALG::LinSpaced(0, n_procs, 2).get();
    comm.gatherv(send_buf, recv_buf, counts, displs, root);

Here, the argument ``counts = {1, 1, 1, ...}`` specifies that the root receives 
one data item from each process, and ``displs = {0, 2, 4, ...}`` specifies 
that those integers are put at displacements ``0, 2, 4, ...`` relative to 
the starting address of the receive buffer and in the unit of receive datatype.
We use the class :class:`~HIPP::ALGORITHM::LinSpaced` in the ALGORITHM module 
to generate the displacements array.

In contrast, the following codes show how to scatter integers with a stride 2 
to processes::

    vector<int> send_buf(2*n_procs), counts(n_procs, 1),
        displs = ALG::LinSpaced(0, n_procs, 2).get();
    int recv_buf;
    comm.scatterv(send_buf, counts, displs, recv_buf, root);

Reduce
""""""""""""

:expr:`Comm::reduce()` performs collective computations. It is equivalent to first collect 
all data buffers from processes through a gather, and then perform the 
corresponding reduction operations element-wisely.

For example, the following codes sum a integer ``send_buf`` from each process 
and put the result into ``recv_buf`` in the root::

    int root = 0, send_buf, recv_buf;
    comm.reduce(send_buf, recv_buf, mpi::SUM, root);

Another method, :expr:`Comm::allreduce()`, puts the reduction result into receive 
buffers in all processes, equivalent to a reduce operation followed by a 
scatter::

    comm.allreduce(send_buf, recv_buf, mpi::SUM);