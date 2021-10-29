/**
create: Yangyao CHEN, 2020/01/11
    [write   ] Obj - base class for all HDF5 high-level objects.
*/ 

#ifndef _HIPPIO_H5_OBJ_BASE_H_
#define _HIPPIO_H5_OBJ_BASE_H_

#include "../hippio_hdf5_raw/h5_raw.h"

namespace HIPP::IO::H5 {

/**
The base class of all HDF5 objects. 

``Obj`` is copyable and movable (both in construction and assignment). 
The copy, move and destruction are ``noexcept``. The copy operation 
is shallow - the resulting object always refers to the same HDF5 resource
as the source object. The move operation sets the move-from object an empty
state.
*/
class Obj {
public:
    typedef _Obj _obj_raw_t;
    typedef std::shared_ptr<_Obj> _obj_ptr_t;

    /**
    Constructors.
    @ptr: HDF5 resource (represented by an intermediate-level object) that is to
    be taken over by the instance. If nullptr, the instance is initialized as 
    an empty state that refers to no HDF5 resource 
    (i.e., ``has_referenced()->false``).
    
    (1): default constructor - get an empty-state object.
    (2,3): use a shared pointer or a raw pointer.
    */
    explicit Obj() noexcept;
    explicit Obj(_obj_ptr_t ptr) noexcept;
    explicit Obj(_obj_raw_t *ptr) noexcept;

    virtual ~Obj() noexcept;
    
    Obj(const Obj &obj) noexcept;
    Obj(Obj &&obj) noexcept;
    Obj & operator=(const Obj &obj) noexcept;
    Obj & operator=(Obj &&obj) noexcept;

    /**
    Detach the instance from the underlying HDF5 resource. If no other object
    refers to this resource, the resource gets destroyed.
    
    On return, the instance becomes empty.
    */
    void free() noexcept;

    /**
    Return the original HDF5 resource/object handler (type ``hid_t``).
    */
    hid_t raw() const noexcept;
    
    /**
    Return the intermediate-level HDF5 object.
    */
    _obj_raw_t & obj_raw() noexcept;
    const _obj_raw_t & obj_raw() const noexcept;

    /**
    Find whether or not the object refers to an HDF5 resource.
    If not, the object is in an empty state.
    */
    bool has_referenced() const noexcept;
    explicit operator bool () const noexcept;

    /**
    Equality test - return true if either
    
    - lhs and rhs are both empty;
    - lhs and rhs refers to the same HDF5 resource (i.e., represented by the 
      same value of HDF5 identifier typed ``hid_t``).

    In any other case, two objects are unequal. For example, if one dataset
    in a file is opened by two calls of ``Group::open_dataset()``, the two
    resulting dataset objects are unequal.
    */
    bool operator==(const Obj &o) const;
    bool operator!=(const Obj &o) const;
protected:
    _obj_ptr_t _obj_ptr;

    template<typename T>
    static constexpr bool _is_buff() noexcept;

    template<typename T>
    static constexpr bool _is_array() noexcept;

    template<typename T>
    static constexpr bool _is_raw_array() noexcept;

    template<typename ...Args>
    static _obj_ptr_t _ptr_from_raw(Args &&...args);

    template<typename T>
    static const vector<T> & _buff_to_vec(const vector<T> &v);

    template<typename T, typename Buff>
    static vector<T> _buff_to_vec(const Buff &buff);
};

inline Obj::Obj() noexcept : _obj_ptr() {}

inline Obj::Obj( _obj_ptr_t ptr ) noexcept: _obj_ptr( std::move(ptr) ) { }

inline Obj::Obj( _obj_raw_t *ptr ) noexcept: _obj_ptr(ptr) { }

inline Obj::~Obj() noexcept { }

inline Obj::Obj( const Obj &obj ) noexcept: _obj_ptr( obj._obj_ptr ) { }

inline Obj::Obj( Obj &&obj ) noexcept: _obj_ptr( std::move(obj._obj_ptr) ) { }

inline Obj & Obj::operator=( const Obj &obj ) noexcept { 
    _obj_ptr = obj._obj_ptr;  return *this; 
}

inline Obj & Obj::operator=( Obj &&obj ) noexcept { 
    _obj_ptr = std::move(obj._obj_ptr);
    return *this; 
}

inline void Obj::free() noexcept {
    _obj_ptr.reset();
}

inline hid_t Obj::raw() const noexcept  { return _obj_ptr->raw(); }

inline auto Obj::obj_raw() noexcept -> _obj_raw_t & { return *_obj_ptr; }

inline auto Obj::obj_raw() const noexcept -> const _obj_raw_t &  { 
    return *_obj_ptr; 
}

inline bool Obj::has_referenced() const noexcept { 
    return static_cast<bool>(_obj_ptr); 
}

inline Obj::operator bool () const noexcept {
        return has_referenced();
}

inline bool Obj::operator==(const Obj &o) const {
    if( has_referenced() ) {
        if( o.has_referenced() )
            return raw() == o.raw();
        else 
            return false;
    }
    return !o.has_referenced();
}

inline bool Obj::operator!=(const Obj &o) const {
    return ! this->operator==(o);
}

template<typename T>
inline constexpr bool Obj::_is_buff() noexcept {
    return ContiguousBufferTraits<T>::is_buffer;
}


template<typename T>
inline constexpr bool Obj::_is_array() noexcept {
    return GeneralArrayTraits<T>::is_array;
}

template<typename T>
inline constexpr bool Obj::_is_raw_array() noexcept {
    return RawArrayTraits<T>::is_array;
}

template<typename ...Args>
inline auto  Obj::_ptr_from_raw(Args &&...args) -> _obj_ptr_t {
    return std::make_shared<_obj_raw_t>(std::forward<Args>(args)...);
}


template<typename T>
inline const vector<T> & Obj::_buff_to_vec(const vector<T> &v) {
    return v;
}

template<typename T, typename Buff>
inline vector<T> Obj::_buff_to_vec(const Buff &buff) {
    auto [p, n] = ContiguousBuffer{buff};
    return vector<T>(p, p+n);
}

} // namespace HIPP::IO::H5

#endif	//_HIPPIO_H5_OBJ_BASE_H_