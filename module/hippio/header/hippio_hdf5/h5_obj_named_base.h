#ifndef _HIPPIO_H5_OBJ_NAMED_BASE_H_
#define _HIPPIO_H5_OBJ_NAMED_BASE_H_

#include "h5_obj_base.h"
#include "h5_obj_proplist.h"
#include "h5_obj_dataspace.h"
#include <vector>

namespace HIPP::IO::H5 {

class Attr;
class Datatype;
class AttrManager;

/**
NameObj - the base class of all named HDF5 object (``Dataset``, ``Datatype``, 
``Group`` and ``File``).

The named object class is copyable and movable (both in construction and 
assignment). The copy, move and destruction are ``noexcept``. The copy operation 
is shallow - the resulting object always refers to the same HDF5 resource
as the source object. The move operation sets the move-from object an empty
state.
*/
class NamedObj : public Obj {
public:
    typedef Obj parent_t;

    typedef _NamedObj _obj_raw_t;
    typedef std::shared_ptr<_obj_raw_t> _obj_ptr_t;

    /** Structured type that records object meta-info. */
    typedef _obj_raw_t::info_t info_t;
    typedef _obj_raw_t::info_field_t info_field_t;
    
    /**
    Fields of named object meta-info.
    */
    inline static constexpr info_field_t
        infoBASIC      = _obj_raw_t::infoBASIC,
        infoTIME       = _obj_raw_t::infoTIME,
        infoNUM_ATTRS  = _obj_raw_t::infoNUM_ATTRS,
        infoHDR        = _obj_raw_t::infoHDR,
        infoMETA_SIZE  = _obj_raw_t::infoMETA_SIZE,
        infoALL        = _obj_raw_t::infoALL;

    /**
    Object types.
    */
    typedef _obj_raw_t::type_t type_t;
    inline static constexpr type_t 
        tUNKNOWN        = _obj_raw_t::tUNKNOWN,
        tGROUP          = _obj_raw_t::tGROUP,
        tDATASET        = _obj_raw_t::tDATASET,
        tNAMED_DATATYPE = _obj_raw_t::tNAMED_DATATYPE,
        tMAP            = _obj_raw_t::tMAP,
        tNTYPES         = _obj_raw_t::tNTYPES;

    /** 
    Class ``NamedObj`` "inherits" all constructors from its parent class.
    */
    using parent_t::parent_t;    

    /**
    Retrive the object meta info of the current object.

    @info: object into which the meta info is put.
    
    The second overload returns the info structure instead of fills the 
    argument.
    */
    void get_info(info_t &info, info_field_t  fields = infoALL) const;
    info_t get_info(info_field_t fields = infoALL) const;

    /**
    Returns an attribute manager of this named object.
    Tha manager provides frequently-used shortcuts for attribute creation and 
    access.
    */
    AttrManager attrs() noexcept;

    /** Find whether or not the attribute of given ``name`` exists. */
    bool attr_exists(const string &name) const;

    /**
    Create an attributed named ``name`` under the current object.
    
    (1): The most general method that is the direct HDF5 C API counterpart.
    (2): The datatype is inferred from ``T`` as if calling 
        ``Datatype::from_type<T>()``.

    Other methods are defined for special cases. All of them use the default 
    property list :expr:`Proplist::vDFLT`.

    (3): Create a scalar dataset whose datatype is inferred from T as if calling 
        ``Datatype::from_type<T>()``.
    (4-5): Create a dataset for fix-length ATOMIC STRING sized ``n``, or a list 
    of ``n_str`` such strings. ``n`` must include the space for the null-term.

    @flag: determine what to do if the attribute already exists. Can be
        - "x": failed and throw an ``ErrH5`` exception.
        - "ac" | "ca" : open it anyway. In this case ``cprop``
          are ignored.
    Deprecated flags
        - "trunc" | "w": the same as "ac".
        - "excl": the same as "x".
    */
    Attr create_attr(const string &name, 
        const Datatype &dtype, const Dataspace &dspace, 
        const string &flag = "ac",
        const Proplist &cprop = Proplist::vDFLT, 
        const Proplist &aprop = Proplist::vDFLT);
    
    template<typename T>
    Attr create_attr(const string &name, const Dataspace &dspace,
        const string &flag = "ac", 
        const Proplist &cprop = Proplist::vDFLT, 
        const Proplist &aprop = Proplist::vDFLT);

    template<typename T>
    Attr create_attr_scalar(const string &name, const string &flag = "ac");
    
    Attr create_attr_str(const string &name, size_t n, 
        const string &flag = "ac");
    Attr create_attr_str(const string &name, size_t n_str, size_t n, 
        const string &flag = "ac");

    /**
    create_attr_for(): create an attribute from the object to be written. The 
    library infers the datatype and dataspace from the object.
    
    (1): any non-string object, which can be any single object that is 
    resolvable by :class:`ConstDatapacket`. The returned attribute can be
    called with ``write(buff)`` to write the object into it. Examples of 
    ``buff`` include
    - single scalar value, e.g., ``int x``;
    - array-like object, e.g., ``std::array<int, 3> x``, ``int x[3][4]``;
    - vector of scalar values, e.g., ``std::vector<double> x``;
    - vector of array-like objects, e.g., 
      ``std::vector< std::array<int, 3> > x``.
      
    (2): any object that can be resolved as a ATOMIC type, i.e., any valid
    type to :func:`Datatype::from_cvt`. 
    The difference from (1) is that, array-like objects are treated as 
    ATOMIC ARRAY datatype here. The returned attribute can be applied with
    ``write_scalar(x)``.

    (3-7): string of list of strings. The returned attribute can therefore
    be written by ``write_str(s)`` or ``write_str(ss)``. The created attribute
    has fix-length ATOMIC STRING datatype.
    */
    template<typename T>
    Attr create_attr_for(const string &name, const T &buff, 
        const string &flag = "ac");

