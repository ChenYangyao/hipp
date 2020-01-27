/**
 * creat: Yangyao CHEN, 2020/01/11
 *      [write   ] H5Dataspace - HDF5 high-level dataspace object.
 *      [modify  ]
 *      [debug   ]
 *      [comment ]
 */ 

#ifndef _HIPPIO_H5_OBJ_DATASPACE_H_
#define _HIPPIO_H5_OBJ_DATASPACE_H_
#include "h5_obj_base.h"
namespace HIPP{
namespace IO{

/**
 * HDF5 high-level dataspace object.
 */
class H5Dataspace: public H5Obj<_H5Dataspace>{
public:
    typedef H5Obj<_H5Dataspace> _obj_base_t;
    using _obj_base_t::_obj_base_t;

    int ndims( )const{
        return _obj_ptr->get_simple_extent_ndims();
    }
    vector<size_t> dims( )const{
        int _ndims = ndims();
        vector<size_t> _dims( _ndims, 0 );
        _obj_ptr->get_simple_extent_dims( _dims.data(), NULL );
        return _dims;
    }
    size_t size() const{
        auto _dims = dims();
        size_t _size = std::accumulate(
            _dims.begin(), _dims.end(), size_t(1), std::multiplies<size_t>());
        return _size;
    }
    vector<size_t> maxdims( )const{
        int _ndims = ndims();
        vector<size_t> _maxdims( _ndims, 0 );
        _obj_ptr->get_simple_extent_dims( NULL, _maxdims.data() );
        return _maxdims;
    }
};

} // namespace IO
} // namespace HIPP
#endif	//_HIPPIO_H5_OBJ_DATASPACE_H_