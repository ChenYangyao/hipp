#ifndef _HIPPMPI_MPI_COMM_H_
#define _HIPPMPI_MPI_COMM_H_
#include "mpi_obj_base.h"
#include "mpi_raw_comm.h"
#include "mpi_group.h"
#include "mpi_datapacket.h"
#include "mpi_status.h"
namespace HIPP{
namespace MPI{

class Comm: public MPIObj<_Comm> {
public:
    typedef MPIObj<_Comm> _obj_base_t;
    using _obj_base_t::_obj_base_t;

    ostream &info( ostream &os = cout, int fmt_cntl = 1 ) const;
    friend ostream & operator<<( ostream &os, const Comm &comm );
    
    void free() noexcept;

    int size() const;
    int rank() const;
    bool is_null() const;

    Comm split( int color, int key = 0 )const;
    Comm dup();
    Comm create( const Group &group);
    static Comm world() noexcept;
    static Comm selfval() noexcept;
    static Comm nullval() noexcept;

    const Group group() const;
    Group group();

    template<typename ...Args>
    void send( int dest, int tag, Args && ...args );

    template<typename ...Args>
    Status recv( int src, int tag, Args && ...args );
protected:
    static Comm _from_raw(mpi_t obj, int state);
};

inline ostream & operator<<( ostream &os, const Comm &comm )
    { return comm.info(os); }
inline Comm Comm::_from_raw(mpi_t obj, int state){
    return Comm( std::make_shared<_obj_raw_t>(obj, state) );
}

template<typename ...Args>
void Comm::send( int dest, int tag, Args && ...args ){
    Datapacket dp( std::forward<Args>(args)... );
    _obj_ptr->send( dp._buff, dp._size, dp._dtype.raw(), dest, tag );
}

template<typename ...Args>
Status Comm::recv( int src, int tag, Args && ...args ){
    Datapacket dp( std::forward<Args>(args)... );
    return _obj_ptr->recv( dp._buff, dp._size, dp._dtype.raw(), src, tag );
}


} // namespace MPI
} // namespace HIPP

#endif	//_HIPPMPI_MPI_COMM_H_