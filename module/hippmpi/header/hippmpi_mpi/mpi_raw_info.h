#ifndef _HIPPMPI_MPI_RAW_INFO_H_
#define _HIPPMPI_MPI_RAW_INFO_H_
#include "mpi_base.h"
#include "mpi_error.h"

namespace HIPP::MPI {

class _Info{
public:
    typedef MPI_Info mpi_t;
    enum: int { 
        MAX_KEY = MPI_MAX_INFO_KEY, 
        MAX_VAL = MPI_MAX_INFO_VAL };

    _Info(mpi_t val, int state) noexcept;
    ~_Info() noexcept;

    static mpi_t create();
    static void free( mpi_t &info );
    mpi_t raw() const noexcept;
    
    void set(const string &key, const string &val);
    void del(const string &key);

    bool get(const string &key, string &val) const;
    bool get_valuelen(const string &key, int &len) const;
    int get_nkeys() const;
    string get_nthkey(int n) const;
    mpi_t dup() const;

    static mpi_t nullval() noexcept;
    bool is_null() const noexcept;
protected:
    mpi_t _val;
    int _state;
};


inline 
_Info::_Info( mpi_t val, int state ) noexcept
: _val(val), _state(state) { }

inline 
_Info::~_Info() noexcept { 
    if( _state != 0 && ! is_null() ){
        if( MPI_Info_free(&_val) != MPI_SUCCESS )
            ErrMPI::abort( 1, emFLPFB );
    }
}

inline 
_Info::mpi_t _Info::create(){
    mpi_t val;
    ErrMPI::check( MPI_Info_create( &val ), emFLPFB );
    return val;
}

inline 
void _Info::free( mpi_t &info ){
    ErrMPI::check( MPI_Info_free( &info ), emFLPFB );
}

inline 
_Info::mpi_t _Info::raw() const noexcept { return _val; }


inline 
void _Info::set( const string &key, const string &val ){
    ErrMPI::check( MPI_Info_set( _val, key.c_str(), val.c_str() ), 
        emFLPFB, "  ... set key/value=", key, "/", val, " failed\n" );
}

inline 
void _Info::del( const string &key ){
    ErrMPI::check( MPI_Info_delete( _val, key.c_str() ), emFLPFB,
        "  ... failed (key=", key, ")\n" );
}

inline 
bool _Info::get( const string &key, string &val ) const{
    int valuelen = 0, flag = get_valuelen( key, valuelen );
    if( flag ){
        vector<char> buf( valuelen );
        ErrMPI::check( MPI_Info_get( _val, key.c_str(), valuelen, 
            buf.data(), &flag), 
            emFLPFB, "  ... failed (key=", key, ")\n" );
        val.assign( buf.cbegin(), buf.cend() );
    }
    return bool(flag);
}

inline 
bool _Info::get_valuelen( const string &key, int &len ) const{
    int flag;
    ErrMPI::check( MPI_Info_get_valuelen( _val, key.c_str(), &len, &flag ),
        emFLPFB, "  ... failed (key=", key, ")\n" );
    return bool(flag);
}

inline 
int _Info::get_nkeys() const{
    int nkeys;
    ErrMPI::check( MPI_Info_get_nkeys( _val, &nkeys ), emFLPFB );
    return nkeys;
}

inline 
string _Info::get_nthkey( int n ) const{
    char key[ MAX_KEY ];
    ErrMPI::check( MPI_Info_get_nthkey( _val, n, key ), 
        emFLPFB, "  ... failed (n=", n, ")\n" );
    return string( (const char *)key );
}

inline 
_Info::mpi_t _Info::dup() const {
    mpi_t newinfo;
    ErrMPI::check( MPI_Info_dup( _val, &newinfo ), emFLPFB );
    return newinfo;
}


inline 
_Info::mpi_t _Info::nullval() noexcept { 
    return MPI_INFO_NULL; 
}

inline 
bool _Info::is_null() const noexcept { 
    return _val == nullval(); 
}

} // namespace HIPP::MPI

#endif	//_HIPPMPI_MPI_RAW_INFO_H_