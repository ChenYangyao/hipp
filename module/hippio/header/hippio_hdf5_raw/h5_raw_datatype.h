/**
create: Yangyao CHEN, 2020/11/29
    [write   ] _Datatype:   Intermediate-level HDF5 datatype API.
update: Yangyao Chen, 2021/10/13
    [write   ] _DatatypeTraits: feature detection and type map from native
        types to HDF5 types.
*/ 

#ifndef _HIPPIO_H5_RAW_DATATYPE_H_
#define _HIPPIO_H5_RAW_DATATYPE_H_

#include "h5_raw_named_base.h"

namespace HIPP::IO::H5 {

class _Datatype: public _NamedObj {
public:
    typedef _NamedObj parent_t;

    typedef H5T_class_t class_t;
    inline static constexpr class_t 
        cCOMPOUND  = H5T_COMPOUND, 
        cOPAQUE    = H5T_OPAQUE,
        cENUM      = H5T_ENUM,
        cSTRING    = H5T_STRING,
        cARRAY     = H5T_ARRAY,
        cINTEGER   = H5T_INTEGER,
        cFLOAT     = H5T_FLOAT,
        cBITFIELD  = H5T_BITFIELD,
        cREFERENCE = H5T_REFERENCE,
        cVLEN      = H5T_VLEN;

    using parent_t::parent_t;
    
    virtual ~_Datatype() noexcept override;

    static hid_t open(hid_t loc, const char *name, 
        hid_t aprop =_Proplist::vDFLT);
    static void close(hid_t dtype);

    void commit(hid_t loc, const char *name, hid_t lcprop =_Proplist::vDFLT, 
        hid_t tcprop =_Proplist::vDFLT, hid_t taprop =_Proplist::vDFLT);
    void lock();
    htri_t committed() const;
    htri_t equal(hid_t dtype) const;
    class_t get_class() const;
    size_t get_size() const;

    /* copy from old type to new type */
    static hid_t copy(hid_t dtype);
    hid_t copy();

    /* change the size of current type */
    void set_size(size_t size);

    /* for ATOMIC types */
    void set_precision(size_t precision);
   
    /* compound datatype creation and access */
    static hid_t create(class_t c, size_t size);
    void insert(const char *name, size_t offset, hid_t field_dtype);
    void pack();
    unsigned get_nmembers() const;
    unsigned get_member_index(const char *field_name) const;
    class_t get_member_class(unsigned field_idx) const;
    size_t get_member_offset(unsigned field_idx) const;
    hid_t get_member_type(unsigned field_idx) const;
    string get_member_name(unsigned field_idx) const;

    /* array datatype creation and access */
    static hid_t create_array(hid_t dtype, unsigned rank, const hsize_t dims[]);
    unsigned get_array_ndims() const;
    unsigned get_array_dims(hsize_t dims[]) const;

