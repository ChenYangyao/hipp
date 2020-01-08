#ifndef _HIPPIO_H5_OBJ_BASE_H_
#define _HIPPIO_H5_OBJ_BASE_H_
#include "h5_obj_raw.h"
#include "h5_type_raw.h"
namespace HIPP{
namespace IO{

template<typename H5ObjRaw>
class H5Obj {
public:
    typedef hid_t id_t;
    typedef herr_t err_t;
    typedef htri_t tri_t;
    typedef hsize_t size_t;
    typedef H5ObjRaw _obj_raw_t;
    typedef std::shared_ptr<H5ObjRaw> _obj_ptr_t;

    explicit H5Obj( _obj_raw_t *ptr ) noexcept: _obj_ptr(ptr){ }
    explicit H5Obj( _obj_ptr_t ptr ) noexcept: _obj_ptr(ptr){ }
    ~H5Obj() noexcept { }
    
    H5Obj( const H5Obj &obj ) noexcept: _obj_ptr( obj._obj_ptr ) { }
    H5Obj( H5Obj &&obj ) noexcept: _obj_ptr( obj._obj_ptr ) { obj._obj_ptr.reset(); }
    H5Obj & operator=( const H5Obj &obj ) noexcept { _obj_ptr = obj._obj_ptr;  return *this; }
    H5Obj & operator=( H5Obj &&obj ) noexcept 
        { _obj_ptr = obj._obj_ptr; obj._obj_ptr.reset(); return *this; }

    id_t raw() const noexcept { return _obj_ptr->raw(); }
protected:
    _obj_ptr_t _obj_ptr;
};

} // namespace IO
} // namespace HIPP
#endif	//_HIPPIO_H5_OBJ_BASE_H_