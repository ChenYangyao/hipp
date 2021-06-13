#ifndef _HIPPMPI_MPI_OBJ_BASE_H_
#define _HIPPMPI_MPI_OBJ_BASE_H_
#include "mpi_base.h"
#include "mpi_error.h"
namespace HIPP{
namespace MPI{

template<typename MPIObjRaw>
class MPIObj {
public:
    typedef MPIObjRaw _obj_raw_t;
    typedef typename _obj_raw_t::mpi_t mpi_t;
    typedef std::shared_ptr<_obj_raw_t> _obj_ptr_t;

    explicit MPIObj( _obj_ptr_t ptr ) noexcept: _obj_ptr( ptr ){ }
    explicit MPIObj( _obj_raw_t *ptr ) noexcept: _obj_ptr( ptr ){ }
    ~MPIObj() noexcept{ }

    MPIObj( const MPIObj &obj ) noexcept
        : _obj_ptr(obj._obj_ptr){ }
    MPIObj( MPIObj &&obj ) noexcept
        : _obj_ptr(obj._obj_ptr){ obj._obj_ptr.reset(); }
    MPIObj & operator=( const MPIObj &obj ) noexcept
        { _obj_ptr = obj._obj_ptr; return *this; }
    MPIObj & operator=( MPIObj &&obj ) noexcept
        { _obj_ptr = obj._obj_ptr; obj._obj_ptr.reset(); return *this; }
    
    mpi_t raw() const noexcept { return _obj_ptr->raw(); }
protected:
    _obj_ptr_t _obj_ptr;
};
    
} // namespace MPI
} // namespace HIPP

#endif	//_HIPPMPI_MPI_OBJ_BASE_H_