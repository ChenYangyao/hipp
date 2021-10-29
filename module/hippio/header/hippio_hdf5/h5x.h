#ifndef _HIPPIO_H5X_H_
#define _HIPPIO_H5X_H_

#include "h5x_table.h"

namespace HIPP::IO {

template<typename R>
using H5XTable = H5::XTable<R>;


} // namespace HIPP::IO

#endif	//_HIPPIO_H5X_H_