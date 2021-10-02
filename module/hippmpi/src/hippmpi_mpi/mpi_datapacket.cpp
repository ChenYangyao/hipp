#include "mpi_datapacket.h"
namespace HIPP::MPI {

ostream & Datapacket::info(ostream &os, int fmt_cntl) const {
    PStream ps(os);
    if( fmt_cntl == 0 ) {
        ps << "Datapacket{buff=", _buff, ", size", _size, 
            ", dtype=", _dtype, "}";
    } else {
        ps << HIPPCNTL_CLASS_INFO(HIPP::MPI::Datapacket), 
            "  Buffer{buff=", _buff, ", size=", _size, "}\n",
            _dtype, '\n';
    }
    return os;
}

ostream & ConstDatapacket::info(ostream &os, int fmt_cntl) const {
    PStream ps(os);
    if( fmt_cntl == 0 ) {
        ps << "Datapacket{buff=", _buff, ", size", _size, 
            ", dtype=", _dtype, "}";
    } else {
        ps << HIPPCNTL_CLASS_INFO(HIPP::MPI::Datapacket), 
            "  Buffer{buff=", _buff, ", size=", _size, "}\n",
            _dtype, '\n';
    }
    return os;
}

} // namespace HIPP::MPI
