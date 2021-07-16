#ifndef _HIPPMPI_MPI_MEMORY_H_
#define _HIPPMPI_MPI_MEMORY_H_
#include "mpi_info.h"
namespace HIPP{
namespace MPI{
    
inline 
void * alloc_mem(aint_t size, const Info &info = Info::nullval()){
    void *base_ptr;
    ErrMPI::check(
        MPI_Alloc_mem(size, info.raw(), &base_ptr), emFLPFB, 
        "  ... allocate size=", size, '\n');
    return base_ptr;
}

inline 
void free_mem(void *base){
    ErrMPI::check(
        MPI_Free_mem(base), emFLPFB, "  ... free memory at ", base, '\n');
}

inline 
void buffer_attach(void *buffer, int size){
    ErrMPI::check(
        MPI_Buffer_attach(buffer, size), emFLPFB, 
        " ... failed to attach ", size, " bytes at ", buffer, '\n');
}

inline 
std::pair<void *, int> buffer_detach(){
    void *buffer_addr;
    int size;
    ErrMPI::check(MPI_Buffer_detach(&buffer_addr, &size), 
        emFLPFB, "  ... detach failed\n");
    return {buffer_addr, size};
}

} // namespace MPI
} // namespace HIPP
#endif	//_HIPPMPI_MPI_MEMORY_H_