#include <h5_obj_dataset.h>
namespace HIPP {
namespace IO {

H5Dataspace H5Dataset::dataspace(){
    auto ptr = std::make_shared<H5Dataspace::_obj_raw_t>( 
        _obj_ptr->get_space() );
    return H5Dataspace( ptr );        
}
const H5Dataspace H5Dataset::dataspace() const
    { return const_cast<H5Dataset *>(this)->dataspace(); }

H5Datatype H5Dataset::datatype(){
    auto ptr = std::make_shared<H5Datatype::_obj_raw_t>( 
        _obj_ptr->get_type() );
    return H5Datatype( ptr );
}
const H5Datatype H5Dataset::datatype() const
    { return const_cast<H5Dataset *>(this)->datatype(); }

H5Attr H5Dataset::open_attr(const string &name){
    return H5Attr::open( raw(), name );
}
bool H5Dataset::attr_exists(const string &name) const{
    return bool(_obj_ptr->exist_attr(name.c_str()));
}
void H5Dataset::write( const string &buff, 
    const H5Proplist &xprop ){
    typedef H5TypeStr str_t;
    _H5Datatype dtype( _H5Datatype::copy( str_t::h5_id ) );
    dtype.set_size( buff.size()+1 );    
    _obj_ptr->write( dtype.raw(), H5S_ALL, H5S_ALL, xprop.raw(), buff.c_str() );
}
void H5Dataset::read( string &buff, 
    const H5Proplist &xprop ) const{
    auto filetype = datatype();
    auto len = filetype.size();
    vector<char> _cbuf(len);
    _obj_ptr->read( filetype.raw(), H5S_ALL, H5S_ALL, xprop.raw(), 
        _cbuf.data() );
    buff.assign( (const char *)_cbuf.data() );
}
H5Dataset H5Dataset::create_str( 
    id_t loc, const string &name, size_t len, const string &flag,
    const H5Proplist &lcprop,
    const H5Proplist &cprop,
    const H5Proplist &aprop ){
    typedef H5TypeStr str_t;
    _H5Datatype dtype( _H5Datatype::copy( str_t::h5_id ) );
    dtype.set_size( len );    
    return create( loc, name, dtype, H5Dataspace::scalarval.obj_raw(), 
        flag, lcprop, cprop, aprop );
}

H5Dataset H5Dataset::create( 
    id_t loc, const string &name, 
    const _H5Datatype &dtype, const _H5Dataspace &dspace, 
    const string &flag,
    const H5Proplist &lcprop,
    const H5Proplist &cprop,
    const H5Proplist &aprop )
{
    H5Dataset dset(NULL);
    try{
        _H5EStackTempOff estk(H5E_DEFAULT);
        dset = H5Dataset(
            std::make_shared<_obj_raw_t>( loc, name.c_str(), 
                dtype.raw(), dspace.raw(), lcprop.raw(), 
                cprop.raw(), aprop.raw() ));
    }catch( const ErrH5 &e ){
        if( flag == "trunc" ){
            dset = open( loc, name, aprop );
        }else if( flag == "excl" ){
            ErrH5::throw_( -1, emFLPFB, "  ... dataset ", name, " exists\n" );
        }else{
            ErrLogic::throw_(ErrLogic::eINVALIDARG, emFLPFB, 
                "  ... invalid flag ", flag, '\n');
        }
    }
    return dset;   
}
H5Dataset H5Dataset::open( id_t loc, const string &name,
    const H5Proplist &aprop )
{
    auto ptr = std::make_shared<_obj_raw_t>( 
        _obj_raw_t::open( loc, name.c_str(), aprop.raw() ) );
    return H5Dataset( ptr );
}
bool H5Dataset::exists( id_t loc, const string &name ){
    bool exists = true;
    try{
        _H5EStackTempOff estk(H5E_DEFAULT);
        open( loc, name );
    }catch( const ErrH5 & ){
        exists = false;
    }
    return exists;    
}
H5Proplist H5Dataset::create_proplist( const string &cls ){
    id_t _cls = H5P_DATASET_CREATE;
    if( cls == "c" || cls == "create" )
        _cls = H5P_DATASET_CREATE;
    else if( cls == "a" || cls == "access" )
        _cls = H5P_DATASET_ACCESS;
    else if( cls == "x" || cls == "xfer" || cls == "transfer" )
        _cls = H5P_DATASET_XFER;
    else
        ErrLogic::throw_( ErrLogic::eDOMAIN, emFLPFB, 
            "   ... dataset property list class ", cls, " invalid "  );
    return H5Proplist::_from_raw( H5Proplist::_obj_raw_t::create( _cls ) );
}

H5Proplist H5Dataset::proplist(const string &cls) const{
    id_t plist;
    if( cls == "c" || cls == "create" )
        plist = H5Dget_create_plist( raw() );
    else if( cls == "a" || cls == "access" )
        plist = H5Dget_access_plist( raw() );
    else 
        ErrLogic::throw_( ErrLogic::eDOMAIN, emFLPFB, 
            "   ... dataset property list class ", cls, " invalid "  );
    ErrH5::check( plist, emFLPFB );
    return H5Proplist::_from_raw( plist );
}
} // namespace IO
} // namespace HIPP