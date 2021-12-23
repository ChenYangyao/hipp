Point-to-Point Communications
====================================================

.. include:: /global.rst

.. namespace:: HIPP::MPI

.. _tutor-mpi-p2p:

Standard Point-to-Point Communications
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

The Communication Buffer
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