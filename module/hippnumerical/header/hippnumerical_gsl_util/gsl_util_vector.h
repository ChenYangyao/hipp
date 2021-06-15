/**
create: Yangyao CHEN, 2021/01/04
    [write   ] 
    GSLVector - wrapper of gsl_vector.
*/ 

#ifndef _HIPPNUMERICAL_GSL_UTIL_VECTOR_H_
#define _HIPPNUMERICAL_GSL_UTIL_VECTOR_H_
#include "gsl_util_base.h"
#include "gsl_util_error.h"
#include <gsl/gsl_vector.h>
namespace HIPP::NUMERICAL {

class GSLBaseVector {
public:
    enum: int { 
        INIT_UNINIT = 0, INIT_ZERO = 1, 
        OWN_REF = 12, BORROW_REF = 13, COPY = 14 };
};

template<typename ValueT>
class GSLVector {
    ~GSLVector() = delete;
};

template<>
class GSLVector<double>: public GSLBaseVector {
public:
    typedef double value_t;
    typedef gsl_vector *raw_t;
    typedef const gsl_vector *craw_t;
    
    /**
     * (1)  create a new vector object with self-owned gsl_block memory.
     *      @init_as: can be
     *      - INIT_UNINIT: vector content is uninitialized.
     *      - INIT_ZERO: vector is zero out (i.e., the same as calloc).
     * (2)  host the raw_t pointer.
     *      @vec: must be a valid raw_t pointer.
     *      @ref: can be 
     *      - BORROW_REF: the new object does not free the vector on 
     *        desrtruction.
     *      - OWN_REF: free is called on destruction, i.e., owner is transferred.
     *      - COPY: deep copy the vector.
     * (3)  deep copy, i.e., by creating a new vector and copying original 
     *      content.
     * (4)  move. The new object takes over the raw_t pointer and owner state,
     *      leaving the moved object a correct but unusable state.
     */
    explicit GSLVector(size_t n, int init_as = INIT_UNINIT);
    explicit GSLVector(craw_t vec, int ref = BORROW_REF);
    GSLVector(std::initializer_list<value_t> il);
    template<typename T, typename A>
        GSLVector(const vector<T, A> &v);
    GSLVector(const GSLVector &o);
    GSLVector(GSLVector &&o) noexcept;
    ~GSLVector();

    /**
     * Copy and move assignments.
     * Copy operation is equavalent to destruct the lhs object and construct a 
     * new one by copy constructor. 
     * Move operation simply lets the lhs take over the rhs' raw_t pointer
     * and owner state, leaving rhs a correct but unusable state.
     */
    GSLVector & operator=(const GSLVector &o);
    GSLVector & operator=(GSLVector &&o) noexcept;

    value_t & operator[](size_t i);
    const value_t & operator[](size_t i) const;
    template<typename Container = vector<value_t> >
    Container to_stl() const;

    value_t get(size_t i) const;
    void set(size_t i, value_t x);
    void set_all(value_t x);
    void set_zero(value_t x);
    void set_basis(size_t i);

    void swap_vector(GSLVector &o);             // swap the content of two equal-length vectors
    void swap_elements(size_t i, size_t j);     // swap two elements
    void reverse();                             // reverse the order of elements
    value_t max() const;
    value_t min() const;
    std::pair<value_t, value_t> minmax() const;
    size_t max_index() const;
    size_t min_index() const;
    std::pair<size_t, size_t> minmax_index() const;

    /**
     * feature test operations
     */
    bool isnull() const;                        // if all zero
    bool ispos() const;                         //    all positive
    bool isneg() const;                         //    all negative
    bool isnonneg() const;                      //    all non-negative
    friend bool operator==(const GSLVector &lhs, const GSLVector &rhs);  
                                                //   *this and b are equal (by comparison of element values).

    size_t size() const noexcept;         
    size_t stride() const noexcept;       
    value_t *data() noexcept;             
    const value_t *data() const noexcept; 
    raw_t raw() const noexcept;  

