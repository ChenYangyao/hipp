/**
create: Yangyao CHEN, 2020/11/29
    [write   ] _Obj - intermediate-level wrapper for general HDF5 object.
*/ 
#ifndef _HIPPIO_H5_RAW_BASE_H_
#define _HIPPIO_H5_RAW_BASE_H_

#include "h5_base.h"
#include "h5_error.h"
#include <cstring>

namespace HIPP::IO::H5 {

class _Obj {
public:
    inline static constexpr int stFREE = 0x1;
    
    /**
    @state: how to control the behavior of the object.
        - 0: the object is not responsible for the underlying HDF5 object.
        - stFREE: the object release the underlying HDF5 object on destruction.
    */
    explicit _Obj(hid_t obj = hid_t(-1), int state = stFREE) noexcept;

    virtual ~_Obj() noexcept {}

    _Obj(const _Obj &) = delete;
    _Obj & operator=(const _Obj &) = delete;
    
    /**
    After move, the move-from object is no longer corresponds to a valid HDF5 
    object.
    */
    _Obj(_Obj &&) noexcept;
    _Obj & operator=(_Obj &&) noexcept;
    
    /**
    Return the raw HDF5 object.
    With an argument, set it to the new value and return the old value.
    */
    hid_t raw() const noexcept;
    hid_t raw(hid_t new_val) noexcept;

    /** 
    state_flag() with no argument gets the state flag.
    With argument ``new_val``, set the state flag to it and return the old one.
    */
    int state_flag() const noexcept;
    int state_flag(int new_val) noexcept;
    
    /**
    Whether the life-time of the object is maintained by the library.
    i.e., state_flag() & stFREE.
    */
    bool is_auto_free() const noexcept;
protected:
    hid_t _obj;
    int _state;
};    

inline _Obj::_Obj(hid_t obj, int state) noexcept 
: _obj(obj), _state(state){ }

inline _Obj::_Obj( _Obj && o) noexcept
: _Obj(o._obj, o._state) {
    o._state = 0;
}

inline _Obj & _Obj::operator=(_Obj && o) noexcept {
    if( this != &o ) {
        _obj = o._obj;
        _state = o._state;
        o._state = 0;
    }
    return *this;
}

inline hid_t _Obj::raw() const noexcept { return _obj; }

inline hid_t _Obj::raw(hid_t new_val) noexcept {
    hid_t old_val = _obj;
    _obj = new_val;
    return old_val;
}

inline int _Obj::state_flag() const noexcept {
    return _state;
}

inline int _Obj::state_flag(int new_val) noexcept {
    int old_val = _state;
    _state = new_val;
    return old_val;
}

inline bool _Obj::is_auto_free() const noexcept {
    return _state & stFREE;
}


} // namespace HIPP::IO::H5
#endif	//_HIPPIO_H5_RAW_BASE_H_