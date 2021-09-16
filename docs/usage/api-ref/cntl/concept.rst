The Concept API
================

.. include:: /global.rst

The following variables, functions and classes are all defined within the namespace ``HIPP``.

.. namespace:: HIPP

RawArrayHelper
--------------------

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
---------------

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