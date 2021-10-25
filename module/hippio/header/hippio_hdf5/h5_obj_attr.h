/**
 create: Yangyao CHEN, 2020/01/11
    [write   ] Attr: HDF5 attribute high-level object.
*/ 

#ifndef _HIPPIO_H5_OBJ_ATTR_H_
#define _HIPPIO_H5_OBJ_ATTR_H_

#include "h5_obj_base.h"

namespace HIPP::IO::H5 {

class Dataspace;
class Datatype;
class NamedObj;

/**
Attributes are "small datasets" attached on HDF5 named objects.

Class ``Attr`` encapsulates the methods available on attributes. 

The attribute class is copyable and movable (both in construction and 
assignment). The copy, move and destruction are ``noexcept``. The copy operation 
is shallow - the resulting object always refers to the same HDF5 resource
as the source object. The move operation sets the move-from object an empty
state.
*/
class Attr: public Obj {
public:
    typedef Obj parent_t;

    typedef _Attr _obj_raw_t;
    typedef std::shared_ptr<_obj_raw_t> _obj_ptr_t;

    /**
    Class ``Attr`` "inherits" all constructors from its parent class.
    */
    using parent_t::parent_t;

    /**
    Return copy of the dataspace or datatype of the attribute.
    The datatype is read-only. 
    */
    Dataspace dataspace() const;
    Datatype datatype() const;

    /**
    Write data into the attribute.
    
    (1): The most general method that is the direct HDF5 C API counterpart. 
    Write a buffer starting at ``buff`` to the attribute with ``memtype``
    specifying is datatype.

    (2): Write an object. The ``buff`` argument can be
    - A raw pointer. The memory datatype is inferred from is point-to type 
      by :expr:`Datatype::from_type`.
    - any object that is resolvable by :class:`ConstDatapacket`, including 
      numerical scalars (e.g., ``int``, ``float``), 
      general arrays or numerical types (e.g., ``std::array<int, 4>``, 
      ``std::vector<int>``), or general arrays of raw arrays 
      (e.g., ``std::vector< std::array<int, 4> >``).

    Other methods are defined for special cases.

    (3): Write a scalar. The datatype is inferred from T as if calling 
    :expr:`Datatype::from_type<T>()`.

    (4-11): write a string or a list of strings as fix-length ATOMIC string
    datatype. In the list-of-strings case, the size of the string is taken from 
    the maximum of the strings. 
    
    In all cases, the attribute must be created with consistent dataspace and
    datatype.
    */
    void write(const void *buff, const Datatype &memtype);
    
    template<typename T>
    void write(const T &buff);

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
    Read data from the attribute.
    
    (1): The most general method that is the direct HDF5 C API counterpart. 
    Read into buffer starting at ``buff`` from the attribute with ``memtype``
    specifying its datatype.

    (2): Read into an object. The ``buff`` argument can be
    - A raw pointer. The memory datatype is inferred from is point-to type 
      by :expr:`Datatype::from_type`.
    - ``std::vector<T, Alloc>``. ``T`` must be valid argument to 
      :expr:`Datatype::from_type` (e.g., numerical scalar or raw-array).
      The vector is always resized to exactly hold all the elements. 
      If the resize operation cannot exactly fits the need, an ``ErrLogic``
      is thrown.
    - any object that is resolvable by :class:`ConstDatapacket`, including 
      numerical scalars (e.g., ``int``, ``float``), 
      general arrays or numerical types (e.g., ``std::array<int, 4>``).

    Other methods are defined for special cases.

    (3): Read a scalar. The datatype is inferred from T as if calling 
    :expr:`Datatype::from_type<T>()`.

    (4-8): Read a string or a list of strings as fix-length ATOMIC string
    datatype. In the list-of-strings case, the size of the string is taken from 
    the maximum of the strings. 
    The ``std::string`` or ``std::vector`` is auto-resized to fit the data. 
    Otherwise (e.g., ``char *``), the buffer must be large enough to hold
    the string that is read.
    
    In all cases, the attribute must has consistent dataspace and datatype.
    */
    void read(void *buff, const Datatype &memtype);
    
    template<typename T>
    void read(T &&buff);

    template<typename T>
    void read_scalar(T &x);

    void read_str(string &s);
    void read_str(char *s);
    
    void read_str(vector<string> &ss);
    void read_str(string ss[]);
    template<size_t N>
    void read_str(char ss[][N]);

    /** Return a reference to the intermediate-level HDF5 object. */
    _obj_raw_t & obj_raw() noexcept;
    const _obj_raw_t & obj_raw() const noexcept;
private: 
    friend class NamedObj;
    template<typename ...Args>
    static _obj_ptr_t _ptr_from_raw(Args &&...args);
    template<typename ...Args>
    static Attr _from_raw(Args &&...args);
};

} // namespace HIPP::IO::H5

#endif	//_HIPPIO_H5_OBJ_ATTR_H_