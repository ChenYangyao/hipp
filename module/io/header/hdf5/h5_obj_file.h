/**
 * creat: Yangyao CHEN, 2020/01/11
 *      [write   ] H5File - HDF5 high-level file object.
 */ 

#ifndef _HIPPIO_H5_OBJ_FILE_H_
#define _HIPPIO_H5_OBJ_FILE_H_
#include "h5_obj_base.h"
#include "h5_obj_dataset.h"
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

    H5File( const string &name, const string &flag, 
        const H5Proplist &cporp = H5Proplist::defaultval, 
        const H5Proplist &aprop = H5Proplist::defaultval)
        : _obj_base_t( std::make_shared<_obj_raw_t>( 
            name.c_str(), flag, cporp.raw(), aprop.raw() ) ){ }

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
    H5Dataset create_dataset( const string &name, const vector<size_t> &dims, 
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
        const string &name, const vector<size_t> &dims, 
        const string &flag="trunc");
    H5Attr open_attr(const string &name);
    bool attr_exists(const string &name) const;

    H5Group create_group( const string &name );
    H5Group open_group( const string &name );

    static H5Proplist create_proplist(const string &cls = "c");
};

template<typename T>
H5Dataset H5File::create_dataset( 
    const string &name, const vector<size_t> &dims, 
    const string &flag,
    const H5Proplist &lcprop, const H5Proplist &cprop, 
    const H5Proplist &aprop )
{
    return H5Dataset::create<T>( 
            raw(), name, dims, flag, lcprop, cprop, aprop );
}
inline H5Dataset
H5File::open_dataset( const string &name, const H5Proplist &aprop ){
    return H5Dataset::open(raw(), name, aprop);
}
inline bool 
H5File::dataset_exists( const string &name ) const{
    return H5Dataset::exists( raw(), name );
}

template<typename T>
H5Attr H5File::create_attr(
    const string &name, const vector<size_t> &dims, 
    const string &flag)
{
    return H5Attr::create<T>( raw(), name, dims, flag );
}

inline H5Attr 
H5File::open_attr(const string &name){
    return H5Attr::open( raw(), name );
}

inline bool 
H5File::attr_exists(const string &name) const{
    return bool( _obj_ptr->exist_attr( name.c_str() ) );
}

inline H5Group H5File::create_group( const string &name ){
    auto id = H5Group::_obj_raw_t::create( raw(), name.c_str() );
    return H5Group( std::make_shared<H5Group::_obj_raw_t>( id ) );
}

inline H5Group H5File::open_group( const string &name ){
    auto id = H5Group::_obj_raw_t::open( raw(), name.c_str() );
    return H5Group( std::make_shared<H5Group::_obj_raw_t>( id ) );
}

inline H5Proplist 
H5File::create_proplist(const string &cls){
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
#endif	//_HIPPIO_H5_OBJ_FILE_H_