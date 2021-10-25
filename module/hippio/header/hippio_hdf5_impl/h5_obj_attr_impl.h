#ifndef _HIPPIO_H5_OBJ_ATTR_IMPL_H_
#define _HIPPIO_H5_OBJ_ATTR_IMPL_H_

#include "../hippio_hdf5/h5_obj_attr.h"
#include "../hippio_hdf5/h5_util_datapacket.h"

namespace HIPP::IO::H5 {

inline Dataspace Attr::dataspace() const {
    auto sp = obj_raw().get_space();
    return Dataspace::_from_raw(sp);
}

inline Datatype Attr::datatype() const {
    auto dt = obj_raw().get_type();
    return Datatype::_from_raw(dt);
}

template<typename T>
void Attr::write(const T &buff) {
    if constexpr( std::is_pointer_v<T> ) {
        write(buff, Datatype::from_type< std::remove_pointer_t<T> >() );
    } else {
        auto [p, dsp, dt] = ConstDatapacket {buff};
        write(p, dt);
    }
}

template<typename T>
void Attr::write_scalar(const T &x) {
    ConstDatapacketScalar dp {x};
    write(dp.buff, dp.dtype);
}

template<size_t N_STR>
void Attr::write_str(const string (&ss)[N_STR]) {
    write_str(ss, N_STR);
}

template<size_t N>
void Attr::write_str(const char ss[][N], size_t n_str) {
    auto [p, dsp, dt] = ConstDatapacketStr{ss, n_str};
    write(p, dt);
}

template<size_t N_STR, size_t N>
void Attr::write_str(const char (&ss)[N_STR][N]) {
    write_str(ss, N_STR);
}
    
template<typename T>
void Attr::read(T &&buff) {
    if constexpr ( std::is_pointer_v<T> ) {
        read(buff, Datatype::from_type< std::remove_pointer_t<T> >());
    } else if constexpr (TypeCvtBuff< std::remove_reference_t<T> >::is_vector) 
    {
        const size_t file_sz = dataspace().size();
        auto [p, dsp, dt] = Datapacket::resize_buff(buff, file_sz);
        read(p, dt);
    } else {
        auto [p, dsp, dt] = Datapacket{buff};
        read(p, dt);
    }
}

template<typename T>
void Attr::read_scalar(T &x) {
    DatapacketScalar dp {x};
    read(dp.buff, dp.dtype);
}

template<size_t N>
void Attr::read_str(char ss[][N]) {
    auto dt = C_S1_T.resized(N);
    read(ss, dt);
}

inline auto Attr::obj_raw() noexcept -> _obj_raw_t & {
    return *reinterpret_cast<_obj_raw_t *>(_obj_ptr.get());
}

inline auto Attr::obj_raw() const noexcept -> const _obj_raw_t & {
    return *reinterpret_cast<const _obj_raw_t *>(_obj_ptr.get());
}

template<typename ...Args>
Attr::_obj_ptr_t Attr::_ptr_from_raw(Args &&...args) {
    return std::make_shared<_obj_raw_t>(std::forward<Args>(args)...);
}

template<typename ...Args>
Attr Attr::_from_raw(Args &&...args) {
    return Attr { _ptr_from_raw(std::forward<Args>(args)...) };
}
    
} // namespace HIPP::IO::H5


#endif	//_HIPPIO_H5_OBJ_ATTR_IMPL_H_