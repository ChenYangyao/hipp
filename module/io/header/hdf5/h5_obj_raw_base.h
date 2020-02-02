#ifndef _HIPPIO_H5_OBJ_RAW_BASE_H_
#define _HIPPIO_H5_OBJ_RAW_BASE_H_
#include "h5_base.h"
#include "h5_error.h"
#include <cstring>
namespace HIPP{
namespace IO{
    
class _H5Obj {
public:
    typedef hid_t id_t;
    typedef hsize_t size_t;
    typedef herr_t err_t;
    typedef htri_t tri_t;
    
    explicit _H5Obj( id_t id = -1, int state=1 ) noexcept : _obj( id ), 
        _state(state){}
    ~_H5Obj() noexcept {}

    _H5Obj( const _H5Obj & ) = delete;
    _H5Obj & operator=( const _H5Obj & ) = delete;
    _H5Obj( _H5Obj && ) = delete;
    _H5Obj & operator=( _H5Obj && ) = delete;
    
    id_t raw() const noexcept { return _obj; }
protected:
    id_t _obj;
    int _state;
};    


} // namespace IO
} // namespace HIPP

#endif	//_HIPPIO_H5_OBJ_RAW_BASE_H_