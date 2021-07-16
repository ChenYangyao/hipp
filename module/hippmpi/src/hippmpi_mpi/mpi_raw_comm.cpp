#include <mpi_raw_comm.h>
namespace HIPP{
namespace MPI{
const  _Comm::copy_attr_fn_t 
    *const _Comm::NULL_COPY_FN = MPI_NULL_COPY_FN, 
    *const _Comm::DUP_FN = MPI_COMM_DUP_FN;
const  _Comm::del_attr_fn_t 
    *const _Comm::NULL_DEL_FN = MPI_NULL_DELETE_FN;

}
}