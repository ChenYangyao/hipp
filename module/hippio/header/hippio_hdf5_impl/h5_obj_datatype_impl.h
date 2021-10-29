#ifndef _HIPPIO_H5_OBJ_DATATYPE_IMPL_H_
#define _HIPPIO_H5_OBJ_DATATYPE_IMPL_H_

#include "../hippio_hdf5/h5_obj_datatype.h"

namespace HIPP::IO::H5  {

template<typename Record, typename Member, typename ...Args>
Datatype Datatype::create_compound(const string &mem_name, 
    Member Record::*mem_ptr, Args &&...args)
{
    auto dt = create_compound(sizeof(Record));
    dt._compound_insert(mem_name, mem_ptr, std::forward<Args>(args)...);
    return dt;
}

template<typename R, typename M, typename ...Args>
void Datatype::_compound_insert(const string &mem_name, 
    M R::*mem_ptr, Args &&...args) 
{
    insert(mem_name, mem_ptr);

    constexpr size_t n_rem = sizeof...(Args);
    static_assert( n_rem % 2 == 0, "Member name and pointer must be paired" );
    if constexpr(n_rem > 0)
        _compound_insert(std::forward<Args>(args)...);
}

template<typename Record, typename Member>
Datatype & Datatype::insert(const string &mem_name, Member Record::*mem_ptr) 
{
    TypeCvtField cvt {mem_ptr};
    insert(mem_name, cvt.offset(), cvt.datatype());
    return *this;
}

template<typename Record, typename Member>
constexpr ptrdiff_t Datatype::offset(Member Record::*mem_ptr) noexcept {
    return TypeCvtField(mem_ptr).offset();
}

template<typename RawArray, DatatypeArch TA>
Datatype Datatype::create_array_for() {
    typedef std::remove_cv_t<std::remove_reference_t<RawArray> > _input_t;
    typedef RawArrayTraits<_input_t> tr_t;

    static_assert( tr_t::is_array, "Template argument RawArray "
        "is not protocol-compliant" );
    return from_type<typename tr_t::value_t, TA>().create_array(tr_t::extents);
}

template<typename NativeT, DatatypeArch TA>
const Datatype & Datatype::from_type() {
    return TypeCvt<NativeT, TA>::datatype();
}

template<typename T>
Datatype Datatype::from_buff(const T &buff) {
    if constexpr ( std::is_pointer_v<T> ) {
        return Datatype::from_type< std::remove_pointer_t<T> >();
    } else {
        return ConstDatapacket{buff}.dtype;
    }
}

inline auto Datatype::obj_raw() noexcept -> _obj_raw_t & {
    return *reinterpret_cast<_obj_raw_t *>(_obj_ptr.get());
}

inline auto Datatype::obj_raw() const noexcept -> const _obj_raw_t & {
    return *reinterpret_cast<const _obj_raw_t *>(_obj_ptr.get());
}

template<typename ...Args>
Datatype::_obj_ptr_t Datatype::_ptr_from_raw(Args &&...args) {
    return std::make_shared<_obj_raw_t>(std::forward<Args>(args)...);
}

template<typename ...Args>
Datatype Datatype::_from_raw(Args &&...args) {
    return Datatype { _ptr_from_raw(std::forward<Args>(args)...) };
}

#define _HIPP_TEMPHD template<typename R, typename M>
#define _HIPP_TEMPARG <R, M>
#define _HIPP_TEMPCLS TypeCvtField _HIPP_TEMPARG
#define _HIPP_TEMPRET _HIPP_TEMPHD inline auto _HIPP_TEMPCLS::
#define _HIPP_TEMPNORET _HIPP_TEMPHD inline _HIPP_TEMPCLS::

_HIPP_TEMPHD
constexpr _HIPP_TEMPCLS::TypeCvtField(mem_ptr_t mem_ptr) noexcept 
: _mem_ptr(mem_ptr) {}

_HIPP_TEMPHD
ptrdiff_t _HIPP_TEMPCLS::offset() const noexcept {
    R *pr = nullptr;
    auto * pm = &(pr->*_mem_ptr);
    return reinterpret_cast<char *>(pm) - reinterpret_cast<char *>(pr);
}

_HIPP_TEMPHD
constexpr size_t _HIPP_TEMPCLS::size() noexcept { 
    return sizeof(M); 
}

_HIPP_TEMPHD
const Datatype & _HIPP_TEMPCLS::datatype() noexcept {
    return type_cvt_t::datatype();
}

_HIPP_TEMPHD
constexpr bool _HIPP_TEMPCLS::is_raw_array() noexcept { 
    return raw_array_traits_t::is_array; 
}

_HIPP_TEMPHD
const Datatype & _HIPP_TEMPCLS::value_datatype() noexcept {
    if constexpr( is_raw_array() ) 
        return TypeCvt<typename raw_array_traits_t::value_t>::datatype();
    else 
        return datatype();
}
_HIPP_TEMPHD
constexpr auto _HIPP_TEMPCLS::extents() noexcept {
    if constexpr( is_raw_array() )
        return raw_array_traits_t::extents;
    else
        return std::array<size_t, 1>{1};
}


#undef _HIPP_TEMPHD
#undef _HIPP_TEMPARG
#undef _HIPP_TEMPCLS
#undef _HIPP_TEMPRET
#undef _HIPP_TEMPNORET

} // namespace HIPP::IO::H5 

#endif	//_HIPPIO_H5_OBJ_DATATYPE_IMPL_H_