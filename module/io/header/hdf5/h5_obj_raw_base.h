/**
 * creat: Yangyao CHEN, 2020/11/29
 *      [write   ] 
 *      @_H5Obj:   Intermediate-level HDF5 object common base class.
 *      @_H5EStackTempOff:  turn off/on the error stack of original HDF5 
 *          library.
 */ 
#ifndef _HIPPIO_H5_OBJ_RAW_BASE_H_
#define _HIPPIO_H5_OBJ_RAW_BASE_H_
#include "h5_base.h"
#include "h5_error.h"
#include <cstring>
namespace HIPP{
namespace IO{
    
class _H5Obj {
public:
    enum: int { stFREE=0x1 };
    
    explicit _H5Obj( id_t id = -1, int state=stFREE ) noexcept : _obj( id ), 
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

class _H5EStackTempOff: public _H5Obj {
public:
    explicit _H5EStackTempOff( id_t estack, ErrH5::flag_t flag=0 ) noexcept {
        _obj = estack;
        H5Eget_auto( _obj, &_old_efunc, &_old_edata );
        H5Eset_auto( _obj, NULL, NULL );
        _old_flag = ErrH5::err_cntl_flag();
        ErrH5::err_cntl_flag(flag);
    }
    ~_H5EStackTempOff() noexcept{
        ErrH5::err_cntl_flag(_old_flag);
        H5Eset_auto( _obj, _old_efunc, _old_edata );
    }
private:
    H5E_auto_t _old_efunc = NULL;
    void *_old_edata = NULL;
    ErrH5::flag_t _old_flag = 0;
};

} // namespace IO
} // namespace HIPP

#endif	//_HIPPIO_H5_OBJ_RAW_BASE_H_