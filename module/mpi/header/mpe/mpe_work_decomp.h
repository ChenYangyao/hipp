#ifndef _HIPPMPI_MPE_WORK_DECOMP_H_
#define _HIPPMPI_MPE_WORK_DECOMP_H_
#include "mpe_base.h"
namespace HIPP::MPI {

class WorkDecomp {};

class WorkDecomp1D : public WorkDecomp {
public:
    /**
     * Decompose 1-d domain into uniform-distributed blocks.
     * @gsize: global domain size, i.e., size to be decomposed.
     * @n_blocks: no. of blocks.
     * @rank: the rank of the target block. Beginning and endding indices, 
     *      [b, e), of the target block are returned. "rank" must be in the 
     *      range [0, n_blocks).
     * 
     * E.g., gsize = 21, n_blocks = 6. Then, for different rank, the result is 
     * ------- ---------------
     * rank    returned values
     * ------- ---------------
     * 0       0, 4
     * 1       4, 8
     * 2       8, 12
     * 3       12, 15
     * 4       15, 18
     * 5       18, 21
     * ------- ---------------
     * The first three blocks has size 4, and the last three blocks has size 3.
     */
    static std::pair<size_t, size_t> uniform_block(
        size_t gsize, size_t n_blocks, size_t rank) noexcept;
};

inline std::pair<size_t, size_t> WorkDecomp1D::uniform_block(
    size_t gsize, size_t n_blocks, size_t rank) noexcept 
{
    size_t bsize = gsize / n_blocks, 
        n_rem = gsize % n_blocks, b, e;
    if( rank < n_rem ){
        b = (bsize+1)*rank;
        e = b + bsize+1;
    }else{
        b = (bsize+1)*n_rem + bsize*(rank-n_rem);
        e = b + bsize;
    }
    return {b, e};
}

} // namespace HIPP::MPI
#endif	//_HIPPMPI_MPE_WORK_DECOMP_H_