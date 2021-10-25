/**
create: Yangyao CHEN, 2020/01/11
    [write   ] Datatype: HDF5 high-level datatype object.
*/ 

#ifndef _HIPPIO_H5_OBJ_DATATYPE_H_
#define _HIPPIO_H5_OBJ_DATATYPE_H_

#include "h5_obj_named_base.h"

namespace HIPP::IO::H5 {

typedef _DatatypeArch DatatypeArch;
class Group;
class Attr;
class Dataset;

/**
Datatype describe the bit-level memory layout, representation and conversion
of data element in HDF5 dataset and attribute.

Class ``Datatype`` encapsulates the methods available on datatypes. 

The library predefines a set of native and standard datatypes, based on which, 
or from scratch, customized datatype can be derived. 

The datatype class is copyable and movable (both in construction and 
assignment). The copy, move and destruction are ``noexcept``. The copy operation 
is shallow - the resulting object always refers to the same HDF5 resource
as the source object. The move operation sets the move-from object an empty
state.
*/
class Datatype: public NamedObj {
public:
    typedef NamedObj parent_t;
    
    typedef _Datatype _obj_raw_t;
    typedef std::shared_ptr<_obj_raw_t> _obj_ptr_t;

    /**
    Datatype classes. The first four can be passed into method :func:`create` 
    to create new datatypes. All of them may be returned by :func:`get_class` 
    or :func:`member_class`.
    */
    typedef _obj_raw_t::class_t class_t;
    inline static constexpr class_t 
        cCOMPOUND  = _obj_raw_t::cCOMPOUND, 
        cOPAQUE    = _obj_raw_t::cOPAQUE,
        cENUM      = _obj_raw_t::cENUM,
        cSTRING    = _obj_raw_t::cSTRING,
        cARRAY     = _obj_raw_t::cARRAY,
        cINTEGER   = _obj_raw_t::cINTEGER,
        cFLOAT     = _obj_raw_t::cFLOAT,
        cBITFIELD  = _obj_raw_t::cBITFIELD,
        cREFERENCE = _obj_raw_t::cREFERENCE,
        cVLEN      = _obj_raw_t::cVLEN;
    
    /** 
    Class ``Datatype`` "inherits" all constructors from its parent class.
    */
    using parent_t::parent_t;

    /** Produce a deep copy of the datatype. */
    Datatype copy() const;

    /** 
    Commite the datatype to file. The committed datatype (named datatype) 
    may be shared by many objects.
    */
    void commit(Group &group, const string &name, const Proplist &lcprop,
        Proplist &tcprop, Proplist &taprop);

    /**
    Lock the datatype so that it cannot be changed. All the predefined datatypes
    are already locked.
    */
    void lock();
    
    /**
    Retrieve the meta-info from the datatype instance, whether or not it is 
    committed, whether it is equal to another datatype instance, its size, 
    and its datatype class.
    */
    bool committed() const;
    bool equal(const Datatype &dtype) const;
    size_t size() const;
    class_t get_class() const;

    /**
    resize(): sets the size of the current datatype instance.

    resized(): produces a resized copy (i.e., similar to ``copy()`` and then
    ``resize()``).
    */
    void resize(size_t size);
    Datatype resized(size_t size) const;

    /** For ATOMIC datatypes, set the numerical precision to ``precision``. */
    void set_precision(size_t precision);

    /** 
    General new datatype creators. 

    @c: datatype class. Can be ``cCOMPOUND`` | ``cOPAQUE`` | ``cENUM`` | 
    ``cSTRING``.
    @size: the size in bytes.

    More specific creators like ``create_compound()`` and ``create_array()`` 
    are easier in most cases.
    */
    static Datatype create(class_t c, size_t size);

    /** 
    Methods for the creation of COMPOUND datatypes. 

    create_compound(name1, ptr1, name2, ptr2, ...): 
    directly creates a new compound datatype (usually corresponding to a C++ 
    struct/class). Arguments are passed pair-wisely to define the name and
    and datatype detail of each member in the COMPOUND datatype. The detail
    is described by the member pointer from which the datatype and offset 
    are inferred by the library.
    
    The C++ type of the member can be any valid type to 
    :expr:`Datatype::from_type` (e.g., ``int``, ``float[3][4][5]``). 

    create_compound(size): create an empty COMPOUND datatype sized ``size`` 
    bytes. 
    
    Members can be inserted later by :func:`insert`.
    */
    static Datatype create_compound(size_t size);

