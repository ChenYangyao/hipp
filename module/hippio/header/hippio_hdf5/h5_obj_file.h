/**
create: Yangyao CHEN, 2020/01/11
    [write   ] File: HDF5 high-level file object.
*/ 

#ifndef _HIPPIO_H5_OBJ_FILE_H_
#define _HIPPIO_H5_OBJ_FILE_H_

#include "h5_obj_group.h"

namespace HIPP::IO::H5 {

/**
``File`` provides interfaces to manipulate (create, open, or retrieve 
metainfo from) HDF5 files.

The file class is copyable and movable (both in construction and 
assignment). The copy, move and destruction are ``noexcept``. The copy operation 
is shallow - the resulting object always refers to the same HDF5 resource
as the source object. The move operation sets the move-from objects an empty
state.
*/
class File: public Group {
public:
    typedef Group parent_t;
    
    typedef _File _obj_raw_t;
    typedef std::shared_ptr<_obj_raw_t> _obj_ptr_t;

    /** 
    Class ``File`` "inherits" all constructors from its parent class.
    */
    using parent_t::parent_t;
    
    /**
    Create a new file or open an existing file.
    @name:  file name
    @flag:  file access mode, must be one of the following
        "w": create and truncate, then open as R/W mode.
        "x": exclusively create (failed if existing), then open as R/W mode.
        "ac" | "ca": open existing file as R/W mode. Create it if not existing.
        "r": open existing file as R mode.
        "a": open existing file as R/W mode.
    @cprop, aprop:   
            creation and access property lists. If to open existing one, cprop
            is ignored.
    */
    File(const string &name, const string &flag = "ac", 
        const Proplist &cporp = Proplist::vDFLT, 
        const Proplist &aprop = Proplist::vDFLT);
    

    /**
    Open an existing file named ``name`` in file system.
    @flag: "r" (Read-only) | "a" (W/R). The open mode.
    */
    static File open(const string &name, const string &flag = "a", 
        const Proplist &aprop = Proplist::vDFLT);

    /**
    Create a file.
    @flag: "w" (truncate if existing) | "x" (fail if existing).
    */
    static File create(const string &name, const string &flag = "w",
        const Proplist &cprop = Proplist::vDFLT,
        const Proplist &aprop = Proplist::vDFLT);

    /**
    Create a property list for file operation.
    @cls: The property class, must be be one from
        "c" | "create": file creation property list.
        "a" | "access": file access property list.
        "m" | "mount":  file mount property list.
    */
    static Proplist create_proplist(const string &cls = "c");

    /** Return a reference to the intermediate-level HDF5 object. */
    _obj_raw_t & obj_raw() noexcept;
    const _obj_raw_t & obj_raw() const noexcept;
private: 
    template<typename ...Args>
    static _obj_ptr_t _ptr_from_raw(Args &&...args);
    template<typename ...Args>
    static File _from_raw(Args &&...args);
};

inline auto File::obj_raw() noexcept -> _obj_raw_t & {
    return *reinterpret_cast<_obj_raw_t *>(_obj_ptr.get());
}

inline auto File::obj_raw() const noexcept -> const _obj_raw_t & {
    return *reinterpret_cast<const _obj_raw_t *>(_obj_ptr.get());
}

template<typename ...Args>
File::_obj_ptr_t File::_ptr_from_raw(Args &&...args) {
    return std::make_shared<_obj_raw_t>(std::forward<Args>(args)...);
}

template<typename ...Args>
File File::_from_raw(Args &&...args) {
    return File { _ptr_from_raw(std::forward<Args>(args)...) };
}

} // namespace HIPP::IO::H5

#endif	//_HIPPIO_H5_OBJ_FILE_H_