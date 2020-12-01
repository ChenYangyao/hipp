/**
 * creat: Yangyao CHEN, 2020/01/11
 *      [write   ] 
 *      @H5Dataset: HDF5 high-level dataset object.
 */ 

#ifndef _HIPPIO_H5_OBJ_DATASET_H_
#define _HIPPIO_H5_OBJ_DATASET_H_
#include "h5_obj_base.h"
#include "h5_obj_dataspace.h"
#include "h5_obj_datatype.h"
#include "h5_obj_attr.h"
#include "h5_obj_proplist.h"
namespace HIPP{
namespace IO{

class H5Group;
class H5File;

class H5Dataset: public H5Obj<_H5Dataset>{
public:
    typedef H5Obj<_H5Dataset> _obj_base_t;

    using _obj_base_t::_obj_base_t;
    
    /**
     * retrive the info of this dataset.
     * dataspace(): return the dataspace.
     * datatype():  return the datatype.
     */
    H5Dataspace dataspace();
    const H5Dataspace dataspace() const;
    H5Datatype datatype();
    const H5Datatype datatype() const;
    
    /**
     * create an attribute of type `T` under the dataset instance.
     * @name:   attribute name.
     * @dims:   dimensions.
     * @flag:   if the attribute of name `name` exists, flag specifies how to
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
    H5Attr create_attr(
        const string &name, const vector<hsize_t> &dims, 
        const string &flag="trunc");
    /**
     * open an existing attribute of name `name`.
     * If not existing, throw an error `ErrH5`.
     * 
     * To check whether a attribute of name `name` exists, use 
     * attr_exists(name).
     */
    H5Attr open_attr(const string &name);
    bool attr_exists(const string &name) const;

    /**
     * write data in `buff` into the dataset.
     * Type and number of elements in the `buff` must be compatible with the 
     * dataset size.
     * Vector version: check the size of `buff`. If not compatible an ErrLogic
     * is thrown.
     * Pointer version, no check is performed.
     * @T:  Type of element. Must be a numeric type.
     * 
     * For the vector version, vector<string> is allowed.
     */
    template<typename T, typename A>
    void write( const vector<T, A> &buff, 
        const H5Dataspace &memspace = H5Dataspace::allval, 
        const H5Dataspace &filespace=H5Dataspace::allval, 
        const H5Proplist &xprop=H5Proplist::defaultval );
    template<typename T>
    void write( const T *buff, 
        const H5Dataspace &memspace = H5Dataspace::allval, 
        const H5Dataspace &filespace=H5Dataspace::allval,
        const H5Proplist &xprop=H5Proplist::defaultval );
    void write( const string &buff, 
        const H5Proplist &xprop=H5Proplist::defaultval );
    template<typename T, typename A>
    void write( const vector<T, A> &buff, 
        const H5Datatype &memtype,
        const H5Dataspace &memspace = H5Dataspace::allval, 
        const H5Dataspace &filespace=H5Dataspace::allval, 
        const H5Proplist &xprop=H5Proplist::defaultval );
    template<typename T>
    void write( const T *buff, 
        const H5Datatype &memtype,
        const H5Dataspace &memspace = H5Dataspace::allval, 
        const H5Dataspace &filespace=H5Dataspace::allval, 
        const H5Proplist &xprop=H5Proplist::defaultval );

