#include <mpi_info.h>

namespace HIPP{
namespace MPI{

Info::Info()
: Info( std::make_shared<_obj_raw_t>( _obj_raw_t::create(), 1 ) ) 
{}

ostream & Info::info(ostream &os, int fmt_cntl, int level) const {
    PStream ps{os};
    bool _is_null = is_null();
    if( fmt_cntl == 0 ){
        ps << HIPPCNTL_CLASS_INFO_INLINE(Info), '{';
        if( _is_null ) ps << "NULL";
        else ps << "no. keys=", get_nkeys();
        ps << '}';
        return os;
    }
    auto ind = HIPPCNTL_CLASS_INFO_INDENT_STR(level);
    ps << HIPPCNTL_CLASS_INFO(Info);
    if( _is_null ) {
        ps << ind, "NULL\n"; return os;
    }    
    int nkeys = get_nkeys();
    if( nkeys == 0 ) {
        ps << ind, "Empty\n"; return os;
    }
    ps << ind, "No. keys=", nkeys, '\n', 
        ind, "Pairs={";
    auto [k, v] = get(0);
    ps << k, "->", v;
    for(int i=1; i<nkeys; ++i){
        auto [k, v] = get(i);
        ps << ", ", k, "->", v;
    }
    ps << "}\n";
    return os;
}

void Info::free() noexcept{
    *this = nullval();
}

bool Info::is_null() const { return _obj_ptr->is_null(); }

Info & Info::set( const string &key, const string &val ){
    _obj_ptr->set( key, val );
    return *this;
}

Info & Info::del( const string &key ){
    _obj_ptr->del(key);
    return *this;
}

bool Info::get( const string &key, string &val ) const{
    return _obj_ptr->get( key, val );
}

string Info::get( const string &key ) const{
    string val;
    bool flag = get(key, val);
    if( !flag )
        ErrLogic::throw_(ErrLogic::eDOMAIN, emFLPFB, 
            "  ... info object does not have key ", key );
    return val;
}

Info::item_t Info::get( int n ) const{
    string key = get_nthkey( n ), val = get(key);
    return item_t( key, val );
}

vector<Info::item_t> Info::get() const{
    int nkeys = get_nkeys();
    vector<item_t> items( nkeys );
    for(int i=0; i<nkeys; ++i) items[i] = get(i);
    return items;
}

string Info::operator[]( const string &key ) const { return get(key); }

Info::item_t Info::operator[]( int n ) const { return get(n); }

bool Info::get_valuelen( const string &key, int &len ) const{
    return _obj_ptr->get_valuelen( key, len );
}

int Info::get_nkeys() const{
    return _obj_ptr->get_nkeys();
}

string Info::get_nthkey( int n ) const{
    return _obj_ptr->get_nthkey( n );
}

Info Info::dup(){
    return _from_raw( _obj_ptr->dup(), 1 );
}

Info Info::create(){
    return _from_raw( _obj_raw_t::create(), 1 );
}

Info Info::nullval() noexcept{
    return _from_raw( _obj_raw_t::nullval(), 1 );
}


}
}