#include <mpi_op.h>

namespace HIPP{
namespace MPI{

Op::Op( user_fn_t user_fn, int commute )
: _obj_base_t( 
    std::make_shared<_obj_raw_t>(
        _obj_raw_t::create(user_fn, commute), _obj_raw_t::stFREE )) {  }
        
ostream & Op::info(ostream &os, int fmt_cntl, int level) const {
    PStream ps{os};
    bool _is_null = is_null(), 
        _is_custom = _obj_ptr->_state & _obj_ptr->stFREE;
    const char *op_type = _is_custom ? 
        "user-defined operation" : "pre-defined operation";

    if(fmt_cntl == 0){
        ps << HIPPCNTL_CLASS_INFO_INLINE(Op), '{';
        if( _is_null ) ps << "NULL";
        else ps << op_type;
        ps << '}';
        return os;
    }

    auto ind = HIPPCNTL_CLASS_INFO_INDENT_STR(level);
    ps << HIPPCNTL_CLASS_INFO(Op);
    if( _is_null ) {
        ps << ind, "NULL\n"; return os;
    }
    ps << ind, op_type, '\n';
        
    return os;
}

void Op::free() noexcept{
    *this = nullval();
}

bool Op::is_null() const{
    return _obj_ptr->is_null();
}

bool Op::commutative() const{
    return _obj_ptr->commutative();
}

Op Op::nullval() noexcept{
    return _from_raw( _obj_raw_t::nullval(), 0 );
}

Op Op::create( user_fn_t user_fn, int commute ){
    return _from_raw( _obj_raw_t::create( user_fn, commute ), 
        _obj_raw_t::stFREE );
}


#define _HIPPMPI_MPI_PREOP(name, mpi_op) \
    Op name { std::make_shared<Op::_obj_raw_t>(mpi_op, 0) };

_HIPPMPI_MPI_PREOP(MAX, MPI_MAX)
_HIPPMPI_MPI_PREOP(MIN, MPI_MIN)
_HIPPMPI_MPI_PREOP(SUM, MPI_SUM)
_HIPPMPI_MPI_PREOP(PROD, MPI_PROD)
_HIPPMPI_MPI_PREOP(LAND, MPI_LAND)
_HIPPMPI_MPI_PREOP(BAND, MPI_BAND)
_HIPPMPI_MPI_PREOP(LOR, MPI_LOR)
_HIPPMPI_MPI_PREOP(BOR, MPI_BOR)
_HIPPMPI_MPI_PREOP(LXOR, MPI_LXOR)
_HIPPMPI_MPI_PREOP(BXOR, MPI_BXOR)
_HIPPMPI_MPI_PREOP(MAXLOC, MPI_MAXLOC)
_HIPPMPI_MPI_PREOP(MINLOC, MPI_MINLOC)
_HIPPMPI_MPI_PREOP(REPLACE, MPI_REPLACE)
_HIPPMPI_MPI_PREOP(NO_OP, MPI_NO_OP)

#undef _HIPPMPI_MPI_PREOP

std::unordered_map<string, const Op *> _opcvt = {
    {"max", &MAX},
    {"min", &MIN},
    {"sum", &SUM},          {"+", &SUM},
    {"prod", &PROD},        {"*", &PROD},
    {"land", &LAND},        {"&&", &LAND},
    {"band", &BAND},        {"&", &BAND},
    {"lor", &LOR},          {"||", &LOR},
    {"bor", &BOR},          {"|", &BOR},
    {"lxor", &LXOR},
    {"bxor", &BXOR},        {"^", &BXOR},
    {"maxloc", &MAXLOC},
    {"minloc", &MINLOC},
    {"replace", &REPLACE},  {"=", &REPLACE},
    {"no_op", &NO_OP}
};

Oppacket::Oppacket( const string &op ): _op(NULL){
    auto it = _opcvt.find( op );
    if( it == _opcvt.end() )
        ErrLogic::throw_(ErrLogic::eDOMAIN, emFLPFB, 
            "  ... cannot find op ", op, '\n');
    _op = * it->second;
}

} // namespace MPI    
} // namespace HIPP
