#include <mpi_group.h>
namespace HIPP{
namespace MPI{
ostream &Group::info( ostream &os, int fmt_cntl ) const{
    if( fmt_cntl == 0 ){
        prt(os, HIPPCNTL_CLASS_INFO_INLINE(HIPP::MPI::Group));
        if( is_null() )
            prt(os, "Null");
        else{
            int _rank = rank();
            prt(os, "size: ", size(), ", rank: ", 
                (_rank==UNDEFINED)?string("not belonging"):str(_rank) );
        }
    }
    if( fmt_cntl >= 1 ){
        prt(os, HIPPCNTL_CLASS_INFO(HIPP::MPI::Group));
        if( is_null() ) prt(os, "  Null") << endl;
        else{
            int _rank = rank();
            prt(os, "  Size info (size=", size(), ", rank=", 
                (_rank==UNDEFINED)?string("not belonging"):str(_rank), ")" )
                << endl;
        }
    }
    return os;
}
void Group::free() noexcept{
    *this = nullval();
}
int Group::size() const{
    return _obj_ptr->size();
}
int Group::rank() const{
    return _obj_ptr->rank();
}
int Group::is_null() const{
    return _obj_ptr->is_null();
}
vector<int> Group::translate_ranks(
    const vector<int> &ranks, const Group &othergroup )const
{
    vector<int> res( ranks.size() );
    _obj_raw_t::translate_ranks( raw(), 
        ranks.size(), ranks.data(), othergroup.raw(), res.data() );
    return res;
}
int Group::compare( const Group &othergroup )const{
    return _obj_raw_t::compare( raw(), othergroup.raw() );
}
Group Group::union_( const Group &othergroup )const{
    auto obj = _obj_raw_t::union_( raw(), othergroup.raw() );
    return _from_raw(obj, 1);
}
Group Group::intersection( const Group &othergroup )const{
    auto obj = _obj_raw_t::intersection( raw(), othergroup.raw() );
    return _from_raw(obj, 1);
}
Group Group::difference( const Group &othergroup )const{
    auto obj = _obj_raw_t::difference( raw(), othergroup.raw() );
    return _from_raw(obj, 1);
}
Group Group::incl( const vector<int> &ranks )const{
    auto obj = _obj_ptr->incl( ranks.size(), ranks.data() );
    return _from_raw(obj, 1);
}
Group Group::excl( const vector<int> &ranks )const{
    auto obj = _obj_ptr->excl( ranks.size(), ranks.data() );
    return _from_raw(obj, 1);
}
Group Group::range_incl( const vector<int> &ranks )const{
    auto size = ranks.size();
    if( size % 3 != 0 )
        ErrLogic::throw_(ErrLogic::eLENGTH, emFLPFB, "  ... ranks of size ", 
            size, " are not triplet");
    auto obj = _obj_ptr->range_incl( size/3, ( int (*)[3] )ranks.data() );
    return _from_raw(obj, 1);
}
Group Group::range_excl( const vector<int> &ranks )const{
    auto size = ranks.size();
    if( size % 3 != 0 )
        ErrLogic::throw_(ErrLogic::eLENGTH, emFLPFB, "  ... ranks of size ", 
            size, " are not triplet");
    auto obj = _obj_ptr->range_excl( size/3, ( int (*)[3] )ranks.data() );
    return _from_raw(obj, 1);
}
Group Group::emptyval() noexcept{
    return _from_raw( _obj_raw_t::emptyval(), 0 );
}
Group Group::nullval() noexcept{
    return _from_raw( _obj_raw_t::nullval(), 0 );
}
} // namespace MPI
} // namespace HIPP
