#ifndef _HIPPIO_H5_OBJ_NAMED_BASE_IMPL_H_
#define _HIPPIO_H5_OBJ_NAMED_BASE_IMPL_H_

#include "../hippio_hdf5/h5_obj_named_base.h"
#include "../hippio_hdf5/h5_obj_attr.h"
#include "../hippio_hdf5/h5_util_datapacket.h"

namespace HIPP::IO::H5 {

inline void NamedObj::get_info(info_t &info, info_field_t fields) const {
    obj_raw().get_info(info, fields);
}

inline NamedObj::info_t NamedObj::get_info(info_field_t fields) const {
    info_t info;
    get_info(info, fields);
    return info;
}
    
template<typename T>
Attr NamedObj::create_attr(const string &name, const Dataspace &dspace,
    const string &flag, const Proplist &cprop, const Proplist &aprop) 
{
    return create_attr(name, Datatype::from_type<T>(), dspace, flag, 
        cprop, aprop);
}

template<typename T>
Attr NamedObj::create_attr_scalar(const string &name, const string &flag)
{
    return create_attr<T>(name, Dataspace::vSCALAR, flag);
}

template<typename T>
Attr NamedObj::create_attr_for(const string &name, const T &buff, 
    const string &flag)
{
    auto [p, dsp, dt] = ConstDatapacket{buff};
    return create_attr(name, dt, dsp, flag);
}

template<typename T>
Attr NamedObj::create_attr_for_scalar(const string &name, const T &x, 
    const string &flag)
{
    auto [buff, dsp, dt] = ConstDatapacketScalar {x};
    return create_attr(name, dt, dsp, flag);
}

template<size_t N_STR>
Attr NamedObj::create_attr_for_str(const string &name, const string 
    (&ss)[N_STR], const string &flag)
{
    auto [buff, dsp, dt] = ConstDatapacketStr{ss};
    return create_attr(name, dt, dsp, flag);
}

template<size_t N_STR, size_t N>
Attr NamedObj::create_attr_for_str(const string &name, 
    const char (&ss)[N_STR][N], 
    const string &flag)
{
    auto [buff, dsp, dt] = ConstDatapacketStr{ss};
    return create_attr(name, dt, dsp, flag);
}

inline auto NamedObj::obj_raw() noexcept -> _obj_raw_t & {
    return *reinterpret_cast<_obj_raw_t *>(_obj_ptr.get());
}

inline auto NamedObj::obj_raw() const noexcept -> const _obj_raw_t & {
    return *reinterpret_cast<const _obj_raw_t *>(_obj_ptr.get());
}

template<typename ...Args>
NamedObj::_obj_ptr_t NamedObj::_ptr_from_raw(Args &&...args) {
    return std::make_shared<_obj_raw_t>(std::forward<Args>(args)...);
}

template<typename ...Args>
NamedObj NamedObj::_from_raw(Args &&...args) {
    return NamedObj { _ptr_from_raw(std::forward<Args>(args)...) };
}

template<typename T>
void AttrManager::put(const string &name, const T &x) {
    Attr attr = _obj.create_attr_for(name, x);
    attr.write(x);
}

template<typename T>
void AttrManager::put_str(const string &name, const T &s) {
    Attr attr = _obj.create_attr_for_str(name, s);
    attr.write_str(s);
}

template<typename T>
void AttrManager::get(const string &name, T &x) {
    _obj.open_attr(name).read(x);
}

template<typename T>
void AttrManager::get_str(const string &name, T &s) {
    _obj.open_attr(name).read_str(s);
}

template<typename T>
T AttrManager::get(const string &name) {
    T x;
    get(name, x);
    return x;
}

template<typename T>
T AttrManager::get_str(const string &name) {
    T s;
    get_str(name, s);
    return s;
}


} // namespace HIPP::IO::H5

#endif	//_HIPPIO_H5_OBJ_NAMED_BASE_IMPL_H_