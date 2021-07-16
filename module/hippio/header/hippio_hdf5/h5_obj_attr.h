/**
 * creat: Yangyao CHEN, 2020/01/11
 *      [write   ] 
 *      @H5Attr: HDF5 attribute high-level object.
 */ 

#ifndef _HIPPIO_H5_OBJ_ATTR_H_
#define _HIPPIO_H5_OBJ_ATTR_H_
#include "h5_obj_base.h"
#include "h5_obj_dataspace.h"
#include "h5_obj_datatype.h"
namespace HIPP{
namespace IO{

class H5File;
class H5Group;
class H5Dataset;
class H5Attr: public H5Obj<_H5Attr>{
public:
    typedef H5Obj<_H5Attr> _obj_base_t;
    using _obj_base_t::_obj_base_t;

    H5Dataspace dataspace();
    const H5Dataspace dataspace() const;
    H5Datatype datatype();
    const H5Datatype datatype() const;

    template<typename T, typename A>
    void write( const vector<T, A> &buff );
    template<typename T>
    void write( const T*buff );
    void write( const string &buff );
    template<typename T, typename A>
    void write( const vector<T, A> &buff, const H5Datatype &memtype );
    template<typename T>
    void write( const T *buff, const H5Datatype &memtype );

    template<typename T, typename A>
    void read( vector<T, A> &buff ) const;
    template<typename T>
    void read( T *buff ) const;
    void read( string &buff ) const;
    template<typename T, typename A>
    void read( vector<T, A> &buff, const H5Datatype &memtype ) const;
    template<typename T>
    void read( T *buff, const H5Datatype &memtype ) const;
protected:
    friend class H5File;
    friend class H5Group;
    friend class H5Dataset;

    template<typename T>
    static H5Attr create( id_t loc, const string &name, 
        const vector<hsize_t> &dims, 
        const string &flag);

    template<typename T>
    static H5Attr create_scalar( id_t loc, const string &name,  
        const string &flag);

    static H5Attr create_str( id_t loc, const string &name, size_t len,
        const string &flag);

    static H5Attr create( id_t loc, const string &name, 
        const _H5Datatype &dtype, const _H5Dataspace &dspace, 
        const string &flag);

    static H5Attr open( id_t loc, const string &name );
};

template<typename T, typename A>
void H5Attr::write( const vector<T, A> &buff ){
    auto dims = dataspace().dims();
    auto size = std::accumulate( dims.begin(), dims.end(), hsize_t(1),
        std::multiplies<hsize_t>() );
    if( size != buff.size() )
        ErrLogic::throw_( ErrLogic::eLENGTH, emFLPFB,
            "  ... buff length ", buff.size(), 
            " does not match attribute size ", size, "\n");
    write<T>( buff.data() );
}
template<>
inline void H5Attr::write( const vector<string> &buff ){
    auto size = dataspace().size();
    auto len = datatype().size();
    size_t realsize=buff.size(), reallen = H5TypeStr::shape(buff)[1];
    if( size != realsize )
        ErrLogic::throw_( ErrLogic::eLENGTH, emFLPFB,
            "  ... buff length ", realsize,
            " does not match the dataset size ", size, '\n' );
    if( len != reallen )
        ErrLogic::throw_( ErrLogic::eLENGTH, emFLPFB,
            "  ... buff string length ", reallen, 
            " does not match the dataset string length ", len, '\n' );
    auto data = H5TypeStr::serialize( buff );
    _obj_ptr->write( datatype().raw(), data.data() );
}
template<typename T>
void H5Attr::write( const T*buff ){
    id_t memtype = H5TypeNative<T>::h5_id;
    _obj_ptr->write( memtype, buff );
}
template<typename T, typename A>
void H5Attr::write( const vector<T, A> &buff, const H5Datatype &memtype ) {
    _obj_ptr->write( memtype.raw(), buff.data() );
}
template<typename T>
void H5Attr::write( const T *buff, const H5Datatype &memtype ) {
    _obj_ptr->write( memtype.raw(), buff );
}
template<typename T, typename A>
void H5Attr::read( vector<T, A> &buff ) const{
    auto dims = dataspace().dims();
    auto size = std::accumulate( dims.begin(), dims.end(), hsize_t(1), 
        std::multiplies<hsize_t>() );
    buff.resize( size );
    read<T>( buff.data() );
}
template<>
inline void H5Attr::read( vector<string> &buff ) const{
    auto size = dataspace().size();
    auto len = datatype().size();
    vector<char> _buff( size*len, '\0' );
    _obj_ptr->read( datatype().raw(), _buff.data());
    buff = H5TypeStr::deserialize( _buff, size );
}
template<typename T>
void H5Attr::read( T *buff ) const {
    id_t memtype = H5TypeNative<T>::h5_id;
    _obj_ptr->read( memtype, buff );
}
template<typename T, typename A>
void H5Attr::read( vector<T, A> &buff, const H5Datatype &memtype ) const {
    _obj_ptr->read(memtype.raw(), buff.data());
}
template<typename T>
void H5Attr::read( T *buff, const H5Datatype &memtype ) const {
    _obj_ptr->read(memtype.raw(), buff);
}
template<typename T>
H5Attr H5Attr::create( id_t loc, const string &name, const vector<hsize_t> &dims, 
    const string &flag){
    int rank = dims.size();
    _H5Dataspace dspace( rank, dims.data() );
    _H5Datatype dtype( H5TypeNative<T>::h5_id, 0 );
    return create(loc, name, dtype, dspace, flag );
}
template<>
inline H5Attr H5Attr::create<string>( 
    id_t loc, const string &name, const vector<hsize_t> &dims, 
    const string &flag){
    typedef H5TypeStr str_t;
    _H5Datatype dtype( _H5Datatype::copy( str_t::h5_id ) );
    dtype.set_size( dims[1] );
    _H5Dataspace dspace( 1, &dims[0] );
    return create(loc, name, dtype, dspace, flag);
}

template<typename T>
H5Attr H5Attr::create_scalar( id_t loc, const string &name,  
    const string &flag){
    _H5Datatype dtype( H5TypeNative<T>::h5_id, 0 );
    return create(loc, name, dtype, H5Dataspace::scalarval.obj_raw(), flag );
}

} // namespace IO
} // namespace HIPP
#endif	//_HIPPIO_H5_OBJ_ATTR_H_