    /**
     * read data in the dataset into buff.
     * Type and number of elements in the `buff` must be compatible with the 
     * dataset size.
     * Vector version: size of buff is automatically resized to the dataset 
     * size.
     * Pointer version: no resize is performed.
     * @T:  Type of element. Must be a numeric type.
     * 
     * For the vector version, vector<string> is allowed.
     */
    template<typename T, typename A>
    void read( vector<T, A> &buff,
        const H5Dataspace &memspace = H5Dataspace::allval, 
        const H5Dataspace &filespace=H5Dataspace::allval,
        const H5Proplist &xprop=H5Proplist::defaultval ) const;
    template<typename T>
    void read( T *buff,
        const H5Dataspace &memspace = H5Dataspace::allval, 
        const H5Dataspace &filespace=H5Dataspace::allval,
        const H5Proplist &xprop=H5Proplist::defaultval ) const;
    void read( string &buff, 
        const H5Proplist &xprop=H5Proplist::defaultval ) const;
    template<typename T, typename A>
    void read( vector<T, A> &buff, const H5Datatype &memtype,
        const H5Dataspace &memspace = H5Dataspace::allval, 
        const H5Dataspace &filespace=H5Dataspace::allval,
        const H5Proplist &xprop=H5Proplist::defaultval ) const;
    template<typename T>
    void read( T *buff, const H5Datatype &memtype,
        const H5Dataspace &memspace = H5Dataspace::allval, 
        const H5Dataspace &filespace=H5Dataspace::allval,
        const H5Proplist &xprop=H5Proplist::defaultval ) const;

    static H5Proplist create_proplist(const string &cls);
    H5Proplist proplist(const string &cls) const;
protected:
    friend class H5Group;
    friend class H5File;

