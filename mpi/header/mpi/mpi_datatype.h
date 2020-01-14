#ifndef _HIPPMPI_MPI_DATATYPE_H_
#define _HIPPMPI_MPI_DATATYPE_H_
#include "mpi_obj_base.h"
#include "mpi_raw_datatype.h"
namespace HIPP{
namespace MPI{
    
class Datatype: public MPIObj<_Datatype> {
public:
    typedef MPIObj<_Datatype> _obj_base_t;
    using _obj_base_t::_obj_base_t;
    ostream &info( ostream &os = cout, int fmt_cntl = 1 ) const;
    friend ostream & operator<<( ostream &os, const Datatype &dtype );

    void free() noexcept;
    bool is_null() const;
    void extent( aint_t &lb, aint_t &ext ) const;
    void true_extent( aint_t &lb, aint_t &ext ) const;
    int size() const;
    Datatype dup() const;
    static Datatype nullval() noexcept;

    Datatype resized( aint_t lb, aint_t ext ) const;
    Datatype contiguous( int count ) const;
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
    static Datatype _from_raw( mpi_t dtype, int state );
};

inline ostream & operator<<( ostream &os, const Datatype &dtype ){
    return dtype.info(os);
}
inline Datatype Datatype::_from_raw( mpi_t dtype, int state ){
    return Datatype( std::make_shared<_obj_raw_t>(dtype, state) );
}

extern Datatype CHAR;
extern Datatype SHORT;
extern Datatype INT;
extern Datatype LONG;
extern Datatype LLONG;
extern Datatype FLOAT;
extern Datatype DOUBLE;

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
_HIPPMPI_MPI_TYPECVT(short)
_HIPPMPI_MPI_TYPECVT(int)
_HIPPMPI_MPI_TYPECVT(long)
_HIPPMPI_MPI_TYPECVT(long long)
_HIPPMPI_MPI_TYPECVT(float)
_HIPPMPI_MPI_TYPECVT(double)

#undef _HIPPMPI_MPI_TYPECVT

extern std::unordered_map<string, const Datatype *> _typecvt;


} // namespace MPI
} // namespace HIPP
#endif	//_HIPPMPI_MPI_DATATYPE_H_