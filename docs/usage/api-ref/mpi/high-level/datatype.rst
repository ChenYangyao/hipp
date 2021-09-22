Datatype and Operation
=================================

.. include:: /global.rst

The following classes are all defined within namespace ``HIPP::MPI``.

.. namespace:: HIPP::MPI

Datatype and Datapacket
------------------------------------------

.. class:: Datatype 

    ``Datatype`` describes the type of a data-elements (i.e., its byte-level interpretation and 
    allowed operations on it). ``Datatype`` is used in the communication as a part of the 
    data buffer descriptor.
    
    **Memory management methods:**
    
    .. table::
        :class: tight-table

        =================================================== ====================================================
        Method                                              Detail 
        =================================================== ====================================================
        default constructor                                 Available; ``noexcept``; constructs a null value 
                                                            |br| as returned by :func:`nullval`.
        copy constructor |br| and ``operator=(&&)``         Available; ``noexcept``; shallow copy.
        move constructor |br| and ``operator=(const &)``    Available; ``noexcept``.
        =================================================== ====================================================


    .. function::   ostream &info( ostream &os = cout, int fmt_cntl = 1 ) const
                    friend ostream & operator<<( ostream &os, const Datatype &dtype )

        ``info()`` prints the information of the current instance to the stream 
        ``os``.
    
        :arg fmt_cntl:   control the amount of information to be printed, 0 for a 
                 short and inline priting, 1 for a verbose, multi-line version.
        :return: The argument ``os`` is returned.
    
        The overloaded ``<<`` operator is equivalent to ``info()`` with 
        default ``fmt_cntl``.
    
    .. function:: void free() noexcept

        Free the object and set it to null value as returned by :func:`nullval`.
        It is safe to call this function at any time, or even multiple times, 
        except for exactly the predifined objects (they are `const`, but a 
        non-const copy can be freed). 


    .. function::   bool is_null() const
                    void extent( aint_t &lb, aint_t &ext ) const
                    void true_extent( aint_t &lb, aint_t &ext ) const
                    int size() const

        Inquery the information about this datatype instance.

        ``is_null()`` tests whether this is a null value (an invalid instance 
        for communication).

        ``extent()`` gets the lower bound and extent of the datatype.
        ``true_extent()`` gets the "true" lower bound and extent (ignore the 
        changes made by, e.g., :func:`resized`).

        ``size()`` get the size (i.e., number of bytes required to store 
        the real data).


    .. function::   Datatype dup() const
                    static Datatype nullval() noexcept
                    Datatype resized( aint_t lb, aint_t ext ) const

        ``dup()`` creates a duplication of the current datatype.
        
        ``nullval()`` returns a null value, corresponding to 
        ``MPI_DATATYPE_NULL`` internally.
        
        ``resized()`` creates a resized datatype with new lower bound ``lb`` and extent ``ext``. 


    .. function::   Datatype contiguous( int count ) const
                    Datatype vector( int count, int blklen, int stride ) const
                    Datatype hvector( int count, int blklen, aint_t stride ) const
                    Datatype indexed_block( int blklen, const std::vector<int> &displs ) const
                    Datatype hindexed_block( int blklen, \
                        const std::vector<aint_t> &displs ) const
                    Datatype indexed( const std::vector<int> &blklens, \
                        const std::vector<int> &displs ) const
                    Datatype hindexed( const std::vector<int> &blklens, \
                        const std::vector<aint_t> &displs ) const
                    static Datatype struct_( const std::vector<int> &blklens, \
                        const std::vector<aint_t> &displs, const std::vector<Datatype> &dtypes)

        New datatype creation methods;

        ``contiguous()`` creates a datatype with contiguous ``count`` elements of the current
        datatype. 

        ``vector()`` creates a vector datatype of the current datatype, 
        consisting of ``count`` blocks, each with
        ``blklen`` contiguous elements, separated with stride ``stride`` (in the 
        unit of the current datatype). ``hvector()`` is similar, but the ``stride`` is in bytes. 

        ``indexed_block()`` further allows non-constant stride. The displacements
        of blocks are specified by ``displs`` (in the unit of the current datatype ).
        ``hindexed_block()`` specifies the displacements in bytes. 

        ``indexed()`` allows further flexibility - the blocks can have different lengths specified 
        by ``blklens``.
        ``hindexed()`` uses displacements in bytes;

        ``struct_`` is the most general one - the datatypes of blocks can be different 
        and specified by ``dtypes``.

    .. function::   Datatype darray( int size, int rank, const std::vector<int> &gsizes, \
                        const std::vector<int> &distribs, \
                        const std::vector<int> &dargs,\
                        const std::vector<int> &psizes, int order = ORDER_C )const
                    Datatype subarray( const std::vector<int> &sizes, \
                        const std::vector<int> &subsizes, \
                        const std::vector<int> &starts, int order = ORDER_C )const

        New datatype creation methods for distributed data.

        ``darray()`` creates a datatype which describes a part of a large distributed array.
        ``size`` and ``rank`` are the number of parts and the index of the target part. 
        The array may have any dimension, with ``psizes`` specifying the number of process at each direction, 
        ``gsizes`` describing the size
        of the global array at each direction, ``distribs`` and ``dargs`` describing the 
        distribution method (``distribs`` can be :var:`DISTRIBUTE_BLOCK` or 
        :var:`DISTRIBUTE_CYCLIC` or :var:`DISTRIBUTE_NONE` for each dimension; 
        ``dargs`` is block size, can be :var:`DISTRIBUTE_DFLT_DARG` for nearly uniform distribution or 
        can be a interger).
        ``order`` can be :var:`ORDER_C` or :var:`ORDER_FORTRAN`.

