/**
create: Yangyao CHEN, 2021/10/12
    [write   ] _H5EStackTempOff - turn on/off the HDF5 error stack.
*/

#include "h5_base.h"
#include "h5_error.h"
#include "h5_raw_base.h"

#ifndef _HIPPIO_H5_RAW_ERROR_H_
#define _HIPPIO_H5_RAW_ERROR_H_

namespace HIPP::IO::H5 {
    
class _H5EStackTempOff: public _Obj {
public:
    typedef _Obj parent_t;

    inline static constexpr hid_t vDFLT = H5E_DEFAULT;
    typedef ErrH5::flag_t flag_t;

    /**
    Turn off the HD5 internal error stack ``estack`` and set flag to the
    high-level HDF5 exception reporting flag to ``flag``.

    The environment is restored at the destruction of this instance.
    */
    explicit _H5EStackTempOff(hid_t estack = vDFLT, flag_t flag=0) noexcept;
    virtual ~_H5EStackTempOff() noexcept override;
private:
    H5E_auto_t _old_efunc = NULL;
    void *_old_edata = NULL;
    flag_t _old_flag = 0;
};

inline _H5EStackTempOff::_H5EStackTempOff(hid_t estack, flag_t flag) noexcept 
: _Obj(estack, stFREE), _old_flag(ErrH5::err_cntl_flag())
{
    H5Eget_auto(_obj, &_old_efunc, &_old_edata);
    
    H5Eset_auto(_obj, NULL, NULL);

    ErrH5::err_cntl_flag(flag);
}

inline _H5EStackTempOff::~_H5EStackTempOff() noexcept {
    if( is_auto_free() ) {
        ErrH5::err_cntl_flag(_old_flag);
        H5Eset_auto(_obj, _old_efunc, _old_edata);
    }
}

} // namespace HIPP::IO::H5

#endif	//_HIPPIO_H5_RAW_ERROR_H_
