#include <h5_obj_file.h>
namespace HIPP{
namespace IO{
    
H5File::H5File( const string &name, const string &flag, 
    const H5Proplist &cporp, const H5Proplist &aprop) 
    : _obj_base_t(nullptr)
{
    if( flag == "ac" || flag == "ca" ){
        try{
            _H5EStackTempOff estk(H5E_DEFAULT);
            _obj_ptr = std::make_shared<_obj_raw_t>(
                name.c_str(), "x", cporp.raw(), aprop.raw());
        }catch( const ErrH5 &e ){
            _obj_ptr = std::make_shared<_obj_raw_t>(
                name.c_str(), "a", cporp.raw(), aprop.raw());
        }
    }else{
        _obj_ptr = std::make_shared<_obj_raw_t>( 
            name.c_str(), flag, cporp.raw(), aprop.raw() );
    }
}
H5Dataset H5File::create_dataset_str( const string &name, size_t len,
    const string &flag, const H5Proplist &lcprop, 
    const H5Proplist &cprop, const H5Proplist &aprop ){
    return H5Dataset::create_str( raw(), name, len, flag, lcprop, 
        cprop, aprop );
}
H5Dataset H5File::open_dataset( const string &name, const H5Proplist &aprop ){
    return H5Dataset::open(raw(), name, aprop);
}
bool H5File::dataset_exists( const string &name ) const{
    return H5Dataset::exists( raw(), name );
}
H5Attr H5File::create_attr_str(
    const string &name, size_t len, const string &flag){
    return H5Attr::create_str( raw(), name, len, flag );
}

H5Attr H5File::open_attr(const string &name){
    return H5Attr::open( raw(), name );
}
bool H5File::attr_exists(const string &name) const{
    return bool( _obj_ptr->exist_attr( name.c_str() ) );
}
H5Group H5File::create_group( const string &name ){
    return H5Group::create(raw(), name);
}
H5Group H5File::try_create_group( const string &name ){
    return H5Group::try_create(raw(), name);
}
H5Group H5File::open_group( const string &name ){
    return H5Group::open(raw(), name);
}
bool H5File::group_exists( const string &name ) const {
    return H5Group::exists(raw(), name);
}
H5Proplist H5File::create_proplist(const string &cls){
    id_t _cls = H5P_FILE_CREATE;
    if( cls == "c" || cls == "create" )
        _cls = H5P_FILE_CREATE;
    else if( cls == "a" || cls == "access" )
        _cls = H5P_FILE_ACCESS;
    else if( cls == "m" || cls == "mount" )
        _cls = H5P_FILE_MOUNT;
    else 
        ErrLogic::throw_( ErrLogic::eDOMAIN, emFLPFB, 
            "   ... file property list class ", cls, " invalid "  );
    return H5Proplist::_from_raw( H5Proplist::_obj_raw_t::create( _cls ) );
}

} // namespace IO
} // namespace HIPP
