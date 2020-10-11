#include "mpi_datapacket.h"
namespace HIPP{
namespace MPI{

Datapacket::Datapacket( const void *buff, int size, Datatype dtype ) noexcept
    :_buff((void *)buff), _size(size), _dtype(dtype){ }
Datapacket::Datapacket( const void *buff, int size, const string &dtype )
    :_buff((void *)buff), _size(size), _dtype(NULL)
{
    auto it = _typecvt.find( dtype );
    if( it == _typecvt.end() )
        ErrLogic::throw_(ErrLogic::eDOMAIN, emFLPFB, 
            "  ... cannot find datatype ", dtype, '\n');
    _dtype = *it->second;
}

Datapacket::Datapacket( const string &buff ) noexcept
    :Datapacket( buff.data(), buff.size(), CHAR ){ }
Datapacket::Datapacket(const Datapacket &p) noexcept
    :Datapacket( p._buff, p._size, p._dtype ){ }
Datapacket::Datapacket(Datapacket &&p) noexcept
    :Datapacket( p._buff, p._size, std::move(p._dtype) ){ }
Datapacket::Datapacket(aint_t disp, int size, Datatype dtype) noexcept
:Datapacket((char *)(0)+disp, size, dtype)
{}
Datapacket::Datapacket(aint_t disp, int size, const string &dtype)
:Datapacket((char *)(0)+disp, size, dtype)
{}
Datapacket & Datapacket::operator=(const Datapacket &p) noexcept{
    if( this != &p ){
        _buff = p._buff;
        _size = p._size;
        _dtype = p._dtype;
    }
    return *this;
}
Datapacket & Datapacket::operator=(Datapacket &&p) noexcept{
    if( this != &p ){
        _buff = p._buff;
        _size = p._size;
        _dtype = std::move(p._dtype);
    }
    return *this;
}

} // namespace MPI
} // namespace HIPP
