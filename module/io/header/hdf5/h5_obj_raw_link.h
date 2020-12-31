#ifndef _H5_OBJ_RAW_LINK_H_
#define _H5_OBJ_RAW_LINK_H_
#include "h5_obj_raw_base.h"
namespace HIPP::IO {

class _H5Link {
public:
    static tri_t exists(id_t loc_id, const char *name, id_t lapl_id) {
        tri_t ret = H5Lexists(loc_id, name, lapl_id);
        ErrH5::check(ret, emFLPFB, "  ... name ", name);
        return ret;
    }
    static string get_name_by_idx(id_t loc_id, const char *group_name, 
        index_t index_field, iter_order_t order, hsize_t n, id_t lapl_id)
    {
        auto size = H5Lget_name_by_idx(loc_id, group_name, 
            index_field, order, n, NULL, 0, lapl_id);
        ErrH5::check(size, emFLPFB, "   ... size check failed\n");
        vector<char> name(size+1);
        ErrH5::check(
            H5Lget_name_by_idx(loc_id, group_name, index_field, order, 
            n, name.data(), size+1, lapl_id));
        return string((const char *)name.data(), size);
    }


};

    
} // namespace HIPP::IO
#endif	//_H5_OBJ_RAW_LINK_H_