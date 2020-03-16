/**
 * creat: Yangyao CHEN, 2020/01/11
 *      [write   ] H5Attr - HDF5 attribute high-level object.
 */ 

#ifndef _HIPPIO_H5_OBJ_ATTR_H_
#define _HIPPIO_H5_OBJ_ATTR_H_
#include "h5_obj_base.h"
#include "h5_obj_dataspace.h"
#include "h5_obj_datatype.h"
namespace HIPP{
namespace IO{

/**
 * HDF5 attribute high-level object.
 */
class H5File;
class H5Group;
class H5Dataset;
class H5Attr: public H5Obj<_H5Attr>{
public:
    typedef H5Obj<_H5Attr> _obj_base_t;
    using _obj_base_t::_obj_base_t;

    /**
     * retrive the info of this attribute.
     * dataspace(): return the dataspace.
     * datatype():  return the datatype.
     */
    H5Dataspace dataspace();
    const H5Dataspace dataspace() const;
    H5Datatype datatype();
    const H5Datatype datatype() const;

    /**
     * write data in `buff` into the attribute.
     * Type and number of elements in the `buff` must be compatible with the 
     * attribute size.
     * Vector version: check the size of `buff`. If not compatible an ErrLogic
     * is thrown.
     * Pointer version, no check is performed.
     * @T:  Type of element. Must be a numeric type of std::string.
     */
    template<typename T, typename A>
    void write( const vector<T, A> &buff );
    template<typename T>
    void write( const T*buff );
    void write( const string &buff );

    /**
     * read data in the attribute into buff.
     * Type and number of elements in the `buff` must be compatible with the 
     * attribute size.
     * Vector version: size of buff is automatically resized to the dataset 
     * size.
     * Pointer version: no resize is performed.
     * @T:  Type of element. Must be a numeric type of std::string.
     */
    template<typename T, typename A>
    void read( vector<T, A> &buff ) const;
    template<typename T>
    void read( T*buff ) const;
    void read( string &buff ) const;
protected:
    friend class H5File;
    friend class H5Group;
    friend class H5Dataset;

    template<typename T>
    static H5Attr create( id_t loc, const string &name, 
        const vector<size_t> &dims, 
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

inline H5Dataspace 
H5Attr::dataspace(){
    auto ptr = 
        std::make_shared<H5Dataspace::_obj_raw_t>( _obj_ptr->get_space() );
    return H5Dataspace( ptr );
}

inline const H5Dataspace 
H5Attr::dataspace() const
    { return const_cast<H5Attr*>(this)->dataspace(); }

inline H5Datatype 
H5Attr::datatype(){
    auto ptr = 
        std::make_shared<H5Datatype::_obj_raw_t>( _obj_ptr->get_type() );
    return H5Datatype( ptr );
}

inline const H5Datatype 
H5Attr::datatype() const
    { return const_cast<H5Attr*>(this)->datatype(); }

template<typename T, typename A>
void H5Attr::write( const vector<T, A> &buff ){
    auto dims = dataspace().dims();
    auto size = std::accumulate( dims.begin(), dims.end(), size_t(1),
        std::multiplies<size_t>() );
    if( size != buff.size() )
        ErrLogic::throw_( ErrLogic::eLENGTH, emFLPFB,
            "  ... buff length ", buff.size(), 
            " does not match attribute size ", size, "\n");
    write<T>( buff.data() );
}
template<>
void H5Attr::write( const vector<string> &buff ){
    auto size = dataspace().size(), len=datatype().size();
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
inline void H5Attr::write( const string &buff ){
    typedef H5TypeStr str_t;
    _H5Datatype dtype( _H5Datatype::copy(str_t::h5_id) );
    dtype.set_size( buff.size()+1 );
    _obj_ptr->write( dtype.raw(), buff.c_str() );
}

template<typename T, typename A>
void H5Attr::read( vector<T, A> &buff ) const{
    auto dims = dataspace().dims();
    auto size = std::accumulate( dims.begin(), dims.end(), size_t(1), 
        std::multiplies<size_t>() );
    buff.resize( size );
    read<T>( buff.data() );
}
template<>
void H5Attr::read( vector<string> &buff ) const{
    auto size = dataspace().size(), len = datatype().size();
    vector<char> _buff( size*len, '\0' );
    _obj_ptr->read( datatype().raw(), _buff.data());
    buff = H5TypeStr::deserialize( _buff, size );
}
template<typename T>
void H5Attr::read( T*buff ) const{
    id_t memtype = H5TypeNative<T>::h5_id;
    _obj_ptr->read( memtype, buff );
}
inline void H5Attr::read( string &buff ) const{
    auto filetype = datatype();
    auto len = filetype.size();
    vector<char> _cbuf( len );
    _obj_ptr->read( filetype.raw(), _cbuf.data() );
    buff.assign( (const char *)_cbuf.data() );
}

template<typename T>
H5Attr H5Attr::create( id_t loc, const string &name, const vector<size_t> &dims, 
    const string &flag){
    int rank = dims.size();
    _H5Dataspace dspace( rank, dims.data() );
    _H5Datatype dtype( H5TypeNative<T>::h5_id, 0 );
    return create(loc, name, dtype, dspace, flag );
}
template<>
H5Attr H5Attr::create<string>( 
    id_t loc, const string &name, const vector<size_t> &dims, 
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

inline H5Attr 
H5Attr::create_str( id_t loc, const string &name, size_t len,
    const string &flag){
    typedef H5TypeStr str_t;
    _H5Datatype dtype( _H5Datatype::copy( str_t::h5_id ) );
    dtype.set_size( len );
    return create(loc, name, dtype, H5Dataspace::scalarval.obj_raw(), flag);
}

inline H5Attr 
H5Attr::create( id_t loc, const string &name, 
    const _H5Datatype &dtype, const _H5Dataspace &dspace, 
    const string &flag){
    H5Attr attr(NULL);
    try{
        _H5EStackTempOff estk(H5E_DEFAULT);
        auto ptr = std::make_shared<_obj_raw_t>( 
            loc, name.c_str(), dtype.raw(), dspace.raw() );
        attr = H5Attr( ptr );
    }catch( const ErrH5 &e ){
        if( flag == "trunc" ){
            attr = open(loc, name);
        }else if( flag == "excl" ){
            ErrH5::throw_(-1, emFLPFB, "  ... attribute ", name, " exists\n");
        }else{
            ErrLogic::throw_(ErrLogic::eINVALIDARG, 
                emFLPFB,
                "  ... invalid flag ", flag, '\n');
        }
    }
    return attr;
}


inline H5Attr H5Attr::open( id_t loc, const string &name ){
    auto ptr = std::make_shared<_obj_raw_t>( 
        _obj_raw_t::open( loc, name.c_str() ));
    return H5Attr( ptr );
}

} // namespace IO
} // namespace HIPP
#endif	//_HIPPIO_H5_OBJ_ATTR_H_