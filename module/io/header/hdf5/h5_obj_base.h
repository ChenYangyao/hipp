/**
 * creat: Yangyao CHEN, 2020/01/11
 *      [write   ] H5Obj - base class for all HDF5 high-level objects.
 */ 

#ifndef _HIPPIO_H5_OBJ_BASE_H_
#define _HIPPIO_H5_OBJ_BASE_H_
#include "h5_obj_raw.h"
#include "h5_type_raw.h"
namespace HIPP{
namespace IO{

/**
 * base class for all HDF5 high-level objects.
 * This class should not be directly used.
 * 
 * The H5Obj<H5ObjRaw> contains only a shared pointer member to a H5ObjRaw
 * instance, and use this pointer to maintain the life of the low-level raw
 * object.
 * 
 * The H5Obj instance can be copy-constructed, copy-assigned, move-constructed,
 * and move-assigned. The initializer of H5Obj accepts either an ordinary 
 * pointer (the control is taken over) or a shared pointer of the raw object.
 */
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
    H5Obj( H5Obj &&obj ) noexcept: _obj_ptr( obj._obj_ptr ) 
        { obj._obj_ptr.reset(); }
    H5Obj & operator=( const H5Obj &obj ) noexcept 
        { _obj_ptr = obj._obj_ptr;  return *this; }
    H5Obj & operator=( H5Obj &&obj ) noexcept 
        { _obj_ptr = obj._obj_ptr; obj._obj_ptr.reset(); return *this; }

    /**
     * return the original HDF5 object handler (type hid_t).
     */
    id_t raw() const noexcept { return _obj_ptr->raw(); }
    _obj_raw_t & obj_raw() noexcept { return *_obj_ptr; }
    const _obj_raw_t & obj_raw() const noexcept { return *_obj_ptr; }
protected:
    _obj_ptr_t _obj_ptr;
};

} // namespace IO
} // namespace HIPP
#endif	//_HIPPIO_H5_OBJ_BASE_H_