#ifndef _HIPPMPI_MPE_SYNC_SEQBLOCK_H_
#define _HIPPMPI_MPE_SYNC_SEQBLOCK_H_
#include "mpe_base.h"
namespace HIPP{
namespace MPI{

class SeqBlock{
public:
    SeqBlock( const Comm &comm, int start = 1 );
    ~SeqBlock() noexcept;
    SeqBlock(const SeqBlock &) = delete;
    SeqBlock(SeqBlock &&) noexcept;
    SeqBlock & operator=(const SeqBlock &) = delete;
    SeqBlock & operator=(SeqBlock &&) noexcept;

    void begin();
    void end();

    static void free_cache( const Comm &comm );
protected:
    Comm _comm;
    int _prev, _next;
    int _state;
};

} // namespace MPI
} // namespace HIPP
#endif	//_HIPPMPI_MPE_SYNC_SEQBLOCK_H_