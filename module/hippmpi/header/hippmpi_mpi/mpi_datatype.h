/**
create: Yangyao CHEN, 2020/01/21
    [write   ] Datatype - the high-level interface of MPI datatype.
*/ 

#ifndef _HIPPMPI_MPI_DATATYPE_H_
#define _HIPPMPI_MPI_DATATYPE_H_
#include "mpi_obj_base.h"
#include "mpi_raw_datatype.h"
#include <cstdint>
#include <complex>
namespace HIPP::MPI {
    
/**
One Datatype instance is exactly equivalent to a MPI original datatype,
with automatically managed life time. For any datatype, you may ( but
not necessary) call free() manually so that you can free it in advance.
*/
class File;
class Env;

class Datatype: public MPIObj<_Datatype> {
public:
    typedef MPIObj<_Datatype> _obj_base_t;
    using _obj_base_t::_obj_base_t;

    /**
    Contiguous buffer types that provide unified interfaces for all 
    ContiguousBufferTraits -comformable types.

    ``int_but_t``: contiguous buffer of constant integers. It is used, 
    e.g., to specify the the lengths and displacements of the chunks in a 
    customized datatype.
        
    ``aint_buf_t``: contiguous buffer of constant address-size integers. 
    It is used, e.g., to specify the (byte-)displacements of chunks in a 
    customized datatype.
    */

    typedef ContiguousBuffer<const int> int_buf_t;
    typedef ContiguousBuffer<const aint_t> aint_buf_t;

    /** Default contructor - a null value i.e., ``is_null() -> false``. */
    Datatype() noexcept;

    /**
    ``info()`` prints a short (``fmt_cntl=0``) or a verbose (``fmt_cntl=1``) 
    description of the current instance to the stream ``os``.

    Operator ``<<`` is equivalent to ``info()`` with ``fmt_cntl=0``.

    The passed stream ``os`` is returned.
    */
    ostream &info( ostream &os = cout, int fmt_cntl = 1 ) const;
    friend ostream & operator<<( ostream &os, const Datatype &dtype );

    /**
    Free the object and set it to nullval as returned by nullval().
    It is safe to call this function at any time, or even multiple times, 
    except for exactly the predefined objects (They are ``const``. But a 
    non-const copy can be freed). 
    */
    void free() noexcept;
    
    /** Inquiry the information about this datatype. */
    bool is_null() const;
    void extent( aint_t &lb, aint_t &ext ) const;
    void true_extent( aint_t &lb, aint_t &ext ) const;
    int size() const;

    /**
    New datatype constructors.
    dup(): create a duplication of the current communicator.
    nullval(): return a null value, corresponding to MPI_DATATYPE_NULL.
    Other functions create derived datatypes.
    */
    Datatype dup() const;
    static Datatype nullval() noexcept;
    

    /**
    New datatype constructors. Assume the current instance describe a datatype
    T:

    resized: set the lower bound and extent to ``lb`` and ``ext``, respectively.
    and return the new datatype.
    
    contiguous: contiguous-layout ``count`` items, each typed T.
    
    vector: a vector datatype that consists of ``counts`` blocks. Each block 
    has ``blklen`` contiguous items typed T. Blocks are separated by ``stride`` 
    (in the unit of T).
    
    hvector: similar to ``vector`` except that ``stride`` is in the unit of
    byte.

    indexed_block: similar to ``vector`` but further allowing unequal strides
    specified by displacements ``displs`` (in the unit of T).

    hindexed_block: similar to ``indexed_block`` except that displs is in the 
    unit of byte.

    indexed: similar to ``indexed_block`` but allowing unequal sized blocks 
    specified by ``blklens``.

    hindexed: similar to ``indexed`` except that ``displs`` are in the unit of 
    byte.

    struct_: most general case. Blocks has different lengths, byte 
    displacements, and datatypes specified by ``dtypes``.

    For the convenience of distributing array data among processes, the 
    following two datatypes are defined:

    darray: Cartesian decompose the array type T into blocks, return the 
    datatype that corresponds to one of them.
    @size, rank: total number of blocks and the index of the desired block
        (assuming row-major order).
    @gsizes: dimensions of the whole array.
    @distribs: specifying the distribution algorithm at dimensions. Each can be
        - DISTRIBUTE_NONE (the first block owns all, the others get none).
        - DISTRIBUTE_BLOCK (each block is contiguous).
        - DISTRIBUTE_CYCLIC (each block cyclicly owns elements).
    @dargs: arguments to the distribution algorithm. Each can be
        - DISTRIBUTE_DFLT_DARG: most uniform way for BLOCK distribution, 
          and 1 for CYCLIC.
        - an integer: size of the block (must has darg * psize >= gsize at this
          dimension) for BLOCK distribution, or the size each cyclic chunk.
    @psizes: number of blocks at dimensions. ``1`` for non-distribution at 
        this dimension.

    subarray: a subarray datatype in a larger array typed T.
    @sizes: dimensions of the whole array.
    @subsizes, starts: dimensions at starting location of the subarray.
    @order: ORDER_C or ORDER_FORTRAN, the storage order.
    */
    Datatype resized(aint_t lb, aint_t ext) const;
    Datatype contiguous(int count) const;
    Datatype vector(int count, int blklen, int stride) const;
    Datatype hvector(int count, int blklen, aint_t stride) const;

