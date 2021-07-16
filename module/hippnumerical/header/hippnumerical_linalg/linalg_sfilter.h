/**
create: Yangyao CHEN, 2021/07/14
    [write   ] 
    SFilter - a static/small vector filter.
    SBoolFilter - filter using Boolean array.
*/

#ifndef _HIPPNUMERICAL_LINALG_SFILTER_H_
#define _HIPPNUMERICAL_LINALG_SFILTER_H_
#include "linalg_base.h"
#include "linalg_svec.h"
namespace HIPP::NUMERICAL {

class SFilter {};

/**
SBoolFilter - Filter that selects elements using static Boolean array.
*/
template<size_t N>
class SBoolFilter : public SFilter {
public:
    typedef SVec<bool, N> mask_t;
    inline static constexpr size_t SIZE = mask_t::SIZE;

    /** 
    Constructors.
    (1) select all.
    (2) select all or none, if value is true or false, respectively.
    (3) use a Boolean SVec to select.
    */
    SBoolFilter() noexcept;
    explicit SBoolFilter(bool value) noexcept;
    explicit SBoolFilter(const mask_t &mask) noexcept;

    SBoolFilter(const SBoolFilter &) noexcept = default;
    SBoolFilter(SBoolFilter &&) noexcept = default;
    SBoolFilter & operator=(const SBoolFilter &) noexcept = default;
    SBoolFilter & operator=(SBoolFilter &&) noexcept = default;
    ~SBoolFilter() noexcept;
    friend void swap(SBoolFilter &lhs, SBoolFilter &rhs) noexcept               { swap(lhs._mask, rhs._mask); }
    
    /** Convert to a boolean filter. For SBoolFilter, returns a identical copy. */
    SBoolFilter to_bool() const noexcept;

    /**
    Print the object to `os`.
    operator<<  - a inline, short description.
    info        - controlled by `fmt_cntl`.
                  0 for a inline, short description; 
                  1 for a detailed block version.
    */
    friend ostream & operator<< (ostream &os, const SBoolFilter &f)             { PStream ps(os); ps << "SBoolFilter{", ps(f._mask.begin(), f._mask.end()), "}"; return os; }
    ostream & info(ostream &os=cout, int fmt_cntl=1) const;

    
    /** Return true or false, if i-th element is or is not selected. */
    bool operator[](size_t i) noexcept;
    bool operator[](size_t i) const noexcept;

    /** Get the mask array. */
    mask_t & mask() noexcept;
    const mask_t & mask() const noexcept;

    /** Element-wise and bit-wise operations. */
    SBoolFilter & operator&=(bool rhs) noexcept;
    SBoolFilter & operator|=(bool rhs) noexcept;
    SBoolFilter & operator^=(bool rhs) noexcept;

    SBoolFilter & operator&=(const SBoolFilter &rhs) noexcept;
    SBoolFilter & operator|=(const SBoolFilter &rhs) noexcept;
    SBoolFilter & operator^=(const SBoolFilter &rhs) noexcept;

    friend SBoolFilter operator& (const SBoolFilter &lhs, 
        const SBoolFilter &rhs) noexcept                                        { return SBoolFilter(lhs._mask & rhs._mask); }
    friend SBoolFilter operator| (const SBoolFilter &lhs, 
        const SBoolFilter &rhs) noexcept                                        { return SBoolFilter(lhs._mask | rhs._mask); }
    friend SBoolFilter operator^ (const SBoolFilter &lhs, 
        const SBoolFilter &rhs) noexcept                                        { return SBoolFilter(lhs._mask ^ rhs._mask); }
    
    friend SBoolFilter operator& (bool lhs, 
        const SBoolFilter &rhs) noexcept                                        { return SBoolFilter(lhs & rhs._mask); }
    friend SBoolFilter operator| (bool lhs, 
        const SBoolFilter &rhs) noexcept                                        { return SBoolFilter(lhs | rhs._mask); }
    friend SBoolFilter operator^ (bool lhs, 
        const SBoolFilter &rhs) noexcept                                        { return SBoolFilter(lhs ^ rhs._mask); }
    
    friend SBoolFilter operator& (const SBoolFilter &lhs, 
        bool rhs) noexcept                                                      { return SBoolFilter(lhs._mask & rhs); }
    friend SBoolFilter operator| (const SBoolFilter &lhs, 
        bool rhs) noexcept                                                      { return SBoolFilter(lhs._mask | rhs); }
    friend SBoolFilter operator^ (const SBoolFilter &lhs, 
        bool rhs) noexcept                                                      { return SBoolFilter(lhs._mask ^ rhs); }

