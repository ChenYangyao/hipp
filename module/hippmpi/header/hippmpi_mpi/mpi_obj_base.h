/**
create: Yangyao CHEN, 2021/09/27
    [write   ] MPIObj - The base class of HIPP's high-level MPI classes.
*/

#ifndef _HIPPMPI_MPI_OBJ_BASE_H_
#define _HIPPMPI_MPI_OBJ_BASE_H_
#include "mpi_base.h"
#include "mpi_error.h"
namespace HIPP{
namespace MPI{

/**
The base class of HIPP's high-level MPI classes.
MPIObj is intended to be used as the template of other high-level classes. For
that, the destructors is non-virtual.

@MPIObjRaw - the type of the intermediate-level object hosted by the high-level
object.

MPIObj is copyable and movable. The copy is shallow, i.e., the copied-from 
and copied-to objects referred to the same intermediate object. The move
always transfers the ownership - the resulted moved-from object refers nothing
and has_referenced() returns ``false``.
*/
template<typename MPIObjRaw>
class MPIObj {
public:
    /**
    _obj_raw_t, _obj_ptr_t: type of the intermediate object and the shared 
        pointer type to it.
    mpi_t: underlying standard MPI type.
    */
    typedef MPIObjRaw _obj_raw_t;
    typedef typename _obj_raw_t::mpi_t mpi_t;
    typedef std::shared_ptr<_obj_raw_t> _obj_ptr_t;

    /**
    Constructors.
    @ptr:either a raw pointer or a shared pointer to the intermediate-level 
        object. The raw pointer is taken over by the constructed instance, i.e., 
        the object referred is destructed at the destruction of the instance.
    */
    explicit MPIObj( _obj_ptr_t ptr ) noexcept: _obj_ptr( std::move(ptr) ) { }
    explicit MPIObj( _obj_raw_t *ptr ) noexcept: _obj_ptr( ptr ) { }
    ~MPIObj() noexcept { }

    /**
    Copy and move operations.
    */
    MPIObj(const MPIObj &obj) noexcept : _obj_ptr(obj._obj_ptr){ }
    MPIObj(MPIObj &&obj) noexcept : _obj_ptr( std::move(obj._obj_ptr) ){ }
    MPIObj & operator=(const MPIObj &obj) noexcept { 
        _obj_ptr = obj._obj_ptr; return *this; 
    }
    MPIObj & operator=( MPIObj &&obj ) noexcept { 
        _obj_ptr = std::move(obj._obj_ptr); 
        return *this; 
    }
    
    /**
    Return the MPI standard object. e.g., for the communicator, return a 
    ``MPI_Comm`` value.
    */
    mpi_t raw() const noexcept { return _obj_ptr->raw(); }

    /**
    Find whether or not the high-level object refers to a intermediate object.
    If no, simply like a NULL pointer.
    */
    bool has_referenced() const noexcept { return static_cast<bool>(_obj_ptr); }
protected:
    _obj_ptr_t _obj_ptr;
};
    
} // namespace MPI
} // namespace HIPP

#endif	//_HIPPMPI_MPI_OBJ_BASE_H_