    Datatype indexed_block(int count, int blklen, const int displs[]) const;
    Datatype indexed_block(int blklen, int_buf_t displs) const;
    
    Datatype hindexed_block(int count, int blklen, const aint_t displs[]) const;
    Datatype hindexed_block(int blklen, aint_buf_t displs) const;

    Datatype indexed(int count, const int blklens[], const int displs[]) const;
    Datatype indexed(int_buf_t blklens, int_buf_t displs) const;
    
    Datatype hindexed(int count, const int blklens[], 
        const aint_t displs[]) const;
    Datatype hindexed(int_buf_t blklens, aint_buf_t displs) const;

    static Datatype struct_(int count, const int blklens[], 
        const aint_t displs[], const mpi_t dtypes[]);
    static Datatype struct_(int count, const int blklens[], 
        const aint_t displs[], const Datatype dtypes[]);
    static Datatype struct_(int_buf_t blklens, aint_buf_t displs, 
        ContiguousBuffer<const Datatype> dtypes);

    Datatype darray(int size, int rank, int ndims, 
        const int gsizes[], const int distribs[], 
        const int dargs[], const int psizes[], int order = ORDER_C)const;
    Datatype darray(int size, int rank, int_buf_t gsizes, int_buf_t distribs, 
        int_buf_t dargs, int_buf_t psizes, int order = ORDER_C)const;

    Datatype subarray(int ndims, const int sizes[], const int subsizes[], 
        const int starts[], int order = ORDER_C)const;
    Datatype subarray(int_buf_t sizes, int_buf_t subsizes, int_buf_t starts, 
        int order = ORDER_C)const;

    /** 
    ``add_customized_cache``: 
    add a user-defined datatype to the cache, so that it is freed at the exit of 
    the MPI environment.
    ``clear_customized_cache``: clean the cache, so that all datatypes gets 
    freed (only if all the copies of the ``Datatype`` instance gets destroyed
    will the underlying datatype get freed).
    */
    static void add_customized_cache(Datatype *dtype);
    static void clear_customized_cache() noexcept;

    /**
    Add or remove a named datatype, so that the method ``from_name()`` accepts
    that name.
    */
    static void add_named_datatype(const string &name, const Datatype *dtype);
    static void remove_named_datatype(const string &name);
    
    /** 
    Get the Datatype instance from its C++ type. 
    @NativeT: a DatatypeTraits comformable type, i.e., 
        ``DatatypeTraits<NativeT>::has_mpi_datatype == true``.

    User may extend the traits by adding specializations.
    */
    template<typename NativeT>
    static const Datatype & from_type();

    /**
    Map string to Datatype instance.
    
    @name: could be one of the following
    (1) "byte", "char", "bool"; 
    (2) "X", "signed X", "unsigned X" where X is either
        char, short, int, long, or long long;
    (3) "intX_t", "uintX_t" where X can be either 8, 16, 32, or 64;
    (4) "float", "double", "long double".

    Other named datatype are allowed if manually added, e.g., with 
    ``add_named_datatype()``.
    */
    static const Datatype & from_name(const string &name);
protected:
    friend class File;
    friend class Env;

    static std::unordered_map<string, const Datatype *> _name2types;
    inline thread_local static std::vector<Datatype *> _customized_cache;

