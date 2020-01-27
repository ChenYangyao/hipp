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


} // namespace IO
} // namespace HIPP
#endif	//_HIPPIO_H5_OBJ_ATTR_H_