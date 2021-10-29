/**
create: Yangyao CHEN, 2020/11/29
     [write   ]
     @PropList: High-level HDF5 property list API.
*/ 

#ifndef _HIPPIO_H5_OBJ_PROPLIST_H_
#define _HIPPIO_H5_OBJ_PROPLIST_H_

#include "h5_obj_base.h"
#include "h5_util_geometry.h"

namespace HIPP::IO::H5 {

class Dataset;

/**
Property list that controls the detail behaviors of other HDF5 calls.

``Proplist`` is copyable and movable (both in construction and assignment). 
The copy, move and destruction are ``noexcept``. The copy operation 
is shallow - the resulting object always refers to the same HDF5 resource
as the source object. The move operation sets the move-from object an empty
state.
*/
class Proplist: public Obj {
public:
    typedef Obj parent_t;
    
    typedef _Proplist _obj_raw_t;
    typedef std::shared_ptr<_obj_raw_t> _obj_ptr_t;

    
    /** Dataset layout types. */
    typedef _obj_raw_t::d_layout_t d_layout_t;
    inline static constexpr d_layout_t
        laCONTIGUOUS  = _obj_raw_t::laCONTIGUOUS,
        laCHUNKED     = _obj_raw_t::laCHUNKED,
        laCOMPACT     = _obj_raw_t::laCOMPACT,
        laVIRTUAL     = _obj_raw_t::laVIRTUAL;

    /** Property list class. */   
    inline static const hid_t 
        dACCESS = _obj_raw_t::dACCESS,
        dCREATE = _obj_raw_t::dCREATE,
        dXFER   = _obj_raw_t::dXFER,

        fCREATE = _obj_raw_t::fCREATE,
        fMOUNT  = _obj_raw_t::fMOUNT,
        fACCESS = _obj_raw_t::fACCESS;

    /**
    Predefined property list object.
    ``vDFLT``: the default property list.
    */
    static const Proplist vDFLT;

    /** 
    Class ``Proplist`` "inherits" all constructors from its parent class.
    */
    using parent_t::parent_t;

    /**
    Create a new property list classed ``cls`` which could be predefined 
    member variables of ``Proplist``.
    Instead of using this call, it is safer to use the methods in specific
    types for property list creation, e.g., ``Dataset::create_property_list()``,
    ``File::create_proplist``.
    */
    static Proplist create(hid_t cls);

    /**
    Set and retrive the layout of dataset on the dataset creation property list. 
    Layout string can be either "contiguous", "chunked", "compact" or "virtual".
    Layout value can be the predefined member values.
    */
    Proplist & set_layout(d_layout_t layout);
    Proplist & set_layout(const string &layout);
    d_layout_t layout() const;
    void layout(string &lo) const;

    /**
    Set and retrive the chunk size for a chunk-layout dataset.

    ``dims`` must have the same rank (i.e., number of dimensions) as the 
    dataset. ``dims`` cannot be larger than the dims of a fixed-size dataset, 
    and cannot be larger than “2^32-1”. The size of each chunk cannot be larger 
    than 4GB.

    If the layout of the dataset is not "chunk", ``set_chunk()`` change the 
    layout to "chunk".
    */
    Proplist & set_chunk(const Dimensions &dims);
    int chunk_ndims() const;
    Dimensions chunk() const;

    /**
    Set and retrive the cache method of the dataset on the dataset access 
    property list. If set by user, override the cache method of the file.
    */
    Proplist & set_chunk_cache(size_t nslot, size_t nbytes, double w0);
    void chunk_cache( size_t &nslot, size_t &nbytes, double &w0 ) const;
    std::tuple<size_t, size_t, double> chunk_cache() const;
    
    /**
    Set and retrive the cache method of all datasets of a file on the dataset 
    access property list.
    */
    Proplist & set_cache( size_t nslot, size_t nbytes, double w0 );
    void cache(size_t &nslot, size_t &nbytes, double &w0) const;
    std::tuple<size_t, size_t, double> cache() const;

    /**
    Dataset transfer.
    */
    Proplist & preserve(hbool_t status);
    hbool_t preserve() const;
    
    /** Return a reference to the intermediate-level HDF5 object. */
    _obj_raw_t & obj_raw() noexcept;
    const _obj_raw_t & obj_raw() const noexcept;
private: 
    friend class Dataset;
    template<typename ...Args>
    static _obj_ptr_t _ptr_from_raw(Args &&...args);
    template<typename ...Args>
    static Proplist _from_raw(Args &&...args);
};

inline auto Proplist::obj_raw() noexcept -> _obj_raw_t & {
    return *reinterpret_cast<_obj_raw_t *>(_obj_ptr.get());
}

inline auto Proplist::obj_raw() const noexcept -> const _obj_raw_t & {
    return *reinterpret_cast<const _obj_raw_t *>(_obj_ptr.get());
}

template<typename ...Args>
Proplist::_obj_ptr_t Proplist::_ptr_from_raw(Args &&...args) {
    return std::make_shared<_obj_raw_t>(std::forward<Args>(args)...);
}

template<typename ...Args>
Proplist Proplist::_from_raw(Args &&...args) {
    return Proplist { _ptr_from_raw(std::forward<Args>(args)...) };
}

} // namespace HIPP::IO::H5

#endif	//_HIPPIO_H5_OBJ_PROPLIST_H_