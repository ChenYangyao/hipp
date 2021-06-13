/**
 * creat: Yangyao CHEN, 2020/01/11
 *      [write   ] 
 *      @H5Datatype: HDF5 high-level datatype object.
 */ 

#ifndef _HIPPIO_H5_OBJ_DATATYPE_H_
#define _HIPPIO_H5_OBJ_DATATYPE_H_
#include "h5_obj_base.h"
namespace HIPP{
namespace IO{

class H5Datatype: public H5Obj<_H5Datatype>{
public:
    typedef H5Obj<_H5Datatype> _obj_base_t;
    
    typedef _obj_raw_t::class_t class_t;
    static constexpr class_t 
        COMPOUND_C = _obj_raw_t::COMPOUND_C, 
        OPAQUE_C = _obj_raw_t::OPAQUE_C,
        ENUM_C = _obj_raw_t::ENUM_C,
        STRING_C = _obj_raw_t::STRING_C;
    
    using _obj_base_t::_obj_base_t;

    bool equal( const H5Datatype &dtype ) const;
    size_t size() const;

    H5Datatype copy() const;
    void resize( size_t size );

    static H5Datatype create(class_t c, size_t size);
    template<typename record_t, typename field_t, typename ...Args>
    static H5Datatype create_compound(
        const string &field_name, field_t record_t::*field_ptr,
        Args &&...args);
    H5Datatype & insert(const string &name, size_t offset, const H5Datatype &dtype);
    template<typename record_t, typename field_t>
    H5Datatype & insert(const string &name, field_t record_t::*field_ptr);
    H5Datatype & pack();
    unsigned nmembers() const;
    unsigned member_index(const string &name) const;
    class_t member_class(unsigned idx) const;
    size_t member_offset(unsigned idx) const;
    H5Datatype member_type(unsigned idx) const;
    string member_name(unsigned idx) const;

    template<typename record_t, typename field_t>
    static constexpr size_t offset(field_t record_t::*field_ptr) noexcept;

    /**
     * array datatype creator and visitor
     */
    H5Datatype create_array(const vector<hsize_t> &dims) const;
    template<typename raw_array_t>
    static H5Datatype create_array();
    unsigned array_ndims() const;
    vector<hsize_t> array_dims() const;
protected:
    static H5Datatype _from_raw(id_t h5_id) noexcept;
    template<typename T, typename M, typename ...Args>
    static void _compound_insert(H5Datatype &dtype, 
        const string &name, M T::*field_ptr, Args &&...args);
};

/**
 * We also define the high-level object counterpart for the pre-defined HDF5
 * datatype. 
 */
#define _HIPPIO_H5_PRE_OBJ(obj_name) extern const H5Datatype obj_name;

_HIPPIO_H5_PRE_OBJ(NATIVE_CHAR_T)
_HIPPIO_H5_PRE_OBJ(NATIVE_SCHAR_T)
_HIPPIO_H5_PRE_OBJ(NATIVE_SHORT_T)
_HIPPIO_H5_PRE_OBJ(NATIVE_INT_T)
_HIPPIO_H5_PRE_OBJ(NATIVE_LONG_T)
_HIPPIO_H5_PRE_OBJ(NATIVE_LLONG_T)
_HIPPIO_H5_PRE_OBJ(NATIVE_UCHAR_T)
_HIPPIO_H5_PRE_OBJ(NATIVE_USHORT_T)
_HIPPIO_H5_PRE_OBJ(NATIVE_UINT_T)
_HIPPIO_H5_PRE_OBJ(NATIVE_ULONG_T)
_HIPPIO_H5_PRE_OBJ(NATIVE_ULLONG_T)
_HIPPIO_H5_PRE_OBJ(NATIVE_FLOAT_T)
_HIPPIO_H5_PRE_OBJ(NATIVE_DOUBLE_T)

_HIPPIO_H5_PRE_OBJ(C_S1_T)

_HIPPIO_H5_PRE_OBJ(STD_I8LE_T)
_HIPPIO_H5_PRE_OBJ(STD_I16LE_T)
_HIPPIO_H5_PRE_OBJ(STD_I32LE_T)
_HIPPIO_H5_PRE_OBJ(STD_I64LE_T)
_HIPPIO_H5_PRE_OBJ(STD_U8LE_T)
_HIPPIO_H5_PRE_OBJ(STD_U16LE_T)
_HIPPIO_H5_PRE_OBJ(STD_U32LE_T)
_HIPPIO_H5_PRE_OBJ(STD_U64LE_T)
_HIPPIO_H5_PRE_OBJ(IEEE_F32LE_T)
_HIPPIO_H5_PRE_OBJ(IEEE_F64LE_T)

_HIPPIO_H5_PRE_OBJ(STD_I8BE_T)
_HIPPIO_H5_PRE_OBJ(STD_I16BE_T)
_HIPPIO_H5_PRE_OBJ(STD_I32BE_T)
_HIPPIO_H5_PRE_OBJ(STD_I64BE_T)
_HIPPIO_H5_PRE_OBJ(STD_U8BE_T)
_HIPPIO_H5_PRE_OBJ(STD_U16BE_T)
_HIPPIO_H5_PRE_OBJ(STD_U32BE_T)
_HIPPIO_H5_PRE_OBJ(STD_U64BE_T)
_HIPPIO_H5_PRE_OBJ(IEEE_F32BE_T)
_HIPPIO_H5_PRE_OBJ(IEEE_F64BE_T)

#undef _HIPPIO_H5_PRE_OBJ

namespace _h5_obj_datatype_helper {
template<typename T, typename M>
struct Field {
    typedef std::remove_all_extents_t<M> scalar_t;
    static constexpr size_t n_scalar = sizeof(M) / sizeof(scalar_t);
    static constexpr bool is_array = std::is_array_v<M>;

