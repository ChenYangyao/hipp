/**
 * creat: Yangyao CHEN, 2020/11/29
 *      [write   ]
 *      @H5Group: High-level HDF5 group API.
 */ 


#ifndef _HIPPIO_H5_OBJ_GROUP_H_
#define _HIPPIO_H5_OBJ_GROUP_H_
#include "h5_obj_base.h"
#include "h5_obj_attr.h"
#include "h5_obj_dataset.h"
namespace HIPP{
namespace IO{
class H5File;

class H5Group: public H5Obj<_H5Group>{
public:
    typedef H5Obj<_H5Group> _obj_base_t;
    using _obj_base_t::_obj_base_t;

    template<typename T>
    H5Dataset create_dataset( const string &name, const vector<hsize_t> &dims, 
        const string &flag="trunc",
        const H5Proplist &lcprop = H5Proplist::defaultval,
        const H5Proplist &cprop = H5Proplist::defaultval,
        const H5Proplist &aprop = H5Proplist::defaultval );
    H5Dataset create_dataset(const string &name, const H5Datatype &dtype, 
        const vector<hsize_t> &dims, const string &flag="trunc", 
        const H5Proplist &lcprop = H5Proplist::defaultval,
        const H5Proplist &cprop = H5Proplist::defaultval,
        const H5Proplist &aprop = H5Proplist::defaultval);
    template<typename T>
    H5Dataset create_dataset_scalar( const string &name,
        const string &flag="trunc", 
        const H5Proplist &lcprop = H5Proplist::defaultval,
        const H5Proplist &cprop = H5Proplist::defaultval,
        const H5Proplist &aprop = H5Proplist::defaultval );
    H5Dataset create_dataset_str( const string &name, size_t len,
        const string &flag="trunc", 
        const H5Proplist &lcprop = H5Proplist::defaultval,
        const H5Proplist &cprop = H5Proplist::defaultval,
        const H5Proplist &aprop = H5Proplist::defaultval );

    H5Dataset open_dataset( const string &name,
        const H5Proplist &aprop = H5Proplist::defaultval );
    bool dataset_exists( const string &name )const;

    template<typename T>
    H5Attr create_attr(
        const string &name, const vector<hsize_t> &dims, 
        const string &flag="trunc");
    H5Attr create_attr(
        const string &name, const H5Datatype &dtype, 
        const vector<hsize_t> &dims, const string &flag="trunc");
    template<typename T>
    H5Attr create_attr_scalar(
        const string &name, const string &flag="trunc");
    H5Attr create_attr_str(
        const string &name, size_t len, const string &flag="trunc");

    H5Attr open_attr(const string &name);
    bool attr_exists(const string &name) const;

    H5Group create_group( const string &name );
    H5Group try_create_group( const string &name );
    H5Group open_group( const string &name );
    bool group_exists( const string &name ) const;
protected:
    friend class H5File;
    static H5Group create( id_t loc, const string &name );
    static H5Group try_create( id_t loc, const string &name );
    static H5Group open( id_t loc, const string &name );
    static bool exists( id_t loc, const string &name );
    static H5Group _from_raw(id_t loc) noexcept;
};

template<typename T>
H5Dataset H5Group::create_dataset( const string &name, 
    const vector<hsize_t> &dims, 
    const string &flag, 
    const H5Proplist &lcprop, const H5Proplist &cprop, const H5Proplist &aprop){
    return H5Dataset::create<T>( 
        raw(), name, dims, flag, lcprop, cprop, aprop );
}
template<typename T>
H5Dataset H5Group::create_dataset_scalar( const string &name,
    const string &flag, const H5Proplist &lcprop,
    const H5Proplist &cprop,const H5Proplist &aprop ){
    return H5Dataset::create_scalar<T>( raw(), name, flag, 
        lcprop, cprop, aprop );
}
template<typename T>
H5Attr H5Group::create_attr(
    const string &name, const vector<hsize_t> &dims, 
    const string &flag){
    return H5Attr::create<T>( raw(), name, dims, flag );
}
template<typename T>
H5Attr H5Group::create_attr_scalar(
    const string &name, const string &flag){
    return H5Attr::create_scalar<T>(raw(), name, flag);
}
inline H5Group H5Group::_from_raw(id_t loc) noexcept {
    return H5Group( std::make_shared<_obj_raw_t>(loc) );
}

}
}
#endif	//_HIPPIO_H5_OBJ_GROUP_H_