    template<typename T>
    static H5Dataset create( 
        id_t loc, const string &name, const vector<hsize_t> &dims, 
        const string &flag,
        const H5Proplist &lcprop = H5Proplist::defaultval,
        const H5Proplist &cprop = H5Proplist::defaultval,
        const H5Proplist &aprop = H5Proplist::defaultval );
    template<typename T>
    static H5Dataset create_scalar( 
        id_t loc, const string &name, const string &flag,
        const H5Proplist &lcprop = H5Proplist::defaultval,
        const H5Proplist &cprop = H5Proplist::defaultval,
        const H5Proplist &aprop = H5Proplist::defaultval );
    static H5Dataset create_str( 
        id_t loc, const string &name, size_t len, const string &flag,
        const H5Proplist &lcprop = H5Proplist::defaultval,
        const H5Proplist &cprop = H5Proplist::defaultval,
        const H5Proplist &aprop = H5Proplist::defaultval );
    static H5Dataset create( 
        id_t loc, const string &name, 
        const _H5Datatype &dtype, const _H5Dataspace &dspace, 
        const string &flag,
        const H5Proplist &lcprop = H5Proplist::defaultval,
        const H5Proplist &cprop = H5Proplist::defaultval,
        const H5Proplist &aprop = H5Proplist::defaultval );
    static H5Dataset open( id_t loc, const string &name,
        const H5Proplist &aprop = H5Proplist::defaultval );
    static bool exists( id_t loc, const string &name );
};


template<typename T>
H5Attr H5Dataset::create_attr(const string &name, const vector<hsize_t> &dims, 
    const string &flag)
{
    return H5Attr::create<T>(raw(), name, dims, flag);
}
template<typename T, typename A>
void H5Dataset::write( const vector<T, A> &buff, 
    const H5Dataspace & memspace, const H5Dataspace & filespace, 
    const H5Proplist &xprop ){
    write<T>( buff.data(), memspace, filespace, xprop );
}
template<>
inline void H5Dataset::write( const vector<string> &buff,
    const H5Dataspace & memspace, const H5Dataspace & filespace,
    const H5Proplist &xprop ){
    auto data = H5TypeStr::serialize( buff );
    _obj_ptr->write( datatype().raw(), 
        H5S_ALL, H5S_ALL, xprop.raw(), data.data() );
}
template<typename T>
void H5Dataset::write( const T *buff, 
    const H5Dataspace & memspace, const H5Dataspace & filespace,
    const H5Proplist &xprop ){
    id_t memtype = H5TypeNative<T>::h5_id;
    _obj_ptr->write( memtype, memspace.raw(), filespace.raw(), 
        xprop.raw(), buff );
}
template<typename T, typename A>
void H5Dataset::write( const vector<T, A> &buff, 
    const H5Datatype &memtype,
    const H5Dataspace &memspace, 
    const H5Dataspace &filespace, 
    const H5Proplist &xprop){
    _obj_ptr->write(memtype.raw(), memspace.raw(), filespace.raw(), 
        xprop.raw(), buff.data());
}
template<typename T>
void H5Dataset::write( const T *buff, 
    const H5Datatype &memtype,
    const H5Dataspace &memspace, 
    const H5Dataspace &filespace, 
    const H5Proplist &xprop){
    _obj_ptr->write(memtype.raw(), memspace.raw(), filespace.raw(), 
        xprop.raw(), buff);
}
template<typename T, typename A>
void H5Dataset::read( vector<T, A> &buff, 
    const H5Dataspace & memspace, const H5Dataspace & filespace,
    const H5Proplist &xprop ) const{
    if( memspace.raw() != H5S_ALL ){
        buff.resize( memspace.size() );
    }else if( filespace.raw() != H5S_ALL ){
        buff.resize( filespace.size() );
    }else{
        buff.resize( dataspace().size() );
    }
    read<T>(buff.data(), memspace, filespace, xprop);
}
template<>
inline void H5Dataset::read( vector<string> &buff, 
    const H5Dataspace & memspace, const H5Dataspace & filespace,
    const H5Proplist &xprop ) const{
    auto size = dataspace().size();
    auto len = datatype().size();
    vector<char> _buff( size*len, '\0' );
    _obj_ptr->read( datatype().raw(), H5S_ALL, H5S_ALL, 
        xprop.raw(), _buff.data() );
    buff = H5TypeStr::deserialize( _buff, size );
}
template<typename T>
void H5Dataset::read( T *buff, 
    const H5Dataspace & memspace, const H5Dataspace & filespace,
    const H5Proplist &xprop ) const{
    id_t memtype = H5TypeNative<T>::h5_id;
    _obj_ptr->read( memtype, memspace.raw(), filespace.raw(), xprop.raw(), buff );
}
template<typename T, typename A>
void H5Dataset::read( vector<T, A> &buff, const H5Datatype &memtype,
    const H5Dataspace &memspace, 
    const H5Dataspace &filespace,
    const H5Proplist &xprop) const{
    _obj_ptr->read(memtype.raw(), memspace.raw(), filespace.raw(), 
        xprop.raw(), buff.data());
}
template<typename T>
void H5Dataset::read( T *buff, const H5Datatype &memtype,
    const H5Dataspace &memspace, 
    const H5Dataspace &filespace,
    const H5Proplist &xprop) const {
    _obj_ptr->read(memtype.raw(), memspace.raw(), filespace.raw(), 
        xprop.raw(), buff);
}
template<typename T>
H5Dataset H5Dataset::create( id_t loc, const string &name, 
    const vector<hsize_t> &dims, 
    const string &flag, const H5Proplist &lcprop, const H5Proplist &cprop,
    const H5Proplist &aprop )
{
    int rank = dims.size();
    _H5Dataspace dspace( rank, dims.data() );
    _H5Datatype dtype( H5TypeNative<T>::h5_id, 0 );
    return create( loc, name, dtype, dspace, flag, lcprop, cprop, aprop );
}
template<>
inline H5Dataset H5Dataset::create<string>( id_t loc, const string &name, 
    const vector<hsize_t> &dims, 
    const string &flag,
    const H5Proplist &lcprop, const H5Proplist &cprop,
    const H5Proplist &aprop ){
    typedef H5TypeStr str_t;
    _H5Datatype dtype( _H5Datatype::copy( str_t::h5_id ) );
    dtype.set_size( dims[1] );    
    _H5Dataspace dspace( 1, &dims[0] );
    return create( loc, name, dtype, dspace, flag, lcprop, cprop, aprop );
}

template<typename T>
H5Dataset H5Dataset::create_scalar( 
    id_t loc, const string &name, const string &flag,
    const H5Proplist &lcprop,
    const H5Proplist &cprop,
    const H5Proplist &aprop )
{
    _H5Datatype dtype( H5TypeNative<T>::h5_id, 0 );
    return create( loc, name, dtype, H5Dataspace::scalarval.obj_raw(), flag, 
        lcprop, cprop, aprop );
}


    
} // namespace IO
} // namespace HIPP
#endif	//_HIPPIO_H5_OBJ_DATASET_H_