    template<typename T>
    Attr create_attr_for_scalar(const string &name, const T &x, 
        const string &flag = "ac");
    
    Attr create_attr_for_str(const string &name, const string &s, 
        const string &flag = "ac");
    Attr create_attr_for_str(const string &name, const char *s, 
        const string &flag = "ac");

    Attr create_attr_for_str(const string &name, const vector<string> &ss, 
        const string &flag = "ac");
    template<size_t N_STR>
    Attr create_attr_for_str(const string &name, const string (&ss)[N_STR], 
        const string &flag = "ac");
    template<size_t N_STR, size_t N>
    Attr create_attr_for_str(const string &name, const char (&ss)[N_STR][N], 
        const string &flag = "ac");

    /**
    Open an existing attribute of given path name ``name``.
    */
    Attr open_attr(const string &name, 
        const Proplist &aprop = Proplist::vDFLT) const;

    /** Return a reference to the intermediate-level HDF5 object. */
    _obj_raw_t & obj_raw() noexcept;
    const _obj_raw_t & obj_raw() const noexcept;
private:
    Attr _attr_create_detect_exists(const string &name, const string &flag, 
        const Proplist &aprop = Proplist::vDFLT);

    template<typename ...Args>
    Attr _on_fail_attr_creation(const string &name, const string &flag, 
        const Proplist &aprop, Args &&...to_excpt_prt) const;

    template<typename ...Args>
    static _obj_ptr_t _ptr_from_raw(Args &&...args);
    template<typename ...Args>
    static NamedObj _from_raw(Args &&...args);
};

/**
Tha manager provides commonly used shortcuts for attribute creation and 
access.
*/
class AttrManager {
public:
    typedef NamedObj refobj_t;

    /**
    Constructors.
    
    (1): refers to an empty attribute object. I/O may not be performed in this 
    case.
    
    (2): refers to given dataset.
    */
    AttrManager() noexcept;
    explicit AttrManager(refobj_t obj) noexcept;

    /**
    The attribute manager is copyable and movable. The copy is shallow with 
    resulting instance referring to the same attribute.
    The move is destroyable, leaving the move-from instance an empty state.
    */
    ~AttrManager() noexcept = default;
    AttrManager(const AttrManager &) noexcept = default;
    AttrManager(AttrManager &&) noexcept = default;
    AttrManager & operator=(const AttrManager &) noexcept = default;
    AttrManager & operator=(AttrManager &&) noexcept = default;

    /**
    Retrieve the referred object, i.e., the attribute object.
    */
    refobj_t refobj() const;

    /**
    Reset the referred object.
    (1): to a given dataset object.
    (2): to an empty state.
    */
    void reset(refobj_t obj) noexcept;
    void reset() noexcept;

    /**
    Put operations - write data into attribute of given ``name``.
    
    (1): Create a attribute with given ``name`` whose datatype are dataspace are 
    determined from the data ``x`` and write ``x`` into it.
    If the attribute already exists, open it and update its contents to ``x``
    where ``x`` must have consistent datatype and size.
    ``x`` may be anything that can be used in :expr`NamedObj::create_attr_for`
    except the string overloads.

    (2): the same as (1), but using string data ``x`` acceptable by 
    :expr:`NamedObj::create_attribute_for_str`. Examples include single strings
    (``std::string`` or C-style string ``const char *`` ), vector of 
    strings (``std::vector<std::string>``) or array of strings 
    (``std::string[N]``), 2-D character array (``const char [N_STR][N]``,
    resolved as ``N_STR`` strings with fixed length ``N``).
    The string attribute is created as scalar or 1-D dataspace, with 
    fixed-length ATOMIC STRING datatype.
    */
    template<typename T>
    void put(const string &name, const T &x);

    template<typename T>
    void put_str(const string &name, const T &s);

    /**
    Get operations: read data from existing attribute of given ``name``.
    
    In all case, the attribute must have consistent datatype and dataspace with
    the argument ``x``.

    (1-2): are similar to ``put`` but perform reading here.
    
    (3-4): are similar to (1-2), respectively, but return the data which are
    read (i.e., the object typed ``T`` gets default-initialized, be read into, 
    and returned).
    */
    template<typename T>
    void get(const string &name, T &x);
    
    template<typename T>
    void get_str(const string &name, T &s);
    
    template<typename T>
    T get(const string &name);

    template<typename T>
    T get_str(const string &name);

    /**
    Find whether or not an attribute of given ``name`` exists in the parent 
    object.
    */
    bool exists(const string &name);

    /**
    Open an existing attribute named ``name``.
    */
    Attr open(const string &name);
protected:
    refobj_t _obj;
};

} // namespace HIPP::IO::H5


#endif	//_HIPPIO_H5_OBJ_NAMED_BASE_H_