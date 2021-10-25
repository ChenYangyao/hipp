/**
create: Yangyao CHEN, 2020/11/29
    [write   ] Group: High-level HDF5 group API.
*/ 

#ifndef _HIPPIO_H5_OBJ_GROUP_H_
#define _HIPPIO_H5_OBJ_GROUP_H_

#include "h5_obj_named_base.h"

namespace HIPP::IO::H5 {

class Dataset;
class DatasetManager;

/**
Groups are named objects used to organized other objects in a HDF5 file.
Class ``Group`` encapsulates the methods available on groups. 

The group class is copyable and movable (both in construction and 
assignment). The copy, move and destruction are ``noexcept``. The copy operation 
is shallow - the resulting object always refers to the same HDF5 resource
as the source object. The move operation sets the move-from objects an empty
state.
*/
class Group: public NamedObj {
public:
    typedef NamedObj parent_t;

    typedef _Group _obj_raw_t;
    typedef std::shared_ptr<_obj_raw_t> _obj_ptr_t;

    /** Structured type that records group meta-info. */
    typedef _obj_raw_t::info_t          info_t;

    /** Storage types of group. */
    typedef _obj_raw_t::storage_type_t  storage_type_t;
    inline static constexpr storage_type_t
        storeCOMPACT_T      = _obj_raw_t::storeCOMPACT_T,
        storeDENSE_T        = _obj_raw_t::storeDENSE_T,
        storeSYMBOL_TABLE_T = _obj_raw_t::storeSYMBOL_TABLE_T;

    /** Structured type that records link meta-info. */
    typedef _Link::info_t link_info_t;
    
    /** Type of links. */
    typedef _Link::type_t link_type_t;
    inline static constexpr link_type_t 
        tERROR              = _Link::tERROR,
        tHARD               = _Link::tHARD,
        tSOFT               = _Link::tSOFT,
        tEXTERNAL           = _Link::tEXTERNAL,
        tMAX                = _Link::tMAX;

    /**
    Structured type that records object meta-info.
    */
    typedef NamedObj::info_t        obj_info_t;
    typedef NamedObj::info_field_t  obj_info_field_t;
    typedef NamedObj::type_t        obj_type_t;

    /** 
    Class ``Group`` "inherits" all constructors from its parent class.
    */
    using parent_t::parent_t;

    /**
    Retrieve group meta info of the current object, of a sub object by ``name``,
    or of the sub sub object indexed ``idx`` in a sub group ``group_name`` (
    can be "." to denote the group itself).

    @group_info: object into which the meta info is put.
    @idx_type, order: which type of index is used and in which order the object 
        is visited in the index list.

    The three overloads that return ``info_t`` are the same except the the group
    information is returned rather than filled into the argument.
    */
    void get_info(info_t &group_info) const;
    void get_info(const string &name, info_t &group_info, 
        const Proplist &laprop = Proplist::vDFLT) const;
    void get_info(const string &group_name, hsize_t idx, info_t &group_info, 
        index_t idx_type = idxNAME, iter_order_t order = iterNATIVE, 
        const Proplist &laprop = Proplist::vDFLT) const;
    
    info_t get_info() const;
    info_t get_info(const string &name, 
        const Proplist &laprop = Proplist::vDFLT) const;
    info_t get_info(const string &group_name, hsize_t idx,
        index_t idx_type = idxNAME, iter_order_t order = iterNATIVE, 
        const Proplist &laprop = Proplist::vDFLT) const;

    /**
    Retrive the object meta info of the current object, of a sub object by 
    ``name``, or of a sub sub object indexed ``idx`` in the sub group 
    ``group_name`` (can be "." to denote the group itself). 

    @info: object into which the meta info is put.
    @idx_type, order: which type of index is used and in which order the object 
        is visited in the index list.

    The three overloads that return ``obj_info_t`` are the same except the the 
    object information is returned rather than filled into the argument.
    */
    void get_object_info(obj_info_t &info,
        info_field_t fields = NamedObj::infoALL) const;
    void get_object_info(const string &name, obj_info_t &info, 
        info_field_t fields = NamedObj::infoALL,
        const Proplist &laprop = Proplist::vDFLT) const;
    void get_object_info(const string &group_name, 
        hsize_t idx, obj_info_t &info, 
        info_field_t fields = NamedObj::infoALL,
        index_t idx_type = idxNAME, iter_order_t order = iterNATIVE, 
        const Proplist &laprop = Proplist::vDFLT) const;

