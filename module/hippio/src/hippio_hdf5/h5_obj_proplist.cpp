#include <hippio.h>

namespace HIPP::IO::H5 {

const Proplist Proplist::vDFLT = _from_raw(_Proplist::vDFLT, 0);

Proplist Proplist::create(hid_t cls) {
    auto ret = _obj_raw_t::create(cls);
    return _from_raw(ret);
}

Proplist & Proplist::set_layout(d_layout_t layout) {
    obj_raw().set_layout(layout);
    return *this;
}

Proplist & Proplist::set_layout(const string &layout){
    d_layout_t lo {};
    if( layout == "contiguous" )
        lo = laCONTIGUOUS;
    else if( layout == "chunked" )
        lo = laCHUNKED;
    else if( layout == "compact" )
        lo = laCOMPACT;
    else if( layout == "virtual" )
        lo = laVIRTUAL;
    else 
        ErrLogic::throw_( ErrLogic::eDOMAIN, emFLPFB, 
            "  ... layout specification ", layout, " invalid\n" );
    return set_layout(lo);
}

auto Proplist::layout() const -> d_layout_t {
    return obj_raw().get_layout();
}

void Proplist::layout(string &lo) const {
    d_layout_t lo_value = layout();
    switch (lo_value){
        case laCONTIGUOUS:
            lo = "contiguous"; break;
        case laCHUNKED:
            lo = "chunked"; break;
        case laCOMPACT:
            lo = "compact"; break;
        case laVIRTUAL:
            lo = "virtual"; break;
        default:
            ErrRuntime::throw_( ErrRuntime::eDEFAULT, emFLPFB, 
                "  ... unknown layout value\n");
    }
}

Proplist & Proplist::set_chunk( const Dimensions &dims ){
    obj_raw().set_chunk( dims.ndims(), dims.data() );
    return *this;
}
int Proplist::chunk_ndims()const{
    return obj_raw().get_chunk_ndims();
}
Dimensions Proplist::chunk() const {
    return Dimensions(obj_raw().get_chunk());
}

Proplist & Proplist::set_chunk_cache( 
    size_t nslot, size_t nbytes, double w0 ){
    obj_raw().set_chunk_cache( nslot, nbytes, w0 );
    return *this;
}
void Proplist::chunk_cache( size_t &nslot, 
    size_t &nbytes, double &w0 ) const{
    obj_raw().get_chunk_cache( &nslot, &nbytes, &w0 );
}

std::tuple<size_t, size_t, double> Proplist::chunk_cache() const {
    size_t nslot, nbytes;
    double w0;
    chunk_cache(nslot, nbytes, w0);
    return {nslot, nbytes, w0};
}

Proplist & Proplist::set_cache( size_t nslot, 
    size_t nbytes, double w0 ){
    obj_raw().set_cache( 0, nslot, nbytes, w0 );
    return *this;
}

void Proplist::cache( size_t &nslot, size_t &nbytes, double &w0 ) const{
    obj_raw().get_cache( NULL, &nslot, &nbytes, &w0 );
}

std::tuple<size_t, size_t, double> Proplist::cache() const {
    size_t nslot, nbytes;
    double w0;
    cache(nslot, nbytes, w0);
    return {nslot, nbytes, w0};
}

Proplist & Proplist::preserve(hbool_t status) {
    obj_raw().set_preserve(status);
    return *this;
}

hbool_t Proplist::preserve() const {
    return (bool) obj_raw().get_preserve();
}

} // namespace HIPP::IO::H5