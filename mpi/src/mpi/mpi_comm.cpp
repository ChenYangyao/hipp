#include "mpi_comm.h"

namespace HIPP{
namespace MPI{
ostream & Comm::info( ostream &os, int fmt_cntl ) const{
    if( fmt_cntl == 0 ){
        prt(os, HIPPCNTL_CLASS_INFO_INLINE(HIPP::MPI:Comm));
        if( is_null() ) prt(os, "Null");
        else prt(os, "size: ", size(), ", rank: ", rank());
    }
    if( fmt_cntl >= 1 ){
        prt(os, HIPPCNTL_CLASS_INFO(HIPP::MPI::Comm));
        if( is_null() ) prt(os, "Null") << endl;
        else 
            prt(os, "  Size info (size=", size(), 
                ", rank=", rank(), ")") << endl;
    }
    return os;
}
void Comm::free() noexcept {
    *this = nullval();
}
int Comm::size() const { return _obj_ptr->size(); }
int Comm::rank() const { return _obj_ptr->rank(); }
bool Comm::is_null() const { return _obj_ptr->is_null(); }

Comm Comm::split( int color, int key )const{
    mpi_t newcomm = _obj_ptr->split(color, key);
    int state = (newcomm == _obj_raw_t::nullval())?0:1;
    return _from_raw(newcomm, state );
}
Comm Comm::dup(){
    mpi_t newcomm = _obj_ptr->dup();
    int state = (newcomm == _obj_raw_t::nullval())?0:1;
    return _from_raw( newcomm, state );
}
Comm Comm::create( const Group &group ){
    mpi_t newcomm = _obj_raw_t::create( raw(), group.raw() );
    int state = (newcomm == _obj_raw_t::nullval())?0:1;
    return _from_raw(newcomm, state);
}
Comm Comm::world() noexcept{
    return _from_raw( _obj_raw_t::world(), 0 );
}
Comm Comm::selfval() noexcept{
    return _from_raw( _obj_raw_t::selfval(), 0 );
}
Comm Comm::nullval() noexcept{
    return _from_raw( _obj_raw_t::nullval(), 0 );
}  
const Group Comm::group() const{
    return const_cast<Comm*>(this)->group();
}
Group Comm::group(){
    auto obj = _obj_ptr->group();
    return Group( std::make_shared<Group::_obj_raw_t>(obj, 1) );
}

} // namespace MPI
} // namespace HIPP

