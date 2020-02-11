#include "mpi_file.h"
namespace HIPP{
namespace MPI{
File::File( const Comm &comm, const string &name, 
    const string &amode, const Info &info )
    :_obj_base_t( std::make_shared<_obj_raw_t>( 
        _obj_raw_t::open(comm.raw(), name.c_str(), 
        _from_modestr(amode), info.raw()), 1 ) )
{}

ostream &File::info( ostream &os, int fmt_cntl ) const{
    if( fmt_cntl == 0 ){
        prt(os, HIPPCNTL_CLASS_INFO_INLINE(HIPP::MPI::File));
        if( is_null() ) prt(os, "Null");
        else{
            auto size = get_size();
            auto group = get_group();
            auto amode = get_amode();
            prt(os, "size: ", size, " no. of participant processes: ", 
                group.size(), " access mode: ", amode);
        }
    }
    if( fmt_cntl >= 1 ){
        prt(os, HIPPCNTL_CLASS_INFO(HIPP::MPI::File));
        if( is_null() ) prt(os, " Null");
        else{
            auto size = get_size();
            auto group = get_group();
            auto amode = get_amode();
            auto _info = get_info();
            offset_t disp;
            Datatype et, ft;
            string datarep;
            get_view(disp, et, ft, datarep);
            int atom = get_atomicity();
            prt(os, "  File info" 
                "\n    size:                         ", size,
                "\n  Access info",
                "\n    no. of participant processes: ", group.size(), 
                "\n    access mode:                  ", amode, 
                "\n    info object:                  "); _info.info(os, 0);
            prt(os, 
                "\n    atomicity:                    ", ( atom ? "true" : "false" ), 
                "\n    indivisual pointer:           ", get_position(), 
                "\n    shared pointer:               ", get_position_shared(), '\n');
            prt(os, "  View",
                "\n    displacement:                 ", disp,
                "\n    element datatype:             "); et.info(os, 0);
            prt(os, 
                "\n    file datatype:                ");  ft.info(os, 0);
            prt(os, 
                "\n    data representation:          ", datarep) << endl;
        }
    }
    return os;
}

void File::close() noexcept{
    *this = nullval();
}
void File::del(const string &name, const Info &info){
    _obj_raw_t::del(name.c_str(), info.raw());
}
bool File::is_null() const{
    return _obj_ptr->is_null();
}
File File::nullval() noexcept{
    return File( std::make_shared<_obj_raw_t>( _obj_raw_t::nullval(), 0 ) );
}

File & File::set_size( offset_t size ){
    _obj_ptr->set_size( size );
    return *this;
}
File & File::preallocate( offset_t size ){
    _obj_ptr->preallocate(size);
    return *this;
}
offset_t File::get_size()const{
    return _obj_ptr->get_size();
}
Group File::get_group()const{
    return Group::_from_raw( _obj_ptr->get_group(), 1 );
}
string File::get_amode()const{
    return _to_modestr( _obj_ptr->get_amode() );
}

File & File::set_info( const Info &info ){
    _obj_ptr->set_info(info.raw());
    return *this;
}
Info File::get_info() const {
    return Info::_from_raw( _obj_ptr->get_info(), 1 );
}
File & File::set_view( offset_t disp, const Datatype & etype, 
    const Datatype &filetype, const string &datarep, 
    const Info &info){
    _obj_ptr->set_view(disp, etype.raw(), filetype.raw(), datarep.c_str(), 
        info.raw());
    return *this;
}
void File::get_view( offset_t &disp, Datatype &etype, Datatype &filetype, 
    string &datarep )const{
    Datatype::mpi_t _etype, _filetype;
    _obj_ptr->get_view( disp, _etype, _filetype, datarep );

    int c1,c2,c3,combiner;
    Datatype::_obj_raw_t::get_envelope( _etype, c1, c2, c3, combiner );
    bool etype_tofree = combiner != MPI_COMBINER_NAMED;
    Datatype::_obj_raw_t::get_envelope( _filetype, c1, c2, c3, combiner );
    bool filetype_tofree = combiner != MPI_COMBINER_NAMED;
    
    etype = Datatype::_from_raw( _etype, etype_tofree );
    filetype = Datatype::_from_raw( _filetype, filetype_tofree );
}

aint_t File::get_type_extent(const Datatype &dtype)const{
    return _obj_ptr->get_type_extent( dtype.raw() );
}
File & File::set_atomicity( int flag ){
    _obj_ptr->set_atomicity( flag );
    return *this;
}
int File::get_atomicity() const {
    return _obj_ptr->get_atomicity();
}
void File::sync(){
    _obj_ptr->sync();
}

const std::unordered_map<string, int> File::_name2mode = {
    {"rdonly", _File::MODE_RDONLY}, {"r", _File::MODE_RDONLY}, 
    {"wronly", _File::MODE_RDONLY}, {"w", _File::MODE_WRONLY},
    {"rdwr", _File::MODE_RDWR}, {"wr", _File::MODE_RDWR}, 
        {"rw", _File::MODE_RDWR},
    {"create", _File::MODE_CREATE}, {"c", _File::MODE_CREATE},
    {"excl", _File::MODE_EXCL},
    {"delete_on_close", _File::MODE_DELETE_ON_CLOSE},
    {"unique_open", _File::MODE_UNIQUE_OPEN},
    {"sequential", _File::MODE_SEQUENTIAL},
    {"append", _File::MODE_APPEND}, {"a", _File::MODE_APPEND},
        {"+", _File::MODE_APPEND}
};
const std::unordered_map<int, string> File::_mode2name = {
    {_File::MODE_RDONLY, "rdonly"},
    {_File::MODE_WRONLY, "wronly"},
    {_File::MODE_RDWR, "rdwr"},
    {_File::MODE_CREATE, "create"},
    {_File::MODE_EXCL, "excl"},
    {_File::MODE_DELETE_ON_CLOSE, "delete_on_close"},
    {_File::MODE_UNIQUE_OPEN, "unique_open"},
    {_File::MODE_SEQUENTIAL, "sequential"},
    {_File::MODE_APPEND, "append"}
};


string File::_to_modestr( int amode ){
    vector<string> names;
    for(const auto &opt: _mode2name)
        if( bool(opt.first & amode) )
            names.push_back( opt.second );
    return str_join( names, ',' );
}
int File::_from_modestr( const string &amode ){
    auto names = str_split(amode, ',');
    int _amode = 0;
    for(const auto &name: names){
        auto it = _name2mode.find(name);
        if( it == _name2mode.end() ) 
            ErrLogic::throw_( ErrLogic::eDOMAIN, emFLPFB, 
                "  ... mode ", name, " not supported\n" );
        _amode |= it->second;
    }
    return _amode;
}
int File::_from_whencestr( const string &whence ){
    int _whence;
    if( whence == "b" || whence == "beg" 
        || whence == "begin" || whence == "set" ) 
        _whence = sSET;
    else if( whence == "e" || whence == "end" )
        _whence = sEND;
    else if( whence == "cur" || whence == "current" )
        _whence = sCUR;
    else 
        ErrLogic::throw_(ErrLogic::eDOMAIN, emFLPFB,
            "  ... seek (whence=", whence, ") not allowed\n");
    return _whence;
}

}
}