#include "mpi_datatype.h"
namespace HIPP{
namespace MPI{

Datatype::Datatype() noexcept: 
    _obj_base_t( std::make_shared<_obj_raw_t>(_obj_raw_t::nullval(), 0) ){ }

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
        if( is_null() ) prt(os, "  Null") << endl;
        else{
            int _size = size();
            aint_t lb, ext, tlb, text;
            extent( lb, ext ); true_extent(tlb, text);
            prt( os, "  Size info (size=", _size, 
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
    const Datatype name{ std::make_shared<_Datatype>(mpiname, 0) };

_HIPPMPI_MPI_PRETYPE(CHAR, MPI_CHAR)
_HIPPMPI_MPI_PRETYPE(WCHAR, MPI_WCHAR)

_HIPPMPI_MPI_PRETYPE(SIGNED_CHAR, MPI_SIGNED_CHAR)
_HIPPMPI_MPI_PRETYPE(SHORT, MPI_SHORT)
_HIPPMPI_MPI_PRETYPE(INT, MPI_INT)
_HIPPMPI_MPI_PRETYPE(LONG, MPI_LONG)
_HIPPMPI_MPI_PRETYPE(LLONG, MPI_LONG_LONG)

_HIPPMPI_MPI_PRETYPE(UNSIGNED_CHAR, MPI_UNSIGNED_CHAR)
_HIPPMPI_MPI_PRETYPE(UNSIGNED_SHORT, MPI_UNSIGNED_SHORT)
_HIPPMPI_MPI_PRETYPE(UNSIGNED_INT, MPI_UNSIGNED)
_HIPPMPI_MPI_PRETYPE(UNSIGNED_LONG, MPI_UNSIGNED_LONG)
_HIPPMPI_MPI_PRETYPE(UNSIGNED_LLONG, MPI_UNSIGNED_LONG_LONG)

_HIPPMPI_MPI_PRETYPE(FLOAT, MPI_FLOAT)
_HIPPMPI_MPI_PRETYPE(DOUBLE, MPI_DOUBLE)
_HIPPMPI_MPI_PRETYPE(LDOUBLE, MPI_LONG_DOUBLE)

_HIPPMPI_MPI_PRETYPE(C_BOOL, MPI_C_BOOL)
_HIPPMPI_MPI_PRETYPE(INT8, MPI_INT8_T)
_HIPPMPI_MPI_PRETYPE(INT16, MPI_INT16_T)
_HIPPMPI_MPI_PRETYPE(INT32, MPI_INT32_T)
_HIPPMPI_MPI_PRETYPE(INT64, MPI_INT64_T)
_HIPPMPI_MPI_PRETYPE(UINT8, MPI_UINT8_T)
_HIPPMPI_MPI_PRETYPE(UINT16, MPI_UINT16_T)
_HIPPMPI_MPI_PRETYPE(UINT32, MPI_UINT32_T)
_HIPPMPI_MPI_PRETYPE(UINT64, MPI_UINT64_T)
_HIPPMPI_MPI_PRETYPE(C_COMPLEX, MPI_C_COMPLEX)
_HIPPMPI_MPI_PRETYPE(C_FLOAT_COMPLEX, MPI_C_FLOAT_COMPLEX)
_HIPPMPI_MPI_PRETYPE(C_DOUBLE_COMPLEX, MPI_C_DOUBLE_COMPLEX)
_HIPPMPI_MPI_PRETYPE(C_LDOUBLE_COMPLEX, MPI_C_LONG_DOUBLE_COMPLEX)

_HIPPMPI_MPI_PRETYPE(BYTE, MPI_BYTE)
_HIPPMPI_MPI_PRETYPE(PACKED, MPI_PACKED)

_HIPPMPI_MPI_PRETYPE(BOOL, MPI_CXX_BOOL)
_HIPPMPI_MPI_PRETYPE(FLOAT_COMPLEX, MPI_CXX_FLOAT_COMPLEX)
_HIPPMPI_MPI_PRETYPE(DOUBLE_COMPLEX, MPI_CXX_DOUBLE_COMPLEX)
_HIPPMPI_MPI_PRETYPE(LDOUBLE_COMPLEX, MPI_CXX_LONG_DOUBLE_COMPLEX)

_HIPPMPI_MPI_PRETYPE(FLOAT_INT, MPI_FLOAT_INT)
_HIPPMPI_MPI_PRETYPE(DOUBLE_INT, MPI_DOUBLE_INT)
_HIPPMPI_MPI_PRETYPE(LDOUBLE_INT, MPI_LONG_DOUBLE_INT)
_HIPPMPI_MPI_PRETYPE(SHORT_INT, MPI_SHORT_INT)
_HIPPMPI_MPI_PRETYPE(INT_INT, MPI_2INT)
_HIPPMPI_MPI_PRETYPE(LONG_INT, MPI_LONG_INT)

#undef _HIPPMPI_MPI_PRETYPE

#define _HIPPMPI_MPI_TYPECVT(_native_t, _datatype, _mpi_name)\
const Datatype *const _TypeCvt<_native_t>::datatype = &_datatype; \
const char *const _TypeCvt<_native_t>::mpi_name = #_mpi_name;

_HIPPMPI_MPI_TYPECVT(char, CHAR, MPI_CHAR)
_HIPPMPI_MPI_TYPECVT(int8_t, INT8, MPI_INT8_T)
_HIPPMPI_MPI_TYPECVT(int16_t, INT16, MPI_INT16_T)
_HIPPMPI_MPI_TYPECVT(int32_t, INT32, MPI_INT32_T)
_HIPPMPI_MPI_TYPECVT(int64_t, INT64, MPI_INT64_T)
_HIPPMPI_MPI_TYPECVT(uint8_t, UINT8, MPI_UINT8_T)
_HIPPMPI_MPI_TYPECVT(uint16_t, UINT16, MPI_UINT16_T)
_HIPPMPI_MPI_TYPECVT(uint32_t, UINT32, MPI_UINT32_T)
_HIPPMPI_MPI_TYPECVT(uint64_t, UINT64, MPI_UINT64_T)
_HIPPMPI_MPI_TYPECVT(long long, LLONG, MPI_LONG_LONG)
_HIPPMPI_MPI_TYPECVT(unsigned long long, UNSIGNED_LLONG, MPI_UNSIGNED_LONG_LONG)
_HIPPMPI_MPI_TYPECVT(float, FLOAT, MPI_FLOAT)
_HIPPMPI_MPI_TYPECVT(double, DOUBLE, MPI_DOUBLE)
_HIPPMPI_MPI_TYPECVT(long double, LDOUBLE, MPI_LONG_DOUBLE)
_HIPPMPI_MPI_TYPECVT(bool, BOOL, MPI_CXX_BOOL)
_HIPPMPI_MPI_TYPECVT(std::complex<float>, FLOAT_COMPLEX, MPI_CXX_FLOAT_COMPLEX)
_HIPPMPI_MPI_TYPECVT(std::complex<double>, DOUBLE_COMPLEX, MPI_CXX_DOUBLE_COMPLEX)
_HIPPMPI_MPI_TYPECVT(std::complex<long double>, LDOUBLE_COMPLEX, MPI_CXX_LONG_DOUBLE_COMPLEX)

#undef _HIPPMPI_MPI_TYPECVT

const std::unordered_map<string, const Datatype *> _typecvt = {
    {"byte", &BYTE},

    {"char", &CHAR},
    {"unsigned char", &UNSIGNED_CHAR},
    {"signed char", &SIGNED_CHAR},
    
    {"short", &SHORT},
    {"signed short", &SHORT},
    {"unsigned short", &UNSIGNED_SHORT},
    
    {"int", &INT},
    {"signed int", &INT},
    {"unsigned int", &UNSIGNED_INT},

    {"long", &LONG},
    {"signed long", &LONG},
    {"unsigned long", &UNSIGNED_LONG},
    
    {"long long", &LLONG},
    {"signed long long", &LLONG},
    {"unsigned long long", &UNSIGNED_LLONG},
    
    {"int8_t", &INT8},
    {"int16_t", &INT16},
    {"int32_t", &INT32},
    {"int64_t", &INT64},
    {"uint8_t", &UINT8},
    {"uint16_t", &UINT16},
    {"uint32_t", &UINT32},
    {"uint64_t", &UINT64},
    
    {"float", &FLOAT},
    {"double", &DOUBLE},
    {"long double", &LDOUBLE},

    {"bool", &BOOL}
};


} // namespace MPI
} // namespace HIPP
