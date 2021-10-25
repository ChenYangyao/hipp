Other Utilities
=================

.. include:: /global.rst

The following classes are all defined within namespace ``HIPP::IO::H5``.

.. namespace:: HIPP::IO::H5

Classes for Geometry
------------------------

Dimensions
""""""""""""""""

.. class:: Dimensions

    ``Dimensions`` describes the extent of the regular array dataspace or a 
    coordinate in it.

    .. type:: hsize_t value_t
        value_t * iterator
        const value_t * const_iterator

    .. function:: \    
        Dimensions() noexcept
        Dimensions(std::initializer_list<value_t> dims)
        template<typename T> \
            Dimensions(const T *p, size_t ndims)
        Dimensions(size_t n, value_t value = value_t {})
        template<typename Buff, \
            std::enable_if_t<ContiguousBufferTraits<Buff>::is_buffer, int> =0> \
        Dimensions(const Buff &dims)

        Constructors.

        (1): Default constructor - the instance is initialized to ``ndims = 0`` 
        state.
        
        (2): Specify the dimensions using a initializer_list.
        
        (3): Specify the dimensions using ``ndims`` values starting at ``p``. 
        
        (4): Specify the dimensions by a ContiguousBuffer object.

    .. function:: \
        Dimensions(const Dimensions &) = default
        Dimensions(Dimensions &&) = default
        Dimensions & operator=(const Dimensions &) = default
        Dimensions & operator=(Dimensions &&) = default
        ~Dimensions() = default

        ``Dimensions`` is copyable and movable. The copy operation is deep. The 
        move operation sets the move-from object an undefined by valid state.


    .. function:: \
        ostream & info(ostream &os=cout, int fmt_cntl=0) const
        friend ostream & operator<<(ostream &os, const Dimensions &dims)

        Print information of the instance to stream ``os``.

        ``fmt_cntl``: format controller. ``0`` for a inline, short message and ``1``
        for a verbose one.

        ``operator<<()`` is equivalent to ``info()`` with default format controller.

    .. function:: \
        Dimensions & set() noexcept
        Dimensions & set(std::initializer_list<value_t> dims)
        template<typename T> \
        Dimensions & set(const T *p, size_t n)
        template<typename Buff, \
            std::enable_if_t<ContiguousBufferTraits<Buff>::is_buffer, int> =0> \
        Dimensions & set(const Buff &dims)

        Set the dimensions of the current instance. Theses four overloads are 
        exactly corresponding to the four constructors.

    .. function:: \
        Dimensions & push_back(hsize_t x)
        Dimensions & push_back(std::initializer_list<value_t> dims)
        template<typename T> \
        Dimensions & push_back(const T *p, size_t n)
        template<typename Buff, \
            std::enable_if_t<ContiguousBufferTraits<Buff>::is_buffer, int> =0> \
        Dimensions & push_back(const Buff &dims)

        Extend the dimensions at tail.

        (1): by a single element.
        
        (2-4): by a series of elements.

    .. function:: \
        void resize(size_t new_ndims)

        Change the ndims of the instance. If enlarged, tail elements are 
        value-initialized. If shrinked, the tail elements are truncated.

    .. function:: \
        const value_t & operator[](size_t i) const
        value_t & operator[](size_t i)
        const value_t * data() const noexcept
        value_t * data() noexcept
        const_iterator begin() const noexcept
        const_iterator end() const noexcept
        iterator begin() noexcept
        iterator end() noexcept
        const_iterator cbegin() const noexcept
        const_iterator cend() const noexcept
        const vector<value_t> & as_vector() const noexcept
        vector<value_t> & as_vector() noexcept

        Data access methods.

        ``operator[]()``: visist a single dimension at axis ``i``.

        ``data()``: return the raw buffer of dimensions.
        
        ``begin()``, ``end()`` and their const-counterpart: return the iterators 
        of the dimensions.
        
        ``as_vector()`` returns a reference to the underlying vector that stores 
        the dimensions data.
        
    .. function:: \
        size_t ndims() const noexcept
        size_t size() const noexcept
        bool empty() const noexcept
        value_t n_elems() const noexcept

        Retrieve the properties of the instance.

        ``ndims()``: number of dimensions, i.e., rank.
        
        ``size()``: equal to ``ndims()``.
        
        ``empty()``: tests whether or not the instance has zero-rank.
        
        ``n_elems()``: total number of elements in the array, i.e., product of all
        dimensions.

    .. function:: \
        bool operator==(const Dimensions &) const noexcept
        bool operator!=(const Dimensions &) const noexcept

        Tells whether or not ndims are equal && all dimensions are equal.
    

.. type:: Point = Dimensions

    Represent the coordinate of a single point.

Points
""""""""""""""""

