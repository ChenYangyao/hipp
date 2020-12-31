/**
 * creat: Yangyao CHEN, 2020/01/11
 * Base file for including in all file in the hdf5 module.
 */ 
#ifndef _HIPPIO_H5_BASE_H_
#define _HIPPIO_H5_BASE_H_
#include <hdf5.h>
#include <hippcntl.h>

namespace HIPP {
namespace IO {

typedef std::size_t size_t;
typedef hid_t id_t;
typedef hsize_t hsize_t;
typedef hssize_t hssize_t;
typedef herr_t err_t;
typedef htri_t tri_t;
typedef H5_index_t index_t;
typedef H5_iter_order_t iter_order_t;

inline extern constexpr index_t 
    INDEX_NAME = H5_INDEX_NAME,
    INDEX_CRT_ORDER = H5_INDEX_CRT_ORDER;

inline extern constexpr iter_order_t 
    ITER_INC = H5_ITER_INC, 
    ITER_DEC = H5_ITER_DEC,
    ITER_NATIVE = H5_ITER_NATIVE;

} // namespace IO
} // namespace HIPP


#endif	//_HIPPIO_H5_BASE_H_