Point-to-Point Communications
====================================================

.. include:: /global.rst

.. namespace:: HIPP::MPI

.. _tutor-mpi-p2p:

Basic Point-to-Point Communications
-----------------------------------------

The methods :expr:`Comm::send()` and :expr:`Comm::recv()` are the two ends 
of a point-to-point communication. They must be called in pair, one at 
the message source process and the other one at the message 
destination process (unless the target rank is a special value 
:var:`PROC_NULL`). They follow the standard blocking semantics - they may block 
the calling process, and on return, the communication buffer can be reused by 
the user.

The following codes show a typical point-to-point communication::

    int tag = 0, buf[5];
    if( rank == 0 ) {
        comm.send(1, tag, buf);
    }else if ( rank == 1 ) {
        comm.recv(0, tag, buf);
    }

The sending process specifies the rank of message destination process, while 
the receiving process specifies the rank of message source process.
Both the sending and receiving processes specify a tag for message-matching, 
and a buffer object that stores the outgoing/in-coming data.

.. _tutor-mpi-p2p-buffer:

Communication Buffer
""""""""""""""""""""""""""

HIPP supports various types of buffer object. The supported types and examples 
for each type are listed below:

.. table::
    :class: fix-width-table tight-table
    :widths: 40 60

    =========================================================================================================================== ==========================================================================================
    Type                                                                                                                        Examples
    =========================================================================================================================== ==========================================================================================
    numerical scalar                                                                                                            ``int``, ``double``, ``float``
    raw array (see the protocol defined by :class:`~HIPP::RawArrayTraits`)                                                      ``int [3]``, ``float[3][4]``, |br|
                                                                                                                                ``std::array<int, 3>``, |br|
                                                                                                                                ``std::array<std::array<float, 4>, 3>``, |br|
                                                                                                                                :type:`~HIPP::NUMERICAL::SVec`, |br|
                                                                                                                                :class:`~HIPP::NUMERICAL::SArray`
    contiguous buffer of numerical scalar or raw array (see the protocol defined by :class:`~HIPP::ContiguousBufferTraits`)     ``std::vector<int>``, |br| 
                                                                                                                                ``std::vector< std::array<float, 3> >``, |br|
                                                                                                                                :class:`~HIPP::NUMERICAL::DArray`
    STL string                                                                                                                  ``std::string``
    =========================================================================================================================== ==========================================================================================

Hence, the following buffer objects are valid arguments to send/recv calls::

    double scalar;                                  // numerical scalar
    
    int raw_arr[3][4];                              // 3x4 array
    array<float, 3> arr;                            // 3-element array
    array<array<int, 4>, 3> arr_of_arr;             // 3x4 array
    SArray<double, 3, 4> s_arr;                     // 3x4 array
    SVec<int, 4> s_vec;                             // 4-element array

    vector<double> vec(10);                         // 10 scalar values
    vector<array<int, 3> > vec_of_arr(10);          // 10 arrays, each has length 3
    DArray<int, 3>  matrix({2,3,4});                // 3x3x3 matrix
    
    string s(5, 'c');

For example, to send them, write::

    comm.send(1, tag, scalar);

    comm.send(1, tag, raw_arr);
    comm.send(1, tag, arr);
    comm.send(1, tag, arr_of_arr);
    comm.send(1, tag, s_arr);
    comm.send(1, tag, s_vec);
    
    comm.send(1, tag, vec);
    comm.send(1, tag, vec_of_arr);
    comm.send(1, tag, matrix);
    
    comm.send(1, tag, s);

The above types should be sufficient for most tasks. In rare cases, especially 
when dynamically allocated buffer or customized datatypes are used, the communication buffer does not fall into 
any of the above types. It is safe to roll back to the MPI standard style, i.e., 
use ``buffer, count, datatype`` triplet to specify the buffer. For example::

    float *buf1 = new float [12],
          *buf2 = (float *)malloc(sizeof(float)*12);

    comm.send(1, tag, buf1, 12, mpi::FLOAT);
    comm.send(1, tag, buf2, 12, mpi::FLOAT);


