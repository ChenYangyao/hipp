/**
 * creat: Yangyao CHEN, 2019/12/28
 *      [write   ] MemRaw - raw (un-constructed) memory handlers
 */ 

#ifndef _HIPPCNTL_MEM_RAW_H_
#define _HIPPCNTL_MEM_RAW_H_
#include <stdlib.h>
#include <unistd.h>
#include "../stream/stream.h"
#include "../error/error.h"
namespace HIPP{

/**
 * raw (un-constructed) memory handlers.
 */
class MemRaw {
public:
    /**
     * C-style raw (un-constructed) memory managements.
     * One-to-one corresponds to GNU-C library functions.
     */
    static void *malloc( size_t size );
    static void *calloc( size_t nmemb, size_t size );
    static void *realloc( void *ptr, size_t size );
    /*
    static void *reallocarray( void *ptr, size_t nmemb, size_t size );
    */
    static void *aligned_alloc( size_t alignment, size_t size );
    static void free( void *ptr );
    /**
     * allocate page-aligned size bytes
     * Equavalent to aligned_alloc( getpagesize(), size ).
     */
    static void *page( size_t size );

    /**
     * error-safe versions.
     * If any error raised in an allocation function, print `args` to std::cerr
     * and throw an ErrSytem.
     * free_e() is the same as free(), but set the entry ptr to nullptr.
     */
    template<typename... Args>
    static void *malloc_e( size_t size, Args && ...args );
    template<typename... Args>
    static void *calloc_e( size_t nmemb, size_t size, Args && ...args );
    template<typename... Args>
    static void *realloc_e( void *ptr, size_t size, Args && ...args );
    /*
    template<typename... Args>
    static void *reallocarray_e( void *ptr, size_t nmemb, size_t size, Args && ...args );
    */
    template<typename... Args>
    static void *aligned_alloc_e( size_t alignment, size_t size, Args && ...args );
    template<typename T>
    static void free_e( T* &ptr );
    template<typename... Args>
    static void *page_e( size_t size, Args && ...args );
};
    

inline void *MemRaw::malloc( size_t size ){
    return ::malloc( size );
}
inline void *MemRaw::calloc( size_t nmemb, size_t size ){
    return ::calloc(nmemb, size);
}
inline void *MemRaw::realloc( void *ptr, size_t size ){
    return ::realloc(ptr, size);
}
/*
inline void *MemRaw::reallocarray( void *ptr, size_t nmemb, size_t size ){
    return ::reallocarray(ptr, nmemb, size);
}*/
inline void *MemRaw::aligned_alloc( size_t alignment, size_t size ){
    return ::aligned_alloc( alignment, size );
}
inline void MemRaw::free( void *ptr ){
    ::free(ptr);
}
inline void *MemRaw::page( size_t size ){
    return aligned_alloc( getpagesize(), size );
}
template<typename... Args>
void *MemRaw::malloc_e( size_t size, Args && ...args ){
    void *ptr = malloc( size );
    if( !ptr && size ){
        prt( cerr, std::forward<Args>(args)... ) << endl;
        throw ErrSystem( errno );
    }
    return ptr;
}
template<typename... Args>
void *MemRaw::calloc_e( size_t nmemb, size_t size, Args && ...args ){
    void *ptr = calloc(nmemb,size);
    if( !ptr && size && nmemb ){
        prt( cerr, std::forward<Args>(args)... ) << endl;
        throw ErrSystem( errno );
    }
    return ptr;
}
template<typename... Args>
void *MemRaw::realloc_e( void *ptr, size_t size, Args && ...args ){
    void *ptr_re = realloc(ptr, size);
    if( !ptr_re && size ){
        prt( cerr, std::forward<Args>(args)... ) << endl;
        throw ErrSystem( errno );
    }
}
/*
template<typename... Args>
void *MemRaw::reallocarray_e( void *ptr, size_t nmemb, size_t size, Args && ...args ){
    void *ptr_re = reallocarray(ptr, nmemb, size);
    if( !ptr_re && size && nmemb ){
        prt( cerr, std::forward<Args>(args)... ) << endl;
        throw ErrSystem( errno );
    }
    return ptr;
}*/
template<typename... Args>
void *MemRaw::aligned_alloc_e( size_t alignment, size_t size, Args && ...args ){
    void *ptr = aligned_alloc(alignment, size);
    if( !ptr && size ){
        prt( cerr, std::forward<Args>(args)... ) << endl;
        throw ErrSystem( errno );
    }
    return ptr;
}
template<typename T>
void MemRaw::free_e( T* &ptr ){
    free( ptr );
    ptr = nullptr;
}
template<typename... Args>
void *MemRaw::page_e( size_t size, Args && ...args ){
    return aligned_alloc_e( getpagesize(), size, std::forward<Args>(args)... );
}









} // namespace HIPP
#endif	//_HIPPCNTL_MEM_RAW_H_