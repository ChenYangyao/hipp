#ifndef _HIPPMPI_MPE_SYMM_COUNTER_H_
#define _HIPPMPI_MPE_SYMM_COUNTER_H_
#include "mpe_base.h"
namespace HIPP{
namespace MPI{

#define _HIPP_TEMPHD template<typename ValueT>
#define _HIPP_TEMPARG <ValueT>

template<typename ValueT=int>
class CounterS{
public:
    typedef ValueT value_t;

    CounterS(const Comm &comm, int n_counters=1);
    ~CounterS() noexcept;
    CounterS( const CounterS & ) = delete;
    CounterS & operator=( const CounterS & ) = delete;

    CounterS( CounterS && )  noexcept;
    CounterS & operator=( CounterS && )  noexcept;

    value_t get(int counter_id=0);
    void reset();
protected:
    Win _win;
    int _n_counters, _n_procs;
};

_HIPP_TEMPHD
CounterS _HIPP_TEMPARG::CounterS(const Comm &comm, int n_counters)
: _win(nullptr), _n_counters(n_counters), _n_procs(comm.size())
{
    if( _n_counters <= 0 )
        ErrLogic::throw_(ErrLogic::eDOMAIN, emFLPFB, 
            "  ... no. of counters ", _n_counters, " is non-positive\n");
    if( _n_procs <= 0 )
        ErrLogic::throw_(ErrLogic::eINVALIDARG, emFLPFB,
            "  ... communicator have ", _n_procs, " processes\n"); 
            
    int n_each = (_n_counters + _n_procs-1 )/ _n_procs;
    size_t buf_size = (size_t)n_each * sizeof(value_t);
    value_t *base_ptr = (value_t *)alloc_mem(buf_size);
    std::fill_n(base_ptr, n_each, value_t(0));

    auto info = Info::create();
    info.set( "accumulate_ordering", "none" );
    try{
        _win = comm.win_create(base_ptr, buf_size, sizeof(value_t), info);
    }catch( ... ){
        free_mem(base_ptr);
        ErrMPI::print_err(emFLPFB, "  ... RMA window creation failed\n");
        throw;
    }
}
_HIPP_TEMPHD
CounterS _HIPP_TEMPARG::~CounterS() noexcept{
    if( !_win.is_null() ){
        void *base_ptr = _win.get_base();
        _win.free();
        free_mem(base_ptr);
    }
}
_HIPP_TEMPHD
CounterS _HIPP_TEMPARG::CounterS( CounterS && that) noexcept
: _win(that._win), _n_counters(that._n_counters), _n_procs(that._n_procs)
{
    that._win = Win::nullval();
}
_HIPP_TEMPHD
auto CounterS _HIPP_TEMPARG::operator=( CounterS && that) noexcept -> CounterS &
{
    if( this != &that ){
        this->~CounterS();
        _win = that._win;
        _n_counters = that._n_counters;
        _n_procs = that._n_procs;
        that._win = Win::nullval();
    }
    return *this;
}

_HIPP_TEMPHD
auto CounterS _HIPP_TEMPARG::get(int counter_id) ->value_t{
    if( counter_id >= _n_counters || counter_id < 0 )
        ErrLogic::throw_(ErrLogic::eDOMAIN, emFLPFB, 
            "  ... counter id ", counter_id, " not valid (min=", 0, 
            ", max=", _n_counters-1, ")\n");
    int disp = counter_id / _n_procs, target_rank = counter_id % _n_procs;
    value_t value, one=1;
    {
        auto g = _win.lock_g(_win.LOCK_SHARED, target_rank);
        _win.fetch_and_op(target_rank, SUM, value, one, disp);
    }
    return value;
}
_HIPP_TEMPHD
void CounterS _HIPP_TEMPARG::reset(){
    int rank = _win.get_group().rank();
    value_t *base_ptr = (value_t *)_win.get_base();
    int n_each = (_n_counters+_n_procs-1)/_n_procs;
    {
        auto g = _win.lock_g(_win.LOCK_EXCLUSIVE, rank);   
        std::fill_n(base_ptr, n_each, value_t(0));
    }
}


#undef _HIPP_TEMPHD
#undef _HIPP_TEMPARG

} // namespace MPI
} // namespace HIPP
#endif	//_HIPPMPI_MPE_SYMM_COUNTER_H_