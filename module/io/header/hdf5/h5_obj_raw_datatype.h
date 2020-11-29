/**
 * creat: Yangyao CHEN, 2020/11/29
 *      [write   ] 
 *      @_H5Datatype:   Intermediate-level HDF5 datatype API.
 */ 
#ifndef _HIPPIO_H5_OBJ_RAW_DATATYPE_H_
#define _HIPPIO_H5_OBJ_RAW_DATATYPE_H_
#include "h5_obj_raw_base.h"
namespace HIPP {
namespace IO {

class _H5Datatype: public _H5Obj {
public:
    typedef H5T_class_t class_t;
    static constexpr class_t 
        COMPOUND_C = H5T_COMPOUND, 
        OPAQUE_C = H5T_OPAQUE,
        ENUM_C = H5T_ENUM,
        STRING_C = H5T_STRING;

    explicit _H5Datatype( id_t datatype, int state=stFREE ) noexcept;
     ~_H5Datatype();
    static void close(id_t dtype);

    tri_t equal( hid_t dtype ) const;
    size_t get_size() const;

    /* copy from old type to new type */
    static id_t copy( id_t dtype );

    /* change the size of current type */
    void set_size( size_t size );
   
    /* compound datatype creation and access */
    static id_t create(class_t c, size_t size);
    void insert(const char *name, size_t offset, 
        id_t field_dtype);
    void pack();
    unsigned get_nmembers() const;
    unsigned get_member_index(const char *field_name) const;
    class_t get_member_class(unsigned field_idx) const;
    size_t get_member_offset(unsigned field_idx) const;
    id_t get_member_type(unsigned field_idx) const;
    string get_member_name(unsigned field_idx) const;

