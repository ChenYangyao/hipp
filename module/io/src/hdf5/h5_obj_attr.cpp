#include <h5_obj_attr.h>
namespace HIPP {
namespace IO {

H5Dataspace H5Attr::dataspace(){
    auto ptr = std::make_shared<H5Dataspace::_obj_raw_t>( 
        _obj_ptr->get_space() );
    return H5Dataspace( ptr );
}

const H5Dataspace H5Attr::dataspace() const { 
    return const_cast<H5Attr*>(this)->dataspace(); 
}

H5Datatype H5Attr::datatype(){
    auto ptr = std::make_shared<H5Datatype::_obj_raw_t>( 
        _obj_ptr->get_type() );
    return H5Datatype( ptr );
}

const H5Datatype H5Attr::datatype() const { 
    return const_cast<H5Attr*>(this)->datatype(); 
}

void H5Attr::write( const string &buff ){
    typedef H5TypeStr str_t;
    _H5Datatype dtype( _H5Datatype::copy(str_t::h5_id) );
    dtype.set_size( buff.size()+1 );
    _obj_ptr->write( dtype.raw(), buff.c_str() );
}

void H5Attr::read( string &buff ) const{
    auto filetype = datatype();
    auto len = filetype.size();
    vector<char> _cbuf( len );
    _obj_ptr->read( filetype.raw(), _cbuf.data() );
    buff.assign( (const char *)_cbuf.data() );
}

H5Attr H5Attr::create_str( id_t loc, const string &name, size_t len,
    const string &flag)
{
    typedef H5TypeStr str_t;
    _H5Datatype dtype( _H5Datatype::copy( str_t::h5_id ) );
    dtype.set_size( len );
    return create(loc, name, dtype, H5Dataspace::scalarval.obj_raw(), flag);
}

H5Attr H5Attr::create( id_t loc, const string &name, 
    const _H5Datatype &dtype, const _H5Dataspace &dspace, 
    const string &flag)
{
    H5Attr attr(NULL);
    try{
        _H5EStackTempOff estk(H5E_DEFAULT);
        auto ptr = std::make_shared<_obj_raw_t>( 
            loc, name.c_str(), dtype.raw(), dspace.raw() );
        attr = H5Attr( ptr );
    }catch( const ErrH5 &e ){
        if( flag == "trunc" ){
            attr = open(loc, name);
        }else if( flag == "excl" ){
            ErrH5::throw_(-1, emFLPFB, "  ... attribute ", name, " exists\n");
        }else{
            ErrLogic::throw_(ErrLogic::eINVALIDARG, 
                emFLPFB,
                "  ... invalid flag ", flag, '\n');
        }
    }
    return attr;
}

H5Attr H5Attr::open( id_t loc, const string &name ) {
    auto ptr = std::make_shared<_obj_raw_t>( 
        _obj_raw_t::open( loc, name.c_str() ));
    return H5Attr( ptr );
}

} // namespace IO
} // namespace HIPP