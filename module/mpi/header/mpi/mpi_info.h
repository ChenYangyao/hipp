#ifndef _HIPPMPI_MPI_INFO_H_
#define _HIPPMPI_MPI_INFO_H_
#include "mpi_obj_base.h"
#include "mpi_raw_info.h"
namespace HIPP{
namespace MPI{

class File;
class Info: public MPIObj<_Info> {
public:
    typedef MPIObj<_Info> _obj_base_t;
    using _obj_base_t::_obj_base_t;
    typedef std::pair<string, string> item_t;

    Info();
    ostream & info( ostream &os = cout, int fmt_cntl = 1 ) const;
    friend ostream & operator<<( ostream &os, const Info &info );

    void free() noexcept;

    bool is_null() const;
    Info & set( const string &key, const string &val );
    Info & del( const string &key );

    bool get( const string &key, string &val ) const;
    string get( const string &key ) const;
    item_t get( int n ) const; 
    vector<item_t> get() const;
    string operator[]( const string &key ) const;
    item_t operator[]( int n ) const;

    bool get_valuelen( const string &key, int &len ) const;
    int get_nkeys() const;
    string get_nthkey( int n ) const;

    Info dup();
    static Info create();
    static Info nullval() noexcept;
protected:
    friend class File;
    static Info _from_raw( mpi_t info, int state ) noexcept;
};


inline Info Info::_from_raw( mpi_t info, int state ) noexcept{
    return Info( std::make_shared<_obj_raw_t>(info, state) );
}

inline ostream & operator<<( ostream &os, const Info &info ){
    return info.info( os );
}

}
}
#endif	//_HIPPMPI_MPI_INFO_H_