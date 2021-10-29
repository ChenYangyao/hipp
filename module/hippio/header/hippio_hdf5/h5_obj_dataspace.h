/**
create: Yangyao CHEN, 2020/01/11
    [write   ] Dataspace: HDF5 high-level dataspace object.
*/ 

#ifndef _HIPPIO_H5_OBJ_DATASPACE_H_
#define _HIPPIO_H5_OBJ_DATASPACE_H_

#include "h5_obj_base.h"
#include "h5_util_geometry.h"

namespace HIPP::IO::H5 {

class Dataset;
class Attr;

/**
Dataspace describe the layout of elements in a dataset or attribute.
 
Class ``Dataspace`` encapsulates the methods available on dataspaces. 

The dataspace class is copyable and movable (both in construction and 
assignment). The copy, move and destruction are ``noexcept``. The copy operation 
is shallow - the resulting object always refers to the same HDF5 resource
as the source object. 
The move operation sets the move-from object an empty state.
*/
class Dataspace: public Obj {
public:
    typedef Obj parent_t;
    
    typedef _Dataspace _obj_raw_t;
    typedef std::shared_ptr<_obj_raw_t> _obj_ptr_t;

    /** Dataspace type. */
    typedef _obj_raw_t::class_t class_t;
    inline static constexpr class_t 
        cNULL   =  _obj_raw_t::cNULL,
        cSIMPLE =  _obj_raw_t::cSIMPLE,
        cSCALAR =  _obj_raw_t::cSCALAR;
    
    /** Dataspace selection operator. */
    typedef _obj_raw_t::seloper_t seloper_t;
    inline static constexpr seloper_t
        selSET     =   _obj_raw_t::selSET,
        selOR      =   _obj_raw_t::selOR,
        selAND     =   _obj_raw_t::selAND,
        selXOR     =   _obj_raw_t::selXOR,
        selNOTB    =   _obj_raw_t::selNOTB,
        selNOTA    =   _obj_raw_t::selNOTA,

        selAPPEND  =   _obj_raw_t::selAPPEND,
        selPREPEND =   _obj_raw_t::selPREPEND;

    /** Dataspace selection type. */
    typedef _obj_raw_t::seltype_t seltype_t;
    inline static constexpr seltype_t
        selNONE_T       = _obj_raw_t::selNONE_T,
        selALL_T        = _obj_raw_t::selALL_T,
        selPOINTS_T     = _obj_raw_t::selPOINTS_T,
        selHYPERSLABS_T = _obj_raw_t::selHYPERSLABS_T;

    /**
    Predefined dataspace values.
    ``allval`` represents "all" data in a dataset or a memory buffer whose exact meaning depends on the context. 
    ``nullval`` represents an empty dataspace.
    ``scalarval`` represents the dataspace for a single element, although its datatype 
        may be composite such as ARRAY or COMPOUND.
    */
    static const Dataspace vALL, vNULL, vSCALAR;

    /** 
    Class ``Dataspace`` "inherits" all constructors from its parent class.
    */
    using parent_t::parent_t;

    /**
    Create a new, SIMPLE class dataspace (i.e., regular array, classed 
    ``cSIMPLE``).
    @dims: current dimensions of the array, i.e., number of elements along 
        axes.
    @maxdims: maximum limit of dimensions. If provided larger than ``dims``,
        the size of the dataset created with such dataspace 
        may be extended later.
    The maximal available rank is limited by the HDF5 implementation.

    (1-3): ``maxdims = dims``.
    (4): set ``dims`` and ``max_dims`` separately.
    */
    Dataspace(std::initializer_list<hsize_t> dims);
    Dataspace(vector<hsize_t> & dims);
    Dataspace(const Dimensions &dims);
    Dataspace(const Dimensions &dims, const Dimensions &maxdims);

    /**
    Print information of the instance to stream ``os``.
    @fmt_cntl: format controller. ``0`` for a inline, short message and ``1``
    for a verbose one.

    ``operator<<()`` is equivalent to ``info()`` with default format controller.
    */
    ostream & info(ostream &os=cout, int fmt_cntl=0) const;
    friend ostream & operator<< (ostream &os, const Dataspace &dspace);

    /**
    (1): create data space from scratch.
    @type: cSIMPLE | cNULL | cSCALAR. The dataspace class.

    (2-4): shortcuts of (1).
    */
    static Dataspace create(class_t type);
    static Dataspace create_null();
    static Dataspace create_scalar();
    static Dataspace create_simple();

    /**
    copy(): return an exact copy of the dataspace.
    
    extent_copy(): copy the current extent to ``dst_dspace``. May change its 
    dataspace class.
    */
    Dataspace copy() const;
    void extent_copy(Dataspace &dst_dspace) const;

    /** Get dataspace class. */
    class_t get_type() const;
    
    /**
    For SIMPLE dataspace, retrieve its information by the following methods:
    
    ndims(), dims(), maxdims(): return its extents, i.e., number of dimensions 
        (rank), dimensions and maximal dimensions.
    size(): number of elements, i.e., product of ``dims()``.
    maxsize(): maximal number of elements, i.e., product of ``maxdims()``.
    */
    int ndims() const;
    Dimensions dims() const;
    Dimensions maxdims() const;
    hsize_t size() const;
    hsize_t maxsize() const;

    /**
    Selections that can be applied to SIMPLE dataspace - select none or all.
    */
    void select_none();
    void select_all();

