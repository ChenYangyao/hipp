#ifndef _HIPPIO_H5_OBJ_FILE_H_
#define _HIPPIO_H5_OBJ_FILE_H_
#include "h5_obj_base.h"
#include "h5_obj_dataset.h"
namespace HIPP{
namespace IO{


class H5File: public H5Obj<_H5File>{
public:
    typedef H5Obj<_H5File> _obj_base_t;
    using _obj_base_t::_obj_base_t;

    H5File( const string &name, const string &flag)
        : _obj_base_t( std::make_shared<_obj_raw_t>( name.c_str(), flag ) ){ }

    /**
     * create a dataset of native type `T` under the file instance.
     * @name:   dataset name.
     * @dims:   dimensions.
     * @flag:   if the dataset of name `name` exists, flag specifies how to
     *          deal with it. Possible values are:
     *          "trunc": just open it.
     *          "excl":  do not open and throw a exception `ErrH5`.
     */
    template<typename T>
    H5Dataset create_dataset( const string &name, const vector<size_t> &dims, 
        const string &flag="trunc" )
    {
        H5Dataset dset(NULL);
        try{
            _H5EStackTempOff estk(H5E_DEFAULT);
            int rank = dims.size();
            _H5Dataspace dspace( rank, dims.data() );
            dset = H5Dataset(
                std::make_shared<H5Dataset::_obj_raw_t>( raw(), name.c_str(), 
                    H5TypeNative<T>::h5_id, dspace.raw() ));
        }catch( ErrH5 &e ){
            if( flag == "trunc" ){
                dset = open_dataset( name );
            }else if( flag == "excl" ){
                ErrH5::throw_( -1, emFLPFB, "  ... dataset ", name, " exists\n" );
            }else{
                ErrLogic::throw_(ErrLogic::eINVALIDARG, emFLPFB, 
                    "  ... invalid flag ", trunc, '\n');
            }
        }
        return dset;
    }
    /**
     * open an existing dataset of name `name`.
     * If not existing, throw an error `ErrH5`.
     */
    H5Dataset open_dataset( const string &name ){
        return H5Dataset(
            std::make_shared<H5Dataset::_obj_raw_t>( 
                _obj_ptr->open_dataset( name.c_str() ) 
            ));
    }
};


} // namespace IO
} // namespace HIPP
#endif	//_HIPPIO_H5_OBJ_FILE_H_