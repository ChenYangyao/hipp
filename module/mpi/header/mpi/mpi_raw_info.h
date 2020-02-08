#ifndef _HIPPMPI_MPI_RAW_INFO_H_
#define _HIPPMPI_MPI_RAW_INFO_H_
#include "mpi_base.h"
#include "mpi_error.h"
namespace HIPP{
namespace MPI{

class _Info{
public:
    typedef MPI_Info mpi_t;
    enum: int { MAX_KEY = MPI_MAX_INFO_KEY, MAX_VAL = MPI_MAX_INFO_VAL };

    _Info( mpi_t val, int state ): _val(val), _state(state){ }
    ~_Info() noexcept { 
        if( _state != 0 && ! is_null() ){
            if( MPI_Info_free(&_val) != MPI_SUCCESS )
                ErrMPI::abort( 1, emFLPFB );
        }
    }
    static mpi_t create(){
        mpi_t val;
        ErrMPI::check( MPI_Info_create( &val ), emFLPFB );
        return val;
    }
    static void free( mpi_t &info ){
        ErrMPI::check( MPI_Info_free( &info ), emFLPFB );
    }
    mpi_t raw() const noexcept { return _val; }
    
    void set( const string &key, const string &val ){
        ErrMPI::check( MPI_Info_set( _val, key.c_str(), val.c_str() ), 
            emFLPFB, "  ... set key/value=", key, "/", val, " failed\n" );
    }
    void del( const string &key ){
        ErrMPI::check( MPI_Info_delete( _val, key.c_str() ), emFLPFB,
            "  ... delete info item with key=", key, " failed\n" );
    }
    bool get( const string &key, string &val ) const{
        int valuelen = 0, flag = get_valuelen( key, valuelen );
        if( flag ){
            vector<char> buf( valuelen );
            ErrMPI::check( MPI_Info_get( _val, key.c_str(), valuelen, 
                buf.data(), &flag), 
                emFLPFB, "  ... get info with key=", key, " failed\n" );
            val.assign( buf.cbegin(), buf.cend() );
        }
        return bool(flag);
    }
    bool get_valuelen( const string &key, int &len ) const{
        int flag;
        ErrMPI::check( MPI_Info_get_valuelen( _val, key.c_str(), &len, &flag ),
            emFLPFB, "  ... get value length with key=", key, " failed\n" );
        return bool(flag);
    }
    int get_nkeys() const{
        int nkeys;
        ErrMPI::check( MPI_Info_get_nkeys( _val, &nkeys ), emFLPFB );
        return nkeys;
    }
    string get_nthkey( int n ) const{
        char key[ MAX_KEY ];
        ErrMPI::check( MPI_Info_get_nthkey( _val, n, key ), 
            emFLPFB, "  ... get ", n, "-th key failed\n" );
        return string( (const char *)key );
    }
    mpi_t dup() const {
        mpi_t newinfo;
        ErrMPI::check( MPI_Info_dup( _val, &newinfo ), emFLPFB );
        return newinfo;
    }

    static mpi_t nullval() noexcept { return MPI_INFO_NULL; }
    bool is_null() const noexcept { return _val == nullval(); }
protected:
    mpi_t _val;
    int _state;
};

}
}
#endif	//_HIPPMPI_MPI_RAW_INFO_H_