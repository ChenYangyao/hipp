/**
create: Yangyao CHEN, 2020/01/11
    [write   ] Base header file to be included by any in this module.
*/
#ifndef _HIPPIO_H5_BASE_H_
#define _HIPPIO_H5_BASE_H_

#include <hdf5.h>
#include <hippcntl.h>

namespace HIPP::IO::H5 {

/**
Typedefs in the HDF5 IO module.
*/
typedef ::hid_t             hid_t;
typedef ::hsize_t           hsize_t;
typedef ::hssize_t          hssize_t;
typedef ::herr_t            herr_t;
typedef ::htri_t            htri_t;

typedef H5_index_t          index_t;
typedef H5_iter_order_t     iter_order_t;

inline static constexpr index_t 
    idxNAME      = H5_INDEX_NAME,
    idxCRT_ORDER = H5_INDEX_CRT_ORDER;

inline static constexpr iter_order_t 
    iterINC        = H5_ITER_INC, 
    iterDEC        = H5_ITER_DEC,
    iterNATIVE     = H5_ITER_NATIVE;

} // namespace HIPP::IO::H5

#endif	//_HIPPIO_H5_BASE_H_