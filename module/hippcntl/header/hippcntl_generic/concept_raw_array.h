/**
create: Yangyao CHEN, 2021/07/15
    [write   ] RawArrayHelper, RawArrayTraits - compile-time type traits for 
        the raw-array-like types.
*/

#ifndef _HIPPCNTL_CONCEPT_RAW_ARRAY_H_
#define _HIPPCNTL_CONCEPT_RAW_ARRAY_H_
#include "generic_base.h"

namespace HIPP {

/**
Helpers for the raw array traits.
*/
class RawArrayHelper {
public:
    /**
    Determines the extents of a raw array. 
    e.g., int [3][4] => std::array {3,4};
    */
    template<typename ArrayT>
    static constexpr std::array<size_t, std::rank_v<ArrayT> > extents() 
    {
        constexpr size_t N = std::rank_v<ArrayT>;
        std::array<size_t,  N> out {};
        _fill_extent<ArrayT, N>(0, out);
        return out;
    }

    /**
    Find the strides of a raw array.
    e.g., int [2][3][4] => std::array {12, 4, 1};
    */
    template<typename ArrayT>
    static constexpr std::array<size_t, std::rank_v<ArrayT> > strides()
    {
        constexpr size_t N = std::rank_v<ArrayT>;
        constexpr std::array<size_t, N> exts = extents<ArrayT>();
        std::array<size_t, N> out {};
        for(size_t i=0; i<N; ++i){
            out[i] = 1;
            for(size_t j=i+1; j<N; ++j){
                out[i] *= exts[j];
            }
        }
        return out;
    }

    /**
    Determine the size, i.e., total number of elements in the raw array.
    e.g., int[3][4] => 12.
    */
    template<typename ArrayT>
    static constexpr size_t size() {
        if constexpr ( !std::is_array_v<ArrayT> ) return 1;
        else{
            return std::extent_v<ArrayT> * 
                size< std::remove_extent_t<ArrayT> >();
        }
    }

    /**
    Get an element from a raw array typed object.
    e.g., int (&a)[3][4], 0, 1 => a[0][1].
        int &a => a.
    */
    template<typename ArrayT>
    static ArrayT & get_elem(ArrayT &a) { return a; }

    template<typename ArrayT, typename ...SizeTs>
    static std::remove_all_extents_t<ArrayT> & get_elem(ArrayT &a, size_t id1, SizeTs &&...ids) {
        return get_elem(a[id1], std::forward<SizeTs>(ids)...);
    }

    /**
    Get raw array type from from the extents list.
    e.g.,
    int, 3, 4, 5 => int[3][4][5].
    int => int.
    */
    template<typename T, size_t ...Ds> 
    struct extents_to_array
    {};

    template<typename T> 
    struct extents_to_array<T>
    {
        typedef T type;
    };

    template<typename T, size_t D1, size_t ...Ds> 
    struct extents_to_array<T, D1, Ds...>
    {
        typedef typename extents_to_array<T, Ds...>::type type[D1];
    };

    template<typename T, size_t ...Ds>
    using extents_to_array_t = typename extents_to_array<T, Ds...>::type;

    /**
    Determine if T is a std::array.
    e.g., std::array< std::array<int, 3>, 4 > => true.
        int [3] => false;
    */
    template<typename T, typename _V = void>
    struct is_std_array : std::false_type {};

    template<typename T, size_t N>
    struct is_std_array< std::array<T, N> > : std::true_type {};

    template<typename T, size_t N>
    struct is_std_array< const std::array<T, N> > : std::true_type {};

    template<typename T>
    inline static constexpr bool is_std_array_v = is_std_array<T>::value;

    /**
    Find the raw array type corresponding to a std::array.
    e.g., std::array< std::array<double, 4>, 3 > => double[3][4].
    */
    template<typename T, typename _V=void>
    struct std_array_to_raw {};

    template<typename T, size_t N>
    struct std_array_to_raw<
        std::array<T, N>, 
        std::enable_if_t<!is_std_array_v<T> > 
    > {
        typedef T type[N];
    };