.. _api-mpi-predefined-dtype:

Predefined Datatypes 
"""""""""""""""""""""""""

.. var::    extern const Datatype CHAR
            extern const Datatype WCHAR
            extern const Datatype SIGNED_CHAR
            extern const Datatype SHORT
            extern const Datatype INT
            extern const Datatype LONG
            extern const Datatype LLONG
            extern const Datatype UNSIGNED_CHAR
            extern const Datatype UNSIGNED_SHORT
            extern const Datatype UNSIGNED_INT
            extern const Datatype UNSIGNED_LONG
            extern const Datatype UNSIGNED_LLONG
            extern const Datatype C_BOOL

    Predefined datatypes that have C counterparts ``char``, ``wchar_t``, ``signed char``
    ``short``, ``int``, ``long``, ``long long``, ``unsigned char``
    ``unsigned short``, ``unsigned int``, ``unsigned long``, ``unsigned long long``
    ``_Bool``.

.. var::    extern const Datatype INT8
            extern const Datatype INT16
            extern const Datatype INT32
            extern const Datatype INT64
            extern const Datatype UINT8
            extern const Datatype UINT16
            extern const Datatype UINT32
            extern const Datatype UINT64

    Predefined for fixed-length integers.

.. var::    extern const Datatype FLOAT
            extern const Datatype DOUBLE
            extern const Datatype LDOUBLE
            extern const Datatype C_COMPLEX
            extern const Datatype C_FLOAT_COMPLEX
            extern const Datatype C_DOUBLE_COMPLEX
            extern const Datatype C_LDOUBLE_COMPLEX
            
    Predefined datatypes for floating-points numbers ``float``, ``double``, ``long double``
    ``float _Complex``, ``float _Complex``, ``double _Complex``, ``long double _Complex``.

            
.. var::    extern const Datatype BOOL
            extern const Datatype FLOAT_COMPLEX
            extern const Datatype DOUBLE_COMPLEX
            extern const Datatype LDOUBLE_COMPLEX

    Predefined datatypes for C++. ``bool``, ``std::complex<float>``, ``std::complex<double>``, ``std::complex<long double>``.

.. var::    extern const Datatype BYTE
            extern const Datatype PACKED
            extern const Datatype AINT
            extern const Datatype OFFSET
            extern const Datatype COUNT

    Special datatypes for a byte (8-bits), for the packed calls, and for :type:`aint_t`, :type:`offset_t` and :type:`count_t`.


.. var::    extern const Datatype FLOAT_INT
            extern const Datatype DOUBLE_INT
            extern const Datatype LONG_DOUBLE_INT
            extern const Datatype SHORT_INT
            extern const Datatype INT_INT
            extern const Datatype LONG_INT

    Predefined datatypes for reduction operations (such as :var:`MINLOC` and :var:`MAXLOC`). 


