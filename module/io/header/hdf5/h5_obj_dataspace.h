/**
 * creat: Yangyao CHEN, 2020/01/11
 *      [write   ] 
 *      @H5Dataspace: HDF5 high-level dataspace object.
 */ 

#ifndef _HIPPIO_H5_OBJ_DATASPACE_H_
#define _HIPPIO_H5_OBJ_DATASPACE_H_
#include "h5_obj_base.h"
namespace HIPP{
namespace IO{

class H5Dataspace: public H5Obj<_H5Dataspace>{
public:
    typedef H5Obj<_H5Dataspace> _obj_base_t;
    
    static const H5Dataspace allval, nullval, scalarval;

    using _obj_base_t::_obj_base_t;
    H5Dataspace( const vector<hsize_t> &dims );
    H5Dataspace( const vector<hsize_t> &dims, const vector<hsize_t> &maxdims );
    static H5Dataspace create_null();
    static H5Dataspace create_scalar();
    static H5Dataspace create_simple();

    int ndims( )const;
    vector<hsize_t> dims( )const;
    vector<hsize_t> maxdims( )const;
    hsize_t size() const;
    void select_hyperslab( 
        const vector<hsize_t> &start, const vector<hsize_t> &count );
    void select_hyperslab( const string &op,
        const hsize_t *start, const hsize_t *count, 
        const hsize_t *stride = NULL, const hsize_t *block = NULL );
    hssize_t get_select_npoint()const;

    
protected:
    static H5Dataspace _from_raw( id_t dspace ) noexcept;
};

inline H5Dataspace H5Dataspace::_from_raw( id_t dspace ) noexcept{
    auto ptr = std::make_shared<_obj_raw_t>( dspace );
    return H5Dataspace( ptr );
}

} // namespace IO
} // namespace HIPP
#endif	//_HIPPIO_H5_OBJ_DATASPACE_H_