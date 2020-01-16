#include "h5_type_raw.h"

namespace HIPP{
namespace IO{
    
#define _HIPPIO_H5_TYPENATIVE_DEF( native, id ) \
const hid_t H5Type<native, H5TypeClass::NATIVE>::h5_id = id;\
const char *const H5Type<native, H5TypeClass::NATIVE>::h5_name = #id;

_HIPPIO_H5_TYPENATIVE_DEF(double, H5T_NATIVE_DOUBLE)
_HIPPIO_H5_TYPENATIVE_DEF(float, H5T_NATIVE_FLOAT)
_HIPPIO_H5_TYPENATIVE_DEF(long double, H5T_NATIVE_LDOUBLE)
_HIPPIO_H5_TYPENATIVE_DEF(char, H5T_NATIVE_CHAR)
_HIPPIO_H5_TYPENATIVE_DEF(short, H5T_NATIVE_SHORT)
_HIPPIO_H5_TYPENATIVE_DEF(int, H5T_NATIVE_INT)
_HIPPIO_H5_TYPENATIVE_DEF(long, H5T_NATIVE_LONG)
_HIPPIO_H5_TYPENATIVE_DEF(long long, H5T_NATIVE_LLONG)
_HIPPIO_H5_TYPENATIVE_DEF(unsigned short, H5T_NATIVE_USHORT)
_HIPPIO_H5_TYPENATIVE_DEF(unsigned int, H5T_NATIVE_UINT)
_HIPPIO_H5_TYPENATIVE_DEF(unsigned long, H5T_NATIVE_ULONG)
_HIPPIO_H5_TYPENATIVE_DEF(unsigned long long, H5T_NATIVE_ULLONG)

#undef _HIPPIO_H5_TYPENATIVE_DEF

#define _HIPPIO_H5_TYPESTD_DEF( native, id ) \
const hid_t H5Type<native, H5TypeClass::STD>::h5_id = id;\
const char *const H5Type<native, H5TypeClass::STD>::h5_name = #id;

_HIPPIO_H5_TYPESTD_DEF(double, H5T_IEEE_F64LE)
_HIPPIO_H5_TYPESTD_DEF(float, H5T_IEEE_F32LE)
_HIPPIO_H5_TYPESTD_DEF(long double, H5T_IEEE_F64LE)
_HIPPIO_H5_TYPESTD_DEF(char, H5T_STD_I8LE)
_HIPPIO_H5_TYPESTD_DEF(short, H5T_STD_I16LE)
_HIPPIO_H5_TYPESTD_DEF(int, H5T_STD_I32LE)
_HIPPIO_H5_TYPESTD_DEF(long, H5T_STD_I64LE)
_HIPPIO_H5_TYPESTD_DEF(long long, H5T_STD_I64LE)
_HIPPIO_H5_TYPESTD_DEF(unsigned short, H5T_STD_U16LE)
_HIPPIO_H5_TYPESTD_DEF(unsigned int, H5T_STD_U32LE)
_HIPPIO_H5_TYPESTD_DEF(unsigned long, H5T_STD_U64LE)
_HIPPIO_H5_TYPESTD_DEF(unsigned long long, H5T_STD_U64LE)

#undef _HIPPIO_H5_TYPESTD_DEF

const hid_t H5Type<char *, H5TypeClass::COMPOUND>::h5_id = H5T_C_S1;
const char *const H5Type<char *, H5TypeClass::COMPOUND>::h5_name = "H5T_C_S1";

} // namespace IO
} // namespace HIPP
