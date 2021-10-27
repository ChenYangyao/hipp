/**
create: Yangyao CHEN, 2021/10/18
    [write   ] DefaultInitAllocator - allocator with default initialization
        for object.
*/

#ifndef _HIPPCNTL_CONSTRUCTOR_H_
#define _HIPPCNTL_CONSTRUCTOR_H_
#include <memory>
#include <type_traits>

namespace HIPP {

/**
See, the answer on StackOverflow by Casey - 
https://stackoverflow.com/questions/21028299/is-this-behavior-of-
vectorresizesize\-type-n-under-c11-and-boost-container/21028912#21028912.
*/
template <typename T, typename A = std::allocator<T>>
class DefaultInitAllocator : public A {
    typedef std::allocator_traits<A> a_t;
public:
    template <typename U> struct rebind {
        using other = DefaultInitAllocator<
            U, 
            typename a_t::template rebind_alloc<U> >;
    };

    using A::A;

    template <typename U>
    void construct(U* ptr)
        noexcept(std::is_nothrow_default_constructible<U>::value) {
        ::new(static_cast<void*>(ptr)) U;
    }
    template <typename U, typename...Args>
    void construct(U* ptr, Args&&... args) {
        a_t::construct(static_cast<A&>(*this), 
            ptr, std::forward<Args>(args)...);
    }
};

} // namespace HIPP

#endif	//_HIPPCNTL_CONSTRUCTOR_H_