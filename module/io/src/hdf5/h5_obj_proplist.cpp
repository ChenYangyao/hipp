#include "h5_obj_proplist.h"
namespace HIPP{
namespace IO{

const H5Proplist H5Proplist::defaultval 
    { std::make_shared<H5Proplist::_obj_raw_t>(H5P_DEFAULT, 0) };

}
}