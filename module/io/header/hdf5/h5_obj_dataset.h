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
#include "h5_obj_proplist.h"
namespace HIPP{
namespace IO{

class H5Group;
class H5File;

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

    static H5Proplist create_proplist( const string &cls );
    H5Proplist proplist(const string &cls) const;
protected:
    friend class H5Group;
    friend class H5File;

    template<typename T>
    static H5Dataset create( 
        id_t loc, const string &name, const vector<size_t> &dims, 
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
    return H5Attr::create<T>(raw(), name, dims, flag);
}

inline H5Attr 
H5Dataset::open_attr(const string &name){
    return H5Attr::open( raw(), name );
}
inline bool 
H5Dataset::attr_exists(const string &name) const{
    return bool(_obj_ptr->exist_attr(name.c_str()));
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
inline void H5Dataset::write( const string &buff, 
    const H5Proplist &xprop ){
    typedef H5TypeStr str_t;
    _H5Datatype dtype( _H5Datatype::copy( str_t::h5_id ) );
    dtype.set_size( buff.size()+1 );    
    _obj_ptr->write( dtype.raw(), H5S_ALL, H5S_ALL, xprop.raw(), buff.c_str() );
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
    auto size = dataspace().size(), len = datatype().size();
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


inline void H5Dataset::read( string &buff, 
    const H5Proplist &xprop ) const{
    auto filetype = datatype();
    auto len = filetype.size();
    vector<char> _cbuf(len);
    _obj_ptr->read( filetype.raw(), H5S_ALL, H5S_ALL, xprop.raw(), 
        _cbuf.data() );
    buff.assign( (const char *)_cbuf.data() );
}

template<typename T>
H5Dataset H5Dataset::create( id_t loc, const string &name, 
    const vector<size_t> &dims, 
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
    const vector<size_t> &dims, 
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

inline H5Dataset H5Dataset::create_str( 
    id_t loc, const string &name, size_t len, const string &flag,
    const H5Proplist &lcprop,
    const H5Proplist &cprop,
    const H5Proplist &aprop ){
    typedef H5TypeStr str_t;
    _H5Datatype dtype( _H5Datatype::copy( str_t::h5_id ) );
    dtype.set_size( len );    
    return create( loc, name, dtype, H5Dataspace::scalarval.obj_raw(), 
        flag, lcprop, cprop, aprop );
}

inline H5Dataset 
H5Dataset::create( 
    id_t loc, const string &name, 
    const _H5Datatype &dtype, const _H5Dataspace &dspace, 
    const string &flag,
    const H5Proplist &lcprop,
    const H5Proplist &cprop,
    const H5Proplist &aprop )
{
    H5Dataset dset(NULL);
    try{
        _H5EStackTempOff estk(H5E_DEFAULT);
        dset = H5Dataset(
            std::make_shared<_obj_raw_t>( loc, name.c_str(), 
                dtype.raw(), dspace.raw(), lcprop.raw(), 
                cprop.raw(), aprop.raw() ));
    }catch( const ErrH5 &e ){
        if( flag == "trunc" ){
            dset = open( loc, name, aprop );
        }else if( flag == "excl" ){
            ErrH5::throw_( -1, emFLPFB, "  ... dataset ", name, " exists\n" );
        }else{
            ErrLogic::throw_(ErrLogic::eINVALIDARG, emFLPFB, 
                "  ... invalid flag ", flag, '\n');
        }
    }
    return dset;   
}


inline H5Dataset H5Dataset::open( id_t loc, const string &name,
    const H5Proplist &aprop )
{
    auto ptr = std::make_shared<_obj_raw_t>( 
        _obj_raw_t::open( loc, name.c_str(), aprop.raw() ) );
    return H5Dataset( ptr );
}
inline bool H5Dataset::exists( id_t loc, const string &name ){
    bool exists = true;
    try{
        _H5EStackTempOff estk(H5E_DEFAULT);
        open( loc, name );
    }catch( const ErrH5 & ){
        exists = false;
    }
    return exists;    
}

inline H5Proplist 
H5Dataset::create_proplist( const string &cls ){
    id_t _cls = H5P_DATASET_CREATE;
    if( cls == "c" || cls == "create" )
        _cls = H5P_DATASET_CREATE;
    else if( cls == "a" || cls == "access" )
        _cls = H5P_DATASET_ACCESS;
    else if( cls == "x" || cls == "xfer" || cls == "transfer" )
        _cls = H5P_DATASET_XFER;
    else
        ErrLogic::throw_( ErrLogic::eDOMAIN, emFLPFB, 
            "   ... dataset property list class ", cls, " invalid "  );
    return H5Proplist::_from_raw( H5Proplist::_obj_raw_t::create( _cls ) );
}

inline H5Proplist 
H5Dataset::proplist(const string &cls) const{
    id_t plist;
    if( cls == "c" || cls == "create" )
        plist = H5Dget_create_plist( raw() );
    else if( cls == "a" || cls == "access" )
        plist = H5Dget_access_plist( raw() );
    else 
        ErrLogic::throw_( ErrLogic::eDOMAIN, emFLPFB, 
            "   ... dataset property list class ", cls, " invalid "  );
    ErrH5::check( plist, emFLPFB );
    return H5Proplist::_from_raw( plist );
}
    
} // namespace IO
} // namespace HIPP
#endif	//_HIPPIO_H5_OBJ_DATASET_H_