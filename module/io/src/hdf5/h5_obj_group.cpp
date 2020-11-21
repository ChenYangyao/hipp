#include <h5_obj_group.h>
namespace HIPP {
namespace IO{

H5Dataset H5Group::create_dataset_str( const string &name, size_t len,
    const string &flag, const H5Proplist &lcprop, 
    const H5Proplist &cprop, const H5Proplist &aprop ){
    return H5Dataset::create_str( raw(), name, len, flag, lcprop, 
        cprop, aprop );
}
H5Dataset H5Group::open_dataset( const string &name, 
    const H5Proplist &aprop ){
    return H5Dataset::open( raw(), name, aprop );
}
bool H5Group::dataset_exists( const string &name )const{
    return H5Dataset::exists( raw(), name );
}

H5Attr H5Group::create_attr_str(
    const string &name, size_t len, const string &flag){
    return H5Attr::create_str( raw(), name, len, flag );
}
H5Attr H5Group::open_attr(const string &name){
    return H5Attr::open(raw(), name);
}
bool H5Group::attr_exists(const string &name) const{
    return bool( _obj_ptr->exist_attr(name.c_str()) );
}

H5Group H5Group::create_group( const string &name ){
    return create(raw(), name);
}
H5Group H5Group::try_create_group( const string &name ) {
    return try_create(raw(), name);
}
H5Group H5Group::open_group( const string &name ){
    return open(raw(), name);
}
bool H5Group::group_exists( const string &name ) const {
    return exists(raw(), name);
}

/**
 * protected member functions
 */
H5Group H5Group::create( id_t loc, const string &name ){
    return _from_raw(_obj_raw_t::create(loc, name.c_str()));
}
H5Group H5Group::try_create( id_t loc, const string &name ){
    H5Group dgrp(nullptr);
    try {
        _H5EStackTempOff estk(H5E_DEFAULT);
        dgrp = create(loc, name);
    }catch( const ErrH5 &e ){
        dgrp = open(loc, name);
    }
    return dgrp;
}
H5Group H5Group::open( id_t loc, const string &name ){
    return _from_raw(_obj_raw_t::open(loc, name.c_str()));
}
bool H5Group::exists( id_t loc, const string &name ){
    bool ans = true;
    try{
        _H5EStackTempOff estk(H5E_DEFAULT);
        auto dgrp = open(loc, name);
    }catch(const ErrH5 &e){
        ans = false;
    }
    return ans;
}

} // namespace IO
} // namespace HIPP
