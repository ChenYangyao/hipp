/**
 * creat: Yangyao CHEN, 2020/01/21
 *      [write   ] Datatype - the high-level interface of MPI datatype.
 */ 


#ifndef _HIPPMPI_MPI_DATATYPE_H_
#define _HIPPMPI_MPI_DATATYPE_H_
#include "mpi_obj_base.h"
#include "mpi_raw_datatype.h"
#include <cstdint>
#include <complex>
namespace HIPP{
namespace MPI{
    
/**
 * the high-level interface of MPI datatype.
 * 
 * One Datatype instance is exactly equivalent to a MPI original datatype,
 * with automatically managed life time. For any datatype, you may ( but
 * not necessary ) call free() manually so that you can free it in advance.
 */
class File;
class Datatype: public MPIObj<_Datatype> {
public:
    typedef MPIObj<_Datatype> _obj_base_t;
    using _obj_base_t::_obj_base_t;
    Datatype() noexcept;

    ostream &info( ostream &os = cout, int fmt_cntl = 1 ) const;
    friend ostream & operator<<( ostream &os, const Datatype &dtype );

    /**
     * free the object and set it to nullval as returned by nullval().
     * It is safe to call this function at any time, or even multiple times, 
     * except for exactly the predifined objects (They are `const`. But a 
     * non-const copy can be freed). 
     */
    void free() noexcept;
    /**
     * inquiry the information about this datatype.
     */
    bool is_null() const;
    void extent( aint_t &lb, aint_t &ext ) const;
    void true_extent( aint_t &lb, aint_t &ext ) const;
    int size() const;

    /**
     * new datatype constructors
     * dup()    - create a duplication of the current communicator.
     * nullval()- return a null value, corresponding to MPI_DATATYPE_NULL.
     * Other functions create derived datatypes.
     */
    Datatype dup() const;
    static Datatype nullval() noexcept;
    /**
     * reset the lower bound `lb` and extent `ext` of the datatype.
     */
    Datatype resized( aint_t lb, aint_t ext ) const;
    Datatype contiguous( int count ) const;
    /**
     * create a vector datatype, consisting of `count` blocks, each with
     * `blklen` contiguous elements, with stride `stride`.
     */
    Datatype vector( int count, int blklen, int stride ) const;
    Datatype hvector( int count, int blklen, aint_t stride ) const;
    Datatype indexed_block( int blklen, const std::vector<int> &displs ) const;
    Datatype hindexed_block( int blklen, 
        const std::vector<aint_t> &displs ) const;
    Datatype indexed( const std::vector<int> &blklens, 
        const std::vector<int> &displs ) const;
    Datatype hindexed( const std::vector<int> &blklens, 
        const std::vector<aint_t> &displs ) const;
    static Datatype struct_( const std::vector<int> &blklens, 
        const std::vector<aint_t> &displs, const std::vector<Datatype> &dtypes);
    Datatype darray( int size, int rank, const std::vector<int> &gsizes, 
        const std::vector<int> &distribs, 
        const std::vector<int> &dargs,
        const std::vector<int> &psizes, int order = ORDER_C )const;
    Datatype subarray( const std::vector<int> &sizes, 
        const std::vector<int> &subsizes, 
        const std::vector<int> &starts, int order = ORDER_C )const;
protected:
    friend class File;
    static Datatype _from_raw( mpi_t dtype, int state ) noexcept;
};

inline ostream & operator<<( ostream &os, const Datatype &dtype ){
    return dtype.info(os);
}
inline Datatype Datatype::_from_raw( mpi_t dtype, int state ) noexcept{
    return Datatype( std::make_shared<_obj_raw_t>(dtype, state) );
}

extern const Datatype CHAR;
extern const Datatype WCHAR;

extern const Datatype SIGNED_CHAR;
extern const Datatype SHORT;
extern const Datatype INT;
extern const Datatype LONG;
extern const Datatype LLONG;

extern const Datatype UNSIGNED_CHAR;
extern const Datatype UNSIGNED_SHORT;
extern const Datatype UNSIGNED_INT;
extern const Datatype UNSIGNED_LONG;
extern const Datatype UNSIGNED_LLONG;

extern const Datatype FLOAT;
extern const Datatype DOUBLE;
extern const Datatype LDOUBLE;

extern const Datatype C_BOOL;
extern const Datatype INT8;
extern const Datatype INT16;
extern const Datatype INT32;
extern const Datatype INT64;
extern const Datatype UINT8;
extern const Datatype UINT16;
extern const Datatype UINT32;
extern const Datatype UINT64;
extern const Datatype C_COMPLEX;
extern const Datatype C_FLOAT_COMPLEX;
extern const Datatype C_DOUBLE_COMPLEX;
extern const Datatype C_LDOUBLE_COMPLEX;

extern const Datatype BYTE;
extern const Datatype PACKED;

extern const Datatype BOOL;
extern const Datatype FLOAT_COMPLEX;
extern const Datatype DOUBLE_COMPLEX;
extern const Datatype LDOUBLE_COMPLEX;

extern const Datatype FLOAT_INT;
extern const Datatype DOUBLE_INT;
extern const Datatype LONG_DOUBLE_INT;
extern const Datatype SHORT_INT;
extern const Datatype INT_INT;
extern const Datatype LONG_INT;


template<typename NativeT>
class _TypeCvt{ };

#define _HIPPMPI_MPI_TYPECVT(_native_t) \
template<> \
class _TypeCvt<_native_t> { \
public: \
    typedef _native_t native_t; \
    static const Datatype *const datatype; \
    static const char *const mpi_name; \
};

_HIPPMPI_MPI_TYPECVT(char)
_HIPPMPI_MPI_TYPECVT(int8_t)
_HIPPMPI_MPI_TYPECVT(int16_t)
_HIPPMPI_MPI_TYPECVT(int32_t)
_HIPPMPI_MPI_TYPECVT(int64_t)
_HIPPMPI_MPI_TYPECVT(uint8_t)
_HIPPMPI_MPI_TYPECVT(uint16_t)
_HIPPMPI_MPI_TYPECVT(uint32_t)
_HIPPMPI_MPI_TYPECVT(uint64_t)
_HIPPMPI_MPI_TYPECVT(long long)
_HIPPMPI_MPI_TYPECVT(unsigned long long)
_HIPPMPI_MPI_TYPECVT(float)
_HIPPMPI_MPI_TYPECVT(double)
_HIPPMPI_MPI_TYPECVT(long double)
_HIPPMPI_MPI_TYPECVT(bool)
_HIPPMPI_MPI_TYPECVT(std::complex<float>)
_HIPPMPI_MPI_TYPECVT(std::complex<double>)
_HIPPMPI_MPI_TYPECVT(std::complex<long double>)

#undef _HIPPMPI_MPI_TYPECVT

extern const std::unordered_map<string, const Datatype *> _typecvt;


} // namespace MPI
} // namespace HIPP
#endif	//_HIPPMPI_MPI_DATATYPE_H_