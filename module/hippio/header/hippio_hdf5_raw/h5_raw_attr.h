/**
create: Yangyao CHEN, 2020/11/29
    [write   ] _Attr - intermediate-level wrapper for HDF5 attribute.
*/ 

#ifndef _HIPPIO_H5_RAW_ATTR_H_
#define _HIPPIO_H5_RAW_ATTR_H_

#include "h5_raw_base.h"
#include "h5_raw_proplist.h"

namespace HIPP::IO::H5 {

class _Attr: public _Obj {
public:
    typedef _Obj parent_t;
    
    using parent_t::parent_t;

    _Attr(hid_t loc, const char *name, hid_t dtype, hid_t dspace, 
        hid_t cprop = _Proplist::vDFLT, 
        hid_t aprop = _Proplist::vDFLT);

    virtual ~_Attr( ) noexcept override;

    static hid_t create(hid_t loc, const char *name, 
        hid_t dtype, hid_t dspace, 
        hid_t cprop = _Proplist::vDFLT, hid_t aprop = _Proplist::vDFLT);
    
    static hid_t open(hid_t loc, const char *name, 
        hid_t aprop = _Proplist::vDFLT);
    
    static void close(hid_t attr);
    
    hid_t get_space() const;
    hid_t get_type() const;
    
    static htri_t exists(hid_t id, const char *name);
    void write(hid_t memtype, const void *buff);
    void read(hid_t memtype, void *buff);
};

/** Implementation. */

inline _Attr::_Attr( hid_t loc, const char *name, hid_t dtype, 
    hid_t dspace, hid_t cprop, hid_t aprop )
: _Attr( create(loc, name, dtype, dspace, cprop, aprop) )
{ }

inline _Attr::~_Attr( ) noexcept { 
    if( is_auto_free() ) {
        close(raw()); state_flag(0);
    }
        
}

inline hid_t _Attr::create( hid_t loc, const char *name, hid_t dtype, 
    hid_t dspace, hid_t cprop, hid_t aprop )
{
    hid_t attr = H5Acreate( loc, name, dtype, dspace, cprop, aprop );
    ErrH5::check(
        attr, emFLPFB, 
        "  ... creation of attribute ", name, "failed\n");
    return attr;
}

inline hid_t _Attr::open( hid_t loc, const char *name, hid_t aprop)
{
    hid_t attr = H5Aopen( loc, name, aprop );
    ErrH5::check(
        attr, emFLPFB, 
        "  ... open of attribute ", name, "failed\n");
    return attr;
}

inline void _Attr::close(hid_t attr) {
    ErrH5::check(H5Aclose(attr), emFLPFB); 
}

inline hid_t _Attr::get_space() const{
    hid_t dspace = H5Aget_space( raw() );
    ErrH5::check(dspace, emFLPFB);
    return dspace;
}
inline hid_t _Attr::get_type() const{
    hid_t dtype = H5Aget_type( raw() );
    ErrH5::check(dtype, emFLPFB);
    return dtype;
}

inline htri_t _Attr::exists(hid_t id, const char *name ){
    htri_t ret = H5Aexists( id, name );
    ErrH5::check(ret, emFLPFB, "  ... attributed ", name, '\n');
    return ret;
}

inline void _Attr::write( hid_t memtype, const void *buff )
{
    ErrH5::check(H5Awrite( raw(), memtype, buff ), emFLPFB );
}

inline void _Attr::read( hid_t memtype, void *buff )
{
    ErrH5::check(H5Aread( raw(), memtype, buff ), emFLPFB );
}

} // namespace HIPP::IO::H5

#endif	//_HIPPIO_H5_RAW_ATTR_H_