/**
 * creat: Yangyao CHEN, 2019/12/28
 *      [write   ] MemRaw - raw (un-constructed) memory handlers
 */ 

#ifndef _HIPPCNTL_MEM_RAW_H_
#define _HIPPCNTL_MEM_RAW_H_
#include <stdlib.h>
#include <unistd.h>
#include "../hippcntl_stream/stream.h"
#include "../hippcntl_error/error.h"
namespace HIPP{

/**
 * raw (un-constructed) memory handlers.
 */
class MemRaw {
public:
    typedef std::size_t size_t;
    typedef void *ptr_t;
    /**
     * C-style raw (un-constructed) memory managements.
     * One-to-one corresponds to GNU-C library functions.
     * 
     * The aligned_alloc requires C++17.
     */
    static ptr_t malloc( size_t size );
    static ptr_t calloc( size_t nmemb, size_t size );
    static ptr_t realloc( ptr_t ptr, size_t size );
    static ptr_t aligned_alloc( size_t alignment, size_t size );
    static void free( ptr_t ptr );

#ifdef _HIPP_SYS_SPEC_POSIX
    /**
     * allocate n page
     * Equavalent to aligned_alloc( getpagesize(), n*getpagesize() ).
     */
    static ptr_t page( size_t n );
    /**
     * Return the page size. Only works in a POSIX system.
     */ 
    static size_t pagesize() noexcept;
#endif

    /**
     * error-safe versions.
     * If any error raised in an allocation function, print `args` to std::cerr
     * and throw an ErrSytem.
     * free_e() is the same as free(), but set the entry ptr to nullptr.
     */
    template<typename... Args>
    static ptr_t malloc_e( size_t size, Args && ...args );
    template<typename... Args>
    static ptr_t calloc_e( size_t nmemb, size_t size, Args && ...args );
    template<typename... Args>
    static ptr_t realloc_e( ptr_t ptr, size_t size, Args && ...args );
    template<typename... Args>
    static ptr_t aligned_alloc_e( 
        size_t alignment, size_t size, Args && ...args );
    template<typename T>
    static void free_e( T* &ptr );

#ifdef _HIPP_SYS_SPEC_POSIX
    template<typename... Args>
    static ptr_t page_e( size_t n, Args && ...args );
#endif 
};
    

inline MemRaw::ptr_t MemRaw::malloc( size_t size ){
    return ::malloc( size );
}
inline MemRaw::ptr_t MemRaw::calloc( size_t nmemb, size_t size ){
    return ::calloc(nmemb, size);
}
inline MemRaw::ptr_t MemRaw::realloc( ptr_t ptr, size_t size ){
    return ::realloc(ptr, size);
}
inline MemRaw::ptr_t MemRaw::aligned_alloc( size_t alignment, size_t size ){
    return ::aligned_alloc(alignment, size);
}
inline void MemRaw::free( ptr_t ptr ){
    ::free(ptr);
}

#ifdef _HIPP_SYS_SPEC_POSIX
inline MemRaw::ptr_t MemRaw::page( size_t n ){
    size_t _pagesize = pagesize();
    return aligned_alloc( _pagesize, n*_pagesize );
}
inline MemRaw::size_t MemRaw::pagesize() noexcept{
    return static_cast<size_t>( getpagesize() );
}
#endif

template<typename... Args>
MemRaw::ptr_t MemRaw::malloc_e( size_t size, Args && ...args ){
    ptr_t ptr = malloc( size );
    if( !ptr && size )
        ErrSystem::throw_( errno, std::forward<Args>(args)... );
    return ptr;
}
template<typename... Args>
MemRaw::ptr_t MemRaw::calloc_e( size_t nmemb, size_t size, Args && ...args ){
    ptr_t ptr = calloc(nmemb,size);
    if( !ptr && size && nmemb ){
        ErrSystem::throw_( errno, std::forward<Args>(args)... );
    }
    return ptr;
}
template<typename... Args>
MemRaw::ptr_t MemRaw::realloc_e( ptr_t ptr, size_t size, Args && ...args ){
    ptr_t ptr_re = realloc(ptr, size);
    if( !ptr_re && size ){
        ErrSystem::throw_( errno, std::forward<Args>(args)... );
    }
    return ptr_re;
}
template<typename... Args>
MemRaw::ptr_t MemRaw::aligned_alloc_e( size_t alignment, size_t size, 
    Args && ...args )
{
    ptr_t ptr = aligned_alloc(alignment, size);
    if( !ptr && size ){
        ErrSystem::throw_( errno, std::forward<Args>(args)... );
    }
    return ptr;
}
template<typename T>
void MemRaw::free_e( T* &ptr ){
    free( ptr );
    ptr = nullptr;
}
#ifdef _HIPP_SYS_SPEC_POSIX
template<typename... Args>
MemRaw::ptr_t MemRaw::page_e( size_t n, Args && ...args ){
    size_t _pagesize = pagesize();
    return aligned_alloc_e( _pagesize, _pagesize*n, 
        std::forward<Args>(args)... );
}
#endif









} // namespace HIPP
#endif	//_HIPPCNTL_MEM_RAW_H_