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

} // namespace IO
} // namespace HIPP


#endif	//_HIPPIO_H5_BASE_H_