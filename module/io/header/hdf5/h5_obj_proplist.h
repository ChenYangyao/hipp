#ifndef _HIPPIO_H5_OBJ_PROPLIST_H_
#define _HIPPIO_H5_OBJ_PROPLIST_H_
#include "h5_obj_base.h"
namespace HIPP{
namespace IO{

class H5File;
class H5Dataset;
class H5Group;
class H5Attr;

class H5Proplist: public H5Obj<_H5Proplist>{
public:
    typedef H5Obj<_H5Proplist> _obj_base_t;
    using _obj_base_t::_obj_base_t;

    H5Proplist & set_layout( const string &layout );
    string layout() const;

    H5Proplist & set_chunk( vector<size_t> dims );
    int chunk_ndims() const;
    vector<size_t> chunk() const;

    H5Proplist & set_chunk_cache( 
        std::size_t nslot, std::size_t nbytes, double w0 );
    void chunk_cache( std::size_t &nslot, 
        std::size_t &nbytes, double &w0 ) const;
    
    H5Proplist & set_cache( std::size_t nslot, 
        std::size_t nbytes, double w0 );
    void cache( std::size_t &nslot, std::size_t &nbytes, double &w0 ) const;

    static const H5Proplist defaultval;
protected:
    static H5Proplist _from_raw( id_t plist) noexcept;
    friend class H5File;
    friend class H5Dataset;
    friend class H5Group;
    friend class H5Attr;
};

inline H5Proplist H5Proplist::_from_raw( id_t plist) noexcept{
    return H5Proplist( std::make_shared<_obj_raw_t>( plist ) );
}

}
}
#endif	//_HIPPIO_H5_OBJ_PROPLIST_H_