    template<typename Record, typename Member, typename ...Args>
    static Datatype create_compound(const string &mem_name, 
        Member Record::*mem_ptr, Args &&...args);

    /**
    insert(): inserts a new field into the current compound datatype.
    
    (1): insert(name, offset, dtype) inserts a member whose name is ``name``, 
    offset is ``offset`` and datatype is ``dtype``.

    (2): insert(name, field_ptr) inserts a member whose name is name and whose 
    datatype, offset are inferred from member-pointer ``field_ptr``. 
    The member can be any type that is mappable to a HDF5 datatype using 
    :expr:`Datatype::from_type`, (e.g., int, float[3][4][5]). 
    */
    Datatype & insert(const string &mem_name, size_t mem_offset, 
        const Datatype &mem_dtype);

    template<typename Record, typename Member>
    Datatype & insert(const string &mem_name, Member Record::*mem_ptr);
    
    /**
    Manually find the offset of a member typed ``Member`` in a structured 
    type ``Record``. 
    */
    template<typename Record, typename Member>
    static constexpr ptrdiff_t offset(Member Record::*mem_ptr) noexcept;

    /**
    Recursively removes the paddings in members to make the datatype 
    more memory efficient.
    */
    Datatype & pack();

    /**
    members() returns the number of members in the current compound 
    datatype instance.

    member_index(name) returns the index to the member named name for a 
    compound/enum datatype. 
    
    member_name(idx) converts the index back to the name. Index can be any 
    number in the range [0, N-1] where N is returned by ``nmembers()``.

    The datatype class, offset, and type can be retrieved by ``member_class()``,
    ``member_offset()`` and ``member_type()``.
    */
    unsigned nmembers() const;
    unsigned member_index(const string &name) const;
    string member_name(unsigned idx) const;
    class_t member_class(unsigned idx) const;
    size_t member_offset(unsigned idx) const;
    Datatype member_type(unsigned idx) const;


    /**
    ARRAY datatype creator and visitor.

    create_array() creates an array datatype with given dimensions ``dims``.

    create_array_for<Array>() creates an ARRAY datatype for any RawArray 
    protocol type, e.g., int a[2][3], std::array<std::array<float, 3>, 2>.
    */
    Datatype create_array(const Dimensions &dims) const;
    
    template<typename RawArray, DatatypeArch TA = DatatypeArch::NATIVE>
    static Datatype create_array_for();

    /** Retrieve the array information. */
    unsigned array_ndims() const;
    Dimensions array_dims() const;

    /**
    Map from a C++ type to its corresponding HDF5 datatype.
    @NativeT: a DatatypeTraits comformable type, i.e., 
        ``DatatypeTraits<NativeT>::has_h5_datatype == true``.
    @TA: the architecture specifier.

    User may extend the traits by adding specializations.
    */
    template<typename NativeT, DatatypeArch TA = DatatypeArch::NATIVE>
    static const Datatype & from_type();

    /**
    Infer the datatype from an C++ object.
    e.g., 
    ``buff = double``, the returned datatype is a scalar double type.
    ``buff = vector<int>``, the returned datatype is a scalar integer type.
    */
    template<typename T>
    static Datatype from_buff(const T &buff);

    /** Return a reference to the intermediate-level HDF5 object. */
    _obj_raw_t & obj_raw() noexcept;
    const _obj_raw_t & obj_raw() const noexcept;
private: 
    friend class Attr;
    friend class Dataset;

    template<typename R, typename M, typename ...Args>
    void _compound_insert(const string &mem_name, 
        M R::*mem_ptr, Args &&...args);

