#ifndef _HIPPMPI_MPI_FILE_H_
#define _HIPPMPI_MPI_FILE_H_
#include "mpi_obj_base.h"
#include "mpi_comm.h"
#include "mpi_info.h"
namespace HIPP{
namespace MPI{
class File: public MPIObj<_File>{
public:
    enum: int {
        sSET=MPI_SEEK_SET,
        sCUR=MPI_SEEK_CUR,
        sEND=MPI_SEEK_END };

    typedef MPIObj<_File> _obj_base_t;
    using _obj_base_t::_obj_base_t;

    /**
    @amode: could be "r", "w", "wr", "c" ("create"), "excl", 
    "delete_on_close", "unique_open", "sequential", "a" ("append"),
    or their comma-separated combination, e.g., "wr,c".
    */
    File( const Comm &comm, const string &name, 
        const string &amode, const Info &info = Info::nullval() );

    ostream &info( ostream &os = cout, int fmt_cntl = 1 ) const;
    friend ostream & operator<<( ostream &os, const File &file );

    void close() noexcept;
    static void del(const string &name, const Info &info = Info::nullval());
    bool is_null() const;
    static File nullval() noexcept;

    File & set_size( offset_t size );
    File & preallocate( offset_t size );
    offset_t get_size()const;
    Group get_group()const;
    string get_amode()const;

    File & set_info( const Info &info );
    Info get_info()const;
    File & set_view( offset_t disp, const Datatype & etype, 
        const Datatype &filetype, const string &datarep="native", 
        const Info &info = Info::nullval());
    void get_view( offset_t &disp, Datatype &etype, Datatype &filetype, 
        string &datarep )const;

    aint_t get_type_extent(const Datatype &dtype)const;
    File & set_atomicity( int flag );
    int get_atomicity()const;
    void sync();

    template<typename ...Args> 
    Status read_at( offset_t offset, Args && ...args );
    template<typename ...Args> 
    Status read_at_all( offset_t offset, Args && ...args );
    template<typename ...Args> 
    Status write_at( offset_t offset, Args && ...args );
    template<typename ...Args> 
    Status write_at_all( offset_t offset, Args && ...args );

    template<typename ...Args> 
    Requests iread_at( offset_t offset, Args && ...args );
    /*template<typename ...Args> 
    Requests iread_at_all( offset_t offset, Args && ...args );*/
    template<typename ...Args> 
    Requests iwrite_at( offset_t offset, Args && ...args );
    /*template<typename ...Args> 
    Requests iwrite_at_all( offset_t offset, Args && ...args );*/

    template<typename ...Args> 
    void read_at_all_begin( offset_t offset, Args && ...args );
    Status read_at_all_end( void *buf );
    template<typename ...Args> 
    void write_at_all_begin( offset_t offset, Args && ...args );
    Status write_at_all_end( const void *buf );

    void seek(offset_t offset, int whence);
    void seek(offset_t offset, const string &whence);
    offset_t get_position()const;
    offset_t get_byte_offset( offset_t offset )const;
    template<typename ...Args> 
    Status read( Args && ...args );
    template<typename ...Args> 
    Status read_all( Args && ...args );
    template<typename ...Args> 
    Status write( Args && ...args );
    template<typename ...Args> 
    Status write_all( Args && ...args );

    template<typename ...Args> 
    Requests iread( Args && ...args );
    /*template<typename ...Args> 
    Requests iread_all( Args && ...args );*/
    template<typename ...Args> 
    Requests iwrite( Args && ...args );
    /*template<typename ...Args> 
    Requests iwrite_all( Args && ...args );*/

    template<typename ...Args> 
    void read_all_begin( Args && ...args );
    Status read_all_end( void *buf );
    template<typename ...Args> 
    void write_all_begin( Args && ...args );
    Status write_all_end( const void *buf );

    void seek_shared(offset_t offset, int whence);
    void seek_shared(offset_t offset, const string &whence);
    offset_t get_position_shared()const;
    template<typename ...Args> 
    Status read_shared( Args && ...args );
    template<typename ...Args> 
    Status read_ordered( Args && ...args );
    template<typename ...Args> 
    Status write_shared( Args && ...args );
    template<typename ...Args> 
    Status write_ordered( Args && ...args );

    template<typename ...Args> 
    Requests iread_shared( Args && ...args );
    template<typename ...Args> 
    Requests iwrite_shared( Args && ...args );
    template<typename ...Args> 
    void read_ordered_begin( Args && ...args );
    Status read_ordered_end( void *buf );
    template<typename ...Args> 
    void write_ordered_begin( Args && ...args );
    Status write_ordered_end( const void *buf );
protected:
    static const std::unordered_map<string, int> _name2mode;
    static const std::unordered_map<int, string> _mode2name;

