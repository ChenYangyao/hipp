/**
 * creat: Yangyao CHEN, 2020/01/11
 *      [write   ] 
 *      @H5File: HDF5 high-level file object.
 */ 

#ifndef _HIPPIO_H5_OBJ_FILE_H_
#define _HIPPIO_H5_OBJ_FILE_H_
#include "h5_obj_base.h"
#include "h5_obj_dataset.h"
#include "h5_obj_datatype.h"
#include "h5_obj_proplist.h"
#include "h5_obj_group.h"
namespace HIPP{
namespace IO{

class H5File: public H5Obj<_H5File>{
public:
    typedef H5Obj<_H5File> _obj_base_t;
    using _obj_base_t::_obj_base_t;

    H5File( const string &name, const string &flag, 
        const H5Proplist &cporp = H5Proplist::defaultval, 
        const H5Proplist &aprop = H5Proplist::defaultval);

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
    bool dataset_exists( const string &name ) const;

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

    static H5Proplist create_proplist(const string &cls = "c");
};

template<typename T>
H5Dataset H5File::create_dataset( 
    const string &name, const vector<hsize_t> &dims, 
    const string &flag,
    const H5Proplist &lcprop, const H5Proplist &cprop, 
    const H5Proplist &aprop )
{
    return H5Dataset::create<T>( 
            raw(), name, dims, flag, lcprop, cprop, aprop );
}

template<typename T>
H5Dataset H5File::create_dataset_scalar( const string &name,
    const string &flag, const H5Proplist &lcprop,
    const H5Proplist &cprop,const H5Proplist &aprop ){
    return H5Dataset::create_scalar<T>( raw(), name, flag, 
        lcprop, cprop, aprop );
}

template<typename T>
H5Attr H5File::create_attr(
    const string &name, const vector<hsize_t> &dims, 
    const string &flag)
{
    return H5Attr::create<T>(raw(), name, dims, flag);
}

template<typename T>
H5Attr H5File::create_attr_scalar(
    const string &name, const string &flag){
    return H5Attr::create_scalar<T>(raw(), name, flag);
}

} // namespace IO
} // namespace HIPP
#endif	//_HIPPIO_H5_OBJ_FILE_H_