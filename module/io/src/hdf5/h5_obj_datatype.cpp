#include <h5_obj_datatype.h>

namespace HIPP {
namespace IO {


bool H5Datatype::equal( const H5Datatype &dtype ) const {
    return bool( _obj_ptr->equal( dtype.raw() ) );
}
size_t H5Datatype::size() const {
    return _obj_ptr->get_size();
}
H5Datatype H5Datatype::copy() const {
    auto ptr = std::make_shared<_obj_raw_t>( _obj_ptr->copy( raw() ) );
    return H5Datatype( ptr );
}
void H5Datatype::resize( size_t size ){
    _obj_ptr->set_size( size );
}

H5Datatype H5Datatype::create(class_t c, size_t size){
    return _from_raw(_obj_raw_t::create(c, size));
}
H5Datatype & H5Datatype::insert(const string &name, size_t offset, 
    const H5Datatype &dtype) {
    _obj_ptr->insert(name.c_str(), offset, dtype.raw());
    return *this;
}
H5Datatype & H5Datatype::pack(){
    _obj_ptr->pack();
    return *this;
}
unsigned H5Datatype::nmembers() const {
    return _obj_ptr->get_nmembers();
}
unsigned H5Datatype::member_index(const string &name) const {
    return _obj_ptr->get_member_index(name.c_str());
}
H5Datatype::class_t H5Datatype::member_class(unsigned idx) const {
    return _obj_ptr->get_member_class(idx);
}
size_t H5Datatype::member_offset(unsigned idx) const {
    return _obj_ptr->get_member_offset(idx);
}
H5Datatype H5Datatype::member_type(unsigned idx) const {
    return _from_raw(_obj_ptr->get_member_type(idx));
}
string H5Datatype::member_name(unsigned idx) const {
    return _obj_ptr->get_member_name(idx);
}
H5Datatype H5Datatype::create_array(const vector<hsize_t> &dims) const {
    return _from_raw(_obj_raw_t::create_array(raw(), dims.size(), dims.data()));
}
unsigned H5Datatype::array_ndims() const {
    return _obj_ptr->get_array_ndims();
}
vector<hsize_t> H5Datatype::array_dims() const {
    size_t ndims = _obj_ptr->get_array_ndims();
    vector<hsize_t> ret(ndims);
    _obj_ptr->get_array_dims(ret.data());
    return ret;
}



#define _HIPPIO_H5_PRE_OBJ(obj_name, h5_id) \
    extern const H5Datatype obj_name \
        { std::make_shared<H5Datatype::_obj_raw_t>(h5_id, 0) };

_HIPPIO_H5_PRE_OBJ(NATIVE_CHAR_T, H5T_NATIVE_CHAR)
_HIPPIO_H5_PRE_OBJ(NATIVE_SCHAR_T, H5T_NATIVE_SCHAR)
_HIPPIO_H5_PRE_OBJ(NATIVE_SHORT_T, H5T_NATIVE_SHORT)
_HIPPIO_H5_PRE_OBJ(NATIVE_INT_T, H5T_NATIVE_INT)
_HIPPIO_H5_PRE_OBJ(NATIVE_LONG_T, H5T_NATIVE_LONG)
_HIPPIO_H5_PRE_OBJ(NATIVE_LLONG_T, H5T_NATIVE_LLONG)
_HIPPIO_H5_PRE_OBJ(NATIVE_UCHAR_T, H5T_NATIVE_UCHAR)
_HIPPIO_H5_PRE_OBJ(NATIVE_USHORT_T, H5T_NATIVE_USHORT)
_HIPPIO_H5_PRE_OBJ(NATIVE_UINT_T, H5T_NATIVE_UINT)
_HIPPIO_H5_PRE_OBJ(NATIVE_ULONG_T, H5T_NATIVE_ULONG)
_HIPPIO_H5_PRE_OBJ(NATIVE_ULLONG_T, H5T_NATIVE_ULLONG)
_HIPPIO_H5_PRE_OBJ(NATIVE_FLOAT_T, H5T_NATIVE_FLOAT)
_HIPPIO_H5_PRE_OBJ(NATIVE_DOUBLE_T, H5T_NATIVE_DOUBLE)

_HIPPIO_H5_PRE_OBJ(C_S1_T, H5T_C_S1)

_HIPPIO_H5_PRE_OBJ(STD_I8LE_T, H5T_STD_I8LE)
_HIPPIO_H5_PRE_OBJ(STD_I16LE_T, H5T_STD_I16LE)
_HIPPIO_H5_PRE_OBJ(STD_I32LE_T, H5T_STD_I32LE)
_HIPPIO_H5_PRE_OBJ(STD_I64LE_T, H5T_STD_I64LE)
_HIPPIO_H5_PRE_OBJ(STD_U8LE_T, H5T_STD_U8LE)
_HIPPIO_H5_PRE_OBJ(STD_U16LE_T, H5T_STD_U16LE)
_HIPPIO_H5_PRE_OBJ(STD_U32LE_T, H5T_STD_U32LE)
_HIPPIO_H5_PRE_OBJ(STD_U64LE_T, H5T_STD_U64LE)
_HIPPIO_H5_PRE_OBJ(IEEE_F32LE_T, H5T_IEEE_F32LE)
_HIPPIO_H5_PRE_OBJ(IEEE_F64LE_T, H5T_IEEE_F64LE)

_HIPPIO_H5_PRE_OBJ(STD_I8BE_T, H5T_STD_I8BE)
_HIPPIO_H5_PRE_OBJ(STD_I16BE_T, H5T_STD_I16BE)
_HIPPIO_H5_PRE_OBJ(STD_I32BE_T, H5T_STD_I32BE)
_HIPPIO_H5_PRE_OBJ(STD_I64BE_T, H5T_STD_I64BE)
_HIPPIO_H5_PRE_OBJ(STD_U8BE_T, H5T_STD_U8BE)
_HIPPIO_H5_PRE_OBJ(STD_U16BE_T, H5T_STD_U16BE)
_HIPPIO_H5_PRE_OBJ(STD_U32BE_T, H5T_STD_U32BE)
_HIPPIO_H5_PRE_OBJ(STD_U64BE_T, H5T_STD_U64BE)
_HIPPIO_H5_PRE_OBJ(IEEE_F32BE_T, H5T_IEEE_F32BE)
_HIPPIO_H5_PRE_OBJ(IEEE_F64BE_T, H5T_IEEE_F64BE)


#undef _HIPPIO_H5_PRE_OBJ

} // namespace IO
} // namespace HIPP
