#ifndef _HIPPNUMERICAL_GSL_UTIL_BASE_H_
#define _HIPPNUMERICAL_GSL_UTIL_BASE_H_
#include <hippcntl.h>
#include <gsl/gsl_errno.h>

#define _HIPPNUMERICAL_GSL_UTIL_DEFAULT_CP_MV(classname)\
    classname( const classname & ) = default;\
    classname( classname && ) = default;\
    classname & operator=( const classname & ) = default;\
    classname & operator=( classname && ) = default;

#define _HIPPNUMERICAL_GSL_UTIL_DELETE_CP_MV(classname)\
    classname( const classname & ) = delete;\
    classname( classname && ) = delete;\
    classname & operator=( const classname & ) = delete;\
    classname & operator=( classname && ) = delete;

#endif	//_HIPPNUMERICAL_GSL_UTIL_BASE_H_