#ifndef _HIPPIO_H5_TYPE_RAW_H_
#define _HIPPIO_H5_TYPE_RAW_H_
#include "h5_base.h"
#include "h5_error.h"
namespace HIPP{
namespace IO{

enum class H5TypeClass: int { NATIVE=0, STD=1 };

template<typename T, H5TypeClass TC> class H5Type { };
template<typename T> using H5TypeNative = H5Type<T, H5TypeClass::NATIVE>;

#define _HIPPIO_H5_TYPENATIVE_DEF( native, id ) \
template<>\
class H5Type<native, H5TypeClass::NATIVE> { \
public: \
    typedef native native_t; \
    static hid_t h5_id;\
    static const char *const h5_name;\
};\
hid_t H5Type<native, H5TypeClass::NATIVE>::h5_id = id;\
const char *const H5Type<native, H5TypeClass::NATIVE>::h5_name = #id;\

_HIPPIO_H5_TYPENATIVE_DEF(double, H5T_NATIVE_DOUBLE)
_HIPPIO_H5_TYPENATIVE_DEF(float, H5T_NATIVE_FLOAT)
_HIPPIO_H5_TYPENATIVE_DEF(long double, H5T_NATIVE_LDOUBLE)
_HIPPIO_H5_TYPENATIVE_DEF(char, H5T_NATIVE_CHAR)
_HIPPIO_H5_TYPENATIVE_DEF(short, H5T_NATIVE_SHORT)
_HIPPIO_H5_TYPENATIVE_DEF(int, H5T_NATIVE_INT)
_HIPPIO_H5_TYPENATIVE_DEF(long, H5T_NATIVE_LONG)
_HIPPIO_H5_TYPENATIVE_DEF(unsigned short, H5T_NATIVE_USHORT)
_HIPPIO_H5_TYPENATIVE_DEF(unsigned int, H5T_NATIVE_UINT)
_HIPPIO_H5_TYPENATIVE_DEF(unsigned long, H5T_NATIVE_ULONG)

#undef _HIPPIO_H5_TYPENATIVE_DEF

#define _HIPPIO_H5_TYPESTD_DEF( native, id ) \
template<>\
class H5Type<native, H5TypeClass::STD> { \
public: \
    typedef native native_t; \
    static hid_t h5_id;\
    static const char *const h5_name;\
};\
hid_t H5Type<native, H5TypeClass::STD>::h5_id = id;\
const char *const H5Type<native, H5TypeClass::STD>::h5_name = #id;\

_HIPPIO_H5_TYPESTD_DEF(double, H5T_IEEE_F64LE)
_HIPPIO_H5_TYPESTD_DEF(float, H5T_IEEE_F32LE)
_HIPPIO_H5_TYPESTD_DEF(long double, H5T_IEEE_F64LE)
_HIPPIO_H5_TYPESTD_DEF(char, H5T_STD_I8LE)
_HIPPIO_H5_TYPESTD_DEF(short, H5T_STD_I16LE)
_HIPPIO_H5_TYPESTD_DEF(int, H5T_STD_I32LE)
_HIPPIO_H5_TYPESTD_DEF(long, H5T_STD_I64LE)
_HIPPIO_H5_TYPESTD_DEF(unsigned short, H5T_STD_U16LE)
_HIPPIO_H5_TYPESTD_DEF(unsigned int, H5T_STD_U32LE)
_HIPPIO_H5_TYPESTD_DEF(unsigned long, H5T_STD_U64LE)

#undef _HIPPIO_H5_TYPESTD_DEF




} // namespace IO
} // namespace HIPP
#endif	//_HIPPIO_H5_TYPE_RAW_H_