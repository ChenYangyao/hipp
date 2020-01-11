/**
 * creat: Yangyao CHEN, 2020/01/11
 *      [write   ] H5Dataset - HDF5 high-level dataset object.
 */ 

#ifndef _HIPPIO_H5_OBJ_DATASET_H_
#define _HIPPIO_H5_OBJ_DATASET_H_
#include "h5_obj_base.h"
#include "h5_obj_dataspace.h"
#include "h5_obj_datatype.h"
#include "h5_obj_attr.h"
namespace HIPP{
namespace IO{

/**
 * HDF5 high-level dataset object.
 */
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
        const string &name, const vector<size_t> &dims, 
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
     * @T:  Type of element. Must be a numeric type of std::string.
     */
    template<typename T, typename A>
    void write( const vector<T, A> &buff );
    template<typename T>
    void write( const T *buff );

    /**
     * read data in the dataset into buff.
     * Type and number of elements in the `buff` must be compatible with the 
     * dataset size.
     * Vector version: size of buff is automatically resized to the dataset 
     * size.
     * Pointer version: no resize is performed.
     * @T:  Type of element. Must be a numeric type of std::string.
     */
    template<typename T, typename A>
    void read( vector<T, A> &buff ) const;
    template<typename T>
    void read( T *buff ) const;
};


inline H5Dataspace 
H5Dataset::dataspace(){
    auto ptr = std::make_shared<H5Dataspace::_obj_raw_t>( 
        _obj_ptr->get_space() );
    return H5Dataspace( ptr );        
}

inline const H5Dataspace 
H5Dataset::dataspace() const
    { return const_cast<H5Dataset *>(this)->dataspace(); }

inline H5Datatype 
H5Dataset::datatype(){
    auto ptr = std::make_shared<H5Datatype::_obj_raw_t>( 
        _obj_ptr->get_type() );
    return H5Datatype( ptr );
}

inline const H5Datatype 
H5Dataset::datatype() const
    { return const_cast<H5Dataset *>(this)->datatype(); }

template<typename T>
H5Attr H5Dataset::create_attr(const string &name, const vector<size_t> &dims, 
    const string &flag)
{
    H5Attr attr(NULL);
    int rank = dims.size();
    _H5Dataspace dspace( rank, dims.data() );
    try{
        _H5EStackTempOff estk(H5E_DEFAULT);
        auto ptr = std::make_shared<H5Attr::_obj_raw_t>( 
            raw(), name.c_str(), H5TypeNative<T>::h5_id, dspace.raw() );
        attr = H5Attr( ptr );
    }catch( const ErrH5 &e ){
        if( flag == "trunc" ){
            attr = open_attr(name);
        }else if( flag == "excl" ){
            ErrH5::throw_(-1, emFLPFB, "  ... attribute ", name, " exists\n");
        }else{
            ErrLogic::throw_(ErrLogic::eINVALIDARG, emFLPFB, 
                "  ... invalid flag ", flag, '\n');
        }
    }
    return attr;
}

template<>
H5Attr H5Dataset::create_attr<string>(const string &name, 
    const vector<size_t> &dims, const string &flag)
{
    H5Attr attr(NULL);

    typedef H5TypeStr str_t;
    _H5Datatype dtype( _H5Datatype::copy( str_t::h5_id ) );
    dtype.set_size( dims[1] );
    _H5Dataspace dspace( 1, &dims[0] );

    try{
        _H5EStackTempOff estk(H5E_DEFAULT);
        auto ptr = std::make_shared<H5Attr::_obj_raw_t>( raw(), name.c_str(), 
            dtype.raw(), dspace.raw() );
        attr = H5Attr(ptr);
    }catch( const ErrH5 &e ){
        if( flag == "trunc" ){
            attr = open_attr(name);
        }else if( flag == "excl" ){
            ErrH5::throw_(-1, emFLPFB, "  ... attribute ", name, " exists\n");
        }else{
            ErrLogic::throw_(ErrLogic::eINVALIDARG, emFLPFB, 
                "  ... invalid flag ", flag, '\n');
        }
    }
    return attr;
}

inline H5Attr 
H5Dataset::open_attr(const string &name){
    auto ptr = std::make_shared<_H5Attr>( 
        _obj_ptr->open_attr( name.c_str() ) );
    return H5Attr(ptr);
}
inline bool 
H5Dataset::attr_exists(const string &name) const{
    return bool(_obj_ptr->exist_attr(name.c_str()));
}

template<typename T, typename A>
void H5Dataset::write( const vector<T, A> &buff ){
    auto size = dataspace().size();
    if( size != buff.size() )
        ErrLogic::throw_( ErrLogic::eLENGTH, emFLPFB, 
            "  ... buff length ", buff.size(), 
            " does not match dataset size ", size, '\n' );
    write<T>( buff.data() );
}
template<>
void H5Dataset::write( const vector<string> &buff ){
    auto size = dataspace().size(), len = datatype().size();
    size_t realsize = buff.size(), reallen = H5TypeStr::shape(buff)[1];
    if( size != realsize )
        ErrLogic::throw_( ErrLogic::eLENGTH, emFLPFB,
            "  ... buff length ", realsize,
            " does not match the dataset size ", size, '\n' );
    if( len != reallen )
        ErrLogic::throw_( ErrLogic::eLENGTH, emFLPFB,
            "  ... buff string length ", reallen, 
            " does not match the dataset string length ", len, '\n' );
    auto data = H5TypeStr::serialize( buff );
    _obj_ptr->write( datatype().raw(), H5S_ALL, H5S_ALL, H5P_DEFAULT, data.data() );
}

template<typename T>
void H5Dataset::write( const T *buff ){
    id_t memtype = H5TypeNative<T>::h5_id;
    id_t dspace = H5S_ALL;
    _obj_ptr->write( memtype, dspace, dspace, H5P_DEFAULT, buff );
}
template<typename T, typename A>
void H5Dataset::read( vector<T, A> &buff ) const{
    buff.resize( dataspace().size() );
    read<T>(buff.data());
}
template<>
void H5Dataset::read( vector<string> &buff ) const{
    auto size = dataspace().size(), len = datatype().size();
    vector<char> _buff( size*len, '\0' );
    _obj_ptr->read( datatype().raw(), H5S_ALL, H5S_ALL, H5P_DEFAULT, _buff.data() );
    buff = H5TypeStr::deserialize( _buff, size );
}
template<typename T>
void H5Dataset::read( T *buff ) const{
    id_t memtype = H5TypeNative<T>::h5_id;
    id_t dspace =H5S_ALL;
    _obj_ptr->read( memtype, dspace, dspace, H5P_DEFAULT, buff );
}
    
} // namespace IO
} // namespace HIPP
#endif	//_HIPPIO_H5_OBJ_DATASET_H_