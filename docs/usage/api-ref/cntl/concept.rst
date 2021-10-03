.. _api-cntl-concept:

The Concept API
================

.. include:: /global.rst

The following variables, functions and classes are all defined within the namespace ``HIPP``.

.. namespace:: HIPP

.. _api-cntl-concept-raw-array:

The RawArray Concept
-----------------------

RawArrayHelper
""""""""""""""""

.. class:: RawArrayHelper

    Helpers for the raw array traits.

    .. function:: template<typename ArrayT> \
        static constexpr std::array<size_t, std::rank_v<ArrayT> > extents()
        
        Determines the extents of a raw array. For example::
            
            extents<int [3][4]>();      // => std::array {3,4};

    .. function:: template<typename ArrayT> \
        static constexpr std::array<size_t, std::rank_v<ArrayT> > strides()
    
        Find the strides of a raw array. For example::
        
            strides<int [2][3][4]>();   // => std::array {12, 4, 1}

    .. function:: \
        template<typename ArrayT> static constexpr size_t size()

        Determine the size, i.e., total number of elements in the raw array. 
        For example::

            size<int [3][4]>();         // => 12
    
    .. function:: \
        template<typename ArrayT> static ArrayT & get_elem(ArrayT &a)
        template<typename ArrayT, typename ...SizeTs> \
        static std::remove_all_extents_t<ArrayT> & get_elem(ArrayT &a, size_t id1, SizeTs &&...ids)

        Get an element from a raw array typed object. For scalar, return the reference
        to itself. For example::

            int a[3][4];
            get_elem(a, 0, 1);          // => a[0][1], typed int &

            int b;
            get_elem(b);                // => the reference to b itself
    
    .. class:: template<typename T, size_t ...Ds> extents_to_array
    
    .. type:: template<typename T, size_t ...Ds> \
            extents_to_array_t = typename extents_to_array<T, Ds...>::type

        Get raw array type from from the extents list. Without any extent, 
        return the type ``T`` itself.
        For example::

            extents_to_array_t<int,3,4,5>;      // => int[3][4][5]
            extents_to_array_t<int>;            // => int


    .. class:: template<typename T, typename _V = void> is_std_array

    .. member:: template<typename T> \
        static constexpr bool is_std_array_v = is_std_array<T>::value

        Determine if ``T`` is a ``std::array``. For example::
        
            is_std_array_v<std::array< std::array<int, 3>, 4 >>;    // => true
            is_std_array_v<int [3]>;                                // => false
    
    .. class:: template<typename T, typename _V=void> std_array_to_raw
    
    .. type:: template<typename T> \
            std_array_to_raw_t = typename std_array_to_raw<T>::type

        Find the raw array type corresponding to a ``std::array``. For example::

            std_array_to_raw_t< std::array< std::array<double, 4>, 3 > >;    // => double [3][4]

        If std::array is ``const`` (in any nested depth), the raw array is also const. For example::
    
            std::array< std::array<const double, 4>, 3 >    // => const double[3][4].
            std::array<const std::array<double, 4>, 3 >     // => const double[3][4].
            const std::array<std::array<double, 4>, 3 >     // => const double[3][4].


RawArrayTraits 
""""""""""""""""""""

RawArrayTraits gives features for a raw-array-like type.
User may add specializations to this generic class.

For example::

    RawArrayTraits<int [3][4]>::extents;                                    // => std::array{3,4}.
    RawArrayTraits< std::array<std::array<double, 4>, 3> >::extents;        // => std::array{3,4}.

If an array has ``const`` value, or the array itself is ``const``, the ``value_t``, 
i.e., type of the array element, is also ``const``. e.g.::

    RawArrayTraits<const int [3][4]>::value_t             // => const int
    RawArrayTraits<std::array<const int, 3> >::value_t    // => const int
    RawArrayTraits<const std::array<int, 3> >::value_t    // => const int
    RawArrayTraits<const std::array<int, 3> >::array_t    // => const int [3]

.. class:: template<typename T, typename V=void> RawArrayTraits
    
    .. member:: static constexpr bool is_array = false
    .. type:: T value_t


.. class:: template<typename RawArrayT> RawArrayTraits < RawArrayT >

    Specialization for a raw array type.

    .. type:: RawArrayT array_t
        std::remove_all_extents_t<array_t> value_t

        The raw array type and its scalar type.

    .. member:: static constexpr bool is_array = true
        static constexpr size_t rank = std::rank_v<array_t>
        static constexpr size_t size = RawArrayHelper::size<array_t>()
        static constexpr std::array<size_t, rank> extents = RawArrayHelper::extents<array_t>()
        static constexpr std::array<size_t, rank> strides = RawArrayHelper::strides<array_t>()

        ``is_array`` tells whether or not the template parameter is an raw-array-like type.
        If true, ``rank``, ``size``, ``extents``, and ``strides`` gives its details.

    .. function:: ostream & info(ostream &os=cout, int fmt_cntl=1) const
        friend ostream & operator<< (ostream &os, const RawArrayTraits &t)

        Print the traits detail.
        
        :arg fmt_cntl: ``0`` for a inline message, ``1`` for a long and verbose block message.

        ``operator<<`` prints a inline message.

.. class:: template<typename T, size_t N> RawArrayTraits< std::array<T, N> >

.. class:: template<typename T, size_t N> RawArrayTraits< const std::array<T, N> > 

    Specialization for the ``std::array`` and its const version. 
    It is defined according to the corresponding raw-array type.

.. _api-cntl-concept-contiguous-buffer:

The ContiguousBuffer Concept
-------------------------------

ContiguousBufferTraits
""""""""""""""""""""""""

.. class:: template<typename T, typename V=void> ContiguousBufferTraits

    .. member:: static constexpr bool is_buffer = false
    
    .. type:: T buffer_t


    ContiguousBuffer protocol. 

    For any type T that does not satisfying this protocol, member ``is_buffer`` is 
    false and ``buffer_t`` is ``T`` itself.

    Otherwise, the following members must be defined for the specialization of
    ``ContiguousBufferTraits``:

    - ``is_buffer``: ``true``.
    - ``is_const``: whether or not the buffer is constant.
    - ``value_t`` and ``cvalue_t``:  type of the element in the buffer, and its 
      const counterpart. If ``is_const``, ``cvalue_t`` equals to ``value_t``.
    - ``buffer_t``: the type of the object that hosts the buffer.

    Template argument V is not significant but reserved for type matching.

    The traits type must be default-constructable, resulting in a ``size == 0`` 
    buffer.

    The traits object can be constructed by passing a reference to the 
    buffer-host object.

    ContiguousBufferTraits is copy-constructable and copy-assignable, resulting
    in object that refers to the same buffer.

    The traits object has methods ``get_buff()``, ``get_cbuff()`` and ``get_size()``
    which returns the buffer pointer, const buffer pointer and size (i.e., number 
    of elements) of the buffer, respectively. Structured binding 
    ``cv-ref auto [buff, size]`` is allowed.

    ``begin()``, ``end()``, ``cbegin()``, ``cend()`` are defined to iterate over
    the referred buffer. They all returns raw pointer typed ``value_t *`` or 
    ``cvalue_t``.

    A special case is ``T = ValueT *`` or ``T = const ValueT *``, i.e., the 
    buffer-host type is a pointer. In this case, the specialization class is used.

    Predefined contiguous-buffer-compliant types include any RawArray-compliant 
    type (i.e., C-style raw array, std::array) and std::vector.



.. class:: template<typename ValueT> ContiguousBufferTraits< const ValueT * >
    
    Specialization for const raw pointer.
    
    Template parameters:

    - ``ValueT``: value of the element in the buffer.

    **Examples:**

    .. code-block:: cpp

        float a1[4];
        const float a2[3];
        ContiguousBufferTraits<const float *> cbt1 {a1, 4}, cbt21 {a2, 3};
        ContiguousBufferTraits cbt22 {a2, 3};           // deduced <const float *>



    .. member:: static constexpr bool is_buffer = true
                static constexpr bool is_const = true

    .. type::   const ValueT *buffer_t
                const ValueT value_t
                value_t cvalue_t

    .. function:: \
        constexpr ContiguousBufferTraits(value_t *p, size_t n) noexcept
        constexpr ContiguousBufferTraits() noexcept

        Constructors

        - ``p``: pointer to the first element in the buffer.
        - ``n``: size of the buffer (i.e., number of elements).

        Default constructor: equivalent to ``p=nullptr; n=0``.

    .. function:: \
        constexpr value_t * get_buff() const noexcept
        constexpr cvalue_t * get_cbuff() const noexcept
        constexpr size_t get_size() const noexcept
        constexpr value_t * begin() const noexcept
        constexpr value_t * end() const noexcept
        constexpr cvalue_t * cbegin() const noexcept
        constexpr cvalue_t * cend() const noexcept

    .. function:: \
        friend ostream & operator<<(ostream &os, const ContiguousBufferTraits &cbt) 

    .. member::  \
        value_t *buff
        size_t size



.. class:: template<typename ValueT> ContiguousBufferTraits< ValueT * >


    Specialization for raw pointer.

    Template parameters:
    
    - ``ValueT``: value of the element in the buffer.

    **Examples:**

    .. code-block:: cpp

        float a1[4];
        const float a2[3];
        ContiguousBufferTraits<float *> cbt11 {a1, 4},    // OK
            cbt2{a2, 4};                                  // ERROR, compile failure
        ContiguousBufferTraits cbt12 {a1, 4};             // deduced <float *>


    .. member:: static constexpr bool is_buffer = true
                static constexpr bool is_const = false
    
    .. type:: \
        ValueT *buffer_t
        ValueT value_t
        std::add_const_t<value_t> cvalue_t
    
    .. function:: \
        constexpr ContiguousBufferTraits(value_t *p, size_t n) noexcept
        constexpr ContiguousBufferTraits() noexcept
        
        Constructors
        
        - ``p``: pointer to the first element in the buffer.
        - ``n``: size of the buffer (i.e., number of elements).

        Default constructor: equivalent to ``p=nullptr; n=0``.

    .. function:: \
        constexpr value_t * get_buff() const noexcept
        constexpr cvalue_t * get_cbuff() const noexcept
        constexpr size_t get_size() const noexcept
        constexpr value_t * begin() const noexcept
        constexpr value_t * end() const noexcept
        constexpr cvalue_t * cbegin() const noexcept
        constexpr cvalue_t * cend() const noexcept

    .. function:: \
        friend ostream & operator<<(ostream &os, const ContiguousBufferTraits &cbt) 
    

    .. member:: value_t *buff
        size_t size

.. class:: template<typename RawArrayT> ContiguousBufferTraits< RawArrayT, std::enable_if_t<RawArrayTraits<RawArrayT>::is_array> > : ContiguousBufferTraits< typename RawArrayTraits<RawArrayT>::value_t * >

    Specialization for raw array concept (include C-style raw array, std::array, 
    etc. ). In these case, they are treated as row-major storage 1D buffer.

    For example::

        std::array<std::array<double, 3>, 2> arr1 {};
        const std::array<double, 3> arr2 {};

        ContiguousBufferTraits<decltype(arr1)> cbt1{arr1};  
                                                // explicit set the template argument
        ContiguousBufferTraits cbr2{arr2};      // auto deduced

    .. type:: \
        RawArrayTraits<RawArrayT> _traits_t
        ContiguousBufferTraits< typename _traits_t::value_t * > _parent_t
        RawArrayT buffer_t
        _parent_t::value_t

    .. function:: \
        constexpr ContiguousBufferTraits( buffer_t &arr ) noexcept



.. class:: template<typename ValueT, typename Allocator> ContiguousBufferTraits< const vector<ValueT, Allocator> > : ContiguousBufferTraits< const ValueT * >

    .. type:: \
        ContiguousBufferTraits< const ValueT * > _parent_t
        const vector<ValueT, Allocator> buffer_t

    .. function:: \
        constexpr ContiguousBufferTraits( buffer_t &vec ) noexcept
        


.. class:: template<typename ValueT, typename Allocator> ContiguousBufferTraits< vector<ValueT, Allocator> > : ContiguousBufferTraits< ValueT * >

    Specialization for std::vector. For any vector v, v.data(), v.size() are used
    as buffer specification. 

    For example::

        const vector<int> a1{1,2,3};
        vector<int> a2{1,2,3};

        ContiguousBufferTraits<vector<int> > cbt1 {a1}; 
                                                // explicit set the template argument
        ContiguousBufferTraits cbt2 {a2};       // auto deduced

    .. type:: \
        ContiguousBufferTraits< ValueT * > _parent_t
        vector<ValueT, Allocator> buffer_t

    .. function:: \
        constexpr ContiguousBufferTraits( buffer_t &vec ) noexcept
    
ContiguousBuffer
""""""""""""""""""""

.. class:: template<typename ValueT> ContiguousBuffer 

    ContiguousBuffer object refers a buffer of element type ``ValueT``.
    Structured binding ``cv-ref auto [buff, size]`` is allowed.

    ContiguousBuffer is copy-constructable and copy-assignable, resulting in 
    object referring to the same buffer.

    .. type:: template<typename T> traits_t = ContiguousBufferTraits<T>

    .. type:: \
        ValueT value_t
        std::add_const_t<value_t> cvalue_t

        Value type of the buffer element, and its const counterpart.

    .. function:: constexpr ContiguousBuffer()
        template<typename T> constexpr ContiguousBuffer(T *p, size_t n) noexcept
        template<typename T> ContiguousBuffer( T &x )

        Constructors.
        
        (1) Default constructor - an empty buffer as initialized with (2) by passing 
        ``p=nullptr, n=0``.
        
        (2) by passing a pointer ``p`` to the buffer starting address and a size 
        ``n`` indicating the number of elements.
        
        (3) by any other ContiguousBuffer protocol compliant object typed ``T``, 
        i.e., ``ContiguousBufferTraits<T>::is_buffer == true``. ``T`` can also
        be a ContiguousBuffer with proper ``value_t``, e.g., copy-construct
        a ``ContiguousBuffer<const int>`` from ``ContiguousBuffer<int>`` is allowed.

    .. function:: template<typename T>  constexpr ContiguousBuffer & operator=(const ContiguousBuffer<T> &cb) noexcept 
    
    .. function:: \
        constexpr value_t * get_buff() const noexcept
        constexpr cvalue_t * get_cbuff() const noexcept
        constexpr size_t get_size() const noexcept

        Return the buffer starting pointer and the number of elements.
    

    .. function:: \
        constexpr value_t * begin() const noexcept
        constexpr value_t * end() const noexcept
        constexpr cvalue_t * cbegin() const noexcept
        constexpr cvalue_t * cend() const noexcept

        Return the beginning and ending pointers to the buffer.

    .. function:: \
        friend ostream & operator<<(ostream &os, const ContiguousBuffer &cb) 
    
        Print the detail of the buffer.

    .. member:: \
        value_t *buff
        size_t size

**Examples:**

:class:`ContiguousBufferTraits` defines the ContiguousBuffer concept. Example 
classes that satisfy this protocol are ``std::vector``, ``std::array``, any RawArray concept 
compliant class. A segment of raw memory buffer is also ContiguousBuffer. For example::

    std::vector<int> a1(4);
    const std::array<double, 4> a2 {};
    float a3[4] {};
    long *a4 = new long [4];                // raw memory buffer
    
:class:`ContiguousBuffer` object represents a view of any of those object. To 
create such a view, simply pass it to the constructor. For raw memory buffer, 
a size argument should be provided. Class template parameters are deduced 
automatically::

    ContiguousBuffer<int> b11 {a1};
    ContiguousBuffer<const int> b12{a1};    // cast to const buffer

    ContiguousBuffer<const double> b21{a2};         
    ContiguousBuffer b22 {a2};              // auto deduced to <const double>
    // ContiguousBuffer<double> b23 {a2};   // ERROR, compile failure - cannot cast const to non-const

    ContiguousBuffer<float> b3 {a3};               
    ContiguousBuffer<long> b4 {a4, 4}; 

    pout << "b11 = ", b11, "\nb12 = ", b12, "\nn21 = ", b21,
        "\nb22 = ", b22, "\nb3 = ", b3, "\nb4 = ", b4, endl;

Output:

.. code-block:: text

    b11 = ContiguousBuffer{buff=0x56055fd1deb0, size=4}
    b12 = ContiguousBuffer{buff=[const]0x56055fd1deb0, size=4}
    n21 = ContiguousBuffer{buff=[const]0x7ffe5a50c520, size=4}
    b22 = ContiguousBuffer{buff=[const]0x7ffe5a50c520, size=4}
    b3 = ContiguousBuffer{buff=0x7ffe5a50c510, size=4}
    b4 = ContiguousBuffer{buff=0x56055fd1ded0, size=4}

To get the info, i.e., buffer address and its size (number of elements),
use the structured binding, ``buff`` and ``size`` members, or 
the iterators returned by ``begin()`` and ``end()``::

    auto [p1, n1] = b11;                    // Get buffer address and size using structured binding,
    
    const double *p2 = b21.buff;            // or by visiting members.
    size_t n2 = b21.size;                   
    
    auto beg3 = b3.begin(), end3 = b3.end();  // Get iterators.
    
    pout << "contents in a1 = {", pout(p1, p1+n1), 
        "}, a2 = {", pout(p2, p2+n2), 
        "}, a3 = {", pout(beg3, end3), "}\n";
    
Output:

.. code-block:: text

    contents in a1 = {0,0,0,0}, a2 = {0,0,0,0}, a3 = {0,0,0,0}