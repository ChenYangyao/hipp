/**
 * creat: Yangyao CHEN, 2020/01/11
 * HD5 interfaces.
 */ 

#ifndef _HIPPIO_H5_H_
#define _HIPPIO_H5_H_

/**
 * basic utils of implementing this sub-package.
 */
#include "h5_base.h"
#include "h5_error.h"

/**
 * intermediate interface, semi-OOP wrapper of original HDF5 interface.
 * Users are strongly recommended to use only the high-level interface below.
 */
#include "h5_obj_raw.h"
#include "h5_type_raw.h"

/**
 * high-level, pure OOP interface.
 */
#include "h5_obj.h"
#endif	//_HIPPIO_H5_H_