    /**
    Hyperslab selections.

    select_hyperslab() allows selects a hyperslab of elements starting at the 
    offset ``start`` with ``count`` blocks at each dimension.
    Each block has ``block`` elements and ``stride`` to the next block.
    @op(string): 
        "set" ("=") | "or" ("|") | "and" ("&") | "xor" (^) | "notb" | "nota", 
        specifies how to operator with the current selection.
    @op(seloper_t): see the predefined members of selection operations.
    
    (1) "set" selection by a ``Hyperslab`` instance.
    (2,3) same with (1), but allow a ``op``.
    (4,5) same with (2,3), but using raw buffers for the hyperslab 
    specification. 

    In (1-3), the Hyperslab may have empty stride or block, means ``1`` at all 
    dimensions. None-empty members must have the same length as the rank 
    of the dataspace.
        
    In (4,5), ``stride`` or ``block`` could be `NULL``, means ``1`` at all 
    dimensions.

    In all cases, the sub-setting cannot extend over the dataspace boundary.
    */
    void select_hyperslab(const Hyperslab &hs);

    void select_hyperslab(const string &op, const Hyperslab &hs);
    void select_hyperslab(seloper_t op, const Hyperslab &hs);
    
    void select_hyperslab(const string &op,
        const hsize_t *start, const hsize_t *count, 
        const hsize_t *stride = NULL, const hsize_t *block = NULL);
    void select_hyperslab(seloper_t op,
        const hsize_t *start, const hsize_t *count, 
        const hsize_t *stride = NULL, const hsize_t *block = NULL);

    /**
    Combine the current hyperslab selection with ``hs`` using operation ``op``.
    Return the result.
    The ``hs`` argument has the same requirement as in ``select_hyperslab``.
    If the current instance is not a hyperslab, it is freed as sel_all()
    and then combined with ``hs``.
    */
    Dataspace combine_hyperslab(const string &op, const Hyperslab &hs) const;
    Dataspace combine_hyperslab(seloper_t op, const Hyperslab &hs) const;

    /**
    Combine the selection of current instance with ``dspace2`` using ``op``.
    Return the result. The extent is taken from the current instance.
    */
    Dataspace combine_select(const string &op, const Dataspace &dspace2) const;
    Dataspace combine_select(seloper_t op, const Dataspace &dspace2) const;

    /**
    Return a dataspace whose current extent is copy from the current instance
    with a hyperslab selection specified by ``hs``.
    */
    Dataspace hyperslabed(const Hyperslab &hs) const;

    /**
    Element-base selection - select a list of coordinates in the dataspace.
    
    (1): use a Points instance for selection with op == "set".
    (2,3): allow an operation with existing selection.
    (4,5): the same as (2,3), but using a raw buffer for coordinates.
    
    @op (string): "set" ("=") | "append" ("+") | "prepend". 
    @op (seloper_t): using member constants.
    
    @points: a 1-D row-major flatten array shaped (n_elems, rank). Rank is 
    not used and is inferred from the dataspace instance.
    @coords: use raw buffer with same effect as ``points``. The ``rank`` is 
    inferred from the dataspace instance.
    */
    void select_elements(const Points &points);
    
    void select_elements(const string &op, const Points &points);
    void select_elements(seloper_t op, const Points &points);
    
    void select_elements(const string &op, size_t n_elems, 
        const hsize_t coords[]);
    void select_elements(seloper_t op, size_t n_elems, const hsize_t coords[]);

    /**
    Return a dataspace whose current extent is copy from the current instance
    with a element selection specified by ``hs``.
    */
    Dataspace elemented(const Points &points) const;
    
    /** 
    Retrieve selection information from the dataspace.
    get_select_npoints(): return the number of elements selected. 
    get_select_type(): return the selection type.
    select_valid(): whether or not the selection is valid, i.e., within the
        current dataspace extent.
    */
    hssize_t get_select_npoints() const;
    seltype_t get_select_type() const;
    bool select_valid() const;

    /** Return the intermediate-level wrapper objects. */
    _obj_raw_t & obj_raw() noexcept;
    const _obj_raw_t & obj_raw() const noexcept;
private: 
    friend class Dataset;
    friend class Attr;

    template<typename ...Args>
    static _obj_ptr_t _ptr_from_raw(Args &&...args);
    template<typename ...Args>
    static Dataspace _from_raw(Args &&...args);

    static const std::unordered_map<string, seloper_t> _seloper_map;

    template<typename ...Args>
    static seloper_t _find_seloper(const string &op, 
        Args &&...args);
};

inline auto Dataspace::obj_raw() noexcept -> _obj_raw_t & {
    return *reinterpret_cast<_obj_raw_t *>(_obj_ptr.get());
}

inline auto Dataspace::obj_raw() const noexcept -> const _obj_raw_t & {
    return *reinterpret_cast<const _obj_raw_t *>(_obj_ptr.get());
}

template<typename ...Args>
Dataspace::_obj_ptr_t Dataspace::_ptr_from_raw(Args &&...args) {
    return std::make_shared<_obj_raw_t>(std::forward<Args>(args)...);
}

template<typename ...Args>
Dataspace Dataspace::_from_raw(Args &&...args) {
    return Dataspace { _ptr_from_raw(std::forward<Args>(args)...) };
}

template<typename ...Args>
Dataspace::seloper_t Dataspace::_find_seloper(const string &op, 
    Args &&...args)
{
    auto it = _seloper_map.find(op);
    if ( it == _seloper_map.end() )
        ErrLogic::throw_(ErrLogic::eINVALIDARG, emFLPFB, 
        "  ... selection operator ", op, " invalid\n");
    return it->second;
}

} // namespace HIPP::IO::H5

#endif	//_HIPPIO_H5_OBJ_DATASPACE_H_