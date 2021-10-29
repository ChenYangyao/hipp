/**
create: Yangyao CHEN, 2020/11/29
     [write   ] _Dataset - Intermediate-level wrapper for HDF5 dataset.
*/ 

#ifndef _HIPPIO_H5_RAW_DATASET_H_
#define _HIPPIO_H5_RAW_DATASET_H_

#include "h5_raw_named_base.h"

namespace HIPP::IO::H5 {

class _Dataset: public _NamedObj {
public:
    typedef _NamedObj parent_t;

    using parent_t::parent_t;

    /**
    Create a new dataset with stFREE state.
    */
    _Dataset(hid_t loc, const char *name, hid_t datatype, hid_t dataspace,
        hid_t lcprop     = _Proplist::vDFLT, 
        hid_t dcprop     = _Proplist::vDFLT, 
        hid_t aprop      = _Proplist::vDFLT);
    
    virtual ~_Dataset() noexcept override;

    static hid_t create(hid_t loc, const char *name, hid_t datatype, 
        hid_t dataspace, 
        hid_t lcprop     = _Proplist::vDFLT, 
        hid_t dcprop     = _Proplist::vDFLT, 
        hid_t aprop      = _Proplist::vDFLT);

    static hid_t open(hid_t loc, const char *name, 
        hid_t aprop = _Proplist::vDFLT);
    static void close(hid_t dset);

    hid_t get_space() const;
    hid_t get_type() const;
    void write( hid_t memtype, hid_t memspace, hid_t filespace, 
        hid_t tprop, const void *buff );
    void read( hid_t memtype, hid_t memspace, hid_t filespace, 
        hid_t tprop, void *buff )const;

    hid_t get_create_plist() const;
    hid_t get_access_plist() const;
};

inline _Dataset::_Dataset( hid_t loc, const char *name, hid_t datatype, 
    hid_t dataspace, hid_t lcprop, hid_t dcprop, hid_t aprop )
: _Dataset(create( loc, name, datatype, dataspace, lcprop, dcprop, aprop ))
{ }

inline _Dataset::~_Dataset() noexcept { 
    if(is_auto_free()) {
        close(raw()); state_flag(0);
    }
}

inline hid_t _Dataset::create(hid_t loc, const char *name, hid_t datatype, 
    hid_t dataspace, hid_t lcprop, hid_t dcprop, hid_t aprop)
{
    hid_t dset = H5Dcreate( loc, name, datatype, dataspace, lcprop, 
        dcprop, aprop );
    ErrH5::check( dset, 
        emFLPFB, "  ... cannot create dataset named ", name, "\n" );
    return dset;
}

inline hid_t _Dataset::open(hid_t loc, const char *name, hid_t aprop){
    hid_t dset = H5Dopen( loc, name, aprop );
    ErrH5::check( dset, emFLPFB, "  ... cannot open dataset named ", 
        name, "\n" );
    return dset;
}

inline void _Dataset::close(hid_t dset) {
    ErrH5::check(H5Dclose(dset), emFLPFB); 
}

inline hid_t _Dataset::get_space() const{
    hid_t dspace = H5Dget_space( _obj );
    ErrH5::check(dspace);
    return dspace;
}

inline hid_t _Dataset::get_type() const{
    hid_t dtype = H5Dget_type( _obj );
    ErrH5::check(dtype);
    return dtype;
}

inline void _Dataset::write(hid_t memtype, hid_t memspace, hid_t filespace, 
    hid_t tprop, const void *buff )
{
    ErrH5::check( 
        H5Dwrite( raw(), memtype, memspace, filespace, 
        tprop, buff ), emFLPFB );
}

inline void _Dataset::read( hid_t memtype, hid_t memspace, hid_t filespace, 
    hid_t tprop, void *buff )const
{
    ErrH5::check( 
        H5Dread( raw(), memtype, memspace, filespace, 
        tprop, buff ), emFLPFB );
}

inline hid_t _Dataset::get_create_plist() const {
    auto ret = H5Dget_create_plist(raw());
    ErrH5::check(ret, emFLPFB, "  ... failed to get creation property list");
    return ret;
}

inline hid_t _Dataset::get_access_plist() const {
    auto ret = H5Dget_access_plist(raw());
    ErrH5::check(ret, emFLPFB, "  ... failed to get access property list");
    return ret;
}

} // namespace HIPP::IO::H5
#endif	//_HIPPIO_H5_RAW_DATASET_H_