    /** For ARRAY, VL, ENUM datatypes. */
    hid_t get_super() const;
};

/** 
The wrapper instance for the predefined data type.
e.g. _NATIVE_CHAR_T.raw() gives H5T_NATIVE_CHAR.
*/
#define _HIPPIO_H5_PRE_T(obj_name, obj_def) \
    inline extern const _Datatype obj_name {obj_def, 0};

_HIPPIO_H5_PRE_T(_NATIVE_FLOAT_T,   H5T_NATIVE_FLOAT)
_HIPPIO_H5_PRE_T(_NATIVE_DOUBLE_T,  H5T_NATIVE_DOUBLE)
_HIPPIO_H5_PRE_T(_NATIVE_LDOUBLE_T, H5T_NATIVE_LDOUBLE)
_HIPPIO_H5_PRE_T(_NATIVE_CHAR_T,    H5T_NATIVE_CHAR)
_HIPPIO_H5_PRE_T(_NATIVE_SCHAR_T,   H5T_NATIVE_SCHAR)
_HIPPIO_H5_PRE_T(_NATIVE_SHORT_T,   H5T_NATIVE_SHORT)
_HIPPIO_H5_PRE_T(_NATIVE_INT_T,     H5T_NATIVE_INT)
_HIPPIO_H5_PRE_T(_NATIVE_LONG_T,    H5T_NATIVE_LONG)
_HIPPIO_H5_PRE_T(_NATIVE_LLONG_T,   H5T_NATIVE_LLONG)
_HIPPIO_H5_PRE_T(_NATIVE_UCHAR_T,   H5T_NATIVE_UCHAR)
_HIPPIO_H5_PRE_T(_NATIVE_USHORT_T,  H5T_NATIVE_USHORT)
_HIPPIO_H5_PRE_T(_NATIVE_UINT_T,    H5T_NATIVE_UINT)
_HIPPIO_H5_PRE_T(_NATIVE_ULONG_T,   H5T_NATIVE_ULONG)
_HIPPIO_H5_PRE_T(_NATIVE_ULLONG_T,  H5T_NATIVE_ULLONG)

_HIPPIO_H5_PRE_T(_C_S1_T, H5T_C_S1)

_HIPPIO_H5_PRE_T(_IEEE_F32LE_T,     H5T_IEEE_F32LE)
_HIPPIO_H5_PRE_T(_IEEE_F64LE_T,     H5T_IEEE_F64LE)
_HIPPIO_H5_PRE_T(_STD_I8LE_T,       H5T_STD_I8LE)
_HIPPIO_H5_PRE_T(_STD_I16LE_T,      H5T_STD_I16LE)
_HIPPIO_H5_PRE_T(_STD_I32LE_T,      H5T_STD_I32LE)
_HIPPIO_H5_PRE_T(_STD_I64LE_T,      H5T_STD_I64LE)
_HIPPIO_H5_PRE_T(_STD_U8LE_T,       H5T_STD_U8LE)
_HIPPIO_H5_PRE_T(_STD_U16LE_T,      H5T_STD_U16LE)
_HIPPIO_H5_PRE_T(_STD_U32LE_T,      H5T_STD_U32LE)
_HIPPIO_H5_PRE_T(_STD_U64LE_T,      H5T_STD_U64LE)

_HIPPIO_H5_PRE_T(_IEEE_F32BE_T,     H5T_IEEE_F32BE)
_HIPPIO_H5_PRE_T(_IEEE_F64BE_T,     H5T_IEEE_F64BE)
_HIPPIO_H5_PRE_T(_STD_I8BE_T,       H5T_STD_I8BE)
_HIPPIO_H5_PRE_T(_STD_I16BE_T,      H5T_STD_I16BE)
_HIPPIO_H5_PRE_T(_STD_I32BE_T,      H5T_STD_I32BE)
_HIPPIO_H5_PRE_T(_STD_I64BE_T,      H5T_STD_I64BE)
_HIPPIO_H5_PRE_T(_STD_U8BE_T,       H5T_STD_U8BE)
_HIPPIO_H5_PRE_T(_STD_U16BE_T,      H5T_STD_U16BE)
_HIPPIO_H5_PRE_T(_STD_U32BE_T,      H5T_STD_U32BE)
_HIPPIO_H5_PRE_T(_STD_U64BE_T,      H5T_STD_U64BE)

#undef _HIPPIO_H5_PRE_T

/**
The type map from C++ native type to HDF5 lib's atomic-class type.
*/
enum class _DatatypeArch : int {
    NATIVE=0, 
    STD=1, 
    RESIZED_FIX=2};

template<typename NativeT, _DatatypeArch TA>
struct _DatatypeTraits {
    inline static constexpr bool has_h5_datatype = false;
    typedef NativeT native_t;
};

/** Specialization for native types. */
#define _HIPPIO_H5_PRE_T(_native_t, _h5_t) \
template<> \
struct _DatatypeTraits<_native_t, _DatatypeArch::NATIVE> { \
    inline static constexpr bool has_h5_datatype = true; \
    typedef _native_t native_t; \
    inline static const hid_t datatype = _h5_t; \
    inline static const char *native_name = #_native_t; \
    inline static const char *h5_name = #_h5_t; \
};

_HIPPIO_H5_PRE_T(float,                 H5T_NATIVE_FLOAT)
_HIPPIO_H5_PRE_T(double,                H5T_NATIVE_DOUBLE)
_HIPPIO_H5_PRE_T(long double,           H5T_NATIVE_LDOUBLE)
_HIPPIO_H5_PRE_T(char,                  H5T_NATIVE_CHAR)
_HIPPIO_H5_PRE_T(signed char,           H5T_NATIVE_SCHAR)
_HIPPIO_H5_PRE_T(short,                 H5T_NATIVE_SHORT)
_HIPPIO_H5_PRE_T(int,                   H5T_NATIVE_INT)
_HIPPIO_H5_PRE_T(long,                  H5T_NATIVE_LONG)
_HIPPIO_H5_PRE_T(long long,             H5T_NATIVE_LLONG)
_HIPPIO_H5_PRE_T(unsigned char,         H5T_NATIVE_UCHAR)
_HIPPIO_H5_PRE_T(unsigned short,        H5T_NATIVE_USHORT)
_HIPPIO_H5_PRE_T(unsigned int,          H5T_NATIVE_UINT)
_HIPPIO_H5_PRE_T(unsigned long,         H5T_NATIVE_ULONG)
_HIPPIO_H5_PRE_T(unsigned long long,    H5T_NATIVE_ULLONG)

_HIPPIO_H5_PRE_T(const float,                 H5T_NATIVE_FLOAT)
_HIPPIO_H5_PRE_T(const double,                H5T_NATIVE_DOUBLE)
_HIPPIO_H5_PRE_T(const long double,           H5T_NATIVE_LDOUBLE)
_HIPPIO_H5_PRE_T(const char,                  H5T_NATIVE_CHAR)
_HIPPIO_H5_PRE_T(const signed char,           H5T_NATIVE_SCHAR)
_HIPPIO_H5_PRE_T(const short,                 H5T_NATIVE_SHORT)
_HIPPIO_H5_PRE_T(const int,                   H5T_NATIVE_INT)
_HIPPIO_H5_PRE_T(const long,                  H5T_NATIVE_LONG)
_HIPPIO_H5_PRE_T(const long long,             H5T_NATIVE_LLONG)
_HIPPIO_H5_PRE_T(const unsigned char,         H5T_NATIVE_UCHAR)
_HIPPIO_H5_PRE_T(const unsigned short,        H5T_NATIVE_USHORT)
_HIPPIO_H5_PRE_T(const unsigned int,          H5T_NATIVE_UINT)
_HIPPIO_H5_PRE_T(const unsigned long,         H5T_NATIVE_ULONG)
_HIPPIO_H5_PRE_T(const unsigned long long,    H5T_NATIVE_ULLONG)

#undef _HIPPIO_H5_PRE_T

/** Specialization for standard types. */
#define _HIPPIO_H5_PRE_T(_native_t, _h5_t) \
template<> \
struct _DatatypeTraits<_native_t, _DatatypeArch::STD> { \
    inline static constexpr bool has_h5_datatype = true; \
    typedef _native_t native_t; \
    inline static const hid_t datatype = _h5_t; \
    inline static const char *native_name = #_native_t; \
    inline static const char *h5_name = #_h5_t; \
};

_HIPPIO_H5_PRE_T(float,                 H5T_IEEE_F32LE)
_HIPPIO_H5_PRE_T(double,                H5T_IEEE_F64LE)
_HIPPIO_H5_PRE_T(long double,           H5T_IEEE_F64LE)
_HIPPIO_H5_PRE_T(char,                  H5T_STD_I8LE)
_HIPPIO_H5_PRE_T(signed char,           H5T_STD_I8LE)
_HIPPIO_H5_PRE_T(short,                 H5T_STD_I16LE)
_HIPPIO_H5_PRE_T(int,                   H5T_STD_I32LE)
_HIPPIO_H5_PRE_T(long,                  H5T_STD_I64LE)
_HIPPIO_H5_PRE_T(long long,             H5T_STD_I64LE)
_HIPPIO_H5_PRE_T(unsigned char,         H5T_STD_U8LE)
_HIPPIO_H5_PRE_T(unsigned short,        H5T_STD_U16LE)
_HIPPIO_H5_PRE_T(unsigned int,          H5T_STD_U32LE)
_HIPPIO_H5_PRE_T(unsigned long,         H5T_STD_U64LE)
_HIPPIO_H5_PRE_T(unsigned long long,    H5T_STD_U64LE)

_HIPPIO_H5_PRE_T(const float,                 H5T_IEEE_F32LE)
_HIPPIO_H5_PRE_T(const double,                H5T_IEEE_F64LE)
_HIPPIO_H5_PRE_T(const long double,           H5T_IEEE_F64LE)
_HIPPIO_H5_PRE_T(const char,                  H5T_STD_I8LE)
_HIPPIO_H5_PRE_T(const signed char,           H5T_STD_I8LE)
_HIPPIO_H5_PRE_T(const short,                 H5T_STD_I16LE)
_HIPPIO_H5_PRE_T(const int,                   H5T_STD_I32LE)
_HIPPIO_H5_PRE_T(const long,                  H5T_STD_I64LE)
_HIPPIO_H5_PRE_T(const long long,             H5T_STD_I64LE)
_HIPPIO_H5_PRE_T(const unsigned char,         H5T_STD_U8LE)
_HIPPIO_H5_PRE_T(const unsigned short,        H5T_STD_U16LE)
_HIPPIO_H5_PRE_T(const unsigned int,          H5T_STD_U32LE)
_HIPPIO_H5_PRE_T(const unsigned long,         H5T_STD_U64LE)
_HIPPIO_H5_PRE_T(const unsigned long long,    H5T_STD_U64LE)

#undef _HIPPIO_H5_PRE_T

/** 
char *, const char *, string and their top-level const countparts are very 
special examples.
They are treated as HDF5 STRING atomic datatype and used with resized version
of the predefined datatype.

The ``size()`` method includes space for the null-term.
*/
template<>
struct _DatatypeTraits<const char *, _DatatypeArch::RESIZED_FIX> {
    inline static constexpr bool has_h5_datatype = true;
    typedef const char *native_t;
    inline static const hid_t datatype = H5T_C_S1;
    inline static const char *native_name = "const char *";
    inline static const char *h5_name = "H5T_C_S1";