Datapacket
"""""""""""""""""""""""""""""""""""""""""""""""

.. class::  Datapacket 

    A datapacket is defined, in the high-level interface, as a pack of the 
    standard MPI data buffer specification, i.e., 
    ``{buff_addr, buff_size, datatype}``.
    
    Datapacket gives the communications calls flexibility in that multiple types
    of objects can be directly passed in as buffer.

    The Datapacket type has tuple-like API for structured binding, e.g.::

        std::vector<int> v {1,2,3};
        auto [buff, size, datatype] = Datapacket(v);
    
    Here, buff, size, datatype are reference type to ``void *``, ``int`` and ``Datatype``, 
    respectively.

    **Memory management methods:**
    
    .. table::
        :class: tight-table
        
        =================================================== ============================================================
        Method                                              Detail 
        =================================================== ============================================================
        default constructor                                 An empty buffer, i.e., ``(buff=BOTTOM, size=0, dtype=INT)``
        copy constructor |br| and ``operator=(&&)``         Defined; ``noexcept``.
        move constructor |br| and ``operator=(const &)``    Defined; ``noexcept``.
        =================================================== ============================================================

    .. _api-mpi-dpacket-constructor:

    .. function::   Datapacket(const void *buff, int size, Datatype dtype) noexcept
                    Datapacket(const void *buff, int size, const string &dtype)
                    Datapacket(const string &buff) noexcept
                    template<typename T, std::enable_if_t<_is_intern_dtype<T>(), int> =0> \
                    Datapacket(const T &buff) noexcept
                    template<typename T, std::enable_if_t<_is_intern_dtype<T>(), size_t> N> \
                    Datapacket(const T (&buff)[N]) noexcept
                    template<typename T, std::enable_if_t<_is_intern_dtype<T>(), int> =0> \
                    Datapacket(const T *buff, size_t n) noexcept
                    template<typename T, std::enable_if_t<_is_intern_dtype<T>(), size_t> N> \
                    Datapacket(const std::array<T, N> &buff) noexcept
                    template<typename T, typename A, std::enable_if_t<_is_intern_dtype<T>(), int> =0> \
                    Datapacket(const vector<T,A> &buff) noexcept
                    template<typename T, \
                        std::enable_if_t<ContiguousBufferTraits<T>::is_buffer,int> =0> \
                        Datapacket(const T &cb)

        Data packet constructors. A variety of ways can be used to construct a data packet, by providing the 
        following arguments:

        .. table::
            :class: fix-width-table tight-table
            :widths: 30 70
            
            =================================================== ===========================================================================================
            Arguments                                           Description 
            =================================================== ===========================================================================================
            Standard triplet                                    ``(buff, size, dtype)``, where ``buff`` is the starting address, ``size``
                                                                is the number of elements and ``dtype`` is the datatype of each element. |br|
                                                                ``dtype`` can be either :class:`Datatype` or ``std::string`` (automatically converted to 
                                                                :class:`Datatype`; see below for valid strings for datatype).
            A string or a arithmetic scalar variable            ``std::string`` or any arithmetic scalar (see below for valid scalars). In C++, 
                                                                a ``std::string`` is not writable, so that it can not be used in any receiving call.
            An array-like object of arithmetic scalars          raw array (e.g., ``int [3]``), raw buffer
                                                                (e.g., a ``int *`` that points to ``n`` integers), ``std::array`` or ``std::vector``
            Any other ContiguousBuffer protocol object          -
            =================================================== ===========================================================================================
        
        When constructing a data packet with the standard triplet, a ``std::string`` can be used to specify the datatype. Valid 
        strings are:
        
        - ``"byte"``, ``"char"`` or ``"bool"``;
        - ``"X"``, ``"signed X"`` or ``"unsigned X"``, where ``X`` can be ``char``, ``short``, ``int``, ``long`` or ``long long``;
        - ``"intX_t"`` or ``"uintX_t"``, where ``X`` can be ``8``, ``16``, ``32`` or ``64``;
        - ``"float"``, ``"double"``, ``"long double"``.

        .. _api-mpi-dpacket-predefined-type:

        Where the "arithmetic scalar" are referred, we mean one of the following C++ types:

        - ``bool``, ``char``, ``signed char`` or ``unsigned char``;
        - ``X`` or  ``unsigned X``, where ``X`` can be ``short``, ``int``, ``long`` or ``long long``;
        - ``float``, ``double`` or ``long double``;
        - ``std::complex<float>``, ``std::complex<double>`` or ``std::complex<long double>``.
    
    .. function::   Datapacket(aint_t disp, int size, Datatype dtype) noexcept
                    Datapacket(aint_t disp, int size, const string &dtype)

        Sometimes, triplet is used to specify a memory segement relative to 
        a base address (e.g., in RMA operations). In such cases, the first 
        part of the triplet is a displacement relative to the base address. 
        
        The ``Datapacket`` type can represent such triplets, too. Internally, the
        displacement is stored by casting into a ``void *``.

Tuple-like API are defined for ``Datapacket``:

.. function:: \
    template<std::size_t I> decltype(auto) get( Datapacket &dp )
    template<std::size_t I> decltype(auto) get( const Datapacket &dp )
    template<std::size_t I> decltype(auto) get( Datapacket &&dp )

.. class:: template<> std::tuple_size<Datapacket>
    
    .. member:: static constexpr std::size_t value = 3

.. class:: template<> tuple_element<0, Datapacket>

    .. type:: type = void *

.. class:: template<> tuple_element<1, HIPP::MPI::Datapacket>
    
    .. type:: type = int

.. class:: template<> tuple_element<2, HIPP::MPI::Datapacket>

    .. type:: type = Datatype


**Examples:**

In the point-to-point communication, the send/recv call needs data buffer as argument.
Traditionally, MPI uses triplet ``{addr, size, datatype}`` to describe the data buffer.
In HIPP, the following send (or recv) calls are valid and equivalent for ``std::vector``::

    vector<int> send_buff(10), recv_buff(10);

    comm.send(dest, tag, send_buff);
    comm.send(dest, tag, &send_buff[0], 10, INT);
    comm.send(dest, tag, &send_buff[0], 10, "int");

    comm.recv(src, tag, recv_buff);
    comm.recv(src, tag, &recv_buff[0], 10, INT);
    comm.recv(src, tag, &recv_buff[0], 10, "int");

Note that in a send (or recv) call, HIPP uses the arguments after ``src`` (or ``dest``) and ``tag`` to construct a 
``Datapacket`` instance, and uses members in this data packet as the buffer arguments
to the underlying MPI library.

``std::string`` can be used as a sending buffer, but not a receiving buffer. You must provide a buffer by 
yourself in the receiving call, like using a ``std::vector``::

    string send_str = "content to send";
    vector<char> recv_str(128);

    comm.send(dest, tag, send_str);
    comm.recv(src, tag, recv_str);

Arithematic scalars and raw arrays (or raw buffers) of them can be easily send/recv, like::

    int x,
        arr[3],
        *buff = new int [3];

    comm.send(dest, tag, x);
    comm.send(dest, tag, arr);
    comm.send(dest, tag, buff, 3);

    // Or equivalently, using the standard triplet as buffer descriptor
    comm.send(dest, tag, &x, 1, HIPP::MPI::INT);
    comm.send(dest, tag, arr, 3, HIPP::MPI::INT);
    comm.send(dest, tag, buff, 3, HIPP::MPI::INT);



Pack and ExternalPack
---------------------------------

.. class:: Pack 

    ``Pack`` provides, with static methods, the pack/unpack operations like the 
    standard MPI.
    
    A ``Pack`` instance has a buffer (represented by a **base** address + a **size** in 
    bytes) and a **position** pointer indicating the byte offset of the next 
    available location which has not been used.
    On intialization, the buffer has a preallocated size, where position is 
    set to 0.
    
    A ``Pack`` instance encapsulates the buffer and the pack/unpack operations. 
    By using :func:`push` method, the buffer is automatically enlarged to fit the
    need before data are packed into it. The position pointer is also 
    automatically moved forward. The :func:`pop` method, on the other hand, unpack 
    the buffer and move the position pointer also forward. 

    Be cautious to resize the ``Pack`` instance to suitable length before 
    receiving, and to reset the position to 0 before unpacking.

    **Memory management methods:**
    
    .. table::
        :class: tight-table
    
        =================================================== ==================================================
        Method                                              Detail 
        =================================================== ==================================================
        default constructor                                 Not available.
        copy constructor |br| and ``operator=(const &)``    Defined; deep-copy.
        move constructor |br| and ``operator=(&&)``         Defined; ``noexcept``.
        =================================================== ==================================================

    The copy operation uses deep-copy, i.e., all information (include 
    the buffer content, buffer size, position pointer and communicator)
    gets copied.
    The move operator is a cheap move, i.e., the moved object cannot be 
    used any more.

    .. function:: \
        Pack(Comm comm)
        Pack(size_t size_prealloc, Comm comm)

        Initialize the instance with ``size_prealloc`` bytes buffer allocated, 
        used for packing/unpacking in the communicator ``comm``.
        The position pointer is set to 0.

        :arg comm: communicator that would be used in packing/unpacking.
        :arg size_prealloc: preallocated bytes for buffer - not necessary to be 
                            large because :func:`push` automatically resize it or 
                            because it can be adjusted by :func:`set_size`.

    .. function:: \
        Pack & push(const Datapacket &in_dpacket)
        Pack & pop(const Datapacket &out_dpacket)

        ``push()`` packs ``in_dpacket`` into the buffer. It automatically resizes the
        buffer if memory is not enough to hold ``in_dpacket``. The position pointer
        is increased after ``push()``.
        
        ``pop()`` perfoms oppisite operation - unpack from the current position 
        to ``out_dpacket``, and move the position pointer forward.

        ``*this`` is returned which enables chain of methods on the same instance.

    .. function:: \
        Datapacket as_sendbuf() const
        Datapacket as_recvbuf()

        ``as_sendbuf()`` returns a buffer descriptor that can be used in sending operations. 
        The value of the position pointer is used as the size attribute in the descriptor.

        ``as_recvbuf()`` returns a buffer descriptor that can be used in receiving operations.
        The buffer size that has been allocated is used as the size attribute in the descriptor.


    .. function:: \
        const void * get_base() const noexcept
        void * get_base() noexcept
        size_t get_size() const noexcept
        aint_t get_position() const noexcept
        const Comm & get_comm() const noexcept
        Comm & get_comm() noexcept

        Attribute getters. 

        ``get_base()``, ``get_size()`` and ``get_position()`` return the buffer information - 
        its base address, size, and byte offset of the position pointer (points to 
        next available location that has not been used), respectively.

        ``get_comm()`` returns the commnicator. The commnicator is that passed 
        in the constructor or method :func:`set_comm`.

    .. function:: \
        Pack & set_size(size_t s)
        Pack & set_position(aint_t p) noexcept
        Pack & set_comm(Comm comm) noexcept

        Attribute setters.

        ``set_size()`` adjusts the size of the buffer to ``s`` (in byte). If ``s`` is than 
        the current buffer size, the tail is truncated. Otherwise the original content 
        is preserved.

        ``set_position()`` adjusts the position pointer to ``p`` measured as the byte offset 
        from the base address of the buffer.

        ``set_comm()`` adjusts the communicator that will be used in packing/unpacking.

        ``*this`` is returned which enables chain of methods on the same instance.

    .. function:: \
        void pack(const Datapacket &in_dpacket,  \
            void *outbuf, int outsize, int &position, const Comm &comm)
        void unpack(const void *inbuf, int insize, int &position, \
            const Datapacket &out_dpacket, const Comm &comm)
        int size(int incount, const Datatype &dtype, const Comm &comm)
        int size(const Datapacket &in_dpacket, const Comm &comm)

        The functions mapped to the standard MPI calls.

        ``pack()`` packs data described by ``in_dpacket`` to the output buffer ``outbuf`` sized ``outsize`` at 
        the position ``position``. The buffer will be used in the communication in the commnicator ``comm``.
        ``position`` is updated to the next location that can be used for the next packing operation.

        ``upack()`` upacks data in the buffer ``inbuf`` into the user buffer described by ``out_dpacket``.

        ``size()`` returns the maximal buffer size in byte that may be used in ``pack()``.


.. class:: ExternalPack

    ``ExternalPack`` is the same with :class:`Pack`, except that it uses "canonical packing/unpacking" 
    which gives buffer with no header and following predefined standard format.

    ``ExternalPack`` does not accept any commnicator because the buffer format is universal and does not
    depend on the commnicator. Instead, it accepts a ``datarep`` argument in the constructor indicating
    which standard data format is used. The only avaiable ``datarep`` is ``"external32"``.

    .. function:: \
        ExternalPack(const string &datarep="external32")
        ExternalPack(size_t size_prealloc, const string &datarep="external32")

        Constructors.

    .. function:: \
        ExternalPack & push(const Datapacket &in_dpacket)
        ExternalPack & pop(const Datapacket &out_dpacket)

        Pack/unpack data into/from the current buffer.

    .. function:: \
        Datapacket as_sendbuf() const
        Datapacket as_recvbuf()

        Return buffer descriptor that may be used in sending/recving calls.

    .. function:: \
        const void * get_base() const noexcept
        void * get_base() noexcept            
        size_t get_size() const noexcept      
        aint_t get_position() const noexcept  
        const string & get_datarep() const noexcept

        Attribute getters.

    .. function:: \
        ExternalPack & set_size(size_t s)
        ExternalPack & set_position(aint_t p) noexcept
        ExternalPack & set_datarep(const string &datarep)

        Attribute setters.

    .. function:: \
        static void pack(const Datapacket &in_dpacket, \
            void *outbuf, aint_t outsize, aint_t &position, \ 
            const string &datarep="external32")
        static void unpack(const void *inbuf, aint_t insize, \
            aint_t &position, const Datapacket &out_dpacket, \
            const string &datarep="external32")
        static aint_t size(int incount, const Datatype &dtype,  \
            const string &datarep="external32")
        static aint_t size(const Datapacket &in_dpacket, \
            const string &datarep="external32")

        Methods mapped to the standard MPI calls.


Op
---------------------------

.. class::  Op

    ``Op`` describes the operation on data, which is used in, e.g., collective computation and 
    remote accumulates.

    **Memory management methods:**
    
    =================================================== ==================================================
    Method                                              Detail 
    =================================================== ==================================================
    default constructor                                 Not available.
    copy constructor |br| and ``operator=(const &)``    Defined; ``noexcept``.
    move constructor |br| and ``operator=(&&)``         Defined; ``noexcept``.
    =================================================== ==================================================

    .. type::   MPI_User_function user_fn_t

        User defined operation function that have signature 
        ``void (user_fn_t)(void *, void *, int *, MPI_Datatype *)``. 
    
    .. function::   Op( user_fn_t user_fn, int commute)

        Constructor - Construct an ``Op`` instance by an user defined function.

        ``user_fn`` specifies the user-defined operation.
        ``commute``  a Boolean value specifying whether the operator is commutative.

    
    .. function::   ostream & info( ostream &os = cout, int fmt_cntl=1 ) const
                    friend ostream & operator<<( ostream &os, const Op &op )

        Print some information about this instance into ``os``. 
        
        ``fmt_cntl`` controls the amount of information to print - 0 for a
        inline, short information, and 1 for a verbose version.

        ``operator<<`` is equivalent to ``info`` with default ``fmt_cntl``.
        
    
    
    .. function:: void free() noexcept

        Free the operator in advance, an set it to null value as returned by
        :func:`nullval`. It is valid to call it at any time, even multiple times
        on the same instance.

    
    
    .. function::   bool is_null() const
                    bool commutative() const
        
        Test the properties of ``Op``.

        ``is_null()`` tests whether the instance is a null value.
        
        ``commutative()`` tests whether the instance is commutative.

    
    
    .. function::   static Op nullval() noexcept
                    static Op create( user_fn_t user_fn, int commute )

        Create a new operation.
        
        ``nullval()``  returns a null value.

        ``create()``   creates a user-defined operator. 
        The argument is the same as 
        the constructor of Op.

Predefined Operations
"""""""""""""""""""""""""

.. var::    extern Op MAX 
            extern Op MIN
            extern Op SUM
            extern Op PROD
            extern Op LAND
            extern Op BAND
            extern Op LOR
            extern Op BOR
            extern Op LXOR
            extern Op BXOR
            extern Op MAXLOC
            extern Op MINLOC
            extern Op REPLACE
            extern Op NO_OP


Class Oppacket: the Operation Descriptor
"""""""""""""""""""""""""""""""""""""""""

.. class:: Oppacket

    :class:`Op` class provides full interface for data operations. However, 
    on most time, users are willing to use more elegant way to denote an operation
    in the communication.

    HIPP uses ``Oppacket`` to enable multiple ways of describing an operation.
    Just like you can use multiple ways to denote the same data buffer (with :class:`Datapacket`).

    .. function::   Oppacket( const Op &op )
                    Oppacket( const string &op )
                    Oppacket( const char *op )

        Use either an ``Op`` instance or a string to construct an operation.
        Allowed strings are:

        -   Arithematic:  ``"sum"`` (or ``"+"``), ``"prod"`` (or ``"*"``).
        -   Logic:        ``"land"`` (or ``"&&"``), ``"band"`` (or ``"&"``), ``"lor"`` (or ``"||"``),
            ``"bor"`` (or ``"|"``), ``"lxor"``, ``"bxor"`` (or ``"^"``).
        -   Min or max:   ``"max"``, ``"min"``, ``"maxloc"``, ``"minloc"``.
        -   ``"replace"`` (or ``"="``),  ``"no_op"``.

        

    
