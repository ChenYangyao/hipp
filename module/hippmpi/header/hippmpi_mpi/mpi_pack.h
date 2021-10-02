/**
create: Yangyao CHEN, 2021/09/28
    [write   ] Pack and ExternPack - for packing data to communication buffer.
*/

#ifndef _HIPPMPI_MPI_PACK_H_
#define _HIPPMPI_MPI_PACK_H_
#include "mpi_base.h"
#include "mpi_error.h"
#include "mpi_raw_pack.h"
#include "mpi_comm.h"
#include "mpi_datapacket.h"
namespace HIPP::MPI {

namespace _hippmpi_mpi_pack_helper {

class PackBuffer {
public:
    PackBuffer(size_t size_prealloc=0);

    ~PackBuffer();
    PackBuffer(const PackBuffer &o);
    PackBuffer(PackBuffer &&o) noexcept;
    PackBuffer & operator=(const PackBuffer &o);
    PackBuffer & operator=(PackBuffer &&o) noexcept;

    const void * get_base() const noexcept                                      { return _buff.data(); }
    void * get_base() noexcept                                                  { return _buff.data(); }
    size_t get_size() const noexcept                                            { return _buff.size(); }
    aint_t get_position() const noexcept                                        { return _position; }

    PackBuffer & set_size(size_t s);
    PackBuffer & set_position(aint_t p) noexcept;
protected:
    friend class Datapacket;

    vector<char> _buff;
    aint_t _position;

    void _check_and_resize(aint_t insize);
};

} // namespace _hippmpi_mpi_pack_helper

/**
Pack provides, with static methods, the pack/unpack operations like the 
standard MPI. 

A pack instance has a buffer (represented by a base address + a size in 
bytes) and a position pointer which keeps the byte offset from base address. 
On intialization, the buffer is set to be:

---------------- (preallocated size)  -----------------
^ 
base
^
position (started at 0).

A pack instance encapsulates the buffer and the pack/unpack operations. 
By using push() method, the buffer is automatically enlarged to fit the
need before data are packed into it. The position pointer is also 
automatically moved. The pop() method, on the other hand, unpack 
the buffer and move the position pointer. 

Be cautious to resize the Pack instance to suitable length before 
receiving, and to reset the position to 0 before unpack.
*/
class Pack: public _hippmpi_mpi_pack_helper::PackBuffer {
public:
    typedef _Pack _obj_raw_t;
    typedef _hippmpi_mpi_pack_helper::PackBuffer super_t;

    /**
    Initialize the pack instance with "size_prealloc" bytes buffer, used 
    for packing/unpacking in the communicator "comm".
    @comm:   communicator that would be used in packing/unpacking.
    @size_prealloc:  preallocated bytes for buffer - not necessary to be 
         large because push() automatically resize it or because it can 
         be adjusted by set_size().
    The position pointer is set to 0.
    */
    Pack(Comm comm);
    Pack(size_t size_prealloc, Comm comm);

    /**
    The copy operation is a deep-copy, i.e., all information (include 
    the buffer content, buffer size, position pointer and communicator)
    gets copied.
    The move operator is a cheap move, i.e., the moved object cannot be 
    used any more.
    */
    ~Pack() {}
    Pack(const Pack &o);
    Pack(Pack &&o) noexcept;
    Pack & operator=(const Pack &o);
    Pack & operator=(Pack &&o) noexcept;

    /**
    push() packs "in_dpacket" into the buffer. It automatically resizes the
    buffer if memory is not enough to hold "in_dpacket". The position pointer
    is increased after push().
    
    pop() perfoms oppisite operation - unpack from the current position 
    to "out_dpacket", and move the position pointer.
    */
    Pack & push(const ConstDatapacket &in_dpacket);
    Pack & pop(const Datapacket &out_dpacket);
    ConstDatapacket as_sendbuf() const;
    Datapacket as_recvbuf();

    /** 
    Get the communicator.
    Other attribute access methods are provided through super class.
    */
    const Comm & get_comm() const noexcept                                      { return _comm; }
    Comm & get_comm() noexcept                                                  { return _comm; }

    /**
    Set the buffer size, its position pointer, or the communicator.
    */
    Pack & set_size(size_t s)                                                   { super_t::set_size(s); return *this; }
    Pack & set_position(aint_t p) noexcept                                      { super_t::set_position(p); return *this; }
    Pack & set_comm(Comm comm) noexcept                                         { _comm = std::move(comm); return *this; }

    /**
    Mapped to standard MPI calls.
    */
    static void pack(const ConstDatapacket &in_dpacket, 
        void *outbuf, int outsize, int &position, const Comm &comm);
    static void unpack(const void *inbuf, int insize, int &position,
        const Datapacket &out_dpacket, const Comm &comm);
    static int size(int incount, const Datatype &dtype, const Comm &comm);
    static int size(const ConstDatapacket &in_dpacket, const Comm &comm);
protected:
    Comm _comm;
};

/**
The canonical counterpart of Pack.
*/
class ExternalPack: public _hippmpi_mpi_pack_helper::PackBuffer {
public:
    typedef _Pack _obj_raw_t;
    typedef _hippmpi_mpi_pack_helper::PackBuffer super_t;

    ExternalPack(const string &datarep="external32");
    ExternalPack(size_t size_prealloc, const string &datarep="external32");
    
    ~ExternalPack() {}
    ExternalPack(const ExternalPack &o);
    ExternalPack(ExternalPack &&o) noexcept;
    ExternalPack & operator=(const ExternalPack &o);
    ExternalPack & operator=(ExternalPack &&o) noexcept;

    ExternalPack & push(const ConstDatapacket &in_dpacket);
    ExternalPack & pop(const Datapacket &out_dpacket);

    ConstDatapacket as_sendbuf() const;
    Datapacket as_recvbuf();

    const string & get_datarep() const noexcept                                 { return _datarep; }

    ExternalPack & set_size(size_t s)                                           { super_t::set_size(s); return *this; }
    ExternalPack & set_position(aint_t p) noexcept                              { super_t::set_position(p); return *this; }
    ExternalPack & set_datarep(const string &datarep)                           { _datarep = datarep; return *this; }

    static void pack(const ConstDatapacket &in_dpacket, 
        void *outbuf, aint_t outsize, aint_t &position, 
        const string &datarep="external32");
    static void unpack(const void *inbuf, aint_t insize, 
        aint_t &position, const Datapacket &out_dpacket, 
        const string &datarep="external32");
    static aint_t size(int incount, const Datatype &dtype, 
        const string &datarep="external32");
    static aint_t size(const ConstDatapacket &in_dpacket, 
        const string &datarep="external32");
protected:
    string _datarep;
};

} // namespace HIPP::MPI
#endif	//_HIPPMPI_MPI_PACK_H_