    template<typename ...Args>
    static _obj_ptr_t _ptr_from_raw(Args &&...args);
    template<typename ...Args>
    static Datatype _from_raw(Args &&...args);
};

/**
Predefined the high-level object counterparts for the pre-defined HDF5
datatypes. 
*/
#define _HIPPIO_H5_PRE_T(obj_name, obj_def) \
    inline extern const Datatype obj_name \
    { std::make_shared<Datatype::_obj_raw_t>(obj_def, 0) };

_HIPPIO_H5_PRE_T(NATIVE_FLOAT_T,    H5T_NATIVE_FLOAT)
_HIPPIO_H5_PRE_T(NATIVE_DOUBLE_T,   H5T_NATIVE_DOUBLE)
_HIPPIO_H5_PRE_T(NATIVE_LDOUBLE_T,  H5T_NATIVE_LDOUBLE)
_HIPPIO_H5_PRE_T(NATIVE_CHAR_T,     H5T_NATIVE_CHAR)
_HIPPIO_H5_PRE_T(NATIVE_SCHAR_T,    H5T_NATIVE_SCHAR)
_HIPPIO_H5_PRE_T(NATIVE_SHORT_T,    H5T_NATIVE_SHORT)
_HIPPIO_H5_PRE_T(NATIVE_INT_T,      H5T_NATIVE_INT)
_HIPPIO_H5_PRE_T(NATIVE_LONG_T,     H5T_NATIVE_LONG)
_HIPPIO_H5_PRE_T(NATIVE_LLONG_T,    H5T_NATIVE_LLONG)
_HIPPIO_H5_PRE_T(NATIVE_UCHAR_T,    H5T_NATIVE_UCHAR)
_HIPPIO_H5_PRE_T(NATIVE_USHORT_T,   H5T_NATIVE_USHORT)
_HIPPIO_H5_PRE_T(NATIVE_UINT_T,     H5T_NATIVE_UINT)
_HIPPIO_H5_PRE_T(NATIVE_ULONG_T,    H5T_NATIVE_ULONG)
_HIPPIO_H5_PRE_T(NATIVE_ULLONG_T,   H5T_NATIVE_ULLONG)

_HIPPIO_H5_PRE_T(C_S1_T, H5T_C_S1)

_HIPPIO_H5_PRE_T(IEEE_F32LE_T,      H5T_IEEE_F32LE)
_HIPPIO_H5_PRE_T(IEEE_F64LE_T,      H5T_IEEE_F64LE)
_HIPPIO_H5_PRE_T(STD_I8LE_T,        H5T_STD_I8LE)
_HIPPIO_H5_PRE_T(STD_I16LE_T,       H5T_STD_I16LE)
_HIPPIO_H5_PRE_T(STD_I32LE_T,       H5T_STD_I32LE)
_HIPPIO_H5_PRE_T(STD_I64LE_T,       H5T_STD_I64LE)
_HIPPIO_H5_PRE_T(STD_U8LE_T,        H5T_STD_U8LE)
_HIPPIO_H5_PRE_T(STD_U16LE_T,       H5T_STD_U16LE)
_HIPPIO_H5_PRE_T(STD_U32LE_T,       H5T_STD_U32LE)
_HIPPIO_H5_PRE_T(STD_U64LE_T,       H5T_STD_U64LE)

_HIPPIO_H5_PRE_T(IEEE_F32BE_T,      H5T_IEEE_F32BE)
_HIPPIO_H5_PRE_T(IEEE_F64BE_T,      H5T_IEEE_F64BE)
_HIPPIO_H5_PRE_T(STD_I8BE_T,        H5T_STD_I8BE)
_HIPPIO_H5_PRE_T(STD_I16BE_T,       H5T_STD_I16BE)
_HIPPIO_H5_PRE_T(STD_I32BE_T,       H5T_STD_I32BE)
_HIPPIO_H5_PRE_T(STD_I64BE_T,       H5T_STD_I64BE)
_HIPPIO_H5_PRE_T(STD_U8BE_T,        H5T_STD_U8BE)
_HIPPIO_H5_PRE_T(STD_U16BE_T,       H5T_STD_U16BE)
_HIPPIO_H5_PRE_T(STD_U32BE_T,       H5T_STD_U32BE)
_HIPPIO_H5_PRE_T(STD_U64BE_T,       H5T_STD_U64BE)

#undef _HIPPIO_H5_PRE_T


/**
The traits system that maps C++ types to the corresponding HDF5 datatypes.
*/

template<typename NativeT, 
    DatatypeArch TA = DatatypeArch::NATIVE, 
    typename V=void>
struct DatatypeTraits {
    inline static constexpr bool has_h5_datatype = false;
};

class DatatypeTraitsPredefined {
public:
    inline static constexpr bool has_h5_datatype = true,
        is_predefined = true;
};

class DatatypeTraitsCustomized {
public:
    inline static constexpr bool has_h5_datatype = true,
        is_predefined = false;
};

/** Specialization for native types. */
#define _HIPPIO_H5_PRE_T(_native_t, _h5_t, _h5_name) \
template<> \
struct DatatypeTraits<_native_t, DatatypeArch::NATIVE> \
: DatatypeTraitsPredefined { \
    typedef _native_t native_t; \
    inline static const Datatype &datatype = _h5_t; \
    inline static const char *native_name = #_native_t; \
    inline static const char *h5_name = #_h5_name; \
};

_HIPPIO_H5_PRE_T(float,               NATIVE_FLOAT_T,   H5T_NATIVE_FLOAT)
_HIPPIO_H5_PRE_T(double,              NATIVE_DOUBLE_T,  H5T_NATIVE_DOUBLE)
_HIPPIO_H5_PRE_T(long double,         NATIVE_LDOUBLE_T, H5T_NATIVE_LDOUBLE)
_HIPPIO_H5_PRE_T(char,                NATIVE_CHAR_T,    H5T_NATIVE_CHAR)
_HIPPIO_H5_PRE_T(signed char,         NATIVE_SCHAR_T,   H5T_NATIVE_SCHAR)
_HIPPIO_H5_PRE_T(short,               NATIVE_SHORT_T,   H5T_NATIVE_SHORT)
_HIPPIO_H5_PRE_T(int,                 NATIVE_INT_T,     H5T_NATIVE_INT)
_HIPPIO_H5_PRE_T(long,                NATIVE_LONG_T,    H5T_NATIVE_LONG)
_HIPPIO_H5_PRE_T(long long,           NATIVE_LLONG_T,   H5T_NATIVE_LLONG)
_HIPPIO_H5_PRE_T(unsigned char,       NATIVE_UCHAR_T,   H5T_NATIVE_UCHAR)
_HIPPIO_H5_PRE_T(unsigned short,      NATIVE_USHORT_T,  H5T_NATIVE_USHORT)
_HIPPIO_H5_PRE_T(unsigned int,        NATIVE_UINT_T,    H5T_NATIVE_UINT)
_HIPPIO_H5_PRE_T(unsigned long,       NATIVE_ULONG_T,   H5T_NATIVE_ULONG)
_HIPPIO_H5_PRE_T(unsigned long long,  NATIVE_ULLONG_T,  H5T_NATIVE_ULLONG)

_HIPPIO_H5_PRE_T(const float,               NATIVE_FLOAT_T,   H5T_NATIVE_FLOAT)
_HIPPIO_H5_PRE_T(const double,              NATIVE_DOUBLE_T,  H5T_NATIVE_DOUBLE)
_HIPPIO_H5_PRE_T(const long double,         NATIVE_LDOUBLE_T, H5T_NATIVE_LDOUBLE)
_HIPPIO_H5_PRE_T(const char,                NATIVE_CHAR_T,    H5T_NATIVE_CHAR)
_HIPPIO_H5_PRE_T(const signed char,         NATIVE_SCHAR_T,   H5T_NATIVE_SCHAR)
_HIPPIO_H5_PRE_T(const short,               NATIVE_SHORT_T,   H5T_NATIVE_SHORT)
_HIPPIO_H5_PRE_T(const int,                 NATIVE_INT_T,     H5T_NATIVE_INT)
_HIPPIO_H5_PRE_T(const long,                NATIVE_LONG_T,    H5T_NATIVE_LONG)
_HIPPIO_H5_PRE_T(const long long,           NATIVE_LLONG_T,   H5T_NATIVE_LLONG)
_HIPPIO_H5_PRE_T(const unsigned char,       NATIVE_UCHAR_T,   H5T_NATIVE_UCHAR)
_HIPPIO_H5_PRE_T(const unsigned short,      NATIVE_USHORT_T,  H5T_NATIVE_USHORT)
_HIPPIO_H5_PRE_T(const unsigned int,        NATIVE_UINT_T,    H5T_NATIVE_UINT)
_HIPPIO_H5_PRE_T(const unsigned long,       NATIVE_ULONG_T,   H5T_NATIVE_ULONG)
_HIPPIO_H5_PRE_T(const unsigned long long,  NATIVE_ULLONG_T,  H5T_NATIVE_ULLONG)


#undef _HIPPIO_H5_PRE_T

/** Specialization for standard types. */
#define _HIPPIO_H5_PRE_T(_native_t, _h5_t, _h5_name) \
template<> \
struct DatatypeTraits<_native_t, DatatypeArch::STD> \
: DatatypeTraitsPredefined { \
    typedef _native_t native_t; \
    inline static const Datatype &datatype = _h5_t; \
    inline static const char *native_name = #_native_t; \
    inline static const char *h5_name = #_h5_name; \
};

_HIPPIO_H5_PRE_T(float,               IEEE_F32LE_T, H5T_IEEE_F32LE)
_HIPPIO_H5_PRE_T(double,              IEEE_F64LE_T, H5T_IEEE_F64LE)
_HIPPIO_H5_PRE_T(long double,         IEEE_F64LE_T, H5T_IEEE_F64LE)
_HIPPIO_H5_PRE_T(char,                STD_I8LE_T,   H5T_STD_I8LE)
_HIPPIO_H5_PRE_T(signed char,         STD_I8LE_T,   H5T_STD_I8LE)
_HIPPIO_H5_PRE_T(short,               STD_I16LE_T,  H5T_STD_I16LE)
_HIPPIO_H5_PRE_T(int,                 STD_I32LE_T,  H5T_STD_I32LE)
_HIPPIO_H5_PRE_T(long,                STD_I64LE_T,  H5T_STD_I64LE)
_HIPPIO_H5_PRE_T(long long,           STD_I64LE_T,  H5T_STD_I64LE)
_HIPPIO_H5_PRE_T(unsigned char,       STD_U8LE_T,   H5T_STD_U8LE)
_HIPPIO_H5_PRE_T(unsigned short,      STD_U16LE_T,  H5T_STD_U16LE)
_HIPPIO_H5_PRE_T(unsigned int,        STD_U32LE_T,  H5T_STD_U32LE)
_HIPPIO_H5_PRE_T(unsigned long,       STD_U64LE_T,  H5T_STD_U64LE)
_HIPPIO_H5_PRE_T(unsigned long long,  STD_U64LE_T,  H5T_STD_U64LE)

_HIPPIO_H5_PRE_T(const float,               IEEE_F32LE_T, H5T_IEEE_F32LE)
_HIPPIO_H5_PRE_T(const double,              IEEE_F64LE_T, H5T_IEEE_F64LE)
_HIPPIO_H5_PRE_T(const long double,         IEEE_F64LE_T, H5T_IEEE_F64LE)
_HIPPIO_H5_PRE_T(const char,                STD_I8LE_T,   H5T_STD_I8LE)
_HIPPIO_H5_PRE_T(const signed char,         STD_I8LE_T,   H5T_STD_I8LE)
_HIPPIO_H5_PRE_T(const short,               STD_I16LE_T,  H5T_STD_I16LE)
_HIPPIO_H5_PRE_T(const int,                 STD_I32LE_T,  H5T_STD_I32LE)
_HIPPIO_H5_PRE_T(const long,                STD_I64LE_T,  H5T_STD_I64LE)
_HIPPIO_H5_PRE_T(const long long,           STD_I64LE_T,  H5T_STD_I64LE)
_HIPPIO_H5_PRE_T(const unsigned char,       STD_U8LE_T,   H5T_STD_U8LE)
_HIPPIO_H5_PRE_T(const unsigned short,      STD_U16LE_T,  H5T_STD_U16LE)
_HIPPIO_H5_PRE_T(const unsigned int,        STD_U32LE_T,  H5T_STD_U32LE)
_HIPPIO_H5_PRE_T(const unsigned long,       STD_U64LE_T,  H5T_STD_U64LE)
_HIPPIO_H5_PRE_T(const unsigned long long,  STD_U64LE_T,  H5T_STD_U64LE)

#undef _HIPPIO_H5_PRE_T

/** 
char *, const char *, string and their top-level const countparts are very 
special examples.
They are treated as HDF5 STRING atomic datatype and used with resized version
of the predefined datatype.

The ``size()`` method includes space for the null-term.
*/
template<>
struct DatatypeTraits<const char *, DatatypeArch::RESIZED_FIX> 
: DatatypeTraitsPredefined
{
    typedef const char *native_t;
    inline static const Datatype &datatype = C_S1_T;
    inline static const char *native_name = "const char *";
    inline static const char *h5_name = "H5T_C_S1";

    DatatypeTraits(native_t s) noexcept : _tr(s) {}

    size_t size() const noexcept { return _tr.size(); }
    native_t buff() const noexcept { return _tr.buff(); }
protected:
    _DatatypeTraits<native_t, _DatatypeArch::RESIZED_FIX> _tr;
};

template<>
struct DatatypeTraits<const char *const, DatatypeArch::RESIZED_FIX> 
: DatatypeTraitsPredefined
{
    typedef const char * const native_t;
    inline static const Datatype &datatype = C_S1_T;
    inline static const char *native_name = "const char * const";
    inline static const char *h5_name = "H5T_C_S1";

    DatatypeTraits(native_t s) noexcept : _tr(s) {}

    size_t size() const noexcept { return _tr.size(); }
    native_t buff() const noexcept { return _tr.buff(); }
protected:
    _DatatypeTraits<native_t, _DatatypeArch::RESIZED_FIX> _tr;
};

template<>
struct DatatypeTraits<char *, DatatypeArch::RESIZED_FIX> 
: DatatypeTraitsPredefined
{
    typedef char *native_t;
    inline static const Datatype &datatype = C_S1_T;
    inline static const char *native_name = "char *";
    inline static const char *h5_name = "H5T_C_S1";

    DatatypeTraits(native_t s) noexcept : _tr(s) {}

    size_t size() const noexcept { return _tr.size(); }
    native_t buff() const noexcept { return _tr.buff(); }
protected:
    _DatatypeTraits<native_t, _DatatypeArch::RESIZED_FIX> _tr;
};

template<>
struct DatatypeTraits<char * const, DatatypeArch::RESIZED_FIX> 
: DatatypeTraitsPredefined
{
    typedef char * const native_t;
    inline static const Datatype &datatype = C_S1_T;
    inline static const char *native_name = "char * const";
    inline static const char *h5_name = "H5T_C_S1";

    DatatypeTraits(native_t s) noexcept : _tr(s) {}

    size_t size() const noexcept { return _tr.size(); }
    native_t buff() const noexcept { return _tr.buff(); }
protected:
    _DatatypeTraits<native_t, _DatatypeArch::RESIZED_FIX> _tr;
};

template<>
struct DatatypeTraits<string, DatatypeArch::RESIZED_FIX> 
: DatatypeTraitsPredefined 
{
    typedef string native_t;
    inline static const Datatype &datatype = C_S1_T;
    inline static const char *native_name = "string";
    inline static const char *h5_name = "H5T_C_S1";

    DatatypeTraits(native_t &s) noexcept : _tr(s) {}

    size_t size() const noexcept { return _tr.size(); }
    char * buff() const noexcept { return _tr.buff(); }
protected:
    _DatatypeTraits<native_t, _DatatypeArch::RESIZED_FIX> _tr;
};

template<>
struct DatatypeTraits<const string, DatatypeArch::RESIZED_FIX> 
: DatatypeTraitsPredefined 
{
    typedef const string native_t;
    inline static const Datatype &datatype = C_S1_T;
    inline static const char *native_name = "const string";
    inline static const char *h5_name = "H5T_C_S1";

    DatatypeTraits(native_t &s) noexcept : _tr(s) {}

    size_t size() const noexcept { return _tr.size(); }
    const char * buff() const noexcept { return _tr.buff(); }
protected:
    _DatatypeTraits<native_t, _DatatypeArch::RESIZED_FIX> _tr;
};


/**
RawArray specializations.
*/
template<typename NativeT>
struct DatatypeTraits<NativeT, DatatypeArch::NATIVE, 
    std::enable_if_t< 
        RawArrayTraits<NativeT>::is_array && 
        DatatypeTraits<
            typename RawArrayTraits<NativeT>::value_t, 
            DatatypeArch::NATIVE>::has_h5_datatype > 
> : DatatypeTraitsCustomized 
{
    typedef NativeT native_t;

    typedef RawArrayTraits<NativeT> raw_array_traits_t;
    typedef typename raw_array_traits_t::value_t scalar_t;

    inline static const Datatype datatype 
        = Datatype::create_array_for<native_t, DatatypeArch::NATIVE>(); 
    inline static const char *native_name = "RawArray Protocol";
    inline static const char *h5_name = "RawArray Native Datatype";
};

template<typename NativeT>
struct DatatypeTraits<NativeT, DatatypeArch::STD, 
    std::enable_if_t< 
        RawArrayTraits<NativeT>::is_array && 
        DatatypeTraits<
            typename RawArrayTraits<NativeT>::value_t, 
            DatatypeArch::STD>::has_h5_datatype > 
> : DatatypeTraitsCustomized 
{
    typedef NativeT native_t;

    typedef RawArrayTraits<NativeT> raw_array_traits_t;
    typedef typename raw_array_traits_t::value_t scalar_t;

    inline static const Datatype datatype 
        = Datatype::create_array_for<native_t, DatatypeArch::STD>(); 
    inline static const char *native_name = "RawArray Protocol";
    inline static const char *h5_name = "RawArray Standard Datatype";
};

/**
Interface of the type mapping. Do not add specialization to this class template.
Instead, define specialization to DatatypeTraits.
*/
template<typename NativeT, 
    DatatypeArch TA = DatatypeArch::NATIVE, 
    typename V=void>
struct TypeCvt {
    inline static constexpr bool has_h5_datatype = false;
};

template<typename NativeT, DatatypeArch TA>
struct TypeCvt<NativeT, TA, 
    std::enable_if_t<DatatypeTraits<NativeT, TA>::has_h5_datatype> 
> {
    inline static constexpr bool has_h5_datatype = true;
    typedef DatatypeTraits<NativeT, TA> traits_t;
    typedef NativeT native_t;

    static string h5_name() { 
        return traits_t::h5_name;
    }
    static string native_name() { 
        return traits_t::native_name;
    }
    static const Datatype & datatype() noexcept {
        return traits_t::datatype;
    }
};


/**
Auxiliary class for mapping a field of a structured type to HDF5 datatype.
@R, M: the record/structured type and its member type.
    M must have DatatypeTraits defined.
*/
template<typename R, typename M>
struct TypeCvtField {
    typedef M R::*mem_ptr_t;
    typedef TypeCvt<M> type_cvt_t;

