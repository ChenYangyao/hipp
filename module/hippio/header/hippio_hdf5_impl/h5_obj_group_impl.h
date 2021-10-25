#ifndef _HIPPIO_H5_OBJ_GROUP_IMPL_H_
#define _HIPPIO_H5_OBJ_GROUP_IMPL_H_

#include "../hippio_hdf5/h5_obj_group.h"
#include "../hippio_hdf5/h5_util_datapacket.h"
#include "../hippio_hdf5/h5_obj_dataset.h"

namespace HIPP::IO::H5 {
    
template<typename T>
Dataset Group::create_dataset(const string &name, 
    const Dataspace &dspace, const string &flag, const Proplist &lcprop, 
    const Proplist &dcprop, const Proplist &daprop)
{
    return create_dataset(name, Datatype::from_type<T>(), dspace, flag, 
        lcprop, dcprop, daprop);
}

template<typename T>
Dataset Group::create_dataset_scalar(const string &name,
    const string &flag) {
    return create_dataset<T>(name, Dataspace::vSCALAR, flag);
}

template<typename T>
Dataset Group::create_dataset_for(const string &name, const T &buff,
    const string &flag) 
{
    auto [p, dsp, dt] = ConstDatapacket {buff};
    return create_dataset(name, dt, dsp, flag);
}

template<typename T>
Dataset Group::create_dataset_for_scalar(const string &name, const T &x,
    const string &flag) 
{
    auto [buff, dsp, dt] = ConstDatapacketScalar {x};
    return create_dataset(name, dt, dsp, flag);
}

template<size_t N_STR>
Dataset Group::create_dataset_for_str(const string &name, 
    const string (&ss)[N_STR], const string &flag)
{
    auto [buff, dsp, dt] = ConstDatapacketStr {ss};
    return create_dataset(name, dt, dsp, flag);
}

template<size_t N_STR, size_t N>
Dataset Group::create_dataset_for_str(const string &name, 
    const char (&ss)[N_STR][N], const string &flag)
{
    auto [buff, dsp, dt] = ConstDatapacketStr {ss};
    return create_dataset(name, dt, dsp, flag);
}

inline auto Group::obj_raw() noexcept -> _obj_raw_t & {
    return *reinterpret_cast<_obj_raw_t *>(_obj_ptr.get());
}

inline auto Group::obj_raw() const noexcept -> const _obj_raw_t & {
    return *reinterpret_cast<const _obj_raw_t *>(_obj_ptr.get());
}

template<typename ...Args>
Group::_obj_ptr_t Group::_ptr_from_raw(Args &&...args) {
    return std::make_shared<_obj_raw_t>(std::forward<Args>(args)...);
}

template<typename ...Args>
Group Group::_from_raw(Args &&...args) {
    return Group { _ptr_from_raw(std::forward<Args>(args)...) };
}

/** DatasetManager implementation. */
template<typename T>
void DatasetManager::put(const string &name, const T &x) {
    Dataset dset = _obj.create_dataset_for(name, x);
    dset.write(x);
}

template<typename T>
void DatasetManager::put_str(const string &name, const T &x) {
    Dataset dset = _obj.create_dataset_for_str(x);
    dset.write_str(x);
}

template<typename T>
void DatasetManager::put_slab(const string &name, const T &x, 
    const Hyperslab &filespace) 
{
    Dataset dset = _obj.open_dataset(name);
    dset.write_hyperslab(x, filespace);
}

template<typename T>
void DatasetManager::put_ats(const string &name, const T &x, 
    const Points &coords) 
{
    Dataset dset = _obj.open_dataset(name);
    dset.write_elements(x, coords);
}

template<typename T>
void DatasetManager::put_at(const string &name, const T &x, 
    const Dimensions &coord) 
{
    Dataset dset = _obj.open_dataset(name);
    dset.write_element(x, coord);
}

template<typename T>
void DatasetManager::get(const string &name, T &x) {
    Dataset dset = _obj.open_dataset(name);
    dset.read(x);
}

template<typename T>
void DatasetManager::get_str(const string &name, T &x) {
    Dataset dset = _obj.open_dataset(name);
    dset.read_str(x);
}

template<typename T>
void DatasetManager::slab(const string &name, T &x, const Hyperslab &filespace) 
{
    Dataset dset = _obj.open_dataset(name);
    dset.read_hyperslab(x, filespace);
}

template<typename T>
void DatasetManager::ats(const string &name, T &x, const Points &coords) {
    Dataset dset = _obj.open_dataset(name);
    dset.read_elements(x, coords);
}

template<typename T>
void DatasetManager::at(const string &name, T &x, const Dimensions &coord) {
    Dataset dset = _obj.open_dataset(name);
    dset.read_element(x, coord);
}

template<typename T>
T DatasetManager::get(const string &name) {
    T x;
    get(name, x);
    return x;
}

template<typename T>
T DatasetManager::get_str(const string &name) {
    T s;
    get_str(name, s);
    return s;
}

template<typename T>
T DatasetManager::slab(const string &name, const Hyperslab &filespace) {
    T x;
    slab(name, x, filespace);
    return x;
}

template<typename T>
T DatasetManager::ats(const string &name, const Points &coords) {
    T x;
    ats(name, x, coords);
    return x;
}

template<typename T>
T DatasetManager::at(const string &name, const Dimensions &coord) {
    T x;
    at(name, x, coord);
    return x;
}


} // namespace HIPP::IO::H5


#endif	//_HIPPIO_H5_OBJ_GROUP_IMPL_H_