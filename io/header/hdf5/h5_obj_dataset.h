#ifndef _HIPPIO_H5_OBJ_DATASET_H_
#define _HIPPIO_H5_OBJ_DATASET_H_
#include "h5_obj_base.h"
#include "h5_obj_dataspace.h"
#include "h5_obj_datatype.h"
namespace HIPP{
namespace IO{

class H5Dataset: public H5Obj<_H5Dataset>{
public:
    typedef H5Obj<_H5Dataset> _obj_base_t;
    using _obj_base_t::_obj_base_t;
    
    H5Dataspace dataspace(){
        auto ptr = std::make_shared<H5Dataspace::_obj_raw_t>( 
            _obj_ptr->get_space() );
        return H5Dataspace( ptr );        
    }
    H5Datatype datatype(){
        auto ptr = std::make_shared<H5Datatype::_obj_raw_t>( 
            _obj_ptr->get_type() );
        return H5Datatype( ptr );
    }
    bool attr_exists(const string &name){
        return bool(_obj_ptr->exist_attr(name.c_str()));
    }
    template<typename T, typename A>
    void write( const vector<T, A> &buff ){
        auto dims = dataspace().dims();
        auto size = std::accumulate( dims.begin(), dims.end(), size_t(1), 
            std::multiplies<size_t>() );
        if( size != buff.size() )
            ErrLogic::throw_( ErrLogic::eLENGTH, emFLPFB, 
                "  ... buff length ", buff.size(), 
                " does not match dataset size ", size, '\n' );
        write<T>( buff.data() );
    }
    template<typename T>
    void write( const T *buff ){
        id_t memtype = H5TypeNative<T>::h5_id;
        id_t dspace = H5S_ALL;
        _obj_ptr->write( memtype, dspace, dspace, H5P_DEFAULT, buff );
    }
    template<typename T, typename A>
    void read( vector<T, A> &buff ){
        auto dims = dataspace().dims();
        auto size = std::accumulate( dims.begin(), dims.end(), size_t(1), 
            std::multiplies<size_t>() );
        buff.resize( size );
        read<T>(buff.data());
    }
    template<typename T>
    void read( T *buff ){
        id_t memtype = H5TypeNative<T>::h5_id;
        id_t dspace =H5S_ALL;
        _obj_ptr->read( memtype, dspace, dspace, H5P_DEFAULT, buff );
    }
    
};
    
} // namespace IO
} // namespace HIPP
#endif	//_HIPPIO_H5_OBJ_DATASET_H_