.. hint::

    Internally, all types of buffer object and the standard triplet, are 
    mapped to :class:`~HIPP::MPI::Datapacket` or :class:`~HIPP::MPI::ConstDatapacket`
    protocol object where the buffer address, count of data items, and datatype 
    are extracted. These details are then forwarded to the underlying MPI
    library for the actual communication.

    From the view of memory space, a buffer object supported by HIPP must 
    be contiguously layout elements. Hence, ``vector<array<int, 3> > vec_of_arr``
    is a valid buffer object, but ``vector<vector<int> > vec_of_vec`` is not.

    HIPP never resizes the receiving buffer. User must ensure it is non-const 
    and large enough to hold the message.


.. _tutor-mpi-p2p-modes:

Communication Modes
"""""""""""""""""""""

MPI does not specify how the standard send and receive calls are implemented. 
They may or may not buffer the data, may block or may not block the caller. 
On return, the message may or may not be delivered.

To satisfy different application scenarios, MPI defines four variants of 
send (called four different point-to-point communication modes):

.. table::
    :class: tight-table

    =================================== =================================================================================================================================================
    Communication Mode                  Semantics
    =================================== =================================================================================================================================================
    :func:`~Comm::send`                 Standard send.
    :func:`~Comm::bsend`                Buffered send. User must provide a buffer by :func:`buffer_attach` before sending and optionally detach it by :func:`buffer_detach` after using.
    :func:`~Comm::ssend`                Synchronous mode. It never buffers the data. Return only when a receive call is posted and matched, and the actual receiving begins.
    :func:`~Comm::rsend`                Ready mode. On entry, a receive call that matches this send call must have been posted.
    =================================== =================================================================================================================================================

The standard mode is introduced above in :ref:`tutor-mpi-p2p`. 

The buffered mode allows the user to provide a buffer that holds the message.
The message is copied to that buffer so that the sending call never blocks
the calling process.
The buffer must be large enough to hold all on-going buffered send calls. 
To use :func:`~Comm::bsend`, first attaches a buffer, then sends data, and 
finally detaches the buffer if the buffered send is no longer needed.

For example, to send the following two data objects::

    int buf1[4];
    vector<double> buf2(8);

One has to provide a buffer to the MPI library by :func:`buffer_attach`::

    size_t buf_size = sizeof(buf1) + sizeof(double)*buf2.size() 
        + 2 * mpi::BSEND_OVERHEAD;
    char *buff = new char[buf_size];
    mpi::buffer_attach(buff, buf_size);

The buffer size is at least the sum of the data size in the two objects, plus
two :var:`BSEND_OVERHEAD` which may be used by the library to store meta-info 
of the two messages. Then, call the buffered send just like the standard mode::

    comm.bsend(1, tag, buf1);
    comm.bsend(1, tag, buf2);

The synchronous mode :func:`~Comm::ssend` ensures that the message data is never buffered, so that
the consumed system resources for such a communication is as small as possible.
This can be used when the number messages are huge while the system resources 
are limited. The calling signature is similar to that of the standard mode::

    comm.ssend(1, tag, buf1);
    comm.ssend(1, tag, buf2);

The ready mode :func:`~Comm::rsend` should only be used when the sender knows that 
the corresponding receive call is already posted. Such information may be 
provided by other events, or by direct notification from the receiver. 
A common scenario is that, the receiver issues the receive call and notifies 
the sender, while the sender waits for the notification and then sends data 
in ready mode. For example::

    if( rank == 0 ) {    
        comm.recv(1, tag);                          // wait for notification
        comm.rsend(1, tag, buf1);
        comm.rsend(1, tag, buf2);
    } else if( rank == 1 ) {
        auto reqs = comm.irecv(0, tag, buf1);
        reqs += comm.irecv(0, tag, buf2);           // issue the receive calls
        comm.send(0, tag);                          // send an empty message for notification
        reqs.waitall();
    }

The non-blocking receive :func:`~Comm::irecv` will be introduced below.

.. _tutor-mpi-nonblocking-p2p:

Nonblocking Point-to-Point Communications
--------------------------------------------

All the point-to-point methods we described above block the caller thread 
until some predefined conditions satisfied. 
For example, implementation 
must ensure that the data buffer can be reused by the user on the return 
of any such blocking call.

In contrast, a "nonblocking" point-to-point call only issues the communication.
On return, the communication may be, or may not be finished; the data buffer is not 
permitted to be reused. The nonblocking call returns a "request" handler from which 
the process can be monitored. 
A "completion" method must be called later on the handler 
to ensure the completion of the communication.

Suppose we have the following buffer objects::

    int buf1[4];
    vector<double> buf2(8);
    float buf3;

For example, to send ``buf1`` from process 0 to 1 by a nonblocking point-to-point 
call, write::

    if( rank == 0 ) {
        mpi::Requests req = comm.isend(1, tag, buf1);
        req.wait();
    }else if( rank == 1 ){
        mpi::Requests req = comm.irecv(0, tag, buf1);
        req.wait();
    }

The methods :func:`~Comm::isend` and :func:`~Comm::irecv` issue the nonblocking 
send and receive, respectively. Both return a :class:`Requests` handler for 
further monitoring and completion. The call :func:`~Requests::wait` on the 
request handler blocks until the completion of send or receive.

One prominent feature of nonblocking communications is that they never block.
Therefore, multiple nonblocking communications can be issued on one thread 
without deadlock, and then get finished by one "multiple completion" call 
applied to an array of the request handlers.

To send out all the three buffers ``buf1``, ``buf2`` and ``buf3``, call 
:func:`~Comm::isend` three times and join the returned request handlers by 
:func:`~Requests::operator+=`::

    auto reqs = comm.isend(1, tag, buf1);
    reqs += comm.isend(1, tag, buf2);
    reqs += comm.isend(1, tag, buf3);

    pout << reqs.size(), " sends have been issued", endl;

Now, the :class:`Requests` object has three active request handlers 
attached to it. The printed output is:

.. code-block:: text

    3 sends have been issued

The multiple completion call :func:`~Requests::waitall` on a :class:`Requests`
objects blocks until all its active communications finished::

    reqs.waitall();

As introduced in :ref:`tutor-mpi-p2p-modes`, the nonblocking point-to-point 
communications also have four different sending modes, issued by :func:`~Comm::isend`, 
:func:`~Comm::ibsend`, :func:`~Comm::issend`, and :func:`~Comm::irsend`, respectively.

The blocking semantics in the sender and receiver
needs not to be identical. For example, a blocking, buffered send can match 
a nonblocking receive.

MPI defines a rich set of (multiple) completion methods for nonblocking communications. 
They are all supported by HIPP and are listed below. For their detail semantics, refer 
to the MPI standard.

.. table:: 
    :class: tight-table

    ================================ ==================================================================================
    Completion Method                                                                           Description                               
    ================================ ==================================================================================
    :func:`~Requests::wait()`        block until a active communication is finished
    :func:`~Requests::test()`        test whether a communication is finished
    :func:`~Requests::status()`      test whether a communication is finished but do not inactivate it or mark it null
    :func:`~Requests::waitany()`     block until any one active communication in an array is finished
    :func:`~Requests::testany()`     test whether any one active communication in an array is finished
    :func:`~Requests::waitall()`     block until all active communications in an array are finished
    :func:`~Requests::testall()`     test whether all active communications in an array are finished
    :func:`~Requests::waitsome()`    block until one or more communications in an array are finished
    :func:`~Requests::testsome()`    test whether one or more communications in an array are finished
    ================================ ==================================================================================
 




