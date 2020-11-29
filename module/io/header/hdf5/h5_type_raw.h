/**
 * creat: Yangyao CHEN, 2020/01/11
 * HDF5 intermediate-level type handler.
 */ 

#ifndef _HIPPIO_H5_TYPE_RAW_H_
#define _HIPPIO_H5_TYPE_RAW_H_
#include "h5_base.h"
#include "h5_error.h"
namespace HIPP{
namespace IO{

/**
 * All HDF5 datatypes will be mapped to H5Type<T, TC>, where `T` is a native
 * type in C-language, TC is the indentifier indicating how `T` is organized.
 * 
 * Possible values of `TC` supported now are listed in H5TypeClass
 * @NATIVE: when TC=NATIVE, H5Type<T, TC> contains HDF5 oringinal native
 *          datatype corresponding to T. e.g. 
 *              H5Type<double, NATIVE>::h5_id == H5T_NATIVE_DOUBLE.
 * @STD:    when TC=STD, H5Type<T, TC> contains HDF5 original machine-
 *          independent datatype corresponding to T. e.g
 *              H5Type<double, STD>::h5_id == H5T_IEEE_F64LE.
 * @COMPOUND:   
 *          when TC=COMPOUND, H5Type<T, TC> contains a HDF5 original 
 *          compound datatype. e.g. 
 *              H5Type<char *, COMPOUND>::h5_id == H5T_C_S1.
 * 
 * Some useful aliases are provided:
 * H5TypeNative<T> == H5Type<T, NATIVE>.
 * H5TypeStd<T> == H5Type<T, STD>.
 * H5TypeCompound<T> == H5Type<T, COMPOUND>.
 *
 * Specificly:
 * H5TypeStr == H5TypeCompound<char *>.
 */
enum class H5TypeClass: int { NATIVE=0, STD=1, COMPOUND=2 };

template<typename T, H5TypeClass TC> class H5Type { };
template<typename T> using H5TypeNative = H5Type<T, H5TypeClass::NATIVE>;
template<typename T> using H5TypeStd = H5Type<T, H5TypeClass::STD>;
template<typename T> using H5TypeCompound = H5Type<T, H5TypeClass::COMPOUND>;
using H5TypeStr = H5TypeCompound<char *>;

#define _HIPPIO_H5_TYPENATIVE_DEF( native ) \
template<>\
class H5Type<native, H5TypeClass::NATIVE> { \
public: \
    typedef native native_t; \
    static const hid_t h5_id;\
    static const char *const h5_name;\
};

_HIPPIO_H5_TYPENATIVE_DEF(double)
_HIPPIO_H5_TYPENATIVE_DEF(float)
_HIPPIO_H5_TYPENATIVE_DEF(long double)
_HIPPIO_H5_TYPENATIVE_DEF(char)
_HIPPIO_H5_TYPENATIVE_DEF(signed char)
_HIPPIO_H5_TYPENATIVE_DEF(short)
_HIPPIO_H5_TYPENATIVE_DEF(int)
_HIPPIO_H5_TYPENATIVE_DEF(long)
_HIPPIO_H5_TYPENATIVE_DEF(long long)
_HIPPIO_H5_TYPENATIVE_DEF(unsigned char)
_HIPPIO_H5_TYPENATIVE_DEF(unsigned short)
_HIPPIO_H5_TYPENATIVE_DEF(unsigned int)
_HIPPIO_H5_TYPENATIVE_DEF(unsigned long)
_HIPPIO_H5_TYPENATIVE_DEF(unsigned long long)

#undef _HIPPIO_H5_TYPENATIVE_DEF

#define _HIPPIO_H5_TYPESTD_DEF( native ) \
template<>\
class H5Type<native, H5TypeClass::STD> { \
public: \
    typedef native native_t; \
    static const hid_t h5_id;\
    static const char *const h5_name;\
};

_HIPPIO_H5_TYPESTD_DEF(double)
_HIPPIO_H5_TYPESTD_DEF(float)
_HIPPIO_H5_TYPESTD_DEF(long double)
_HIPPIO_H5_TYPESTD_DEF(char)
_HIPPIO_H5_TYPESTD_DEF(signed char)
_HIPPIO_H5_TYPESTD_DEF(short)
_HIPPIO_H5_TYPESTD_DEF(int)
_HIPPIO_H5_TYPESTD_DEF(long)
_HIPPIO_H5_TYPESTD_DEF(long long)
_HIPPIO_H5_TYPESTD_DEF(unsigned char)
_HIPPIO_H5_TYPESTD_DEF(unsigned short)
_HIPPIO_H5_TYPESTD_DEF(unsigned int)
_HIPPIO_H5_TYPESTD_DEF(unsigned long)
_HIPPIO_H5_TYPESTD_DEF(unsigned long long)

#undef _HIPPIO_H5_TYPESTD_DEF

template<>
class H5Type<char *, H5TypeClass::COMPOUND>{
public:
    typedef char *native_t;
    static const hid_t h5_id;
    static const char *const h5_name;

    template<typename A>
    static vector<hsize_t> shape( const vector<string, A> &strs ){
        hsize_t maxlen = 0;    
        for(const auto &s: strs){
            maxlen = std::max( maxlen, hsize_t(s.size()) );
        }
        ++maxlen;
        vector<hsize_t> _shape = { hsize_t(strs.size()), maxlen };
        return _shape;
    }
    template<typename A>
    static vector<char> serialize( const vector<string, A> &strs ){
        auto _shape = shape( strs );
        vector<char> ret( _shape[0]*_shape[1], '\0' );
        for(hsize_t i=0; i<_shape[0]; ++i){
            strcpy( ret.data()+i*_shape[1], strs[i].c_str() );
        }
        return ret;
    }
    template<typename A>
    static vector<string> deserialize( const vector<char, A> &strs, hsize_t nstr ){
        hsize_t maxlen = strs.size()/nstr;
        if( nstr * maxlen != strs.size() )
            ErrLogic::throw_(ErrLogic::eLENGTH, emFLPFB, 
                "  ... serialized string length ", strs.size(), 
                " is not multiple of no. strings ", nstr);
        vector<string> ret;
        for(hsize_t i=0; i<nstr; ++i){
            ret.emplace_back( (const char *)strs.data() + i*maxlen );
        }
        return ret;
    }
};

} // namespace IO
} // namespace HIPP
#endif	//_HIPPIO_H5_TYPE_RAW_H_