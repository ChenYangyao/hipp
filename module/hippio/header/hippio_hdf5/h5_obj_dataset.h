/**
create: Yangyao CHEN, 2020/01/11
    [write   ] Dataset: HDF5 high-level dataset object.
*/ 

#ifndef _HIPPIO_H5_OBJ_DATASET_H_
#define _HIPPIO_H5_OBJ_DATASET_H_

#include "h5_obj_named_base.h"

namespace HIPP::IO::H5 {

class Group;


/**
Datasets are named objects stored in HDF5 files.

Class ``Dataset`` encapsulates the methods available on datasets. 

The dataset class is copyable and movable (both in construction and 
assignment). The copy, move and destruction are ``noexcept``. The copy operation 
is shallow - the resulting object always refers to the same HDF5 resource
as the source object. The move operation sets the move-from object an empty
state.
*/
class Dataset: public NamedObj {
public:
    
    typedef NamedObj parent_t;
    
    typedef _Dataset _obj_raw_t;
    typedef std::shared_ptr<_obj_raw_t> _obj_ptr_t;

    /** 
    Class ``Dataset`` "inherits" all constructors from its parent class.
    */
    using parent_t::parent_t;

    /**
    Return copy of the dataspace or datatype of the dataset.
    
    The datatype is read-only. 
    If the datatype is named, it is opened and returned.
    */
    Dataspace dataspace() const;
    Datatype datatype() const;

    /**
    create_proplist(): create a property list for dataset operation.
    @cls: creation property list of dataset.
        - "c" | "create": dataset creation.
        - "a" | "access": dataset access.
        - "x" | "xfer" | "transfer": data transfer.

    proplist(): retrieve the property list of the current dataset. Only 
    dataset creation and access property list can be retrieved. 
    */
    static Proplist create_proplist(const string &cls);
    Proplist proplist(const string &cls) const;
    
    /**
    Write data into the dataset.
    
    (1): The most general method that is the direct HDF5 C API counterpart. 
    Write a buffer starting at ``buff`` to the dataset. ``memtype``, 
    ``memspace`` and ``filespace`` specify the memory datatype, memory 
    dataspace and dataset dataspace in the file.

    (2): Write an object. The ``buff`` argument can be
    - A raw pointer. The memory datatype is inferred from is point-to type 
      by :expr:`Datatype::from_type`. If ``memspace`` is all-space, the file
      space is used for it.
    - any object that is resolvable by :class:`ConstDatapacket`, including 
      numerical scalars (e.g., ``int``, ``float``), 
      general arrays or numerical types (e.g., ``std::array<int, 4>``, 
      ``std::vector<int>``), or general arrays of raw arrays 
      (e.g., ``std::vector< std::array<int, 4> >``).
      If ``memspace`` is all-space, it is inferred from the object itself.

    Other methods are defined for special cases. All of them use the default 
    property list :expr:`Proplist::vDFLT`.

    (3): Write a scalar. The datatype is inferred from T as if calling 
    :expr:`Datatype::from_type<T>()`.

    (4-11): write a string or a list of strings as fix-length ATOMIC string
    datatype. In the list-of-strings case, the size of the string is taken from 
    the maximum of the strings. 
    
    In all cases, the dataset must be created with consistent dataspace and
    datatype.
    */
    void write(const void *buff, 
        const Datatype &memtype, 
        const Dataspace &memspace  = Dataspace::vALL,
        const Dataspace &filespace = Dataspace::vALL,
        const Proplist &xprop      = Proplist::vDFLT);

    template<typename T>
    void write(const T &buff, 
        const Dataspace &memspace  = Dataspace::vALL,
        const Dataspace &filespace = Dataspace::vALL,
        const Proplist &xprop      = Proplist::vDFLT);

    template<typename T>
    void write_scalar(const T &x);

    void write_str(const string &s);
    void write_str(const char *s);

    void write_str(const vector<string> &ss);
    void write_str(const string ss[], size_t n_str);
    template<size_t N_STR>
    void write_str(const string (&ss)[N_STR]);

    void write_str(const char * const *ss, size_t n_str);

    template<size_t N>
    void write_str(const char ss[][N], size_t n_str);
        
    template<size_t N_STR, size_t N>
    void write_str(const char (&ss)[N_STR][N]);

    /**
    Write data into the selected part of the dataset.
    
    write_hyperslab() : the filespace is selected by a hyperslab.
    write_elements(): the filespace is selected by a list of elements of given
        coordinates.
    write_element(): a single element in the filespace is selected.

    These methods are equivalenent to first get the full filespace, 
    call ``select_hyperslab()`` or ``select_elements()`` on it, and then 
    call ``write(buff, Datapace::vALL, filespace)`` (for hyperslab and elements)
    or ``write(buff, Datapace::vSCALAR, filespace)`` (for a single element).
    */
    template<typename T,
        std::enable_if_t<!std::is_pointer_v<T>, int > =0>
    void write_hyperslab(const T &buff, const Hyperslab &hs);

