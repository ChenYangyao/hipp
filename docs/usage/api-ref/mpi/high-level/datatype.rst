Datatype and Operation
=================================

.. include:: /global.rst

The following classes are all defined within namespace ``HIPP::MPI``.

.. namespace:: HIPP::MPI

Class Datatype
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



    .. type::   ContiguousBuffer<const int> int_buf_t
                ContiguousBuffer<const aint_t> aint_buf_t

        Contiguous buffer types that provide unified interfaces for all :class:`ContiguousBufferTraits` -comformable types.

        ``int_but_t``: contiguous buffer of constant integers. It is used, e.g., to specify the 
        the lengths and displacements of the chunks in a customized datatype.
        
        ``aint_buf_t``: contiguous buffer of constant address-size integers. It is used, e.g., 
        to specify the (byte-)displacements of chunks in a customized datatype.


    .. function::   ostream &info( ostream &os = cout, int fmt_cntl = 1 ) const
                    friend ostream & operator<<( ostream &os, const Datatype &dtype )

        ``info()`` prints a short (``fmt_cntl=0``) or a verbose (``fmt_cntl=1``) 
        description of the current instance to the stream ``os``.

        Operator ``<<`` is equivalent to ``info()`` with ``fmt_cntl=0``.

        The passed stream ``os`` is returned.
    
    .. function:: void free() noexcept

        Free the object and set it to null value as returned by :func:`nullval`.
        It is safe to call this function at any time, or even multiple times, 
        except for exactly the predefined objects (they are ``const``, but a 
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

        New datatype constructors.

        ``dup()`` creates a duplication of the current datatype.
        
        ``nullval()`` returns a null value, corresponding to ``MPI_DATATYPE_NULL`` internally.


    .. function::  \
        Datatype resized(aint_t lb, aint_t ext) const
        Datatype contiguous(int count) const
        Datatype vector(int count, int blklen, int stride) const
        Datatype hvector(int count, int blklen, aint_t stride) const
        Datatype indexed_block(int count, int blklen, const int displs[]) const
        Datatype indexed_block(int blklen, int_buf_t displs) const
        Datatype hindexed_block(int count, int blklen, const aint_t displs[]) const
        Datatype hindexed_block(int blklen, aint_buf_t displs) const
        Datatype indexed(int count, const int blklens[], const int displs[]) const
        Datatype indexed(int_buf_t blklens, int_buf_t displs) const
        Datatype hindexed(int count, const int blklens[], \
            const aint_t displs[]) const
        Datatype hindexed(int_buf_t blklens, aint_buf_t displs) const
        static Datatype struct_(int count, const int blklens[], \
            const aint_t displs[], const mpi_t dtypes[])
        static Datatype struct_(int count, const int blklens[], \
            const aint_t displs[], const Datatype dtypes[])
        static Datatype struct_(int_buf_t blklens, aint_buf_t displs, \
            ContiguousBuffer<const Datatype> dtypes)

        New datatype constructors. Assume the current instance describe a datatype
        ``T``:

        ``resized``: set the lower bound and extent to ``lb`` and ``ext``, respectively,
        and return the new datatype.
        
        ``contiguous``: contiguous-layout ``count`` items, each typed ``T``.
        
        ``vector``: a vector datatype that consists of ``counts`` blocks. Each block 
        has ``blklen`` contiguous items typed T. Blocks are separated by ``stride`` 
        (in the unit of ``T``).
        
        ``hvector``: similar to ``vector`` except that ``stride`` is in the unit of
        byte.

        ``indexed_block``: similar to ``vector`` but further allowing unequal strides
        specified by displacements ``displs`` (in the unit of T).

        ``hindexed_block``: similar to ``indexed_block`` except that displs is in the 
        unit of byte.

        ``indexed``: similar to ``indexed_block`` but allowing unequal sized blocks 
        specified by ``blklens``.

        ``hindexed``: similar to ``indexed`` except that ``displs`` are in the unit of 
        byte.

        ``struct_``: most general case. Blocks has different lengths, byte 
        displacements, and datatypes specified by ``dtypes``.

    .. function::   \
        Datatype darray(int size, int rank, int ndims, \
            const int gsizes[], const int distribs[], \
            const int dargs[], const int psizes[], int order = ORDER_C)const
        Datatype darray(int size, int rank, int_buf_t gsizes, int_buf_t distribs, \
            int_buf_t dargs, int_buf_t psizes, int order = ORDER_C)const
        Datatype subarray(int ndims, const int sizes[], const int subsizes[], \
            const int starts[], int order = ORDER_C)const
        Datatype subarray(int_buf_t sizes, int_buf_t subsizes, int_buf_t starts, \
            int order = ORDER_C)const

        For the convenience of distributing array data among processes, the 
        following two datatypes are defined:

        ``darray``: Cartesian decompose the array type T into blocks, return the 
        datatype that corresponds to one of them.

        - ``size, rank``: total number of blocks and the index of the desired block
          (assuming row-major order).
        - ``gsizes``: dimensions of the whole array.
        - ``distribs``: specifying the distribution algorithm at dimensions. Each can be
        
            - ``DISTRIBUTE_NONE`` (the first block owns all, the others get none).
            - ``DISTRIBUTE_BLOCK`` (each block is contiguous).
            - ``DISTRIBUTE_CYCLIC`` (each block cyclicly owns elements).
        
        - ``dargs``: arguments to the distribution algorithm. Each can be
            
            - ``DISTRIBUTE_DFLT_DARG``: most uniform way for BLOCK distribution, 
              and 1 for CYCLIC.
            - an integer: size of the block (must has darg * psize >= gsize at this
              dimension) for BLOCK distribution, or the size each cyclic chunk.
        
        - ``psizes``: number of blocks at dimensions. ``1`` for non-distribution at 
          this dimension.

        ``subarray``: a subarray datatype in a larger array typed T.
        
        ``sizes``: dimensions of the whole array.
        ``subsizes, starts``: dimensions at starting location of the subarray.
        ``order``: ``ORDER_C`` or ``ORDER_FORTRAN``, the storage order.

    
    .. function:: \
        static void add_customized_cache(Datatype *dtype)
        static void clear_customized_cache() noexcept

        ``add_customized_cache``: 
        add a user-defined datatype to the cache, so that it is freed at the exit of 
        the MPI environment.
        
        ``clear_customized_cache``: clean the cache, so that all datatypes gets 
        freed (only if all the copies of the ``Datatype`` instance gets destroyed
        will the underlying datatype get freed).

    .. function:: \
        static void add_named_datatype(const string &name, const Datatype *dtype)
        static void remove_named_datatype(const string &name)

        Add or remove a named datatype, so that the method :func:`from_name` accepts
        that name.

    .. function:: \
        template<typename NativeT> \
        static const Datatype & from_type() noexcept

        Get the Datatype instance from its C++ type. 

        ``NativeT``: a :class:`DatatypeTraits` -comformable type, i.e., ``DatatypeTraits<NativeT>::has_mpi_datatype -> true``.

        User may extend the traits by adding specializations.

    .. function:: \
        static const Datatype & from_name(const string &name)

        Map string to Datatype instance.
    
        ``name``: a string, could be one of the following:

        (1). ``byte``, ``char``, ``bool``; 
        
        (2). ``X``, ``signed X``, ``unsigned X`` where X is either char, short, int, long, or long long;
        
        (3). ``intX_t``, ``uintX_t`` where X can be either 8, 16, 32, or 64;
        
        (4). ``float``, ``double``, ``long double``.

        Other named datatype are allowed if manually added, e.g., with 
        ``add_named_datatype()``.


.. _api-mpi-predefined-dtype:

Predefined Datatypes 
""""""""""""""""""""""""""""""

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


DatatypeTraits
""""""""""""""""""

:class:`~HIPP::MPI::DatatypeTraits` is defined for mapping a C++ type to its MPI Datatype. 
``DatatypeTraits`` defines the 
Datatype protocol.

If a C++ type, ``T``, has specialization ``DatatypeTraits<T>``, and satisfies
the following requirements, it is compliant to this protocol.

(1). Has a member ``static constexpr bool has_mpi_datatype = true``.

(2). Has a member constexpr bool value, ``is_prededefined``. If ``T`` has a 
predefined standard MPI datatype, ``is_predefined=true``. Otherwise 
``false``. Any customized datatype, defined by HIPP or by the user, 
should define this to ``false``.

(3). Have a member typedef ``native_t=T``.

(4). If predefined, has a member ``static const Datatype * datatype`` which 
points to a ``Datatype`` instance. Otherwise has a method 
``static const Datatype & datatype()`` which returns the reference to 
a ``Datatype`` instance.

(5). If predefined, has a member ``static const char * mpi_name`` which points
to an name to the datatype. Otherwise has a method 
``static string mpi_name()`` which returns to a string of the datatype name.

To satisfy (1) and (2), the best choice is to define the specialization to 
derive from :class:`~HIPP::MPI::DatatypeTraitsCustomized`.

The requirement (5) is only for debugging purpose. It is valid to return an 
empty string.

.. class DatatypeTraitsPredefined::

    static constexpr bool has_mpi_datatype = true
    static constexpr bool is_predefined = true


.. class DatatypeTraitsCustomized::

    static constexpr bool has_mpi_datatype = true
    static constexpr bool is_predefined = false



.. class:: template<typename NativeT, typename V =void> DatatypeTraits

    inline static constexpr bool has_mpi_datatype = false

The followings are specializations for C++ native types that has predefined MPI datatype 
counterpart. 

Supported types are

- ``bool``, ``char``, ``signed char`` or ``unsigned char``;
- ``X`` or  ``unsigned X``, where ``X`` can be ``short``, ``int``, ``long`` 
  or ``long long``;
- ``float``, ``double`` or ``long double``;
- ``std::complex<float>``, ``std::complex<double>`` 
  or ``std::complex<long double>``.

Their const qualified types are also supported, referring to the same Datatype object.

.. class:: template<> DatatypeTraits<char >

.. class:: template<> DatatypeTraits<signed char >

.. class:: template<> DatatypeTraits<short >

.. class:: template<> DatatypeTraits<int >

.. class:: template<> DatatypeTraits<long >

.. class:: template<> DatatypeTraits<long long >

.. class:: template<> DatatypeTraits<unsigned char >

.. class:: template<> DatatypeTraits<unsigned short >

.. class:: template<> DatatypeTraits<unsigned int >

.. class:: template<> DatatypeTraits<unsigned long >

.. class:: template<> DatatypeTraits<unsigned long long >

.. class:: template<> DatatypeTraits<float >

.. class:: template<> DatatypeTraits<double >

.. class:: template<> DatatypeTraits<long double >

.. class:: template<> DatatypeTraits<bool >

.. class:: template<> DatatypeTraits<std::complex<float> >

.. class:: template<> DatatypeTraits<std::complex<double> >

.. class:: template<> DatatypeTraits<std::complex<long double> >

.. class:: template<> DatatypeTraits<const char >

.. class:: template<> DatatypeTraits<const signed char >

.. class:: template<> DatatypeTraits<const short >

.. class:: template<> DatatypeTraits<const int >

.. class:: template<> DatatypeTraits<const long >

.. class:: template<> DatatypeTraits<const long long >

.. class:: template<> DatatypeTraits<const unsigned char >

.. class:: template<> DatatypeTraits<const unsigned short >

.. class:: template<> DatatypeTraits<const unsigned int >

.. class:: template<> DatatypeTraits<const unsigned long >

.. class:: template<> DatatypeTraits<const unsigned long long >

.. class:: template<> DatatypeTraits<const float >

.. class:: template<> DatatypeTraits<const double >

.. class:: template<> DatatypeTraits<const long double >

.. class:: template<> DatatypeTraits<const bool >

.. class:: template<> DatatypeTraits<const std::complex<float> >

.. class:: template<> DatatypeTraits<const std::complex<double> >

.. class:: template<> DatatypeTraits<const std::complex<long double> >

.. class:: template<typename NativeT> DatatypeTraits<NativeT, std::enable_if_t< \
    RawArrayTraits<NativeT>::is_array && \
    DatatypeTraits<typename RawArrayTraits<NativeT>::value_t>::has_mpi_datatype > >  \
    : public DatatypeTraitsCustomized

    Specialization for any RawArray protocol (see :class:`~HIPP::RawArrayTraits`).

    A RawArray type with scalar type ``value_t`` and number of elements ``size``
    is mapped to the MPI contiguous datatype sized ``size`` with element type 
    ``value_t``.

DatatypeConverter
""""""""""""""""""

The followings are the interfaces of the type mapping. 
Do not add specialization to this class template.
Instead, define specialization to ``DatatypeTraits``.

.. class:: template<typename NativeT, typename V> TypeCvt
    
    .. member:: static constexpr bool has_mpi_datatype = false

.. class:: template<typename NativeT> TypeCvt<NativeT, std::enable_if_t<DatatypeTraits<NativeT>::is_predefined> > 

    .. member:: static constexpr bool has_mpi_datatype = true

    .. type:: DatatypeTraits<NativeT> traits_t
        typename traits_t::native_t native_t
    
    .. function:: static const Datatype & datatype() noexcept
        static string mpi_name()

.. class:: template<typename NativeT> TypeCvt<NativeT, std::enable_if_t< ! DatatypeTraits<NativeT>::is_predefined> > 

    .. member:: static constexpr bool has_mpi_datatype = true
    
    .. type:: DatatypeTraits<NativeT> traits_t
        typename traits_t::native_t native_t

    .. function:: static const Datatype & datatype()
        static string mpi_name()


Class Datapacket
----------------------------------------

A datapacket is defined, in the high-level interface, as a pack of the 
standard MPI data buffer specification, i.e., 
``{buff_addr, buff_size, datatype}``.

Datapacket gives the communications calls flexibility, in that multiple types
of objects can be directly passed in as buffer.

The Datapacket type has tuple-like API for structured binding, e.g., ::

    std::vector<int> v {1,2,3};
    auto [buff, size, datatype] = Datapacket(v);

Here, buff, size, datatype are reference type to ``void *``, ``int`` and ``Datatype``, 
respectively.

The const variant, ``ConstDatapacket`` refers to a constant buffer, (i.e., the
buff_addr is a pointer to constant buffer). The non-const variant, 
``Datapacket`` refers the non-const buffer. They are the same except this 
difference.

.. class::  Datapacket 

    **Memory management methods:**
    
    .. table::
        :class: tight-table
        
        =================================================== =================================================================================
        Method                                              Detail 
        =================================================== =================================================================================
        copy constructor |br| and ``operator=(&&)``         Defined; ``noexcept``. The result refers to the same data buffer.
        move constructor |br| and ``operator=(const &)``    Defined; ``noexcept``. The result takes over the move-from buffer-specification.
        ``~Datapacket``                                     ``noexcept``
        =================================================== =================================================================================

    .. _api-mpi-dpacket-constructor:

    .. function::   \
            Datapacket() noexcept 
            Datapacket(void *buff, int size, Datatype dtype) noexcept
            Datapacket(void *buff, int size, const string &dtype)
            Datapacket(string &s) noexcept
            template<typename T, std::enable_if_t<_has_def<T>(), int> =0 > \
                Datapacket(T *buff, int n) noexcept
            template<typename T> \
                Datapacket(T &x)

        Constructors.

        (1). Default constructor - equivalent to (2) with arguments 
        ``{BOTTOM, 0, INT}``.
        
        (2). MPI standard buffer specification, i.e., with a triplet of starting 
        address, number of data items, and datatype of each item.
        
        (3). The same as (2), but specify the datatype by its name. The actual used
        datatype is like that returned by :func:`Datatype::from_name`. Ordinary names are 
        supported, e.g., ``char``, ``int``, ``unsigned short``, ``float``, ``int16_t``, ``bool``, etc.
        
        (4). Equivalent to (2) with arguments ``{s.data(), s.size(), CHAR}``.

        (5). The same as (2), but the datatype is inferred from the pointed type 
        ``T``. The actual used datatype is like that return by :func:`Datatype::from_type`.
        Ordinary types are suppoeted, like ``char``, ``int``, ``unsigned short``, ``float``, ``bool``, 
        etc. Any types that are DatatypeTraits-conformable are also supported (see class :class:`DatatypeTraits` for the protocol definition).
        
        (6). The buffer is inferred from the argument. The library tries to infer
        the buffer by the following order until success:

        - If ``T`` is Predefined DatatypeTraits-conformable, treat ``x`` as a single 
          data element. Examples include a single int, double, etc.
        - If ``T`` is ContiguousBufferTraits-conformable 
          (see class :class:`ContiguousBufferTraits` for the protocol definition) and its element is 
          Predefined DatatypeTraits-conformable, treat ``x`` as a sequence of 
          data elements typed ContiguousBuffer<T>::value_t.
          e.g., ``vector<int> v{1,2,3,4,5}`` gives the buffer 
          ``{v.data(), 5, INT}``.
        - If ``T`` is Customized DatatypeTraits-conformable, treat ``x`` as a single 
          data element.
        - If T is ContiguousBufferTraits-conformable and its element is Customized 
          DatatypeTraits-conformable, treat ``x`` as a sequence of 
          data elements typed ContiguousBuffer<T>::value_t.
        - If all the above inferences failed, raise a compile error.
        
        Note that in any of the constructors, the data buffer must be non-const.
    
    .. function::   Datapacket(aint_t disp, int size, Datatype dtype) noexcept
                    Datapacket(aint_t disp, int size, const string &dtype)

        Sometimes, triplet is used to specify a memory segment relative to 
        a base address (e.g., in RMA operations). In such cases, the first 
        part of the triplet is a displacement relative to the base address. 
        
        The ``Datapacket`` type can represent such triplets, too. Internally, the
        displacement is stored by casting into a ``void *``.

    .. function:: \
        ostream &info(ostream &os = cout, int fmt_cntl = 1) const
        friend ostream & operator<<(ostream &os, const Datapacket &dpacket)

        ``info()`` prints short (``fmt_cntl=0``) or verbose (``fmt_cntl=1``) information 
        about the instance to
        the stream ``os``. 
        
        The ``<<`` operator is equivalent to ``info()`` with ``fmt_cntl=0``.

    .. function:: \
        void * const & get_buff() const noexcept
        int const & get_size() const noexcept
        Datatype const & get_dtype() const noexcept
        void * & get_buff() noexcept
        int & get_size() noexcept
        Datatype & get_dtype() noexcept

        Getters.
        
        ``get_buff()``, ``get_size()``, ``get_dtype()`` return the buffer starting address,
        buffer size, and datatype, respectively.

Tuple-like API are defined for ``Datapacket``:

.. function:: \
    template<std::size_t I> decltype(auto) get( Datapacket &dp )
    template<std::size_t I> decltype(auto) get( const Datapacket &dp )
    template<std::size_t I> decltype(auto) get( Datapacket &&dp )

.. class:: template<> std::tuple_size<Datapacket>
    
    .. member:: static constexpr std::size_t value = 3

.. class:: template<> std::tuple_element<0, Datapacket>

    .. type:: type = void *

.. class:: template<> std::tuple_element<1, HIPP::MPI::Datapacket>
    
    .. type:: type = int

.. class:: template<> std::tuple_element<2, HIPP::MPI::Datapacket>

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


Class ConstDatapacket
----------------------------------------

.. class:: ConstDatapacket

    See the docs for Datapacket.


    .. function:: 
        ConstDatapacket() noexcept
        ConstDatapacket(const void *buff, int size, Datatype dtype) noexcept
        ConstDatapacket(const void *buff, int size, const string &dtype)
        ConstDatapacket(const string &s) noexcept
        template<typename T, std::enable_if_t<_has_def<T>(), int> =0 > \
            ConstDatapacket(const T *buff, int n) noexcept
        template<typename T> \
            ConstDatapacket(const T &x)

    .. function:: \
        ConstDatapacket(aint_t disp, int size, Datatype dtype) noexcept 
        ConstDatapacket(aint_t disp, int size, const string &dtype)

    .. function:: \
        ConstDatapacket(const Datapacket &p) noexcept

        It is valid to use a ``Datapacket`` as a ``ConstDatapacket``. The 
        constructed ``ConstDatapacket`` refers to the same buffer, has the same
        size, and share the same datatype as ``p``.

    .. function:: \
        ostream &info(ostream &os = cout, int fmt_cntl = 1) const
        friend ostream & operator<<(ostream &os, const ConstDatapacket &dpacket)

    .. function:: \
        const void * const & get_buff() const noexcept
        int const & get_size() const noexcept
        Datatype const & get_dtype() const noexcept
        const void * & get_buff() noexcept
        int & get_size() noexcept
        Datatype & get_dtype() noexcept

Tuple-like API are defined for ``ConstDatapacket``:

.. function:: \
    template<std::size_t I> decltype(auto) get( ConstDatapacket &dp )
    template<std::size_t I> decltype(auto) get( const ConstDatapacket &dp )
    template<std::size_t I> decltype(auto) get( ConstDatapacket &&dp )

.. class:: template<> std::tuple_size<ConstDatapacket>
    
    .. member:: static constexpr std::size_t value = 3

.. class:: template<> std::tuple_element<0, ConstDatapacket>

    .. type:: type = const void *

.. class:: template<> std::tuple_element<1, HIPP::MPI::ConstDatapacket>
    
    .. type:: type = int

.. class:: template<> std::tuple_element<2, HIPP::MPI::ConstDatapacket>

    .. type:: type = Datatype


Class Pack
------------------------

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
        Pack & push(const ConstDatapacket &in_dpacket)
        Pack & pop(const Datapacket &out_dpacket)

        ``push()`` packs ``in_dpacket`` into the buffer. It automatically resizes the
        buffer if memory is not enough to hold ``in_dpacket``. The position pointer
        is increased after ``push()``.
        
        ``pop()`` perfoms oppisite operation - unpack from the current position 
        to ``out_dpacket``, and move the position pointer forward.

        ``*this`` is returned which enables chain of methods on the same instance.

    .. function:: \
        ConstDatapacket as_sendbuf() const
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
        static void pack(const ConstDatapacket &in_dpacket, 
            void *outbuf, int outsize, int &position, const Comm &comm)
        static void unpack(const void *inbuf, int insize, int &position, \
            const Datapacket &out_dpacket, const Comm &comm)
        static int size(int incount, const Datatype &dtype, const Comm &comm)
        static int size(const ConstDatapacket &in_dpacket, const Comm &comm)

        The functions mapped to the standard MPI calls.

        ``pack()`` packs data described by ``in_dpacket`` to the output buffer ``outbuf`` sized ``outsize`` at 
        the position ``position``. The buffer will be used in the communication in the commnicator ``comm``.
        ``position`` is updated to the next location that can be used for the next packing operation.

        ``upack()`` upacks data in the buffer ``inbuf`` into the user buffer described by ``out_dpacket``.

        ``size()`` returns the maximal buffer size in byte that may be used in ``pack()``.

Class ExternalPack
------------------------

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
        ExternalPack & push(const ConstDatapacket &in_dpacket)
        ExternalPack & pop(const Datapacket &out_dpacket)

        Pack/unpack data into/from the current buffer.

    .. function:: \
        ConstDatapacket as_sendbuf() const
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
        static void pack(const ConstDatapacket &in_dpacket, \
            void *outbuf, aint_t outsize, aint_t &position, \ 
            const string &datarep="external32")
        static void unpack(const void *inbuf, aint_t insize, \
            aint_t &position, const Datapacket &out_dpacket, \
            const string &datarep="external32")
        static aint_t size(int incount, const Datatype &dtype,  \
            const string &datarep="external32")
        static aint_t size(const ConstDatapacket &in_dpacket, \
            const string &datarep="external32")

        Methods mapped to the standard MPI calls.


Class Op
-------------

.. class::  Op

    ``Op`` describes the operation on data, which is used in, e.g., collective computation and 
    remote accumulates.

    **Memory management methods:**
    
    .. table::
        :class: tight-table
    
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
""""""""""""""""""""""""""""

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


Class Oppacket
-----------------

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

        

    