    typedef R record_t;
    typedef M field_t;

    typedef RawArrayTraits<field_t> raw_array_traits_t;

    explicit constexpr TypeCvtField(mem_ptr_t mem_ptr) noexcept;

    ptrdiff_t offset() const noexcept;

    constexpr static size_t size() noexcept;
    static const Datatype & datatype() noexcept;

    constexpr static bool is_raw_array() noexcept;
    /**
    If the field is RawArray,
    value_datatype() returns its value_t's datatype, extents() returns its 
    extents. Otherwise return the datatype of itself and {1}.
    The return of extents() is std::array<size_t, N>.
    */
    static const Datatype & value_datatype() noexcept;
    constexpr static auto extents() noexcept;
protected:
    mem_ptr_t _mem_ptr;
};

template<typename T, typename V=void>
struct TypeCvtBuff {
    inline static constexpr bool is_vector = false;
};
template<typename T, typename A>
struct TypeCvtBuff< vector<T, A> > {
    inline static constexpr bool is_vector = true;
    typedef T value_t;
    typedef A alloc_t;
};
template<typename T, typename A>
struct TypeCvtBuff< const vector<T, A> > {
    inline static constexpr bool is_vector = true;
    typedef T value_t;
    typedef A alloc_t;
};

} // namespace HIPP::IO::H5

#endif	//_HIPPIO_H5_OBJ_DATATYPE_H_