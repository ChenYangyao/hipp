#include <mpi_pack.h>

namespace HIPP::MPI {

namespace _hippmpi_mpi_pack_helper {

PackBuffer::PackBuffer(size_t size_prealloc)
: _buff(size_prealloc), _position(0)
{}

PackBuffer::~PackBuffer() {}

PackBuffer::PackBuffer(const PackBuffer &o) 
: _buff(o._buff), _position(o._position) {}

PackBuffer::PackBuffer(PackBuffer &&o) noexcept 
: _buff(std::move(o._buff)), _position(o._position)
{}

PackBuffer & PackBuffer::operator=(const PackBuffer &o) {
    if(this != &o){
        _buff = o._buff;
        _position = o._position;
    }
    return *this;
}

PackBuffer & PackBuffer::operator=(PackBuffer &&o) noexcept {
    if( this != &o ){
        _buff = std::move(o._buff);
        _position = o._position;
    }
    return *this;
}

PackBuffer & PackBuffer::set_size(size_t s) { _buff.resize(s); return *this; }

PackBuffer & PackBuffer::set_position(aint_t p) noexcept { 
    _position = p; return *this; 
}

void PackBuffer::_check_and_resize(aint_t insize) {
    size_t size_need = _position + insize, 
        size_avail = _buff.size();
    if( size_need > size_avail ) 
        _buff.resize(std::max(size_need, 2*size_avail));
}
} // namespace _hippmpi_mpi_pack_helper

Pack::Pack(Comm comm)                                                             
: super_t(), _comm(std::move(comm)) {}

Pack::Pack(size_t size_prealloc, Comm comm)                                       
: super_t(size_prealloc), _comm(std::move(comm)) {}

Pack::Pack(const Pack &o)                                                         
: super_t(o), _comm(o._comm) {}

Pack::Pack(Pack &&o) noexcept                                                     
: super_t(std::move(o)), _comm(std::move(_comm)) {}

Pack & Pack::operator=(const Pack &o) { 
    if( this != &o ){ 
        super_t::operator=(o); 
        _comm = o._comm; 
    } 
    return *this; 
}

Pack & Pack::operator=(Pack &&o) noexcept { 
    if( this != &o ){ 
        super_t::operator=(std::move(o)); 
        _comm = std::move(o._comm); 
    } 
    return *this; 
}

Pack & Pack::push(const Datapacket &in_dpacket) {
    int insize = size(in_dpacket, _comm);
    _check_and_resize(insize);

    int position = _position;
    pack(in_dpacket, _buff.data(), _buff.size(), position, _comm);
    _position = position;

    return *this;
}
Pack & Pack::pop(const Datapacket &out_dpacket) {
    int position = _position;
    unpack(_buff.data(), _buff.size(), position, out_dpacket, _comm);
    _position = position;

    return *this;
}
Datapacket Pack::as_sendbuf() const { 
    return { get_base(), int(get_position()), PACKED }; 
}

Datapacket Pack::as_recvbuf() { 
    return { get_base(), int(get_size()), PACKED }; 
}

void Pack::pack(const Datapacket &in_dpacket, 
    void *outbuf, int outsize, int &position, const Comm &comm)
{
    const auto &dp = in_dpacket;
    _obj_raw_t::pack(dp._buff, dp._size, dp._dtype.raw(), 
        outbuf, outsize, position, comm.raw());
}

void Pack::unpack(const void *inbuf, int insize, int &position,
    const Datapacket &out_dpacket, const Comm &comm)
{
    const auto &dp = out_dpacket;
    _obj_raw_t::unpack(inbuf, insize, position, 
        dp._buff, dp._size, dp._dtype.raw(), comm.raw());
}

int Pack::size(int incount, const Datatype &dtype, const Comm &comm) {
    return _obj_raw_t::size(incount, dtype.raw(), comm.raw());
}

int Pack::size(const Datapacket &in_dpacket, const Comm &comm) {
    const auto &dp = in_dpacket;
    return size(dp._size, dp._dtype, comm);
}

ExternalPack::ExternalPack(const string &datarep) 
: super_t(), _datarep(datarep) {}

ExternalPack::ExternalPack(size_t size_prealloc, const string &datarep)
: super_t(size_prealloc), _datarep(datarep) {}

ExternalPack::ExternalPack(const ExternalPack &o)                                         
: super_t(o), _datarep(o._datarep) {}

ExternalPack::ExternalPack(ExternalPack &&o) noexcept                                     
: super_t(std::move(o)), _datarep(std::move(_datarep)) {}

ExternalPack & ExternalPack::operator=(const ExternalPack &o) { 
    if( this != &o ){ 
        super_t::operator=(o); 
        _datarep = o._datarep; 
    } 
    return *this; 
}

ExternalPack & ExternalPack::operator=(ExternalPack &&o) noexcept { 
    if( this != &o ){ 
        super_t::operator=(std::move(o)); 
        _datarep = std::move(o._datarep); 
    } 
    return *this; 
}

ExternalPack & ExternalPack::push(const Datapacket &in_dpacket)
{
    aint_t insize = size(in_dpacket, _datarep);
    _check_and_resize(insize);

    pack(in_dpacket, _buff.data(), _buff.size(), _position, _datarep);
    return *this;
}

ExternalPack & ExternalPack::pop(const Datapacket &out_dpacket)
{
    unpack(_buff.data(), _buff.size(), _position, 
        out_dpacket, _datarep);
    return *this;
}

Datapacket ExternalPack::as_sendbuf() const { 
    return { get_base(), int(get_position()), BYTE }; 
}

Datapacket ExternalPack::as_recvbuf() { 
    return { get_base(), int(get_size()), BYTE }; 
}

void ExternalPack::pack(const Datapacket &in_dpacket, 
    void *outbuf, aint_t outsize, aint_t &position, 
    const string &datarep) 
{
    const auto &dp = in_dpacket;
    _obj_raw_t::pack_external(datarep.c_str(), 
        dp._buff, dp._size, dp._dtype.raw(), 
        outbuf, outsize, position);   
}

void ExternalPack::unpack(const void *inbuf, aint_t insize, 
    aint_t &position, const Datapacket &out_dpacket, 
    const string &datarep)
{
    const auto &dp = out_dpacket;
    _obj_raw_t::unpack_external(datarep.c_str(), inbuf, insize, position, 
        dp._buff, dp._size, dp._dtype.raw());
}

aint_t ExternalPack::size(int incount, const Datatype &dtype, 
    const string &datarep)
{
    return _obj_raw_t::external_size(datarep.c_str(), incount, dtype.raw());
}

aint_t ExternalPack::size(const Datapacket &in_dpacket, 
    const string &datarep)
{
    const auto &dp = in_dpacket;
    return size(dp._size, dp._dtype, datarep);
}

} // namespace HIPP::MPI
