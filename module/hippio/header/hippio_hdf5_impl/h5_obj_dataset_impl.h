#ifndef _HIPPIO_H5_OBJ_DATASET_IMPL_H_
#define _HIPPIO_H5_OBJ_DATASET_IMPL_H_

#include "../hippio_hdf5/h5_obj_dataset.h"
#include "../hippio_hdf5/h5_util_datapacket.h"

namespace HIPP::IO::H5 {

inline auto Dataset::obj_raw() noexcept -> _obj_raw_t & {
    return *reinterpret_cast<_obj_raw_t *>(_obj_ptr.get());
}

inline auto Dataset::obj_raw() const noexcept -> const _obj_raw_t & {
    return *reinterpret_cast<const _obj_raw_t *>(_obj_ptr.get());
}

template<typename ...Args>
Dataset::_obj_ptr_t Dataset::_ptr_from_raw(Args &&...args) {
    return std::make_shared<_obj_raw_t>(std::forward<Args>(args)...);
}

template<typename ...Args>
Dataset Dataset::_from_raw(Args &&...args) {
    return Dataset { _ptr_from_raw(std::forward<Args>(args)...) };
}

inline Dataspace Dataset::dataspace() const {
    auto sp = obj_raw().get_space();
    return Dataspace::_from_raw(sp);
}

inline Datatype Dataset::datatype() const {
    auto dt = obj_raw().get_type();
    return Datatype::_from_raw(dt);
}

template<typename T>
void Dataset::write(const T &buff, 
    const Dataspace &memspace,const Dataspace &filespace, const Proplist &xprop)
{
    if constexpr ( std::is_pointer_v<T> ) {
        write(buff, 
            Datatype::from_type< std::remove_pointer_t<T> >(), 
            memspace, filespace, xprop);
    } else {
        auto [p, dsp, dt] = ConstDatapacket{buff};
        if( memspace.raw() != _Dataspace::vALL ) dsp = memspace;
        write(p, dt, dsp, filespace);
    }
}

template<typename T>
void Dataset::write_scalar(const T &x) {
    ConstDatapacketScalar dp {x};
    write(dp.buff, dp.dtype);
}

template<size_t N_STR>
void Dataset::write_str(const string (&ss)[N_STR]) {
    write_str(ss, N_STR);
}

template<size_t N>
void Dataset::write_str(const char ss[][N], size_t n_str) {
    auto [p, dsp, dt] = ConstDatapacketStr {ss, n_str};
    write(p, dt, dsp);
}

template<size_t N_STR, size_t N>
void Dataset::write_str(const char (&ss)[N_STR][N]) {
    write_str(ss, N_STR);
}

template<typename T, std::enable_if_t<!std::is_pointer_v<T>, int >>
void Dataset::write_hyperslab(const T &buff, const Hyperslab &hs) {
    auto file_sp = dataspace();
    file_sp.select_hyperslab(hs);
    write(buff, Dataspace::vALL, file_sp);
}

template<typename T, std::enable_if_t<!std::is_pointer_v<T>, int >>
void Dataset::write_elements(const T &buff, const Points &points) {
    auto file_sp = dataspace();
    file_sp.select_elements(points);
    write(buff, Dataspace::vALL, file_sp);
}

template<typename T, std::enable_if_t<!std::is_pointer_v<T>, int >>
void Dataset::write_element(const T &x, const Dimensions &coord) {
    auto file_sp = dataspace();
    file_sp.select_elements(Dataspace::selSET, 1, coord.data());
    write(&x, Dataspace::vSCALAR, file_sp);
}

template<typename T>
void Dataset::read(T &&buff, const Dataspace &memspace, 
    const Dataspace &filespace, const Proplist &xprop) const
{
    if constexpr ( std::is_pointer_v<T> ) {
        read(buff, Datatype::from_type< std::remove_pointer_t<T> >(), 
            memspace, filespace, xprop);
    } else if constexpr( TypeCvtBuff< std::remove_reference_t<T> >::is_vector ) 
    {
        hsize_t dst_sz = _find_mem_size(memspace, filespace);
        auto [p, dsp, dt] = Datapacket::resize_buff(buff, dst_sz);
        read(p, dt, dsp, filespace);
    } else {
        auto [p, dsp, dt] = Datapacket(buff);
        if( memspace.raw() != Dataspace::_obj_raw_t::vALL ) 
            dsp = memspace;
        read(p, dt, dsp, filespace);
    }
}

template<typename T>
void Dataset::read_scalar(T &x) {
    DatapacketScalar dp {x};
    read(dp.buff, dp.dtype);
}

template<size_t N>
void Dataset::read_str(char ss[][N]) {
    auto dt = C_S1_T.resized(N);
    read(ss, dt);
}

template<typename T, typename A>
void Dataset::read_hyperslab(vector<T, A> &v, const Hyperslab &hs) const
{
    auto file_sp = dataspace();
    file_sp.select_hyperslab(hs);
    read(v, Dataspace::vALL, file_sp);
}

template<typename T, typename A>
void Dataset::read_elements(vector<T, A> &v, const Points &points) const 
{
    auto file_sp = dataspace();
    file_sp.select_elements(points);
    read(v, Dataspace::vALL, file_sp);
}

template<typename T>
void Dataset::read_element(T &x, const Dimensions &coord) const {
    auto file_sp = dataspace();
    file_sp.select_elements(Dataspace::selSET, 1, coord.data());
    read(&x, Dataspace::vSCALAR, file_sp);
}
    
inline hsize_t Dataset::_find_mem_size(const Dataspace &memspace, 
    const Dataspace &filespace) const
{
    auto all_v = Dataspace::_obj_raw_t::vALL;
    if( memspace.raw() != all_v ) {
        return memspace.get_select_npoints();
    } else if( filespace.raw() == all_v ) {
        return dataspace().size();
    } 
    return filespace.get_select_npoints();
}

} // namespace HIPP::IO::H5


#endif	//_HIPPIO_H5_OBJ_DATASET_IMPL_H_