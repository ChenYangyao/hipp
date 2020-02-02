#ifndef _HIPPIO_H5_OBJ_GROUP_H_
#define _HIPPIO_H5_OBJ_GROUP_H_
#include "h5_obj_base.h"
#include "h5_obj_attr.h"
#include "h5_obj_dataset.h"
namespace HIPP{
namespace IO{

class H5Group: public H5Obj<_H5Group>{
public:
    typedef H5Obj<_H5Group> _obj_base_t;
    using _obj_base_t::_obj_base_t;

    template<typename T>
    H5Dataset create_dataset( const string &name, const vector<size_t> &dims, 
        const string &flag="trunc",
        const H5Proplist &lcprop = H5Proplist::defaultval,
        const H5Proplist &cprop = H5Proplist::defaultval,
        const H5Proplist &aprop = H5Proplist::defaultval );
    H5Dataset open_dataset( const string &name,
        const H5Proplist &aprop = H5Proplist::defaultval );
    bool dataset_exists( const string &name )const;

    template<typename T>
    H5Attr create_attr(
        const string &name, const vector<size_t> &dims, 
        const string &flag="trunc");
    H5Attr open_attr(const string &name);
    bool attr_exists(const string &name) const;

    H5Group create_group( const string &name );
    H5Group open_group( const string &name );
};

template<typename T>
H5Dataset H5Group::create_dataset( const string &name, 
    const vector<size_t> &dims, 
    const string &flag, 
    const H5Proplist &lcprop, const H5Proplist &cprop, const H5Proplist &aprop){
    return H5Dataset::create<T>( 
        raw(), name, dims, flag, lcprop, cprop, aprop );
}
inline H5Dataset H5Group::open_dataset( const string &name, 
    const H5Proplist &aprop ){
    return H5Dataset::open( raw(), name, aprop );
}
inline bool H5Group::dataset_exists( const string &name )const{
    return H5Dataset::exists( raw(), name );
}
template<typename T>
H5Attr H5Group::create_attr(
    const string &name, const vector<size_t> &dims, 
    const string &flag){
    return H5Attr::create<T>( raw(), name, dims, flag );
}
inline H5Attr H5Group::open_attr(const string &name){
    return H5Attr::open(raw(), name);
}
inline bool H5Group::attr_exists(const string &name) const{
    return bool( _obj_ptr->exist_attr(name.c_str()) );
}
inline H5Group H5Group::create_group( const string &name ){
    auto id = _obj_raw_t::create( raw(), name.c_str() );
    return H5Group( std::make_shared<_obj_raw_t>( id ) );
}
inline H5Group H5Group::open_group( const string &name ){
    auto id = _obj_raw_t::open( raw(), name.c_str() );
    return H5Group( std::make_shared<_obj_raw_t>( id ) );
}


}
}
#endif	//_HIPPIO_H5_OBJ_GROUP_H_