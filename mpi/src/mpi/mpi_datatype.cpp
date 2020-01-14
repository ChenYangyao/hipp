#include "mpi_datatype.h"
namespace HIPP{
namespace MPI{

ostream & Datatype::info( ostream &os, int fmt_cntl ) const{
    if(fmt_cntl == 0 ){
        prt( os, HIPPCNTL_CLASS_INFO_INLINE(HIPP::MPI::Datatype) );
        if( is_null() ) prt(os, "Null");
        else{
            int _size = size();
            aint_t lb, ext, tlb, text;
            extent( lb, ext ); true_extent(tlb, text);
            prt( os, "size: ", _size, ", lower bound: ", lb, "(", tlb, ")", 
                ", extent: ", ext, "(", text, ")" );
        }
    }
    if(fmt_cntl >= 1){
        prt( os, HIPPCNTL_CLASS_INFO(HIPP::MPI::Datatype) );
        if( is_null() ) prt(os, "Null") << endl;
        else{
            int _size = size();
            aint_t lb, ext, tlb, text;
            extent( lb, ext ); true_extent(tlb, text);
            prt( os, "Size info (size=", _size, 
                ", lower bound/true lower bound=", lb, "/", tlb, 
                ", extent/true extent=", ext, "/", text, ")" ) << endl;
        }
    }
    return os;
}
void Datatype::free() noexcept{
    *this = nullval();
}
bool Datatype::is_null() const{
    return _obj_ptr->is_null();
}
void Datatype::extent( aint_t &lb, aint_t &ext ) const{
    _obj_ptr->extent(lb, ext);
}
void Datatype::true_extent( aint_t &lb, aint_t &ext ) const{
    _obj_ptr->true_extent(lb, ext);
}
int Datatype::size() const{
    return _obj_ptr->size();
}
Datatype Datatype::dup() const{
    mpi_t obj = _obj_ptr->dup();
    return _from_raw( obj, _obj_raw_t::stFREE );
}
Datatype Datatype::nullval() noexcept{
    return _from_raw( _obj_raw_t::nullval(), 0 );
}
Datatype Datatype::resized( aint_t lb, aint_t ext ) const{
    return _from_raw( _obj_raw_t::resized( raw(), lb, ext ), 
        _obj_raw_t::stFREE | _obj_raw_t::stCOMMIT );
}
Datatype Datatype::contiguous( int count ) const{
    return _from_raw( _obj_raw_t::contiguous( count, raw() ),
        _obj_raw_t::stFREE | _obj_raw_t::stCOMMIT);
}
Datatype Datatype::vector( int count, int blklen, int stride ) const{
    return _from_raw( _obj_raw_t::vector( count, blklen, stride, raw() ),
        _obj_raw_t::stFREE | _obj_raw_t::stCOMMIT);
}
Datatype Datatype::hvector( int count, int blklen, aint_t stride ) const{
    return _from_raw( _obj_raw_t::hvector( count, blklen, stride, raw() ),
        _obj_raw_t::stFREE | _obj_raw_t::stCOMMIT);
}
Datatype Datatype::indexed_block( int blklen, 
    const std::vector<int> &displs ) const{
    return _from_raw( 
        _obj_raw_t::indexed_block( displs.size(), 
            blklen, displs.data(), raw() ),
        _obj_raw_t::stFREE | _obj_raw_t::stCOMMIT);
}
Datatype Datatype::hindexed_block( int blklen, 
    const std::vector<aint_t> &displs ) const{
    return _from_raw( 
        _obj_raw_t::hindexed_block( displs.size(),
            blklen, displs.data(), raw() ),
        _obj_raw_t::stFREE | _obj_raw_t::stCOMMIT);
}
Datatype Datatype::indexed( const std::vector<int> &blklens, 
    const std::vector<int> &displs ) const{
    if( blklens.size() != displs.size() )
        ErrLogic::throw_(ErrLogic::eLENGTH, emFLPFB, "  ... argument sizes"
            " do not match (blklens.size=", blklens.size(), ", displs.size=",
            displs.size(), ")\n" );
    return _from_raw( 
        _obj_raw_t::indexed( displs.size(),
            blklens.data(), displs.data(), raw() ),
        _obj_raw_t::stFREE | _obj_raw_t::stCOMMIT);
}
Datatype Datatype::hindexed( const std::vector<int> &blklens, 
    const std::vector<aint_t> &displs ) const{
    if( blklens.size() != displs.size() )
        ErrLogic::throw_(ErrLogic::eLENGTH, emFLPFB, "  ... argument sizes"
            " do not match (blklens.size=", blklens.size(), ", displs.size=",
            displs.size(), ")\n" );
    return _from_raw( 
        _obj_raw_t::hindexed( displs.size(),
            blklens.data(), displs.data(), raw() ),
        _obj_raw_t::stFREE | _obj_raw_t::stCOMMIT);
}
Datatype Datatype::struct_( const std::vector<int> &blklens, 
    const std::vector<aint_t> &displs, const std::vector<Datatype> &dtypes){
    if( blklens.size() != displs.size() || blklens.size() != dtypes.size() )
        ErrLogic::throw_(ErrLogic::eLENGTH, emFLPFB, "  ... argument sizes"
            " do not match (blklens.size=", blklens.size(), ", displs.size=",
            displs.size(), ", dtypes.size=", dtypes.size(), ")\n" );
    std::vector<mpi_t> _dtypes;
    for(const auto &dtype: dtypes) _dtypes.push_back( dtype.raw() );
    return _from_raw( 
        _obj_raw_t::struct_( displs.size(),
            blklens.data(), displs.data(), _dtypes.data() ),
        _obj_raw_t::stFREE | _obj_raw_t::stCOMMIT);
}
Datatype Datatype::darray( int size, int rank, const std::vector<int> &gsizes, 
    const std::vector<int> &distribs, 
    const std::vector<int> &dargs,
    const std::vector<int> &psizes, int order )const{
    size_t sizeg = gsizes.size(), sized = distribs.size(),
        sizea = dargs.size(), sizep = psizes.size();
    if( sizeg != sized || sizeg != sizea || sizeg != sizep )
        ErrLogic::throw_(ErrLogic::eLENGTH, emFLPFB, "  ... argument sizes"
            "do not match (gsizes/distribs/dargs/psizes.size=", sizeg, '/',
            sized, '/', sizea, '/', sizep, ")\n" );
    return _from_raw( 
        _obj_raw_t::darray( size, rank, sizeg, gsizes.data(),
            distribs.data(), dargs.data(), psizes.data(), order, raw() ), 
        _obj_raw_t::stFREE | _obj_raw_t::stCOMMIT );
}
Datatype Datatype::subarray( const std::vector<int> &sizes, 
    const std::vector<int> &subsizes, 
    const std::vector<int> &starts, int order )const{
    size_t size1 = sizes.size(), size2 = subsizes.size(),
        size3 = starts.size();
    if( size1 != size2 || size1 != size3 )
        ErrLogic::throw_(ErrLogic::eLENGTH, emFLPFB, "  ... argument sizes"
            "do not match (sizes/subsizes/starts.size=", size1, '/',
            size2, '/', size3, ")\n" );
    return _from_raw( 
        _obj_raw_t::subarray( size1, sizes.data(), subsizes.data(), 
            starts.data(), order, raw() ), 
        _obj_raw_t::stFREE | _obj_raw_t::stCOMMIT );
}


#define _HIPPMPI_MPI_PRETYPE(name, mpiname) \
    Datatype name{ std::make_shared<_Datatype>(mpiname, 0) };

_HIPPMPI_MPI_PRETYPE(BYTE, MPI_BYTE)
_HIPPMPI_MPI_PRETYPE(CHAR, MPI_CHAR)
_HIPPMPI_MPI_PRETYPE(SHORT, MPI_SHORT)
_HIPPMPI_MPI_PRETYPE(INT, MPI_INT)
_HIPPMPI_MPI_PRETYPE(LONG, MPI_LONG)
_HIPPMPI_MPI_PRETYPE(LLONG, MPI_LONG_LONG)
_HIPPMPI_MPI_PRETYPE(FLOAT, MPI_FLOAT)
_HIPPMPI_MPI_PRETYPE(DOUBLE, MPI_DOUBLE)

#undef _HIPPMPI_MPI_PRETYPE

#define _HIPPMPI_MPI_TYPECVT(_native_t, _datatype, _mpi_name)\
const Datatype *const _TypeCvt<_native_t>::datatype = &_datatype; \
const char *const _TypeCvt<_native_t>::mpi_name = #_mpi_name;

_HIPPMPI_MPI_TYPECVT(char, CHAR, MPI_CHAR)
_HIPPMPI_MPI_TYPECVT(short, SHORT, MPI_SHORT)
_HIPPMPI_MPI_TYPECVT(int, INT, MPI_INT)
_HIPPMPI_MPI_TYPECVT(long, LONG, MPI_LONG)
_HIPPMPI_MPI_TYPECVT(long long, LLONG, MPI_LONG_LONG)
_HIPPMPI_MPI_TYPECVT(float, FLOAT, MPI_FLOAT)
_HIPPMPI_MPI_TYPECVT(double, DOUBLE, MPI_DOUBLE)

#undef _HIPPMPI_MPI_TYPECVT

std::unordered_map<string, const Datatype *> _typecvt = {
    {"byte", &BYTE},
    {"char", &CHAR},
    {"short", &SHORT},
    {"int", &INT},
    {"long", &LONG},
    {"long long", &LLONG},
    {"float", &FLOAT},
    {"double", &DOUBLE}
};


} // namespace MPI
} // namespace HIPP