    template<typename T,
        std::enable_if_t<!std::is_pointer_v<T>, int > =0>
    void write_elements(const T &buff, const Points &points);

    template<typename T,
        std::enable_if_t<!std::is_pointer_v<T>, int > =0>
    void write_element(const T &x, const Dimensions &coord);
    
    /**
    Read data from the dataset.
    
    (1): The most general method that is the direct HDF5 C API counterpart. 
    Read into buffer starting at ``buff`` from the dataset. ``memtype``, 
    ``memspace`` and ``filespace`` specify the memory datatype, memory 
    dataspace and dataset dataspace in the file.

    (2): Read into an object. The ``buff`` argument can be
    - A raw pointer. The memory datatype is inferred from is point-to type 
      by :expr:`Datatype::from_type`. If ``memspace`` is all-space, the file
      space is used for it.
    - ``std::vector<T, Alloc>``. ``T`` must be valid argument to 
      :expr:`Datatype::from_type` (e.g., numerical scalar or raw-array).
      If ``memspace`` is all-space, it is taken from filespace. 
      The vector is always resized to exactly hold the selected elements. 
      If the resize operation cannot exactly fits the need, an ``ErrLogic``
      is thrown.
    - any object that is resolvable by :class:`ConstDatapacket`, including 
      numerical scalars (e.g., ``int``, ``float``), 
      general arrays or numerical types (e.g., ``std::array<int, 4>``).
      If ``memspace`` is all-space, it is inferred from the object itself.

    Other methods are defined for special cases. All of them use the default 
    property list :expr:`Proplist::vDFLT`.

    (3): Read a scalar. The datatype is inferred from T as if calling 
    :expr:`Datatype::from_type<T>()`.

    (4-8): Read a string or a list of strings as fix-length ATOMIC string
    datatype. In the list-of-strings case, the size of the string is taken from 
    the maximum of the strings. 
    The ``std::string`` or ``std::vector`` is auto-resized to fit the data. 
    Otherwise (e.g., ``char *``), the buffer must be large enough to hold
    the string that is read.
    
    In all cases, the dataset must has consistent dataspace and datatype.
    */
    void read(void *buff, 
        const Datatype &memtype,
        const Dataspace &memspace  = Dataspace::vALL, 
        const Dataspace &filespace = Dataspace::vALL,
        const Proplist &xprop      = Proplist::vDFLT) const;
    
    template<typename T>
    void read(T &&buff, const Dataspace &memspace = Dataspace::vALL, 
        const Dataspace &filespace = Dataspace::vALL,
        const Proplist &xprop = Proplist::vDFLT) const;

    template<typename T>
    void read_scalar(T &x);

    void read_str(string &s);
    void read_str(char *s);
    
    void read_str(vector<string> &ss);
    void read_str(string ss[]);
    template<size_t N>
    void read_str(char ss[][N]);

    /**
    Read data from the selected part of the dataset.
    
    read_hyperslab() : the filespace is selected by a hyperslab.
    read_elements(): the filespace is selected by a list of elements of given
        coordinates.
    read_element(): a single element in the filespace is selected.

    These methods are equivalenent to first get the full filespace, 
    call ``select_hyperslab()`` or ``select_elements()`` on it, and then 
    call ``read(v, Datapace::vALL, filespace)`` (for hyperslab and elements)
    or ``read(x, Datapace::vSCALAR, filespace)`` (for a single element).
    */
    template<typename T, typename A>
    void read_hyperslab(vector<T, A> &v, const Hyperslab &hs) const;

    template<typename T, typename A>
    void read_elements(vector<T, A> &v, const Points &points) const;
    
    template<typename T>
    void read_element(T &x, const Dimensions &coord) const;

    /** Return a reference to the intermediate-level HDF5 object. */
    _obj_raw_t & obj_raw() noexcept;
    const _obj_raw_t & obj_raw() const noexcept;
private: 
    friend class Group;
    
    template<typename ...Args>
    static _obj_ptr_t _ptr_from_raw(Args &&...args);
    template<typename ...Args>
    static Dataset _from_raw(Args &&...args);

    /**
    For SIMPLE mem and file spaces.
    */
    hsize_t _find_mem_size(
        const Dataspace &memspace, 
        const Dataspace &filespace) const;
};

} // namespace HIPP::IO::H5

#endif	//_HIPPIO_H5_OBJ_DATASET_H_