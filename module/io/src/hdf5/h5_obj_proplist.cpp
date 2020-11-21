#include "h5_obj_proplist.h"
namespace HIPP{
namespace IO{

const H5Proplist H5Proplist::defaultval 
    { std::make_shared<H5Proplist::_obj_raw_t>(H5P_DEFAULT, 0) };
H5Proplist & H5Proplist::set_layout( const string &layout ){
    _obj_raw_t::layout_t _layout = H5D_CONTIGUOUS;
    if( layout == "contiguous" )
        _layout = H5D_CONTIGUOUS;
    else if( layout == "chunked" )
        _layout = H5D_CHUNKED;
    else if( layout == "compact" )
        _layout = H5D_COMPACT;
    else if( layout == "virtual" )
        _layout = H5D_VIRTUAL;
    else 
        ErrLogic::throw_( ErrLogic::eDOMAIN, emFLPFB, 
            "  ... layout specification ", layout, " not allowed" );
    _obj_ptr->set_layout( _layout );
    return *this;
}
string H5Proplist::layout() const{
    _obj_raw_t::layout_t _layout = _obj_ptr->get_layout();
    string ans;
    switch (_layout){
    case H5D_CONTIGUOUS:
        ans = "contiguous"; break;
    case H5D_CHUNKED:
        ans = "chunked"; break;
    case H5D_COMPACT:
        ans = "compact"; break;
    case H5D_VIRTUAL:
        ans = "virtual"; break;
    default:
        ErrRuntime::throw_( ErrRuntime::eDEFAULT, emFLPFB, 
            "  ... unknown error" );
    }
    return ans;
}

H5Proplist & H5Proplist::set_chunk( vector<size_t> dims ){
    _obj_ptr->set_chunk( dims.size(), dims.data() );
    return *this;
}
int H5Proplist::chunk_ndims()const{
    return _obj_ptr->get_chunk_ndims();
}
vector<H5Proplist::size_t> H5Proplist::chunk() const{
    return _obj_ptr->get_chunk();
}

H5Proplist & H5Proplist::set_chunk_cache( 
    std::size_t nslot, std::size_t nbytes, double w0 ){
    _obj_ptr->set_chunk_cache( nslot, nbytes, w0 );
    return *this;
}
void H5Proplist::chunk_cache( std::size_t &nslot, 
    std::size_t &nbytes, double &w0 ) const{
    _obj_ptr->get_chunk_cache( &nslot, &nbytes, &w0 );
}

H5Proplist & H5Proplist::set_cache( std::size_t nslot, 
    std::size_t nbytes, double w0 ){
    _obj_ptr->set_cache( 0, nslot, nbytes, w0 );
    return *this;
}
void H5Proplist::cache( std::size_t &nslot, 
    std::size_t &nbytes, double &w0 ) const{
    _obj_ptr->get_cache( NULL, &nslot, &nbytes, &w0 );
}

}
}