.. class:: Points 

    Represent the coordinates of a list of points.
    The instance only keeps a reference to the data whose life-time is maintained by
    the user.

    
    .. type:: hsize_t value_t

    .. function:: \
        Points() noexcept
        Points(size_t n_points, size_t n_dims, const value_t *coords) noexcept
        Points(size_t n_points, value_t *coords) noexcept

        Constructors.

        (1): 0 point.
        
        (2): ``n_points`` points in ``n_dims``-dimensional space. The coordinates
        of them is taken from a contiguous buffer starting at ``coords``.
        
        (3): the same as (2) but ``n_dims`` is set to ``0`` - to be determined from 
        other sources.
        
    .. function:: \
        const value_t * coords() const noexcept
        size_t n_points() const noexcept
        size_t n_dims() const noexcept

        Getters: retrieve the coordinates buffer address, number of points and 
        number of dimensions.

Chunk
""""""""""""""""

.. class:: Chunk
    
    Chunk represents a continuous chunk (i.e., a regular sub-region) of an array.
    
    .. function:: \
        Chunk() noexcept
        Chunk(const Dimensions &start, const Dimensions &count)
        template<typename T1, typename T2> \
        Chunk(const T1 *start, const T2 *count, size_t ndims)

        Constructors.

        (1): the start and count of the chunk are set to empty.
        
        (2): specify the start and count manually. They must be in equal length.
        
        (3): specify the start and count using two buffers, both sized ``ndims``.
        The instance deeply copy the data from these two buffers.

    .. function:: \
        const Dimensions & start() const
        const Dimensions & count() const
        Dimensions & start()
        Dimensions & count()

        Getters: retrieve the starting coordinates and counts along axes.
    
Hyperslab
""""""""""""""""

