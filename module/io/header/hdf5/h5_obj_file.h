/**
 * creat: Yangyao CHEN, 2020/01/11
 *      [write   ] H5File - HDF5 high-level file object.
 */ 

#ifndef _HIPPIO_H5_OBJ_FILE_H_
#define _HIPPIO_H5_OBJ_FILE_H_
#include "h5_obj_base.h"
#include "h5_obj_dataset.h"
namespace HIPP{
namespace IO{

/**
 * HDF5 high-level file object.
 */
class H5File: public H5Obj<_H5File>{
public:
    typedef H5Obj<_H5File> _obj_base_t;
    using _obj_base_t::_obj_base_t;

    H5File( const string &name, const string &flag)
        : _obj_base_t( std::make_shared<_obj_raw_t>( name.c_str(), flag ) ){ }

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
        const string &flag="trunc" );
    /**
     * open an existing dataset of name `name`.
     * If not existing, throw an error `ErrH5`.
     * 
     * To check whether a dataset of name `name` exists, use 
     * dataset_exists(name).
     */
    H5Dataset open_dataset( const string &name );
    bool dataset_exists( const string &name ) const;
};

template<typename T>
H5Dataset H5File::create_dataset( 
    const string &name, const vector<size_t> &dims, 
    const string &flag )
{
    H5Dataset dset(NULL);
    try{
        _H5EStackTempOff estk(H5E_DEFAULT);
        int rank = dims.size();
        _H5Dataspace dspace( rank, dims.data() );
        dset = H5Dataset(
            std::make_shared<H5Dataset::_obj_raw_t>( raw(), name.c_str(), 
                H5TypeNative<T>::h5_id, dspace.raw() ));
    }catch( const ErrH5 &e ){
        if( flag == "trunc" ){
            dset = open_dataset( name );
        }else if( flag == "excl" ){
            ErrH5::throw_( -1, emFLPFB, "  ... dataset ", name, " exists\n" );
        }else{
            ErrLogic::throw_(ErrLogic::eINVALIDARG, emFLPFB, 
                "  ... invalid flag ", flag, '\n');
        }
    }
    return dset;
}
template<>
H5Dataset H5File::create_dataset<string>( 
    const string &name, const vector<size_t> &dims, const string &flag )
{
    H5Dataset dset(NULL);

    typedef H5TypeStr str_t;
    _H5Datatype dtype( _H5Datatype::copy( str_t::h5_id ) );
    dtype.set_size( dims[1] );
    _H5Dataspace dspace( 1, &dims[0] );
    
    try{
        _H5EStackTempOff estk(H5E_DEFAULT);
        dset = H5Dataset(
            std::make_shared<H5Dataset::_obj_raw_t>( raw(), name.c_str(), 
                dtype.raw(), dspace.raw() ));
    }catch( const ErrH5 &e ){
        if( flag == "trunc" ){
            dset = open_dataset( name );
        }else if( flag == "excl" ){
            ErrH5::throw_( -1, emFLPFB, "  ... dataset ", name, " exists\n" );
        }else{
            ErrLogic::throw_(ErrLogic::eINVALIDARG, emFLPFB, 
                "  ... invalid flag ", flag, '\n');
        }
    }
    return dset;
}
inline H5Dataset
H5File::open_dataset( const string &name ){
    return H5Dataset(
        std::make_shared<H5Dataset::_obj_raw_t>( 
            _obj_ptr->open_dataset( name.c_str() ) 
        ));
}
inline bool 
H5File::dataset_exists( const string &name ) const{
    bool exists = true;
    try{
        _H5EStackTempOff estk(H5E_DEFAULT);
        const_cast<H5File *>(this)->open_dataset( name );
    }catch( const ErrH5 & ){
        exists = false;
    }
    return exists;
}

} // namespace IO
} // namespace HIPP
#endif	//_HIPPIO_H5_OBJ_FILE_H_