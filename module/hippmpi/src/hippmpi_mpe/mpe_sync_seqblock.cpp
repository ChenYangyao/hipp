#include <mpe_sync_seqblock.h>
namespace HIPP{
namespace MPI{

SeqBlock::SeqBlock( const Comm &comm, int start )
    : _comm(nullptr), _prev(PROC_NULL), _next(PROC_NULL), _state(0){
    if( comm.is_null() ) 
        ErrLogic::throw_(ErrLogic::eINVALIDARG, emFLPFB, 
            "  ... null communicator");

    Comm &in_comm = const_cast<Comm &>(comm);

    typedef Env::_comm_intern_priv_attr_t attr_t;
    typedef MemObj<attr_t> mem_t;
    int keyval = Env::_comm_intern_priv_keyval;
    attr_t *attr = nullptr;

    if( !in_comm.get_attr( keyval, attr ) ){
        attr = mem_t::alloc(1, emFLPFB);
        mem_t().construct( attr );
        in_comm.set_attr( keyval, attr );
    }
    if( !attr->has_comm_dup() ){
        attr->set_comm_dup( in_comm );
    }
    _comm = attr->comm_dup;
    _prev = attr->prev;
    _next = attr->next;

    if( start ) begin();
}
SeqBlock::~SeqBlock() noexcept{
    if( _state ) end();
}
SeqBlock::SeqBlock(SeqBlock && sb) noexcept
: _comm( std::move(sb._comm) ), _prev(sb._prev), _next(sb._next), 
    _state(sb._state)
{
    sb._state = 0;
}
SeqBlock & SeqBlock::operator=(SeqBlock && sb) noexcept{
    if( &sb != this ){
        if( _state )
            ErrLogic::throw_( ErrLogic::eINVALIDARG, emFLPFB, 
                "  ... cannot move into running sequential block\n" );
        _comm = std::move(sb._comm);
        _prev = sb._prev;
        _next = sb._next;
        _state = sb._state;
        sb._state = 0;
    }
    return *this;
}
void SeqBlock::begin(){
    if( _state )
        ErrLogic::throw_( ErrLogic::eINVALIDARG, emFLPFB, 
            "  ... begin must be called after ending the sequential block\n" );
    _state = 1;
    const int tag = Env::_comm_intern_priv_attr_t::tag_seqblock;
    _comm.recv( _prev, tag, BOTTOM, 0, INT );
}
void SeqBlock::end(){
    if( !_state )
        ErrLogic::throw_( ErrLogic::eINVALIDARG, emFLPFB,
            "  ... end must be called after begining the sequential block\n" );
    const int tag = Env::_comm_intern_priv_attr_t::tag_seqblock;
    _comm.send( _next, tag, BOTTOM, 0, INT );
    _state = 0;
}
void SeqBlock::free_cache( const Comm &comm ){
    typedef Env::_comm_intern_priv_attr_t attr_t;
    int keyval = Env::_comm_intern_priv_keyval;
    attr_t *attr = nullptr;
    if( comm.get_attr(keyval, attr) )
        attr->del_comm_dup();
}

} // namespace MPI
} // namespace HIPP