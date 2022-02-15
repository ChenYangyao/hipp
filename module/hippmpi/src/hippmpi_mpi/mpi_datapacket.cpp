#include "mpi_datapacket.h"
namespace HIPP::MPI {

ostream & Datapacket::info(ostream &os, int fmt_cntl, int level) const {
    PStream ps {os};
    if( fmt_cntl == 0 ) {
        ps << HIPPCNTL_CLASS_INFO_INLINE(Datapacket), 
            "{buff=", _buff, ", size", _size, ", dtype=", _dtype, "}";
    } else {
        auto ind = HIPPCNTL_CLASS_INFO_INDENT_STR(level);
        ps << HIPPCNTL_CLASS_INFO(Datapacket), 
            ind, "Buffer {buff=", _buff, ", size=", _size, "}\n",
            ind; _dtype.info(os, fmt_cntl, level+1);
    }
    return os;
}

ostream & ConstDatapacket::info(ostream &os, int fmt_cntl, int level) const {
    PStream ps(os);
    if( fmt_cntl == 0 ) {
        ps << HIPPCNTL_CLASS_INFO_INLINE(ConstDatapacket), 
        "{buff=", _buff, ", size", _size, ", dtype=", _dtype, "}";
    } else {
        auto ind = HIPPCNTL_CLASS_INFO_INDENT_STR(level);
        ps << HIPPCNTL_CLASS_INFO(Datapacket), 
            ind, "Buffer {buff=", _buff, ", size=", _size, "}\n",
            ind; _dtype.info(os, fmt_cntl, level+1);
    }
    return os;
}

} // namespace HIPP::MPI