    static string _to_modestr( int amode );
    static int _from_modestr( const string &amode );
    static int _from_whencestr( const string &whence );
};

inline ostream & operator<<( ostream &os, const File &file ){
    return file.info(os);
}

template<typename ...Args> 
Status File::read_at( offset_t offset, Args && ...args ){
    auto [p,n,dt] = Datapacket( std::forward<Args>(args)... );
    return _obj_ptr->read_at(offset, p, n, dt.raw());
}
template<typename ...Args> 
Status File::read_at_all( offset_t offset, Args && ...args ){
    auto [p,n,dt] = Datapacket( std::forward<Args>(args)... );
    return _obj_ptr->read_at_all( offset, p, n, dt.raw());
}
template<typename ...Args> 
Status File::write_at( offset_t offset, Args && ...args ){
    auto [p,n,dt] = ConstDatapacket( std::forward<Args>(args)... );
    return _obj_ptr->write_at(offset, p, n, dt.raw());
}
template<typename ...Args> 
Status File::write_at_all( offset_t offset, Args && ...args ){
    auto [p,n,dt] = ConstDatapacket( std::forward<Args>(args)... );
    return _obj_ptr->write_at_all(offset, p, n, dt.raw());
}
template<typename ...Args> 
Requests File::iread_at( offset_t offset, Args && ...args ){
    auto [p,n,dt] = Datapacket( std::forward<Args>(args)... );
    return Requests::_from_raw(
        _obj_ptr->iread_at(offset, p, n, dt.raw()), 0);
}
/*template<typename ...Args> 
Requests File::iread_at_all( offset_t offset, Args && ...args ){
    Datapacket dp( std::forward<Args>(args)... );
    return Requests::_from_raw(
        _obj_ptr->iread_at_all(offset, dp._buff, dp._size, dp._dtype.raw()), 0);
}*/
template<typename ...Args> 
Requests File::iwrite_at( offset_t offset, Args && ...args ){
    auto [p,n,dt] = ConstDatapacket( std::forward<Args>(args)... );
    return Requests::_from_raw(
        _obj_ptr->iwrite_at(offset, p, n, dt.raw()), 0);
}
/*template<typename ...Args> 
Requests File::iwrite_at_all( offset_t offset, Args && ...args ){
    Datapacket dp( std::forward<Args>(args)... );
    return Requests::_from_raw(
        _obj_ptr->iwrite_at_all(offset, dp._buff, dp._size, dp._dtype.raw()), 
        0);
}*/

template<typename ...Args> 
void File::read_at_all_begin( offset_t offset, Args && ...args ){
    auto [p,n,dt] = Datapacket( std::forward<Args>(args)... );
    _obj_ptr->read_at_all_begin( offset, p, n, dt.raw() );
}
inline Status File::read_at_all_end( void *buf ){
    return _obj_ptr->read_at_all_end( buf );
}
template<typename ...Args> 
void File::write_at_all_begin( offset_t offset, Args && ...args ){
    auto [p,n,dt] = ConstDatapacket( std::forward<Args>(args)... );
    _obj_ptr->write_at_all_begin( offset, p, n, dt.raw() );
}
inline Status File::write_at_all_end( const void *buf ){
    return _obj_ptr->write_at_all_end( buf );
}

inline void File::seek(offset_t offset, int whence){
    _obj_ptr->seek( offset, whence );
}
inline void File::seek(offset_t offset, const string &whence){
    seek( offset, _from_whencestr(whence) );
}
inline offset_t File::get_position()const{
    return _obj_ptr->get_position();
}
inline offset_t File::get_byte_offset( offset_t offset )const{
    return _obj_ptr->get_byte_offset(offset);
}
template<typename ...Args> 
Status File::read( Args && ...args ){
    auto [p,n,dt] = Datapacket( std::forward<Args>(args)... );
    return _obj_ptr->read( p, n, dt.raw() );
}
template<typename ...Args> 
Status File::read_all( Args && ...args ){
    auto [p,n,dt] = Datapacket( std::forward<Args>(args)... );
    return _obj_ptr->read_all( p, n, dt.raw() );
}
template<typename ...Args> 
Status File::write( Args && ...args ){
    auto [p,n,dt] = ConstDatapacket( std::forward<Args>(args)... );
    return _obj_ptr->write( p, n, dt.raw() );
}
template<typename ...Args> 
Status File::write_all( Args && ...args ){
    auto [p,n,dt] = ConstDatapacket( std::forward<Args>(args)... );
    return _obj_ptr->write_all( p, n, dt.raw() );
}

template<typename ...Args> 
Requests File::iread( Args && ...args ){
    auto [p,n,dt] = Datapacket( std::forward<Args>(args)... );
    return Requests::_from_raw(
        _obj_ptr->iread(p, n, dt.raw()), 
        0);
}
/*template<typename ...Args> 
Requests File::iread_all( Args && ...args ){
    auto [p,n,dt] = Datapacket( std::forward<Args>(args)... );
    return Requests::_from_raw(
        _obj_ptr->iread_all(p, n, dt.raw()), 
        0);
}*/
template<typename ...Args> 
Requests File::iwrite( Args && ...args ){
    auto [p,n,dt] = ConstDatapacket( std::forward<Args>(args)... );
    return Requests::_from_raw(
        _obj_ptr->iwrite(p, n, dt.raw()), 
        0);
}
/*template<typename ...Args> 
Requests File::iwrite_all( Args && ...args ){
    auto [p,n,dt] = Datapacket( std::forward<Args>(args)... );
    return Requests::_from_raw(
        _obj_ptr->iwrite_all(p, n, dt.raw()), 
        0);
}*/

template<typename ...Args> 
void File::read_all_begin( Args && ...args ){
    auto [p,n,dt] = Datapacket( std::forward<Args>(args)... );
    _obj_ptr->read_all_begin(p, n, dt.raw());
}
inline Status File::read_all_end( void *buf ){
    return _obj_ptr->read_all_end(buf);
}
template<typename ...Args> 
void File::write_all_begin( Args && ...args ){
    auto [p,n,dt] = ConstDatapacket( std::forward<Args>(args)... );
    _obj_ptr->write_all_begin(p, n, dt.raw());
}
inline Status File::write_all_end( const void *buf ){
    return _obj_ptr->write_all_end(buf);
}

inline void File::seek_shared(offset_t offset, int whence){
    _obj_ptr->seek_shared(offset, whence);
}
inline void File::seek_shared(offset_t offset, const string &whence){
    seek_shared(offset, _from_whencestr(whence));
}
inline offset_t File::get_position_shared()const{
    return _obj_ptr->get_position_shared();
}
template<typename ...Args> 
Status File::read_shared( Args && ...args ){
    auto [p,n,dt] = Datapacket( std::forward<Args>(args)... );
    return _obj_ptr->read_shared( p, n, dt.raw() );
}
template<typename ...Args> 
Status File::read_ordered( Args && ...args ){
    auto [p,n,dt] = Datapacket( std::forward<Args>(args)... );
    return _obj_ptr->read_ordered( p, n, dt.raw() );
}
template<typename ...Args> 
Status File::write_shared( Args && ...args ){
    auto [p,n,dt] = ConstDatapacket( std::forward<Args>(args)... );
    return _obj_ptr->write_shared( p, n, dt.raw() );
}
template<typename ...Args> 
Status File::write_ordered( Args && ...args ){
    auto [p,n,dt] = ConstDatapacket( std::forward<Args>(args)... );
    return _obj_ptr->write_ordered( p, n, dt.raw() );
}
template<typename ...Args> 
Requests File::iread_shared( Args && ...args ){
    auto [p,n,dt] = Datapacket( std::forward<Args>(args)... );
    return Requests::_from_raw(
        _obj_ptr->iread_shared( p, n, dt.raw() ), 0);
}
template<typename ...Args> 
Requests File::iwrite_shared( Args && ...args ){
    auto [p,n,dt] = ConstDatapacket( std::forward<Args>(args)... );
    return Requests::_from_raw(
        _obj_ptr->iwrite_shared( p, n, dt.raw() ), 0);
}
template<typename ...Args> 
void File::read_ordered_begin( Args && ...args ){
    auto [p,n,dt] = Datapacket( std::forward<Args>(args)... );
    _obj_ptr->read_ordered_begin( p, n, dt.raw() );
}
inline Status File::read_ordered_end( void *buf ){
    return _obj_ptr->read_ordered_end(buf);
}
template<typename ...Args> 
void File::write_ordered_begin( Args && ...args ){
    auto [p,n,dt] = ConstDatapacket( std::forward<Args>(args)... );
    _obj_ptr->write_ordered_begin( p, n, dt.raw() );
}
inline Status File::write_ordered_end( const void *buf ){
    return _obj_ptr->write_ordered_end(buf);
}

}
}
#endif	//_HIPPMPI_MPI_FILE_H_