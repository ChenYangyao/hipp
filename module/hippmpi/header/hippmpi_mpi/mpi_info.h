/**
create: Yangyao CHEN, 2020/02/11
    [write   ] Info - the high-level info object of MPI
*/ 

#ifndef _HIPPMPI_MPI_INFO_H_
#define _HIPPMPI_MPI_INFO_H_
#include "mpi_obj_base.h"
namespace HIPP{
namespace MPI{

class File;
class Win;

/**
The high-level info object of MPI.

The info object is used to pass hints to MPI implementation to provide
platform-specific optimization or compatibility specification.

An info object can be directly constructed, or returned by other MPI object
such as `File`, `Comm`.

The life-time of `Info` is maintained internally, but you may free it in
advance to save some memory.
*/
class Info: public MPIObj<_Info> {
public:
    typedef MPIObj<_Info> _obj_base_t;
    using _obj_base_t::_obj_base_t;
    typedef std::pair<string, string> item_t;

    /**
    Construct an empty info object with no key.
    */
    Info();

    /**
    ``info()`` prints a short (``fmt_cntl=0``) or a verbose (``fmt_cntl=1``) 
    description of the current instance to the stream ``os``.
    The verbose version prints all key-value pairs.
    Larger ``level`` produces more indents.

    Operator ``<<`` is equivalent to ``info()`` with default ``fmt_cntl`` and
    ``level``.

    The passed stream ``os`` is returned.
    */
    ostream &info(ostream &os = cout, int fmt_cntl = 0, int level = 0) const;
    friend ostream & operator<<(ostream &os, const Info &info);

    /**
    Free the current info object and set it to null value as returned by
    ``nullval()``.
    ``free()`` can be called at any time, even for a pre-defined info object.
    */
    void free() noexcept;

    /**
    Inquiry whether the info object is a null value.
    */
    bool is_null() const;

    /**
    Add (or reset) a key-value pair.
    */
    Info & set( const string &key, const string &val );

    /**
    Delete a key-value pair.
    */
    Info & del( const string &key );

    /**
    Inquiry key-value pair, or pairs, or information of the pair.
    
    get(key, val): return the value into ``val`` corresponding to ``key``. If
        the key is not set, return ``false``, and ``val`` is not modified. 
        Otherwise return `true`.
    get(key): return the value corresponding to `key`. If not set, throw an
        ``ErrLogic``.
    get(n): returns the n-th key-value pair. ``item_t`` is a ``std::pair<>``
        instance with the ``first`` and ``second`` member corresponding to
        key and value, respectively.
    get(): returns a vector containing all key-value pairs.
    
    ``operator[](key)`` and ``oprator[](n)`` are aliased to ``get(key)`` and 
    ``get(n)``.
    
    ``get_valuelen()``: returns the lengths of the value into ``len`` (not 
         including the space of null-terminator) and return ``true`` if the key 
         is set. Otherwise return `false` and `len` is not modified.
    get_nkeys(): get number of keys.
    get_nthkey(): return the n-th key.
    */
    bool get(const string &key, string &val) const;
    string get(const string &key) const;
    item_t get(int n) const; 
    vector<item_t> get() const;
    string operator[](const string &key) const;
    item_t operator[](int n) const;

    bool get_valuelen(const string &key, int &len) const;
    int get_nkeys() const;
    string get_nthkey(int n) const;

    /**
    Duplicate the info object, create a new empty info object, or get a 
    null value object.
    */
    Info dup();
    static Info create();
    static Info nullval() noexcept;
protected:
    friend class File;
    friend class Win;
    static Info _from_raw( mpi_t info, int state ) noexcept;
};


inline Info Info::_from_raw( mpi_t info, int state ) noexcept{
    return Info( std::make_shared<_obj_raw_t>(info, state) );
}

inline ostream & operator<<(ostream &os, const Info &info){
    return info.info(os);
}

}
}
#endif	//_HIPPMPI_MPI_INFO_H_