    _DatatypeTraits(native_t s) noexcept : _s(s) {}
    
    size_t size() const noexcept { return strlen(_s)+1; }
    native_t buff() const noexcept { return _s; }
protected:
    native_t _s;
};

template<>
struct _DatatypeTraits<const char * const, _DatatypeArch::RESIZED_FIX> {
    inline static constexpr bool has_h5_datatype = true;
    typedef const char * const native_t;
    inline static const hid_t datatype = H5T_C_S1;
    inline static const char *native_name = "const char * const";
    inline static const char *h5_name = "H5T_C_S1";

    _DatatypeTraits(native_t s) noexcept : _s(s) {}
    
    size_t size() const noexcept { return strlen(_s)+1; }
    const char * buff() const noexcept { return _s; }
protected:
    native_t _s;
};

template<>
struct _DatatypeTraits<char *, _DatatypeArch::RESIZED_FIX> {
    inline static constexpr bool has_h5_datatype = true;
    typedef char *native_t;
    inline static const hid_t datatype = H5T_C_S1;
    inline static const char *native_name = "char *";
    inline static const char *h5_name = "H5T_C_S1";

    _DatatypeTraits(native_t s) noexcept : _s(s) {}
    
    size_t size() const noexcept { return strlen(_s)+1; }
    native_t buff() const noexcept { return _s; }
protected:
    native_t _s;
};

template<>
struct _DatatypeTraits<char * const, _DatatypeArch::RESIZED_FIX> {
    inline static constexpr bool has_h5_datatype = true;
    typedef char * const native_t;
    inline static const hid_t datatype = H5T_C_S1;
    inline static const char *native_name = "char * const";
    inline static const char *h5_name = "H5T_C_S1";

