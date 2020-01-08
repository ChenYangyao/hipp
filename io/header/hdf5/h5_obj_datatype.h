#ifndef _HIPPIO_H5_OBJ_DATATYPE_H_
#define _HIPPIO_H5_OBJ_DATATYPE_H_
#include "h5_obj_base.h"
namespace HIPP{
namespace IO{
    
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
};

} // namespace IO
} // namespace HIPP
#endif	//_HIPPIO_H5_OBJ_DATATYPE_H_