    /**
    Visit all selected elements, i.e., for each index `size_t i` of selected 
    element, call 
        op(i);
    The second version call binary 
        op(i, b[i]);
    */
    template<typename UnaryOp>
    void visit(UnaryOp op) const;
    template<typename BinaryOp, typename RandomAccessIt>
    void visit(BinaryOp op, RandomAccessIt b) const;

    /**
    For each element e indexed i, call 
        op(size_t i, bool &e);
    The const version use 
        op(size_t i, const bool &e).
    */
    template<typename BinaryOp>
    void for_each(BinaryOp op);
    template<typename BinaryOp>
    void for_each(BinaryOp op) const;
protected:
    mask_t _mask;
};

#define _HIPP_TEMPHD \
    template<size_t N>
#define _HIPP_TEMPARG \
    <N>
#define _HIPP_TEMPRET \
    _HIPP_TEMPHD \
    inline auto SBoolFilter _HIPP_TEMPARG::
#define _HIPP_TEMPCLS \
    SBoolFilter _HIPP_TEMPARG

_HIPP_TEMPHD 
_HIPP_TEMPCLS::SBoolFilter() noexcept
: _mask(true) {}

_HIPP_TEMPHD 
_HIPP_TEMPCLS::SBoolFilter(bool value) noexcept 
: _mask(value) {}

_HIPP_TEMPHD 
_HIPP_TEMPCLS::SBoolFilter(const mask_t &mask) noexcept
: _mask(mask)  {}

_HIPP_TEMPHD 
_HIPP_TEMPCLS::~SBoolFilter() noexcept {}

_HIPP_TEMPRET info(ostream &os, int fmt_cntl) const -> ostream &  {
    PStream ps(os);
    if(fmt_cntl == 0) {
        ps << HIPPCNTL_CLASS_INFO_INLINE(SBoolFilter), 
            "{", ps(_mask.cbegin(), _mask.cend()), "}";
    }else {
        ps << HIPPCNTL_CLASS_INFO(SBoolFilter), 
            "  |- size = ", SIZE, "\n"
            "  |- values = {", ps(_mask.cbegin(), _mask.cend()), "}\n";
    }
    return os;
}

_HIPP_TEMPRET operator[](size_t i) noexcept -> bool {
    return _mask[i];
}

_HIPP_TEMPRET operator[](size_t i) const noexcept -> bool {
    return _mask[i];
}

_HIPP_TEMPRET to_bool() const noexcept -> SBoolFilter {
    return *this;
}

_HIPP_TEMPRET mask() noexcept -> mask_t & {
    return _mask;
}

_HIPP_TEMPRET mask() const noexcept -> const mask_t & {
    return _mask;
}

_HIPP_TEMPRET operator&=(bool rhs) noexcept -> SBoolFilter & { 
    _mask &= rhs; 
    return *this; 
}

_HIPP_TEMPRET operator|=(bool rhs) noexcept -> SBoolFilter & { 
    _mask |= rhs; 
    return *this; 
}

_HIPP_TEMPRET operator^=(bool rhs) noexcept -> SBoolFilter & { 
    _mask ^= rhs; 
    return *this; 
}

_HIPP_TEMPRET operator&=(const SBoolFilter &rhs) noexcept -> SBoolFilter & { 
    _mask &= rhs._mask; 
    return *this; 
}

_HIPP_TEMPRET operator|=(const SBoolFilter &rhs) noexcept -> SBoolFilter & { 
    _mask |= rhs._mask; 
    return *this; 
}

_HIPP_TEMPRET operator^=(const SBoolFilter &rhs) noexcept -> SBoolFilter & { 
    _mask ^= rhs._mask; 
    return *this; 
}

_HIPP_TEMPHD 
template<typename UnaryOp>
void _HIPP_TEMPCLS::visit(UnaryOp op) const {
    for(size_t i=0; i<SIZE; ++i) 
        if(_mask[i]) op(i);  
}

_HIPP_TEMPHD 
template<typename BinaryOp, typename RandomAccessIt>
void _HIPP_TEMPCLS::visit(BinaryOp op, RandomAccessIt b) const {
    for(size_t i=0; i<SIZE; ++i) 
        if(_mask[i]) op(i, b[i]); 
}

_HIPP_TEMPHD 
template<typename BinaryOp>
void _HIPP_TEMPCLS::for_each(BinaryOp op) {
    _mask.visit(op);  
}

_HIPP_TEMPHD 
template<typename BinaryOp>
void _HIPP_TEMPCLS::for_each(BinaryOp op) const {
    _mask.visit(op);  
}

#undef _HIPP_TEMPHD
#undef _HIPP_TEMPARG
#undef _HIPP_TEMPRET
#undef _HIPP_TEMPCLS

} // namespace HIPP::NUMERICAL
#endif	//_HIPPNUMERICAL_LINALG_SFILTER_H_