    explicit Field(M T::* mem_ptr) noexcept: _mem_ptr(mem_ptr){}
    static H5Datatype get_dtype() {
        typedef H5Datatype::_obj_raw_t _obj_raw_t;

        id_t raw_dtype = H5TypeNative<scalar_t>::h5_id;
        if constexpr( is_array ){
            auto dims = get_dims();
            id_t new_type = _obj_raw_t::create_array(
                raw_dtype, dims.size(), dims.data());
            return H5Datatype {
                std::make_shared<_obj_raw_t>(new_type, _obj_raw_t::stFREE) };
        }else {
            return H5Datatype {
                std::make_shared<_obj_raw_t>(raw_dtype, 0) };
        }
    }
    size_t get_offset() const {
        return (size_t)(ptrdiff_t)(void *)&( ((T *)(NULL))->*_mem_ptr );
    }
    static vector<hsize_t> get_dims() {
        vector<hsize_t> dims;
        _push_dims<M>(dims);
        for(size_t i=0; i+1<dims.size(); ++i)
            dims[i] /= dims[i+1];
        return dims;
    }

    const M T::*_mem_ptr;

    template<typename MIter>
    static void _push_dims(vector<hsize_t> &dims){
        constexpr hsize_t n1 = sizeof(MIter),
            n2 = sizeof(scalar_t),
            n_elem =  n1/n2; 
        if constexpr( n_elem > 1 ){
            dims.push_back(n_elem);
            _push_dims<decltype(std::declval<MIter>()[0])>(dims);
        }
    }
};
} // namespace _h5_obj_datatype_helper

template<typename record_t, typename field_t, typename ...Args>
H5Datatype H5Datatype::create_compound(
    const string &field_name, field_t record_t::*field_ptr,
    Args &&...args) 
{
    auto dtype = create(COMPOUND_C, sizeof(record_t));
    _compound_insert(dtype, field_name, field_ptr, std::forward<Args>(args)...);
    return dtype;
}

template<typename record_t, typename field_t>
H5Datatype & H5Datatype::insert(const string &name, field_t record_t::*field_ptr) {
    using namespace _h5_obj_datatype_helper;
    Field f { field_ptr };
    insert(name, f.get_offset(), f.get_dtype());
    return *this;
}

template<typename record_t, typename field_t>
constexpr size_t H5Datatype::offset(field_t record_t::*field_ptr) noexcept {
    return (size_t)(ptrdiff_t)(void *)&( ((record_t *)(NULL))->*field_ptr );
}

inline H5Datatype H5Datatype::_from_raw(id_t h5_id) noexcept {
    return H5Datatype(std::make_shared<_obj_raw_t>(h5_id, _obj_raw_t::stFREE));
}
template<typename raw_array_t>
H5Datatype H5Datatype::create_array(){
    using namespace _h5_obj_datatype_helper;
    struct wrap_t { raw_array_t m; };
    Field f { &wrap_t::m };
    return f.get_dtype();
}
template<typename T, typename M, typename ...Args>
void H5Datatype::_compound_insert(H5Datatype &dtype, 
    const string &name, M T::*field_ptr, Args &&...args)
{
    dtype.insert(name, field_ptr);

    static_assert( (sizeof...(Args))%2==0 );
    if constexpr( sizeof...(Args) > 0 ){
        _compound_insert(dtype, std::forward<Args>(args)...);
    }
}

} // namespace IO
} // namespace HIPP
#endif	//_HIPPIO_H5_OBJ_DATATYPE_H_