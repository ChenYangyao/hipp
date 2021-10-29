/**
create: Yangyao CHEN, 2021/10/13
    [write   ] Datapacket - the data packet (i.e., a view of data buffer).
*/

#ifndef _HIPPIO_H5_UTIL_DATAPACKET_H_
#define _HIPPIO_H5_UTIL_DATAPACKET_H_

#include "h5_obj_dataspace.h"
#include "h5_obj_datatype.h"

namespace HIPP::IO::H5 {

namespace _h5_datapacket_helper {
template<typename T>
struct Traits {
    typedef DatatypeTraits<T> dt_traits_t;
    typedef GeneralArrayTraits<T> gr_traits_t;
    typedef RawArrayTraits<T> rr_traits_t;

    static constexpr bool has_def() noexcept {
        return dt_traits_t::has_h5_datatype;
    }

    static constexpr bool is_gr() noexcept {
        return gr_traits_t::is_array;
    }

    static constexpr bool is_rr() noexcept {
        return rr_traits_t::is_array;
    }

    static constexpr bool is_rr_predef() noexcept {
        if constexpr ( !is_rr() ) return false;
        else return Traits<typename rr_traits_t::value_t>::is_predef();
    }

    static constexpr bool is_rr_custom() noexcept {
        if constexpr ( !is_rr() ) return false;
        else return Traits<typename rr_traits_t::value_t>::is_custom();
    }

    // predefined
    static constexpr bool is_predef() noexcept {    
        if constexpr( !has_def() ) return false;
        else return dt_traits_t::is_predefined;
    }
    
    // gr of predefined, include rr
    static constexpr bool is_gr_predef() noexcept { 
        if constexpr ( !is_gr() ) return false;
        else return Traits<typename gr_traits_t::value_t>::is_predef();
    }

    // gr of rr of predefined
    static constexpr bool is_gr_rr_predef() noexcept {
        if constexpr ( !is_gr() ) return false;
        else return Traits<typename gr_traits_t::value_t>::is_rr_predef();
    }

    // customized
    static constexpr bool is_custom() noexcept {
        if constexpr ( !has_def() ) return false;
        else return !dt_traits_t::is_predefined;
    }

    // gr of customized
    static constexpr bool is_gr_custom() noexcept {
        if constexpr ( !is_gr() ) return false;
        else return Traits<typename gr_traits_t::value_t>::is_custom();
    }

    // gr of rr of customized
    static constexpr bool is_gr_rr_custom() noexcept {
        if constexpr ( !is_gr() ) return false;
        else return Traits<typename gr_traits_t::value_t>::is_rr_custom();
    }
};
} // namespace _h5_datapacket_helper

/**
``Datapacket`` provides unified mapping from C++ data object to HDF5 dataset
specification (datatype and dataspace).
*/
class Datapacket {
public:
    /**
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
    */
    Datapacket() noexcept;
    Datapacket(void *_buff, Dataspace _dspace, Datatype _dtype) noexcept;
    
    template<typename T>
    Datapacket(T *_buff, Dataspace _dspace);

    template<typename T>
    explicit Datapacket(T &x);

    /**
    The only three data members, allowing ``Datapacket`` be used by structured
    binding, e.g., ``auto [buff, dspace, dtype] = Datapacket{x}``.
    */
    void *buff;
    Dataspace dspace;
    Datatype dtype;

    /**
    Resize the buffer vector ``v`` into desired size so that it exactly 
    contains ``target_sz`` elements.
    */
    template<typename T, typename A>
    static Datapacket resize_buff(vector<T, A> &v, size_t target_sz);

    /**
    Convert the datapacket into a tuple object.
    */
    std::tuple<void *, Dataspace, Datatype> to_tuple() const;
};

/**
``DatapacketScalar`` behaves like ``Datapacket``, but can be initialized by 
object whose datatype is interpreted as ATOMIC, i.e., directly mapped from 
:expr:`Datatype::from_type`.
*/
class DatapacketScalar : public Datapacket {
public: 
    DatapacketScalar() noexcept;

    DatapacketScalar(void *_buff, Datatype _dtype) noexcept;

    template<typename T>
    explicit DatapacketScalar(T &x);
};

/**
``DatapacketStr`` behaves like ``Datapacket``, but can be initialized by 
string or list of string object.
*/
class DatapacketStr : public Datapacket {
public:
    DatapacketStr() noexcept;

    DatapacketStr(void *_buff, Dataspace _dspace, 
        Datatype _dtype) noexcept;

    explicit DatapacketStr(string &s);

    template<size_t N_STR>
    explicit DatapacketStr(string (&s)[N_STR]);
    DatapacketStr(string *s, size_t n_str);
    explicit DatapacketStr(vector<string> &s);
    
    explicit DatapacketStr(char *s);
    DatapacketStr(char *s, size_t n);

    template<size_t N_STR, size_t N>
    explicit DatapacketStr(char (&s)[N_STR][N]);
    template<size_t N>
    DatapacketStr(char (*s)[N], size_t n_str);
    DatapacketStr(char **s, size_t n_str);
};

/**
The constant version of ``Datapacket``. The difference is that its constructors
accept const object or const buffer, and that the data member ``buff`` is typed 
with low-level const-qualifier.
*/
class ConstDatapacket {
public:
    ConstDatapacket() noexcept;
    ConstDatapacket(const void *_buff, Dataspace _dspace, 
        Datatype _dtype) noexcept;
    
    template<typename T>
    ConstDatapacket(const T *_buff, Dataspace _dspace);

    template<typename T>
    explicit ConstDatapacket(const T &x);

    const void *buff;
    Dataspace dspace;
    Datatype dtype;

    std::tuple<const void *, Dataspace, Datatype> to_tuple() const;
};

/**
The constant version of ``DatapacketScalar``.
*/
class ConstDatapacketScalar : public ConstDatapacket {
public: 
    ConstDatapacketScalar() noexcept;

    ConstDatapacketScalar(const void *_buff, Datatype _dtype) noexcept;

    template<typename T>
    explicit ConstDatapacketScalar(const T &x);
};

/**
The constant version of ``DatapacketStr``.
*/
class ConstDatapacketStr : public ConstDatapacket {
public:
    ConstDatapacketStr() noexcept;

    ConstDatapacketStr(const void *_buff, Dataspace _dspace, 
        Datatype _dtype) noexcept;

    explicit ConstDatapacketStr(const string &s);
    template<size_t N_STR>
    explicit ConstDatapacketStr(const string (&s)[N_STR]);
    ConstDatapacketStr(const string *s, size_t n_str);
    explicit ConstDatapacketStr(const vector<string> &s);

    explicit ConstDatapacketStr(const char *s);
    ConstDatapacketStr(const char *s, size_t n);

    template<size_t N_STR, size_t N>
    explicit ConstDatapacketStr(const char (&s)[N_STR][N]);
    template<size_t N>
    ConstDatapacketStr(const char (*s)[N], size_t n_str);
    ConstDatapacketStr(const char * const *s, size_t n_str);

    /**
    Write the content of strings into a buffer ``b``. The buffer is resized
    to be able to contain ``n_str`` strings whose size is the maximum size of 
    the strings ``s`` (including the space for the null-terminator). 
    Then the strings are fill into them, strided by this size.
    */
    void buff_to(const string *s, size_t n_str, vector<char> &b);
    void buff_to(const char * const *s, size_t n_str, vector<char> &b);
};


} // namespace HIPP::IO::H5

#endif	//_HIPPIO_H5_UTIL_DATAPACKET_H_