    /**
     * Vector arithmetics. *this and b must be equal-length.
     * axpby: 
     * y = alpha * x + beta * y, where y = *this; 
     */
    GSLVector & operator+=(const GSLVector &b);
    GSLVector & operator-=(const GSLVector &b);
    GSLVector & operator*=(const GSLVector &b);
    GSLVector & operator/=(const GSLVector &b);
    GSLVector & operator*=(const value_t scale);
    GSLVector & operator+=(const value_t offset);
    GSLVector & axpby(const value_t alpha, 
        const GSLVector &x, const value_t beta);
private:
    raw_t _vec;
    int _own;
    void _try_free();
    void _nullify();
};

inline GSLVector<double>::GSLVector(size_t n, int init_as) 
: _own(1)
{
    if( init_as == INIT_UNINIT ){
        _vec = gsl_vector_alloc(n);
    }else if( init_as == INIT_ZERO ){
        _vec = gsl_vector_calloc(n);
    }else{
        HIPP::ErrLogic::throw_(HIPP::ErrLogic::eINVALIDARG, 
            emFLPFB, "  ... init_as illegal (", init_as, ")\n");
    }
}

inline GSLVector<double>::GSLVector(craw_t vec, int ref) 
{
    if( ref == BORROW_REF ){
        _vec = const_cast<raw_t>(vec);
        _own = 0;
    }else if( ref == OWN_REF ){
        _vec = const_cast<raw_t>(vec);
        _own = 1;
    }else if( ref == COPY ){
        _vec = gsl_vector_alloc(vec->size);
        gsl_vector_memcpy(_vec, vec);
        _own = 1;
    }else{
        HIPP::ErrLogic::throw_(HIPP::ErrLogic::eINVALIDARG, 
            emFLPFB, "  ... ref illegal (", ref, ")\n");
    }
}
inline GSLVector<double>::GSLVector(std::initializer_list<value_t> il) 
:GSLVector(il.size())
{
    size_t i = 0;
    for(auto p=il.begin(); p<il.end(); ++p){
        set(i++, *p);
    }
}
template<typename T, typename A>
GSLVector<double>::GSLVector(const vector<T, A> &v)
:GSLVector(v.size())
{
    size_t n = v.size();
    for(size_t i=0; i<n; ++i){
        set(i, v[i]);
    }
}
inline GSLVector<double>::GSLVector(const GSLVector &o) 
:GSLVector(o.size(), INIT_UNINIT)
{
    gsl_vector_memcpy(_vec, o._vec);
}

inline GSLVector<double>::GSLVector(GSLVector &&o) noexcept
:_vec(o._vec), _own(o._own)
{
    o._nullify();
}

inline GSLVector<double>::~GSLVector() {
    _try_free();
}

inline auto GSLVector<double>::operator=(const GSLVector &o) -> GSLVector & {
    if( this != &o ){
        _try_free();
        _vec = gsl_vector_alloc(o.size());
        _own = 1;
        gsl_vector_memcpy(_vec, o._vec);
    }
    return *this;
}

inline auto GSLVector<double>::operator=(GSLVector &&o) noexcept -> GSLVector & {
    using std::swap;
    swap(_vec, o._vec);
    swap(_own, o._own);
    return *this;
}

inline auto GSLVector<double>::operator[](size_t i) -> value_t & { 
    return *gsl_vector_ptr(_vec, i); 
}
inline auto GSLVector<double>::operator[](size_t i) const -> const value_t & { 
    return *gsl_vector_const_ptr(_vec, i); 
}
template<typename Container>
Container GSLVector<double>::to_stl() const
{
    Container c;
    size_t n = size();
    for(size_t i=0; i<n; ++i)
        c.push_back(get(i));
    return c;
}
inline auto GSLVector<double>::get(size_t i) const -> value_t { 
    return gsl_vector_get(_vec, i); 
}
inline void GSLVector<double>::set(size_t i, value_t x) { 
    gsl_vector_set(_vec, i, x); 
}
inline void GSLVector<double>::set_all(value_t x) { 
    gsl_vector_set_all(_vec, x); 
}
inline void GSLVector<double>::set_zero(value_t x) { 
    gsl_vector_set_zero(_vec); 
}
inline void GSLVector<double>::set_basis(size_t i) { 
    gsl_vector_set_basis(_vec, i); 
}

inline void GSLVector<double>::swap_vector(GSLVector &o) { 
    gsl_vector_swap(_vec, o._vec); 
}
inline void GSLVector<double>::swap_elements(size_t i, size_t j) { 
    gsl_vector_swap_elements(_vec, i, j); 
}
inline void GSLVector<double>::reverse() { 
    gsl_vector_reverse(_vec); 
}
inline auto GSLVector<double>::max() const -> value_t { 
    return gsl_vector_max(_vec); 
}
inline auto GSLVector<double>::min() const -> value_t { 
    return gsl_vector_min(_vec); 
}
inline auto GSLVector<double>::minmax() const -> std::pair<value_t, value_t> {
    value_t minval, maxval;
    gsl_vector_minmax(_vec, &minval, &maxval);
    return {minval, maxval};
}
inline auto GSLVector<double>::max_index() const -> size_t { 
    return gsl_vector_max_index(_vec); 
}
inline auto GSLVector<double>::min_index() const -> size_t { 
    return gsl_vector_min_index(_vec); 
}
inline auto GSLVector<double>::minmax_index() const -> std::pair<size_t, size_t> 
{
    size_t minid, maxid;
    gsl_vector_minmax_index(_vec, &minid, &maxid);
    return {minid, maxid};
}
inline bool GSLVector<double>::isnull() const {
    return gsl_vector_isnull(_vec);
}
inline bool GSLVector<double>::ispos() const {
    return gsl_vector_ispos(_vec);
}
inline bool GSLVector<double>::isneg() const {
    return gsl_vector_isneg(_vec);
}
inline bool GSLVector<double>::isnonneg() const {
    return gsl_vector_isnonneg(_vec);
}
inline bool operator==(const GSLVector<double> &lhs, 
    const GSLVector<double> &rhs) 
{
    return gsl_vector_equal(lhs._vec, rhs._vec);
}
inline auto GSLVector<double>::size() const noexcept -> size_t { 
    return _vec->size; 
}
inline auto GSLVector<double>::stride() const noexcept -> size_t { 
    return _vec->stride; 
}
inline auto GSLVector<double>::data() noexcept -> value_t * { 
    return _vec->data; 
}
inline auto GSLVector<double>::data() const noexcept -> const value_t * { 
    return _vec->data; 
}
inline auto GSLVector<double>::raw() const noexcept -> raw_t { 
    return _vec; 
}

inline auto GSLVector<double>::operator+=(const GSLVector &b) -> GSLVector & {
    gsl_vector_add(_vec, b._vec);
    return *this;
}
inline auto GSLVector<double>::operator-=(const GSLVector &b) -> GSLVector & {
    gsl_vector_sub(_vec, b._vec);
    return *this;
}
inline auto GSLVector<double>::operator*=(const GSLVector &b) -> GSLVector & {
    gsl_vector_mul(_vec, b._vec);
    return *this;
}
inline auto GSLVector<double>::operator/=(const GSLVector &b) -> GSLVector & {
    gsl_vector_div(_vec, b._vec);
    return *this;
}
inline auto GSLVector<double>::operator*=(const value_t scale) -> GSLVector & {
    gsl_vector_scale(_vec, scale);
    return *this;
}
inline auto GSLVector<double>::operator+=(const value_t offset) -> GSLVector & {
    gsl_vector_add_constant(_vec, offset);
    return *this;
}
inline auto GSLVector<double>::axpby(const value_t alpha, 
    const GSLVector &x, const value_t beta) -> GSLVector & 
{
    gsl_vector_axpby(alpha, x._vec, beta, _vec);
    return *this;
}
inline void GSLVector<double>::_try_free(){
    if( _own )
        gsl_vector_free(_vec);
}
inline void GSLVector<double>::_nullify() {
    _vec = nullptr;
    _own = 0;
}

} // namespace HIPP::NUMERICAL
#endif	//_HIPPNUMERICAL_GSL_UTIL_VECTOR_H_