    template<typename T, size_t N>
    struct std_array_to_raw<
        const std::array<T, N>, 
        std::enable_if_t<!is_std_array_v<T> > 
    > {
        typedef std::add_const_t<T> type[N];
    };

    template<typename T, size_t N>
    struct std_array_to_raw<
        std::array<T, N>, 
        std::enable_if_t<is_std_array_v<T> > > 
    {
        typedef typename std_array_to_raw<T>::type type[N];
    };

    template<typename T, size_t N>
    struct std_array_to_raw<
        const std::array<T, N>, 
        std::enable_if_t<is_std_array_v<T> > > 
    {
        typedef typename std_array_to_raw<T>::type type[N];
    };

    template<typename T>
    using std_array_to_raw_t = typename std_array_to_raw<T>::type;
private:
    template<typename ArrayT, size_t N>
    static constexpr void _fill_extent(size_t i, std::array<size_t, N> &out) {
        if constexpr ( std::is_array_v<ArrayT> ) {
            out[i] = std::extent_v<ArrayT>;
            _fill_extent<std::remove_extent_t<ArrayT>, N>(i+1, out);
        }
    }
};

/**
RawArrayTraits - gives features for a raw-array-like type.
User may add specializations to this generic class.

e.g.,
RawArrayTraits<int [3][4]>::extents => std::array{3,4}.
RawArrayTraits< std::array<std::array<double, 4>, 3> >::extents 
    => std::array{3,4}.
*/
template<typename T, typename V=void> 
class RawArrayTraits {
public:
    inline static constexpr bool is_array = false;
    typedef T value_t;
};

template<typename RawArrayT>
class RawArrayTraits<RawArrayT, 
    std::enable_if_t<std::is_array_v<RawArrayT>> > {
public:
    /* The raw array type. */
    typedef RawArrayT array_t;

    /* The scalar type. */ 
    typedef std::remove_all_extents_t<array_t> value_t;

    /** 
    ``is_array`` tells whether or not the template parameter is an raw-array-like type.
    If true, ``rank``, ``size``, ``extents``, and ``strides`` gives its details.
    */
    inline static constexpr bool is_array = true;
    inline static constexpr size_t 
        rank = std::rank_v<array_t>,
        size = RawArrayHelper::size<array_t>();

    inline static constexpr std::array<size_t, rank> 
        extents = RawArrayHelper::extents<array_t>(),
        strides = RawArrayHelper::strides<array_t>();

    /**
    Print the traits detail. 
    @fmt_cntl: 0 for a inline message, 1 for a long and verbose block message.

    ``operator<<`` prints a inline message.
    */
    ostream & info(ostream &os=cout, int fmt_cntl=1) const {
        PStream ps(os);
        ps << HIPPCNTL_CLASS_INFO_INLINE(HIPP::RawArrayTraits);
        _prt(ps);
        if(fmt_cntl >= 1) ps << '\n'; 
        return os;
    }
    friend ostream & operator<< (ostream &os, const RawArrayTraits &t) {
        PStream ps(os);
        ps << "HIPP::RawArrayTraits "; t._prt(ps);
        return os;
    }
private:
    void _prt(PStream &ps) const {
        ps << "(sizeof value_t=", sizeof(value_t),  
            ", rank=", rank, ", size=", size, ", extents={", extents, "})";
    }
};

/**
Specialization for the ``std::array``. It is defined according to the 
corresponding raw-array type.
*/
template<typename T, size_t N>
class RawArrayTraits< std::array<T, N> > 
: public RawArrayTraits<
    RawArrayHelper::std_array_to_raw_t< std::array<T, N> > 
> {};

template<typename T, size_t N>
class RawArrayTraits< const std::array<T, N> > 
: public RawArrayTraits<
    RawArrayHelper::std_array_to_raw_t< const std::array<T, N> > 
> {};

} // namespace HIPP
#endif	//_HIPPCNTL_CONCEPT_RAW_ARRAY_H_