    obj_info_t get_object_info(
        info_field_t fields = NamedObj::infoALL) const;
    obj_info_t get_object_info(const string &name,  
        info_field_t fields = NamedObj::infoALL,
        const Proplist &laprop = Proplist::vDFLT) const;
    obj_info_t get_object_info(const string &group_name, hsize_t idx,  
        info_field_t fields = NamedObj::infoALL,
        index_t idx_type = idxNAME, iter_order_t order = iterNATIVE, 
        const Proplist &laprop = Proplist::vDFLT) const;

    /**
    Retrive the link meta info of a sub object by ``name``, or of a sub sub 
    object indexed ``idx`` in the sub group ``group_name`` 
    (can be "." to denote the group itself). 

    @info: object into which the meta info is put.
    @idx_type, order: which type of index is used and in which order the object 
        is visited in the index list.

    The two overloads that return ``link_info_t`` are the same except the the 
    link information is returned rather than filled into the argument.

    ``get_link_val()`` and ``get_link_name()`` return the link value and name,
    respectively. 

    @buff: on return, link value is filled into it and its size is adjusted
        to fit.
    */
    void get_link_info(const string &name, link_info_t &info, 
        const Proplist &laprop = Proplist::vDFLT) const;
    void get_link_info(const string &group_name, hsize_t idx, link_info_t &info,    
        index_t idx_type = idxNAME, iter_order_t order = iterNATIVE,
        const Proplist &laprop = Proplist::vDFLT) const;
    link_info_t get_link_info(const string &name, 
        const Proplist &laprop = Proplist::vDFLT) const;
    link_info_t get_link_info(const string &group_name, hsize_t idx,    
        index_t idx_type = idxNAME, iter_order_t order = iterNATIVE,
        const Proplist &laprop = Proplist::vDFLT) const;

    void get_link_val(const string &name, vector<char> &buff, 
        const Proplist &laprop = Proplist::vDFLT) const;
    void get_link_val(const string &group_name, hsize_t idx, vector<char> &buff, 
        index_t idx_type = idxNAME, iter_order_t order = iterNATIVE,
        const Proplist &laprop = Proplist::vDFLT) const;

    string get_link_name(const string &group_name, hsize_t idx, 
        index_t idx_type = idxNAME, iter_order_t order = iterNATIVE,
        const Proplist &laprop = Proplist::vDFLT) const;

    /**
    Returns a dataset manager of this group.
    Tha manager provides commonly used shortcuts for dataset creation and 
    access.
    */
    DatasetManager datasets() noexcept;

    /**
    Find whether or not certain item exists in the current group. If any of the
    intermediate steps in the name does not exists, or does not resolve to
    an object, or the final target link does not exits, return false.
    
    (1): for link of any type. 
    (2): the same as (1) but also check that the target link resolves to an 
        object of any type.
    (3): the same as (2), but further require that the target match the object
        type given by ``obj_type``.
    (4,5): special cases of (3), and with default link access property list.
    */
    bool link_exists(const string &name, 
        const Proplist &laprop = Proplist::vDFLT) const;
    bool object_exists(const string &name, 
        const Proplist &laprop = Proplist::vDFLT) const;
    bool object_exists_by_type(const string &name, 
        obj_type_t obj_type, const Proplist &laprop = Proplist::vDFLT) const;
    
    bool group_exists(const string &name) const;
    bool dataset_exists(const string &name) const;

    /**
    Create a group with link name ``name`` under the current group.
    @flag: determine what to do if the group named ``name`` already exists. 
        Can be
        - "x": failed and throw an ``ErrH5`` exception.
        - "ac" | "ca" : open it anyway. In this case ``lcprop`` and ``gcprop`` 
          are ignored.
    Deprecated flags
        - "trunc" | "w": the same as "ac".
        - "excl": the same as "x".
    */
    Group create_group(const string &name, const string &flag="ac", 
        const Proplist &lcprop = Proplist::vDFLT,
        const Proplist &gcprop = Proplist::vDFLT,
        const Proplist &gaprop  = Proplist::vDFLT);

