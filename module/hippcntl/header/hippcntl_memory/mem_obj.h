#ifndef _HIPPCNTL_MEM_OBJ_H_
#define _HIPPCNTL_MEM_OBJ_H_
#include "mem_raw.h"
#include <memory>
namespace HIPP{

template<typename T>
class MemObj{
public:
    typedef std::allocator<T> allocator_t;
    typedef typename allocator_t::value_type value_t;
    typedef typename allocator_t::pointer ptr_t;
    typedef typename allocator_t::size_type size_t;
    typedef typename allocator_t::difference_type ptrdiff_t;

    /**
     * intrinsic properties of value_t.
     * size:        sizeof(T).
     * alignment:   alignof(T).
     */    
    static const size_t 
        size = sizeof( value_t ), alignment = alignof( value_t );

    template<typename ...Args>
    static ptr_t alloc( size_t n, Args && ...args );
    template<typename ...Args>
    static ptr_t alloc_c( size_t n, Args && ...args );
    template<typename ...Args>
    static ptr_t alloc_r( ptr_t ptr, size_t n, Args && ...args );
    template<typename ...Args>
    static ptr_t alloc_a( size_t n, size_t n_align, Args && ...args );
#ifdef _HIPP_SYS_SPEC_POSIX
    template<typename ...Args>
    static ptr_t alloc_p( size_t n, Args && ...args );
#endif
    static void dealloc( ptr_t &ptr );

    template<typename ...Args>
    void construct( ptr_t ptr, Args && ...args );
    template<typename ...Args>
    void construct_n( ptr_t ptr, size_t n, Args && ...args );
    template<typename InputIt>
    static void copy( InputIt b, InputIt e, ptr_t ptr );
    template<typename InputIt>
    static void copy_n( InputIt b, size_t n, ptr_t ptr);

    void destroy( ptr_t ptr );
    void destroy_n( ptr_t ptr, size_t n );
protected:
    allocator_t _allocator;
    static size_t _roundup( size_t x, size_t base )
        { return (x+base-1)/base; }
    static size_t _roundup_as_multiple( size_t x, size_t base)
        { return _roundup(x, base)*base; }
};  

template<typename T>
template<typename ...Args>
typename MemObj<T>::ptr_t MemObj<T>::alloc( size_t n, Args && ...args ){
    return static_cast<ptr_t>( 
        MemRaw::malloc_e( size*n, std::forward<Args>(args)... ) );
}

template<typename T>
template<typename ...Args>
typename MemObj<T>::ptr_t MemObj<T>::alloc_c( size_t n, Args && ...args ){
    return static_cast<ptr_t>( 
        MemRaw::calloc_e( n, size, std::forward<Args>(args)... ) );
}

template<typename T>
template<typename ...Args>
typename MemObj<T>::ptr_t MemObj<T>::alloc_r( ptr_t ptr, size_t n, Args && ...args ){
    return static_cast<ptr_t>( 
        MemRaw::realloc_e( ptr, n*size, std::forward<Args>(args)... ) );
}
template<typename T>
template<typename ...Args>
typename MemObj<T>::ptr_t MemObj<T>::alloc_a( size_t n, size_t n_align, 
    Args && ...args )
{
    if( n_align == 0 )
        ErrLogic::throw_( ErrLogic::eDOMAIN, emFLPFB, 
            "  ... n_align cannot be 0" );
    size_t _align = alignment * n_align, _size = size * n;
    return static_cast<ptr_t>( 
        MemRaw::aligned_alloc_e( _align, 
            _roundup_as_multiple(_size, _align), 
            std::forward<Args>(args)... ) );
}
#ifdef _HIPP_SYS_SPEC_POSIX
template<typename T>
template<typename ...Args>
typename MemObj<T>::ptr_t MemObj<T>::alloc_p( size_t n, Args && ...args ){
    size_t _pagesize = MemRaw::pagesize(), _size = n*size;
    return static_cast<ptr_t>(
        MemRaw::page_e( _roundup(_size, _pagesize), 
        std::forward<Args>(args)... ) );
}
#endif

template<typename T>
void MemObj<T>::dealloc( ptr_t &ptr ){
    MemRaw::free_e(ptr);
}
template<typename T>
template<typename ...Args>
void MemObj<T>::construct( ptr_t ptr, Args && ...args ){
    _allocator.construct( ptr, std::forward<Args>(args)... );
}
template<typename T>
template<typename ...Args>
void MemObj<T>::construct_n( ptr_t ptr, size_t n, Args && ...args ){
    auto e = ptr + n;
    while( ptr != e )
        construct( ptr++, std::forward<Args>(args)... );
}
template<typename T>
template<typename InputIt>
void MemObj<T>::copy( InputIt b, InputIt e, ptr_t ptr ){
    std::uninitialized_copy( b, e, ptr );
}
template<typename T>
template<typename InputIt>
void MemObj<T>::copy_n( InputIt b, size_t n, ptr_t ptr){
    std::uninitialized_copy_n( b, n, ptr );
}
template<typename T>
void MemObj<T>::destroy( ptr_t ptr ){
    _allocator.destroy(ptr);
}
template<typename T>
void MemObj<T>::destroy_n( ptr_t ptr, size_t n ){
    auto e = ptr + n;
    while( ptr != e )
        destroy( ptr++ );
}




} // namespace HIPP
#endif	//_HIPPCNTL_MEM_OBJ_H_