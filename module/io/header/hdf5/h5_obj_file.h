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

/**
 * HDF5 high-level file object.
 */
class H5File: public H5Obj<_H5File>{
public:
    typedef H5Obj<_H5File> _obj_base_t;
    using _obj_base_t::_obj_base_t;

    /**
     * flag is the same as in _H5File, but the followings are added for 
     * convenience:
     * "ac" or "ca" for open existing file as R/W mode, create if not existing.
     */
    H5File( const string &name, const string &flag, 
        const H5Proplist &cporp = H5Proplist::defaultval, 
        const H5Proplist &aprop = H5Proplist::defaultval);

    /**
     * create a dataset of type `T` under the file instance.
     * @name:   dataset name.
     * @dims:   dimensions.
     * @flag:   if the dataset of name `name` exists, flag specifies how to
     *          deal with it. Possible values are:
     *          "trunc": just open it.
     *          "excl":  do not open and throw a exception `ErrH5`.
     * Type `T` can be either numeric type (e.g. float, double, int, ...) or 
     * std::string.
     * For nuemric type (e.g. float, double, int, char ...), `dims` is at 
     * actual shape of the array to be created.
     * For std::string, the `dims` must be { no. of str, max str size }. The 
     * `max str size` must contain the space of the terminate null character.
     * It is best to calculate dims use H5TypeStr::shape().
     */
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
    /**
     * open an existing dataset of name `name`.
     * If not existing, throw an error `ErrH5`.
     * 
     * To check whether a dataset of name `name` exists, use 
     * dataset_exists(name).
     */
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
    return H5Attr::create<T>( raw(), name, dims, flag );
}

template<typename T>
H5Attr H5File::create_attr_scalar(
    const string &name, const string &flag){
    return H5Attr::create_scalar<T>(raw(), name, flag);
}

} // namespace IO
} // namespace HIPP
#endif	//_HIPPIO_H5_OBJ_FILE_H_