    /**
    Open an existing group with path name ``name``.
    */
    Group open_group(const string &name, 
        const Proplist &aprop = Proplist::vDFLT) const;

    /**
    Create a dataset named ``name`` under the current group.
    
    (1): The most general method that is the direct HDF5 C API counterpart.
    (2): The datatype is inferred from ``T`` as if calling 
        :expr:`Datatype::from_type<T>()`.

    Other methods are defined for special cases. All of them use the default 
    property list :expr:`Proplist::vDFLT`.

    (3): Create a scalar dataset whose datatype is inferred from T as if calling 
        :expr:`Datatype::from_type<T>()`.
    (4-5): Create a dataset for fix-length ATOMIC STRING sized ``n``, or a list 
    of ``n_str`` such strings. ``n`` must include the space for the null-term.

    @flag: determine what to do if the dataset already exists. Can be
        - "x": failed and throw an ``ErrH5`` exception.
        - "ac" | "ca" : open it anyway. In this case ``lcprop`` and ``dcprop`` 
          are ignored.
    Deprecated flags
        - "trunc" | "w": the same as "ac".
        - "excl": the same as "x".
    */ 
    Dataset create_dataset(const string &name, 
        const Datatype &dtype, const Dataspace &dspace, 
        const string &flag = "ac",
        const Proplist &lcprop = Proplist::vDFLT, 
        const Proplist &dcprop  = Proplist::vDFLT, 
        const Proplist &daprop  = Proplist::vDFLT);

    template<typename T>
    Dataset create_dataset(const string &name, const Dataspace &dspace, 
        const string &flag = "ac",
        const Proplist &lcprop = Proplist::vDFLT, 
        const Proplist &dcprop  = Proplist::vDFLT, 
        const Proplist &daprop  = Proplist::vDFLT);

    template<typename T>
    Dataset create_dataset_scalar(const string &name, 
        const string &flag = "ac");

    Dataset create_dataset_str(const string &name, size_t n, 
        const string &flag = "ac");
    Dataset create_dataset_str(const string &name, size_t n_str, size_t n, 
        const string &flag = "ac");

    /**
    create_dataset_for(): create a dataset from the object to be written. The 
    library infers the datatype and dataspace from the object.
    
    (1): any non-string object, which can be any single object that is 
    resolvable by :class:`ConstDatapacket`. The returned dataset can be
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
    ATOMIC ARRAY datatype here. The returned dataset can be applied with
    ``write_scalar(x)``.

    (3-7): string of list of strings. The returned attribute can therefore
    be written by ``write_str(s)`` or ``write_str(ss)``. The created dataset
    has fix-length ATOMIC STRING datatype.
    */
    template<typename T>
    Dataset create_dataset_for(const string &name, const T &buff, 
        const string &flag = "ac");

    template<typename T>
    Dataset create_dataset_for_scalar(const string &name, 
        const T &x, const string &flag = "ac");
        
    Dataset create_dataset_for_str(const string &name, 
        const string &s, const string &flag = "ac");
    Dataset create_dataset_for_str(const string &name, 
        const char *s, const string &flag = "ac");

    Dataset create_dataset_for_str(const string &name, 
        const vector<string> &ss, const string &flag = "ac");
    template<size_t N_STR>
    Dataset create_dataset_for_str(const string &name, 
        const string (&ss)[N_STR], const string &flag = "ac");

    template<size_t N_STR, size_t N>
    Dataset create_dataset_for_str(const string &name, 
        const char (&ss)[N_STR][N], const string &flag = "ac");

    /**
    Open an existing dataset of given path name ``name``.
    */
    Dataset open_dataset(const string &name, 
        const Proplist &aprop = Proplist::vDFLT) const;

    /** Return a reference to the intermediate-level HDF5 object. */
    _obj_raw_t & obj_raw() noexcept;
    const _obj_raw_t & obj_raw() const noexcept;
private: 
    template<typename ...Args>
    static _obj_ptr_t _ptr_from_raw(Args &&...args);
    template<typename ...Args>
    static Group _from_raw(Args &&...args);
};

/**
Tha manager provides commonly used shortcuts for dataset creation and 
access.
*/
class DatasetManager {
public:
    typedef Group refobj_t;