    static Datatype _from_raw( mpi_t dtype, int state ) noexcept;
    static void _init_predefined_datatypes() noexcept;
    static void _free_predefined_datatypes() noexcept;
};

/** Predefined datatypes. */
extern Datatype CHAR;
extern Datatype WCHAR;

extern Datatype SIGNED_CHAR;
extern Datatype SHORT;
extern Datatype INT;
extern Datatype LONG;
extern Datatype LLONG;

extern Datatype UNSIGNED_CHAR;
extern Datatype UNSIGNED_SHORT;
extern Datatype UNSIGNED_INT;
extern Datatype UNSIGNED_LONG;
extern Datatype UNSIGNED_LLONG;

extern Datatype INT8;
extern Datatype INT16;
extern Datatype INT32;
extern Datatype INT64;
extern Datatype UINT8;
extern Datatype UINT16;
extern Datatype UINT32;
extern Datatype UINT64;

extern Datatype FLOAT;
extern Datatype DOUBLE;
extern Datatype LDOUBLE;

extern Datatype C_COMPLEX;
extern Datatype C_FLOAT_COMPLEX;
extern Datatype C_DOUBLE_COMPLEX;
extern Datatype C_LDOUBLE_COMPLEX;

extern Datatype C_BOOL;
extern Datatype BOOL;
extern Datatype FLOAT_COMPLEX;
extern Datatype DOUBLE_COMPLEX;
extern Datatype LDOUBLE_COMPLEX;

extern Datatype BYTE;
extern Datatype PACKED;
extern Datatype AINT;
extern Datatype OFFSET;
extern Datatype COUNT;

extern Datatype FLOAT_INT;
extern Datatype DOUBLE_INT;
extern Datatype LDOUBLE_INT;
extern Datatype SHORT_INT;
extern Datatype INT_INT;
extern Datatype LONG_INT;


/** 
Traits for mapping a C++ type to its MPI Datatype. DatatypeTraits defines the 
Datatype protocol.

If a C++ type, ``T``, has specialization ``DatatypeTraits<T>``, and satisfies
the following requirements, it is compliant to this protocol.
(1) Has a member ``static constexpr bool has_mpi_datatype = true``.
(2) Has a member constexpr bool value, ``is_prededefined``. If ``T`` has a 
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
derive from ``DatatypeTraitsCustomized``.

The requirement (5) is only for debugging purpose. It is valid to return an 
empty string.
*/
class DatatypeTraitsPredefined {
public:
    inline static constexpr bool has_mpi_datatype = true,
        is_predefined = true;
};

class DatatypeTraitsCustomized {
public:
    inline static constexpr bool has_mpi_datatype = true,
        is_predefined = false;
};

template<typename NativeT, typename V =void>
class DatatypeTraits {
public:
    inline static constexpr bool has_mpi_datatype = false;
};

/** 
Specializations for C++ native types that has predefined MPI datatype 
counterpart. 

Supported types are
- ``bool``, ``char``, ``signed char`` or ``unsigned char``;
- ``X`` or  ``unsigned X``, where ``X`` can be ``short``, ``int``, ``long`` 
  or ``long long``;
- ``float``, ``double`` or ``long double``;
- ``std::complex<float>``, ``std::complex<double>`` 
  or ``std::complex<long double>``.

Their const qualified types are also supported, 
referring to the same Datatype object.
*/
#define _HIPPMPI_MPI_DEFINE_DATATYPE_TRAITS(_native_t, _datatype, _mpi_name) \
template<> \
struct DatatypeTraits<_native_t> : DatatypeTraitsPredefined { \
    typedef _native_t native_t; \
    inline static const Datatype * datatype = &_datatype; \
    inline static const char * mpi_name = #_mpi_name; \
};

_HIPPMPI_MPI_DEFINE_DATATYPE_TRAITS(
    char, CHAR, MPI_CHAR)
_HIPPMPI_MPI_DEFINE_DATATYPE_TRAITS(
    signed char, SIGNED_CHAR, MPI_SIGNED_CHAR)
_HIPPMPI_MPI_DEFINE_DATATYPE_TRAITS(
    short, SHORT, MPI_SHORT)
_HIPPMPI_MPI_DEFINE_DATATYPE_TRAITS(
    int, INT, MPI_INT)
_HIPPMPI_MPI_DEFINE_DATATYPE_TRAITS(
    long, LONG, MPI_LONG)
_HIPPMPI_MPI_DEFINE_DATATYPE_TRAITS(
    long long, LLONG, MPI_LONG_LONG)
_HIPPMPI_MPI_DEFINE_DATATYPE_TRAITS(
    unsigned char, UNSIGNED_CHAR, MPI_UNSIGNED_CHAR)
_HIPPMPI_MPI_DEFINE_DATATYPE_TRAITS(
    unsigned short, UNSIGNED_SHORT, MPI_UNSIGNED_SHORT)
_HIPPMPI_MPI_DEFINE_DATATYPE_TRAITS(
    unsigned int, UNSIGNED_INT, MPI_UNSIGNED)
_HIPPMPI_MPI_DEFINE_DATATYPE_TRAITS(
    unsigned long, UNSIGNED_LONG, MPI_UNSIGNED_LONG)
_HIPPMPI_MPI_DEFINE_DATATYPE_TRAITS(
    unsigned long long, UNSIGNED_LLONG, MPI_UNSIGNED_LONG_LONG)
_HIPPMPI_MPI_DEFINE_DATATYPE_TRAITS(
    float, FLOAT, MPI_FLOAT)
_HIPPMPI_MPI_DEFINE_DATATYPE_TRAITS(
    double, DOUBLE, MPI_DOUBLE)
_HIPPMPI_MPI_DEFINE_DATATYPE_TRAITS(
    long double, LDOUBLE, MPI_LONG_DOUBLE)
_HIPPMPI_MPI_DEFINE_DATATYPE_TRAITS(
    bool, BOOL, MPI_CXX_BOOL)
_HIPPMPI_MPI_DEFINE_DATATYPE_TRAITS(
    std::complex<float>, FLOAT_COMPLEX, MPI_CXX_FLOAT_COMPLEX)
_HIPPMPI_MPI_DEFINE_DATATYPE_TRAITS(
    std::complex<double>, DOUBLE_COMPLEX, MPI_CXX_DOUBLE_COMPLEX)
_HIPPMPI_MPI_DEFINE_DATATYPE_TRAITS(
    std::complex<long double>, LDOUBLE_COMPLEX, MPI_CXX_LONG_DOUBLE_COMPLEX)

_HIPPMPI_MPI_DEFINE_DATATYPE_TRAITS(
    const char, CHAR, MPI_CHAR)
_HIPPMPI_MPI_DEFINE_DATATYPE_TRAITS(
    const signed char, SIGNED_CHAR, MPI_SIGNED_CHAR)
_HIPPMPI_MPI_DEFINE_DATATYPE_TRAITS(
    const short, SHORT, MPI_SHORT)
_HIPPMPI_MPI_DEFINE_DATATYPE_TRAITS(
    const int, INT, MPI_INT)
_HIPPMPI_MPI_DEFINE_DATATYPE_TRAITS(
    const long, LONG, MPI_LONG)
_HIPPMPI_MPI_DEFINE_DATATYPE_TRAITS(
    const long long, LLONG, MPI_LONG_LONG)
_HIPPMPI_MPI_DEFINE_DATATYPE_TRAITS(
    const unsigned char, UNSIGNED_CHAR, MPI_UNSIGNED_CHAR)
_HIPPMPI_MPI_DEFINE_DATATYPE_TRAITS(
    const unsigned short, UNSIGNED_SHORT, MPI_UNSIGNED_SHORT)
_HIPPMPI_MPI_DEFINE_DATATYPE_TRAITS(
    const unsigned int, UNSIGNED_INT, MPI_UNSIGNED)
_HIPPMPI_MPI_DEFINE_DATATYPE_TRAITS(
    const unsigned long, UNSIGNED_LONG, MPI_UNSIGNED_LONG)
_HIPPMPI_MPI_DEFINE_DATATYPE_TRAITS(
    const unsigned long long, UNSIGNED_LLONG, MPI_UNSIGNED_LONG_LONG)
_HIPPMPI_MPI_DEFINE_DATATYPE_TRAITS(
    const float, FLOAT, MPI_FLOAT)
_HIPPMPI_MPI_DEFINE_DATATYPE_TRAITS(
    const double, DOUBLE, MPI_DOUBLE)
_HIPPMPI_MPI_DEFINE_DATATYPE_TRAITS(
    const long double, LDOUBLE, MPI_LONG_DOUBLE)
_HIPPMPI_MPI_DEFINE_DATATYPE_TRAITS(
    const bool, BOOL, MPI_CXX_BOOL)
_HIPPMPI_MPI_DEFINE_DATATYPE_TRAITS(
    const std::complex<float>, FLOAT_COMPLEX, MPI_CXX_FLOAT_COMPLEX)
_HIPPMPI_MPI_DEFINE_DATATYPE_TRAITS(
    const std::complex<double>, DOUBLE_COMPLEX, MPI_CXX_DOUBLE_COMPLEX)
_HIPPMPI_MPI_DEFINE_DATATYPE_TRAITS(
    const std::complex<long double>, LDOUBLE_COMPLEX, MPI_CXX_LONG_DOUBLE_COMPLEX)

#undef _HIPPMPI_MPI_DEFINE_DATATYPE_TRAITS

template<typename NativeT, typename V=void> struct TypeCvt;

/** 
Specialization for any RawArray protocol (see ``HIPP::RawArrayTraits``).

A RawArray type with scalar type ``value_t`` and number of elements ``size``
is mapped to the MPI contiguous datatype sized ``size`` with element type 
``value_t``.
*/
template<typename NativeT> 
class DatatypeTraits<NativeT, std::enable_if_t<
    RawArrayTraits<NativeT>::is_array && 
    DatatypeTraits<typename RawArrayTraits<NativeT>::value_t>::has_mpi_datatype
> > 
: public DatatypeTraitsCustomized
{
public:
    typedef NativeT native_t;

    typedef RawArrayTraits<NativeT> raw_array_traits_t;
    typedef typename raw_array_traits_t::value_t scalar_t;
    inline static constexpr size_t size = raw_array_traits_t::size;

    static const Datatype & datatype() {
        if( !_datatype.has_referenced() ) {
            _datatype = TypeCvt<scalar_t>::datatype().contiguous(size);
            Datatype::add_customized_cache(&_datatype);
        }
        return _datatype;
    }

    static string mpi_name() {
        return str("RawArray (as contiguous datatype sized ", size, ")");
    }
private:
    inline thread_local static Datatype _datatype {nullptr};
};


/** 
Interface of the type mapping. Do not add specialization to this class template.
Instead, define specialization to DatatypeTraits.
*/
template<typename NativeT, typename V>
struct TypeCvt {
    inline static constexpr bool has_mpi_datatype = false;
};

template<typename NativeT>
struct TypeCvt<NativeT, 
    std::enable_if_t<DatatypeTraits<NativeT>::is_predefined> > 
{
    inline static constexpr bool has_mpi_datatype = true;

    typedef DatatypeTraits<NativeT> traits_t;
    static_assert(traits_t::has_mpi_datatype, 
        "DatatypeTraits specialization not found");

    typedef typename traits_t::native_t native_t;
    static const Datatype & datatype() noexcept { 
        return *traits_t::datatype; 
    }
    static string mpi_name() { 
        return traits_t::mpi_name; 
    }
};

template<typename NativeT>
struct TypeCvt<NativeT, 
    std::enable_if_t< ! DatatypeTraits<NativeT>::is_predefined> > 
{
    inline static constexpr bool has_mpi_datatype = true;

    typedef DatatypeTraits<NativeT> traits_t;
    static_assert(traits_t::has_mpi_datatype, 
        "DatatypeTraits specialization not found");
    
    typedef typename traits_t::native_t native_t;
    static const Datatype & datatype() { 
        return traits_t::datatype();
    }
    static string mpi_name() { 
        return traits_t::mpi_name(); 
    }
};


inline ostream & operator<<( ostream &os, const Datatype &dtype ){
    return dtype.info(os, 0);
}

inline Datatype Datatype::_from_raw( mpi_t dtype, int state ) noexcept{
    return Datatype( std::make_shared<_obj_raw_t>(dtype, state) );
}

template<typename NativeT>
inline const Datatype & Datatype::from_type() {
    return TypeCvt<NativeT>::datatype();
}

inline const Datatype & Datatype::from_name(const string &name) {
    auto it = _name2types.find(name);
    if( it == _name2types.end() ) 
        ErrLogic::throw_(ErrLogic::eDEFAULT, emFLPFB, 
            "  ... datatype named ", name, " is not found");
    return *it->second;
}


} // namespace HIPP::MPI
#endif	//_HIPPMPI_MPI_DATATYPE_H_