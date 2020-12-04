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
    
    H5Dataspace dataspace();
    const H5Dataspace dataspace() const;
    H5Datatype datatype();
    const H5Datatype datatype() const;
    
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
template<typename T>
H5Attr H5Dataset::create_attr_scalar(
    const string &name, const string &flag){
    return H5Attr::create_scalar<T>(raw(), name, flag);
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