    /**
    Constructors.
    
    (1): refers to an empty dataset object. I/O may not be performed in this 
    case.
    
    (2): refers to given dataset.
    */
    DatasetManager() noexcept;
    explicit DatasetManager(refobj_t obj) noexcept;

    /**
    The dataset manager is copyable and movable. The copy is shallow with 
    resulting instance referring to the same dataset.
    The move is destroyable, leaving the move-from instance an empty state.
    */
    ~DatasetManager() noexcept = default;
    DatasetManager(const DatasetManager &) noexcept = default;
    DatasetManager(DatasetManager &&) noexcept = default;
    DatasetManager & operator=(const DatasetManager &) noexcept = default;
    DatasetManager & operator=(DatasetManager &&) noexcept = default;

    /**
    Retrieve the referred object, i.e., the dataset object.
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
    Put operations - write data into dataset of given ``name``.
    
    (1): Create a dataset with given ``name`` whose datatype are dataspace are 
    determined from the data ``x`` and write ``x`` into it.
    If the dataset already exists, open it and update its contents to ``x``
    where ``x`` must have consistent datatype and size.
    ``x`` may be anything that can be used in :expr:`Group::create_dataset_for`
    except the string overloads.

    (2): the same as (1), but using string data ``x`` acceptable by 
    :expr`Group::create_dataset_for_str`. Examples include single strings
    (``std::string`` or C-style string ``const char *`` ), vector of 
    strings (``std::vector<std::string>``) or array of strings 
    (``std::string[N]``), 2-D character array (``const char [N_STR][N]``,
    resolved as ``N_STR`` strings with fixed length ``N``).
    The string dataset is created as scalar or 1-D dataspace, with fixed-length 
    ATOMIC STRING datatype.

    (3): Put the data ``x`` into an existsing dataset of given ``name``. The
    filespace of the dataset is selected by a hyperslab ``filespace``.

    (4): Put a list of data elements ``x`` at the given coordinates in an 
    existing dataset of given ``name``.

    (5): Put a single data element ``x`` at the given coordinate in an 
    existing dataset of given ``name``.
    */
    template<typename T>
    void put(const string &name, const T &x);

    template<typename T>
    void put_str(const string &name, const T &x);

    template<typename T>
    void put_slab(const string &name, const T &x, const Hyperslab &filespace);

    template<typename T>
    void put_ats(const string &name, const T &x, const Points &coords);
    
    template<typename T>
    void put_at(const string &name, const T &x, const Dimensions &coord);

    /**
    Get operations: read data from existing dataset of given ``name``.
    
    In all case, the dataset must have consistent datatype and dataspace with
    the argument ``x``.

    (1-5): are similar to ``put`` but perform reading here.
    
    (6-10): are similar to (1-5), respectively, but return the data which are
    read (i.e., the object typed ``T`` gets default-initialized, be read into, 
    and returned).
    */
    template<typename T>
    void get(const string &name, T &x);

    template<typename T>
    void get_str(const string &name, T &x);

    template<typename T>
    void slab(const string &name, T &x, const Hyperslab &filespace);

    template<typename T>
    void ats(const string &name, T &x, const Points &coords);

    template<typename T>
    void at(const string &name, T &x, const Dimensions &coord);

    template<typename T>
    T get(const string &name);

    template<typename T>
    T get_str(const string &name);

    template<typename T>
    T slab(const string &name, const Hyperslab &filespace);

    template<typename T>
    T ats(const string &name, const Points &coords);

    template<typename T>
    T at(const string &name, const Dimensions &coord);

    /**
    Find whether or not a dataset of given ``name`` exists in the parent 
    object.
    */
    bool exists(const string &name);

    /**
    Open an existing dataset named ``name``.
    */
    Dataset open(const string &name);
private:
    refobj_t _obj;
};


} // namespace HIPP::IO::H5

#endif	//_HIPPIO_H5_OBJ_GROUP_H_