    /* array datatype creation and access */
    static id_t create_array(id_t dtype, 
        unsigned rank, const hsize_t dims[]);
    unsigned get_array_ndims() const;
    unsigned get_array_dims(hsize_t dims[]) const;
};

/** 
 * We also define the wrapper instance for the predefined data type.
 * e.g. _NATIVE_CHAR_T.raw() gives H5T_NATIVE_CHAR.
 */
#define _HIPPIO_H5_PRE_T(obj_name) extern const _H5Datatype obj_name;

_HIPPIO_H5_PRE_T(_NATIVE_CHAR_T)
_HIPPIO_H5_PRE_T(_NATIVE_SCHAR_T)
_HIPPIO_H5_PRE_T(_NATIVE_SHORT_T)
_HIPPIO_H5_PRE_T(_NATIVE_INT_T)
_HIPPIO_H5_PRE_T(_NATIVE_LONG_T)
_HIPPIO_H5_PRE_T(_NATIVE_LLONG_T)
_HIPPIO_H5_PRE_T(_NATIVE_UCHAR_T)
_HIPPIO_H5_PRE_T(_NATIVE_USHORT_T)
_HIPPIO_H5_PRE_T(_NATIVE_UINT_T)
_HIPPIO_H5_PRE_T(_NATIVE_ULONG_T)
_HIPPIO_H5_PRE_T(_NATIVE_ULLONG_T)
_HIPPIO_H5_PRE_T(_NATIVE_FLOAT_T)
_HIPPIO_H5_PRE_T(_NATIVE_DOUBLE_T)

_HIPPIO_H5_PRE_T(_C_S1_T)

_HIPPIO_H5_PRE_T(_STD_I8LE_T)
_HIPPIO_H5_PRE_T(_STD_I16LE_T)
_HIPPIO_H5_PRE_T(_STD_I32LE_T)
_HIPPIO_H5_PRE_T(_STD_I64LE_T)
_HIPPIO_H5_PRE_T(_STD_U8LE_T)
_HIPPIO_H5_PRE_T(_STD_U16LE_T)
_HIPPIO_H5_PRE_T(_STD_U32LE_T)
_HIPPIO_H5_PRE_T(_STD_U64LE_T)
_HIPPIO_H5_PRE_T(_IEEE_F32LE_T)
_HIPPIO_H5_PRE_T(_IEEE_F64LE_T)

_HIPPIO_H5_PRE_T(_STD_I8BE_T)
_HIPPIO_H5_PRE_T(_STD_I16BE_T)
_HIPPIO_H5_PRE_T(_STD_I32BE_T)
_HIPPIO_H5_PRE_T(_STD_I64BE_T)
_HIPPIO_H5_PRE_T(_STD_U8BE_T)
_HIPPIO_H5_PRE_T(_STD_U16BE_T)
_HIPPIO_H5_PRE_T(_STD_U32BE_T)
_HIPPIO_H5_PRE_T(_STD_U64BE_T)
_HIPPIO_H5_PRE_T(_IEEE_F32BE_T)
_HIPPIO_H5_PRE_T(_IEEE_F64BE_T)

#undef _HIPPIO_H5_PRE_T


inline _H5Datatype::_H5Datatype( id_t datatype, int state ) noexcept
: _H5Obj( datatype, state ){ }
inline _H5Datatype::~_H5Datatype( ){ 
    if(_state & stFREE) 
        close(_obj);
}
inline void _H5Datatype::close(id_t dtype) {
    ErrH5::check(H5Tclose(dtype), emFLPFB);
}
inline tri_t _H5Datatype::equal( hid_t dtype ) const{
    tri_t is_eq = H5Tequal( _obj, dtype );
    ErrH5::check( is_eq, emFLPFB );
    return is_eq;
}
inline size_t _H5Datatype::get_size() const {
    size_t size = H5Tget_size( _obj );
    if( size == 0 ) ErrH5::throw_( size, emFLPFB );
    return size;
}
inline id_t _H5Datatype::copy( id_t dtype ){
    id_t ret = H5Tcopy( dtype );
    ErrH5::check( ret, emFLPFB );
    return ret;
}
inline void _H5Datatype::set_size( size_t size ){
    ErrH5::check( H5Tset_size( _obj, size ), emFLPFB );
}
inline id_t _H5Datatype::create(class_t c, size_t size){
    auto ret = H5Tcreate(c, size);
    ErrH5::check(ret, emFLPFB, "  ... size=", size, '\n');
    return ret;
}
inline void _H5Datatype::insert(const char *field_name, size_t offset, 
    id_t field_dtype){
    ErrH5::check(H5Tinsert(_obj, field_name, offset, field_dtype), 
        emFLPFB, "  ... field_name=", field_name, ", offset=", offset, '\n');
}
inline void _H5Datatype::pack(){
    ErrH5::check(H5Tpack(_obj), emFLPFB);
}
inline unsigned _H5Datatype::get_nmembers() const {
    int nmems = H5Tget_nmembers(_obj);
    ErrH5::check(nmems, emFLPFB);
    return static_cast<unsigned>(nmems);
}
inline unsigned _H5Datatype::get_member_index(const char *field_name) const {
    int mem_idx = H5Tget_member_index(_obj, field_name);
    ErrH5::check(mem_idx, emFLPFB, "  ... field_name=", field_name, '\n');
    return static_cast<unsigned>(mem_idx);
}
inline auto _H5Datatype::get_member_class(unsigned field_idx) const ->class_t{
    class_t cls = H5Tget_member_class(_obj, field_idx);
    ErrH5::check(cls, emFLPFB, "  ... field index=", field_idx, '\n');
    return cls;
}
inline size_t _H5Datatype::get_member_offset(unsigned field_idx) const{
    get_member_class(field_idx);
    return H5Tget_member_offset(_obj, field_idx);
}
inline id_t _H5Datatype::get_member_type(unsigned field_idx) const {
    id_t tp = H5Tget_member_type(_obj, field_idx);
    ErrH5::check(tp, emFLPFB, "  ... field index=", field_idx, '\n');
    return tp;
}
inline string _H5Datatype::get_member_name(unsigned field_idx) const {
    char *nm = H5Tget_member_name(_obj, field_idx);
    if( !nm )
        ErrH5::throw_(-1, emFLPFB, "  ... field_idx=", field_idx, '\n');
    string ret(nm);
    ErrH5::check(H5free_memory(nm), emFLPFB, 
        "  ... free memory of field name failed. Field index=",
         field_idx, '\n');
    return ret;
}
inline id_t _H5Datatype::create_array(id_t dtype, unsigned rank, 
    const hsize_t dims[]){
    auto ret = H5Tarray_create(dtype, rank, dims);
    ErrH5::check(ret, emFLPFB, "  ... no. of dims=", rank, '\n');
    return ret;
}
inline unsigned _H5Datatype::get_array_ndims() const{
    int ndims = H5Tget_array_ndims(_obj);
    ErrH5::check(ndims, emFLPFB, '\n');
    return static_cast<unsigned>(ndims);
}
inline unsigned _H5Datatype::get_array_dims(hsize_t dims[]) const {
    int ndims = H5Tget_array_dims(_obj, dims);
    ErrH5::check(ndims, emFLPFB, '\n');
    return static_cast<unsigned>(ndims);
}




} // namespace IO
} // namespace HIPP
#endif	//_HIPPIO_H5_OBJ_RAW_DATATYPE_H_