    _DatatypeTraits(native_t s) noexcept : _s(s) {}
    
    size_t size() const noexcept { return strlen(_s)+1; }
    char * buff() const noexcept { return _s; }
protected:
    native_t _s;
};

template<>
struct _DatatypeTraits<string, _DatatypeArch::RESIZED_FIX> {
    inline static constexpr bool has_h5_datatype = true;
    typedef string native_t;
    inline static const hid_t datatype = H5T_C_S1;
    inline static const char *native_name = "string";
    inline static const char *h5_name = "H5T_C_S1";

    _DatatypeTraits(native_t &s) noexcept : _s(s) {}

    size_t size() const noexcept { return _s.size()+1; }
    char * buff() const noexcept { return _s.data(); }
protected:
    native_t &_s;
};

template<>
struct _DatatypeTraits<const string, _DatatypeArch::RESIZED_FIX> {
    inline static constexpr bool has_h5_datatype = true;
    typedef const string native_t;
    inline static const hid_t datatype = H5T_C_S1;
    inline static const char *native_name = "const string";
    inline static const char *h5_name = "H5T_C_S1";

    _DatatypeTraits(native_t &s) noexcept : _s(s) {}

    size_t size() const noexcept { return _s.size()+1; }
    const char * buff() const noexcept { return _s.data(); }
protected:
    native_t &_s;
};

inline _Datatype::~_Datatype() noexcept { 
    if( is_auto_free() ) {
        close(raw()); state_flag(0);
    }
}

inline hid_t _Datatype::open(hid_t loc, const char *name, hid_t aprop) {
    auto ret = H5Topen(loc, name, aprop);
    ErrH5::check(ret, emFLPFB, "  ... open for ", name, 
        " failed\n");
    return ret;
}

inline void _Datatype::close(hid_t dtype) {
    ErrH5::check(H5Tclose(dtype), emFLPFB);
}

inline void _Datatype::commit(hid_t loc, const char *name, 
    hid_t lcprop, hid_t tcprop, hid_t taprop) {
    ErrH5::check(H5Tcommit(loc, name, raw(), lcprop, tcprop, taprop),
        emFLPFB, "  ...  commite to ", name, " failed\n");
}

inline void _Datatype::lock() {
    ErrH5::check(H5Tlock(raw()), emFLPFB, "  ... lock failed\n");
}

inline htri_t _Datatype::committed() const {
    auto ret = H5Tcommitted(raw());
    ErrH5::check(ret, emFLPFB, "  ... check committed failed\n");
    return ret;
}

inline htri_t _Datatype::equal( hid_t dtype ) const{
    htri_t is_eq = H5Tequal( raw(), dtype );
    ErrH5::check( is_eq, emFLPFB );
    return is_eq;
}

inline auto _Datatype::get_class() const -> class_t {
    auto ret = H5Tget_class(raw());
    ErrH5::check(ret, emFLPFB, "  ... failed to get datatype class\n");
    return ret;
}

inline size_t _Datatype::get_size() const {
    size_t size = H5Tget_size( raw() );
    if( size == 0 ) ErrH5::throw_( size, emFLPFB );
    return size;
}

inline hid_t _Datatype::copy( hid_t dtype ){
    hid_t ret = H5Tcopy( dtype );
    ErrH5::check( ret, emFLPFB );
    return ret;
}

inline hid_t _Datatype::copy() {
    return copy(raw());
}

inline void _Datatype::set_size( size_t size ){
    ErrH5::check( H5Tset_size( raw(), size ), emFLPFB,
        "  ... failed to set size ", size, '\n');
}

inline void _Datatype::set_precision( size_t precision ) {
    ErrH5::check( H5Tset_precision( raw(), precision), emFLPFB, 
        "  ... failed to set precision ", precision, '\n');
}

inline hid_t _Datatype::create(class_t c, size_t size){
    auto ret = H5Tcreate(c, size);
    ErrH5::check(ret, emFLPFB, "  ... size=", size, '\n');
    return ret;
}

inline void _Datatype::insert(const char *field_name, size_t offset, 
    hid_t field_dtype){
    ErrH5::check(H5Tinsert(raw(), field_name, offset, field_dtype), 
        emFLPFB, "  ... field_name=", field_name, ", offset=", offset, '\n');
}

inline void _Datatype::pack(){
    ErrH5::check(H5Tpack(raw()), emFLPFB);
}

inline unsigned _Datatype::get_nmembers() const {
    int nmems = H5Tget_nmembers(raw());
    ErrH5::check(nmems, emFLPFB);
    return static_cast<unsigned>(nmems);
}

inline unsigned _Datatype::get_member_index(const char *field_name) const {
    int mem_idx = H5Tget_member_index(raw(), field_name);
    ErrH5::check(mem_idx, emFLPFB, "  ... field_name=", field_name, '\n');
    return static_cast<unsigned>(mem_idx);
}
inline auto _Datatype::get_member_class(unsigned field_idx) const ->class_t{
    class_t cls = H5Tget_member_class(raw(), field_idx);
    ErrH5::check(cls, emFLPFB, "  ... field index=", field_idx, '\n');
    return cls;
}

inline size_t _Datatype::get_member_offset(unsigned field_idx) const{
    get_member_class(field_idx);
    return H5Tget_member_offset(raw(), field_idx);
}

inline hid_t _Datatype::get_member_type(unsigned field_idx) const {
    hid_t tp = H5Tget_member_type(raw(), field_idx);
    ErrH5::check(tp, emFLPFB, "  ... field index=", field_idx, '\n');
    return tp;
}

inline string _Datatype::get_member_name(unsigned field_idx) const {
    char *nm = H5Tget_member_name(raw(), field_idx);
    if( !nm )
        ErrH5::throw_(-1, emFLPFB, "  ... field_idx=", field_idx, '\n');
    string ret(nm);
    ErrH5::check(H5free_memory(nm), emFLPFB, 
        "  ... free memory of field name failed. Field index=",
         field_idx, '\n');
    return ret;
}

inline hid_t _Datatype::create_array(hid_t dtype, unsigned rank, 
    const hsize_t dims[]){
    auto ret = H5Tarray_create(dtype, rank, dims);
    ErrH5::check(ret, emFLPFB, "  ... no. of dims=", rank, '\n');
    return ret;
}

inline unsigned _Datatype::get_array_ndims() const{
    int ndims = H5Tget_array_ndims(raw());
    ErrH5::check(ndims, emFLPFB, '\n');
    return static_cast<unsigned>(ndims);
}

inline unsigned _Datatype::get_array_dims(hsize_t dims[]) const {
    int ndims = H5Tget_array_dims(raw(), dims);
    ErrH5::check(ndims, emFLPFB, '\n');
    return static_cast<unsigned>(ndims);
}

inline hid_t _Datatype::get_super() const {
    auto ret = H5Tget_super(raw());
    ErrH5::check(ret, emFLPFB, "  ... failed to get datatype super\n");
    return ret;
}


} // namespace HIPP::IO::H5

#endif	//_HIPPIO_H5_RAW_DATATYPE_H_