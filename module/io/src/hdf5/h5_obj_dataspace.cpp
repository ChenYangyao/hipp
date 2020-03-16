#include "h5_obj_dataspace.h"
namespace HIPP{
namespace IO{

const H5Dataspace H5Dataspace::allval 
    { std::make_shared<H5Dataspace::_obj_raw_t>(H5S_ALL, 0) },
    H5Dataspace::nullval = H5Dataspace::create_null(),
    H5Dataspace::scalarval = H5Dataspace::create_scalar();
    
}
}