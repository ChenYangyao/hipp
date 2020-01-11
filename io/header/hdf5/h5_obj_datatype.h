/**
 * creat: Yangyao CHEN, 2020/01/11
 *      [write   ] HDF5 high-level datatype object.
 */ 

#ifndef _HIPPIO_H5_OBJ_DATATYPE_H_
#define _HIPPIO_H5_OBJ_DATATYPE_H_
#include "h5_obj_base.h"
namespace HIPP{
namespace IO{

/**
 * HDF5 high-level datatype object.
 */
class H5Datatype: public H5Obj<_H5Datatype>{
public:
    typedef H5Obj<_H5Datatype> _obj_base_t;
    using _obj_base_t::_obj_base_t;

    /**
     * test whether the two datatype is equal.
     */
    bool equal( const H5Datatype &dtype ) const {
        return bool( _obj_ptr->equal( dtype.raw() ) );
    }
    /**
     * retrurn the byte-size of the datatype.
     */
    size_t size() const {
        return _obj_ptr->get_size();
    }

    /**
     * make a copy of the current datatype
     */
    H5Datatype copy() const {
        auto ptr = std::make_shared<_obj_raw_t>( _obj_ptr->copy( raw() ) );
        return H5Datatype( ptr );
    }

    /**
     * set the size of the current datatype
     */
    void resize( size_t size ){
        _obj_ptr->set_size( size );
    }
};

} // namespace IO
} // namespace HIPP
#endif	//_HIPPIO_H5_OBJ_DATATYPE_H_