.. class:: Hyperslab
    
    Hyperslab represents multiple regular sub-regions with regular strides in 
    an array.

    .. type:: hsize_t value_t

    .. function:: \
        Hyperslab() noexcept
        explicit Hyperslab(const Chunk &chunk)
        Hyperslab(const Dimensions &start, const Dimensions &count)
        Hyperslab(const Dimensions &start, const Dimensions &count,  \
            const Dimensions &stride, const Dimensions &block)
        template<typename T1, typename T2> \
        Hyperslab(const T1 *start, const T2 *count, size_t ndims)
        template<typename T1, typename T2, typename T3, typename T4> \
        Hyperslab(const T1 *start, const T2 *count,  \
            const T3 *stride, const T4 *block, size_t ndims)   

        Constructors.

        (1): An empty hyperslab (i.e., start, count, stride, block are all 
        length-zero arrays).
        
        (2): Specifies only start and count, leaving stride and block empty.
        
        (3): Initialize all four fields. stride and block may be empty or array
        with equal length to other two arguments.
        
        (4,5): similar to (2,3) but using raw buffers to fill the start, count,
        stride, and block.
        In (5), stride or block may be NULL, indicating empty, or 
        otherwise interpreted as ``ndims`` elements in the buffer.
        The instance deeply copy the data from the buffers.
        
        Empty stride and block, when used in the selection of HDF5 hyperslabs, 
        are interpreted as ``1`` at all dimensions.
    
    .. function:: \
        static Hyperslab subsetting_1d(Dimensions full_dims, \
            size_t dim, const std::array<hsize_t, 4> &select_1d)

        Select a hyperslab by subsetting along the dimension ``dim``.
        
        ``full_dims``: dimensions of the parent space.
        ``select_1d```: start, count, stride, block at dimension ``dim``.

    .. function:: \
        const Dimensions & start() const noexcept
        const Dimensions & count() const noexcept
        const Dimensions & stride() const noexcept
        const Dimensions & block() const noexcept
        Dimensions & start() noexcept
        Dimensions & count() noexcept
        Dimensions & stride() noexcept
        Dimensions & block() noexcept

        Getters: retrieve the starting coordinates, counts, strides and block 
        lengths along axes.


    .. function:: \
        std::tuple<const value_t*, const value_t*, const value_t*, const value_t*> \
            data() const
        std::tuple<value_t*, value_t*, value_t*, value_t*> \
            data()

        Get the pointers to the underlying buffer of start, count, stride and 
        block.
        If stride or block is empty, the returned pointer for it is ``nullptr``.


Datapackets 
---------------

Datapacket
""""""""""""

.. class:: Datapacket 

    ``Datapacket`` provides unified mapping from C++ data object to HDF5 dataset
    specification (datatype and dataspace).

    .. function:: \
        Datapacket() noexcept
        Datapacket(void *_buff, Dataspace _dspace, Datatype _dtype) noexcept
        template<typename T> \
        Datapacket(T *_buff, Dataspace _dspace)
        template<typename T> \
        explicit Datapacket(T &x)

        Constructors
        
        (1): Default constructor: gives ``buff = nullptr``, default constructed
        ``dspace`` and ``dtype`` (i.e., empty objects).

        (2): Manually specify the buffer, dataspace and datatype.

        (3): The datatype is inferred from ``T``.

        (4): Inferred from a C++ object. The following resolving processes are 
        tried one-by-one until success. If eventually failing to resolve, generate
        a compiling error.
        
        - Any predefined numerical scalar type. (e.g., ``int``, ``float``)
        - A GeneralArray of predefined numerical scalar type. (e.g., ``int[3][4]``, 
          ``std::vector<double>`` )
        - A GeneralArray of RawArray of predefined numerical scalar type. 
          (e.g., ``std::vector< std::array<int, 3> >``).
        - Any other predefined type.
        - A GeneralArray of other predefined type.
        - A GeneralArray of RawArray of other predefined type.


    .. member:: \
        void *buff
        Dataspace dspace
        Datatype dtype

        The only three data members, allowing ``Datapacket`` be used by structured
        binding, e.g., ``auto [buff, dspace, dtype] = Datapacket{x}``.

    .. function:: \
        template<typename T, typename A> \
        static Datapacket resize_buff(vector<T, A> &v, size_t target_sz)

        Resize the buffer vector ``v`` into desired size so that it exactly 
        contains ``target_sz`` elements.

    .. function:: \
        std::tuple<void *, Dataspace, Datatype> to_tuple() const

        Convert the datapacket into a tuple object.

    
DatapacketScalar
""""""""""""""""""""""

.. class:: DatapacketScalar : public Datapacket

    ``DatapacketScalar`` behaves like ``Datapacket``, but can be initialized by 
    object whose datatype is interpreted as ATOMIC, i.e., directly mapped from 
    :expr:`Datatype::from_type`.
    
    .. function:: \
        DatapacketScalar() noexcept
        DatapacketScalar(void *_buff, Datatype _dtype) noexcept
        template<typename T> \
        explicit DatapacketScalar(T &x)

    
DatapacketStr
""""""""""""""""""""""
.. class:: DatapacketStr : public Datapacket 

    ``DatapacketStr`` behaves like ``Datapacket``, but can be initialized by 
    string or list of string object.
    
    .. function:: \
        DatapacketStr() noexcept
        DatapacketStr(void *_buff, Dataspace _dspace, \
            Datatype _dtype) noexcept
        explicit DatapacketStr(string &s)
        template<size_t N_STR> \
            explicit DatapacketStr(string (&s)[N_STR])
        DatapacketStr(string *s, size_t n_str)
        explicit DatapacketStr(vector<string> &s)
        explicit DatapacketStr(char *s)
        DatapacketStr(char *s, size_t n)
        template<size_t N_STR, size_t N> \
            explicit DatapacketStr(char (&s)[N_STR][N])
        template<size_t N> \
            DatapacketStr(char (*s)[N], size_t n_str)
        DatapacketStr(char **s, size_t n_str)


ConstDatapacket
""""""""""""""""""

.. class:: ConstDatapacket 
    
    The constant version of ``Datapacket``. The difference is that its constructors
    accept const object or const buffer, and that the data member ``buff`` is typed 
    with low-level const-qualifier.
    
    .. function:: \
        ConstDatapacket() noexcept
        ConstDatapacket(const void *_buff, Dataspace _dspace, \
            Datatype _dtype) noexcept
        template<typename T> \
        ConstDatapacket(const T *_buff, Dataspace _dspace)
        template<typename T> \
        explicit ConstDatapacket(const T &x)

    .. member:: \
        const void *buff
        Dataspace dspace
        Datatype dtype

    .. function:: \
        std::tuple<const void *, Dataspace, Datatype> to_tuple() const

ConstDatapacketScalar
""""""""""""""""""""""""""

.. class:: ConstDatapacketScalar : public ConstDatapacket
    
    The constant version of ``DatapacketScalar``.
    
    .. function:: \
        ConstDatapacketScalar() noexcept
        ConstDatapacketScalar(const void *_buff, Datatype _dtype) noexcept
        template<typename T> \
        explicit ConstDatapacketScalar(const T &x)

    
ConstDatapacketStr
"""""""""""""""""""""""""

.. class:: ConstDatapacketStr : public ConstDatapacket 

    The constant version of ``DatapacketStr``.

    .. function:: \
        ConstDatapacketStr() noexcept
        ConstDatapacketStr(const void *_buff, Dataspace _dspace,  \
            Datatype _dtype) noexcept
        explicit ConstDatapacketStr(const string &s)
        template<size_t N_STR> \
            explicit ConstDatapacketStr(const string (&s)[N_STR])
        ConstDatapacketStr(const string *s, size_t n_str)
        explicit ConstDatapacketStr(const vector<string> &s)
        explicit ConstDatapacketStr(const char *s)
        ConstDatapacketStr(const char *s, size_t n)
        template<size_t N_STR, size_t N> \
            explicit ConstDatapacketStr(const char (&s)[N_STR][N])
        template<size_t N> \
            ConstDatapacketStr(const char (*s)[N], size_t n_str)
        ConstDatapacketStr(const char * const *s, size_t n_str)

    .. function:: \
        void buff_to(const string *s, size_t n_str, vector<char> &b)
        void buff_to(const char * const *s, size_t n_str, vector<char> &b)

        Write the content of strings into a buffer ``b``. The buffer is resized
        to be able to contain ``n_str`` strings whose size is the maximum size of 
        the strings ``s`` (including the space for the null-terminator). 
        Then the strings are fill into them, strided by this size.
