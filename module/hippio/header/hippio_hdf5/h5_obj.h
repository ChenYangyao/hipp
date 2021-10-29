/**
create: Yangyao CHEN, 2020/01/11
Including all HDF5 high-level OOP API.
*/ 

#ifndef _HIPPIO_H5_OBJ_H_
#define _HIPPIO_H5_OBJ_H_

#include "h5_obj_base.h"

#include "h5_obj_proplist.h"
#include "h5_obj_dataspace.h"
#include "h5_obj_attr.h"

#include "h5_obj_datatype.h"
#include "h5_obj_dataset.h"
#include "h5_obj_group.h"
#include "h5_obj_file.h"

namespace HIPP::IO {

using ErrH5 = H5::ErrH5;

using H5File = H5::File;
using H5Group = H5::Group;
using H5Dataset = H5::Dataset;
using H5Datatype = H5::Datatype;
using H5Attr = H5::Attr;
using H5Dataspace = H5::Dataspace;
using H5Proplist = H5::Proplist;

} // namespace HIPP::IO

#endif	//_HIPPIO_H5_OBJ_H_