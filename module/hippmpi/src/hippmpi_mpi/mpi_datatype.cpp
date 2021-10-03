#include <mpi_datatype.h>
namespace HIPP{
namespace MPI{

Datatype::Datatype() noexcept: 
    _obj_base_t( std::make_shared<_obj_raw_t>(_obj_raw_t::nullval(), 0) ){ }

ostream & Datatype::info( ostream &os, int fmt_cntl ) const{
    PStream ps(os);
    bool null_dtype = is_null();
    if( null_dtype ) {
        if( fmt_cntl == 0 ) 
            ps << "Datatype{Null}";
        else
            ps << HIPPCNTL_CLASS_INFO(HIPP::MPI::Datatype), " Null\n";
        return os;
    }
    int _size = size();
    aint_t lb, ext, tlb, text;
    extent( lb, ext ); 
    true_extent(tlb, text);
    if(fmt_cntl == 0){
        ps << "Datatype{size=", _size, ", lower bound=", lb, ", extent=", ext, 
            ", true lower bound=", tlb, ", true extent=", text, "}";
    } else {
        ps << HIPPCNTL_CLASS_INFO(HIPP::MPI::Datatype),
            "  Size{", _size, "}\n",
            "  Lower bound{", lb, ", true=", tlb, "}\n",
            "  Extent{", ext, ", true=", text, "}\n";
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

Datatype Datatype::indexed_block(int count, int blklen, 
    const int displs[]) const
{
    return _from_raw( 
        _obj_raw_t::indexed_block(count, blklen, displs, raw()),
        _obj_raw_t::stFREE | _obj_raw_t::stCOMMIT);
}

Datatype Datatype::indexed_block(int blklen, int_buf_t displs) const {
    const auto &[p, n] = displs;
    return indexed_block(static_cast<int>(n), blklen, p);
}

Datatype Datatype::hindexed_block(int count, int blklen, 
    const aint_t displs[]) const 
{
    return _from_raw( 
        _obj_raw_t::hindexed_block(count, blklen, displs, raw()),
        _obj_raw_t::stFREE | _obj_raw_t::stCOMMIT);
}

Datatype Datatype::hindexed_block(int blklen, aint_buf_t displs) const {
    const auto &[p, n] = displs;
    return hindexed_block(static_cast<int>(n), blklen, p);
}

Datatype Datatype::indexed(int count, const int blklens[], 
    const int displs[]) const
{
    return _from_raw( 
        _obj_raw_t::indexed(count, blklens, displs, raw()),
        _obj_raw_t::stFREE | _obj_raw_t::stCOMMIT);
}

Datatype Datatype::indexed(int_buf_t blklens, int_buf_t displs) const {
    const auto &[plen, nlen] = blklens;
    const auto &[pdisp, ndisp] = displs;
    if( nlen != ndisp )
        ErrLogic::throw_(ErrLogic::eLENGTH, emFLPFB, 
        "  ... lengths of blklens (", nlen, ") != displs (", ndisp, ")\n");
    return indexed(static_cast<int>(nlen), plen, pdisp);
}

Datatype Datatype::hindexed(int count, const int blklens[], 
    const aint_t displs[]) const
{
    return _from_raw( 
        _obj_raw_t::hindexed(count, blklens, displs, raw()),
        _obj_raw_t::stFREE | _obj_raw_t::stCOMMIT);
}

Datatype Datatype::hindexed(int_buf_t blklens, aint_buf_t displs) const {
    const auto &[plen, nlen] = blklens;
    const auto &[pdisp, ndisp] = displs;
    if( nlen != ndisp )
        ErrLogic::throw_(ErrLogic::eLENGTH, emFLPFB, 
        "  ... lengths of blklens (", nlen, ") != displs (", ndisp, ")\n");
    return hindexed(static_cast<int>(nlen), plen, pdisp);
}

Datatype Datatype::struct_(int count, const int blklens[], 
    const aint_t displs[], const mpi_t dtypes[]) 
{
    return _from_raw( 
        _obj_raw_t::struct_(count, blklens, displs, dtypes),
        _obj_raw_t::stFREE | _obj_raw_t::stCOMMIT);    
}

Datatype Datatype::struct_(int count, const int blklens[], 
    const aint_t displs[], const Datatype dtypes[]) 
{
    std::vector<mpi_t> _dtypes(count);
    for(int i=0; i<count; ++i) _dtypes[i] = dtypes[i].raw();
    return struct_(count, blklens, displs, _dtypes.data());
}

Datatype Datatype::struct_(int_buf_t blklens, aint_buf_t displs, 
    ContiguousBuffer<const Datatype> dtypes)
{
    const auto &[plen, nlen] = blklens;
    const auto &[pdisp, ndisp] = displs;
    const auto &[pdtype, ndtype] = dtypes;
    if( nlen != ndisp || nlen != ndtype )
        ErrLogic::throw_(ErrLogic::eLENGTH, emFLPFB, 
        "  ... lengths of blklens (", nlen, "), displs (", ndisp, 
        "), and dtypes (", ndtype, ") do not match\n" );
    return struct_(static_cast<int>(nlen), plen, pdisp, pdtype);
}

Datatype Datatype::darray(int size, int rank, int ndims, 
    const int gsizes[], const int distribs[], 
    const int dargs[], const int psizes[], int order)const
{
    return _from_raw( 
        _obj_raw_t::darray( size, rank, ndims, gsizes,
            distribs, dargs, psizes, order, raw()), 
        _obj_raw_t::stFREE | _obj_raw_t::stCOMMIT );
}

Datatype Datatype::darray(int size, int rank, int_buf_t gsizes, 
    int_buf_t distribs, int_buf_t dargs, int_buf_t psizes, int order)const
{
    const auto &[pgsize, ngsize] = gsizes;
    const auto &[pdist, ndist] = distribs;
    const auto &[pdarg, ndarg] = dargs;
    const auto &[ppsize, npsize] = psizes;
    if( ngsize != ndist || ngsize != ndarg || ngsize != npsize )
        ErrLogic::throw_(ErrLogic::eLENGTH, emFLPFB, 
            "  ... lengths of gsizes, distribs, dargs, and psizes (", 
            ngsize, ", ", ndist, ", ", ndarg, ", and ", npsize, 
            ") do not match\n");
    return darray(size, rank, static_cast<int>(ngsize), pgsize,
            pdist, pdarg, ppsize, order);
}

Datatype Datatype::subarray(int ndims, const int sizes[], const int subsizes[], 
    const int starts[], int order)const
{
    return _from_raw( 
        _obj_raw_t::subarray(ndims, 
            sizes, subsizes, starts, order, raw()), 
        _obj_raw_t::stFREE | _obj_raw_t::stCOMMIT );
}

Datatype Datatype::subarray(int_buf_t sizes, int_buf_t subsizes, 
    int_buf_t starts, int order)const
{
    const auto &[psz, nsz] = sizes;
    const auto &[psub, nsub] = subsizes;
    const auto &[pst, nst] = starts;
    if( nsz != nsub || nsz != nst ) 
        ErrLogic::throw_(ErrLogic::eLENGTH, emFLPFB, 
            "  ... lengths of sizes (", nsz, "), subsizes (", nsub, 
            "), and starts (", nst, ") do not match\n");
    return subarray(static_cast<int>(nsz), 
            psz, psub, pst, order);
} 

void Datatype::add_customized_cache(Datatype *dtype) {
    _customized_cache.push_back(dtype);
}

void Datatype::clear_customized_cache() noexcept {
    for(const auto &p_dtype: _customized_cache)
        p_dtype->operator=(Datatype {nullptr});
    _customized_cache.clear();
}

void Datatype::add_named_datatype(const string &name, const Datatype *dtype) {
    auto it = _name2types.find(name);
    if( it != _name2types.end() )
        ErrLogic::throw_(ErrLogic::eOUTOFRANGE, 
            emFLPFB, "  ... datatype named ", name, 
            " already exists");
    _name2types.emplace(name, dtype);
}

void Datatype::remove_named_datatype(const string &name) {
    size_t n = _name2types.erase(name);
    if( n == 0 )
        ErrLogic::throw_(ErrLogic::eOUTOFRANGE, 
            emFLPFB, "  ... datatype named ", name, " is not found");
}

void Datatype::_init_predefined_datatypes() noexcept {
    CHAR = _from_raw(MPI_CHAR, 0);
    WCHAR = _from_raw(MPI_WCHAR, 0);

    SIGNED_CHAR = _from_raw(MPI_SIGNED_CHAR, 0);
    SHORT = _from_raw(MPI_SHORT, 0);
    INT = _from_raw(MPI_INT, 0);
    LONG = _from_raw(MPI_LONG, 0);
    LLONG = _from_raw(MPI_LONG_LONG, 0);

    UNSIGNED_CHAR = _from_raw(MPI_UNSIGNED_CHAR, 0);
    UNSIGNED_SHORT = _from_raw(MPI_UNSIGNED_SHORT, 0);
    UNSIGNED_INT = _from_raw(MPI_UNSIGNED, 0);
    UNSIGNED_LONG = _from_raw(MPI_UNSIGNED_LONG, 0);
    UNSIGNED_LLONG = _from_raw(MPI_UNSIGNED_LONG_LONG, 0);
    
    INT8 = _from_raw(MPI_INT8_T, 0);
    INT16 = _from_raw(MPI_INT16_T, 0);
    INT32 = _from_raw(MPI_INT32_T, 0);
    INT64 = _from_raw(MPI_INT64_T, 0);
    
    UINT8 = _from_raw(MPI_UINT8_T, 0);
    UINT16 = _from_raw(MPI_UINT16_T, 0);
    UINT32 = _from_raw(MPI_UINT32_T, 0);
    UINT64 = _from_raw(MPI_UINT64_T, 0);
    
    FLOAT = _from_raw(MPI_FLOAT, 0);
    DOUBLE = _from_raw(MPI_DOUBLE, 0);
    LDOUBLE = _from_raw(MPI_LONG_DOUBLE, 0);
    
    C_COMPLEX = _from_raw(MPI_C_COMPLEX, 0);
    C_FLOAT_COMPLEX = _from_raw(MPI_C_FLOAT_COMPLEX, 0);
    C_DOUBLE_COMPLEX = _from_raw(MPI_C_DOUBLE_COMPLEX, 0);
    C_LDOUBLE_COMPLEX = _from_raw(MPI_C_LONG_DOUBLE_COMPLEX, 0);
    
    C_BOOL = _from_raw(MPI_C_BOOL, 0);
    BOOL = _from_raw(MPI_CXX_BOOL, 0);
    FLOAT_COMPLEX = _from_raw(MPI_CXX_FLOAT_COMPLEX, 0);
    DOUBLE_COMPLEX = _from_raw(MPI_CXX_DOUBLE_COMPLEX, 0);
    LDOUBLE_COMPLEX = _from_raw(MPI_CXX_LONG_DOUBLE_COMPLEX, 0);
    
    BYTE = _from_raw(MPI_BYTE, 0);
    PACKED = _from_raw(MPI_PACKED, 0);
    AINT = _from_raw(MPI_AINT, 0);
    OFFSET = _from_raw(MPI_OFFSET, 0);
    COUNT = _from_raw(MPI_COUNT, 0);
    FLOAT_INT = _from_raw(MPI_FLOAT_INT, 0);
    DOUBLE_INT = _from_raw(MPI_DOUBLE_INT, 0);
    LDOUBLE_INT = _from_raw(MPI_LONG_DOUBLE_INT, 0);
    SHORT_INT = _from_raw(MPI_SHORT_INT, 0);
    INT_INT = _from_raw(MPI_2INT, 0);
    LONG_INT = _from_raw(MPI_LONG_INT, 0);
}

void Datatype::_free_predefined_datatypes() noexcept { 
    clear_customized_cache();
}

Datatype CHAR {nullptr};
Datatype WCHAR {nullptr};
Datatype SIGNED_CHAR {nullptr};
Datatype SHORT {nullptr};
Datatype INT {nullptr};
Datatype LONG {nullptr};
Datatype LLONG {nullptr};
Datatype UNSIGNED_CHAR {nullptr};
Datatype UNSIGNED_SHORT {nullptr};
Datatype UNSIGNED_INT {nullptr};
Datatype UNSIGNED_LONG {nullptr};
Datatype UNSIGNED_LLONG {nullptr};
Datatype INT8 {nullptr};
Datatype INT16 {nullptr};
Datatype INT32 {nullptr};
Datatype INT64 {nullptr};
Datatype UINT8 {nullptr};
Datatype UINT16 {nullptr};
Datatype UINT32 {nullptr};
Datatype UINT64 {nullptr};
Datatype FLOAT {nullptr};
Datatype DOUBLE {nullptr};
Datatype LDOUBLE {nullptr};
Datatype C_COMPLEX {nullptr};
Datatype C_FLOAT_COMPLEX {nullptr};
Datatype C_DOUBLE_COMPLEX {nullptr};
Datatype C_LDOUBLE_COMPLEX {nullptr};
Datatype C_BOOL {nullptr};
Datatype BOOL {nullptr};
Datatype FLOAT_COMPLEX {nullptr};
Datatype DOUBLE_COMPLEX {nullptr};
Datatype LDOUBLE_COMPLEX {nullptr};
Datatype BYTE {nullptr};
Datatype PACKED {nullptr};
Datatype AINT {nullptr};
Datatype OFFSET {nullptr};
Datatype COUNT {nullptr};
Datatype FLOAT_INT {nullptr};
Datatype DOUBLE_INT {nullptr};
Datatype LDOUBLE_INT {nullptr};
Datatype SHORT_INT {nullptr};
Datatype INT_INT {nullptr};
Datatype LONG_INT {nullptr};

std::unordered_map<string, const Datatype *> 
Datatype::_name2types = {
    {"byte", &BYTE},
    {"wchar", &WCHAR},
    {"bool", &BOOL},
    {"float complex", &FLOAT_COMPLEX},
    {"double complex", &DOUBLE_COMPLEX},
    {"long double complex", &LDOUBLE_COMPLEX},
    {"packed", &PACKED},
    {"aint", &AINT},
    {"offset", &OFFSET},
    {"count", &COUNT},
    {"float int", &FLOAT_INT},
    {"double int", &DOUBLE_INT},
    {"long double int", &LDOUBLE_INT},
    {"short int", &SHORT_INT},
    {"int int", &INT_INT},
    {"long int", &LONG_INT},

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
    {"long double", &LDOUBLE}
};


} // namespace MPI
} // namespace HIPP
