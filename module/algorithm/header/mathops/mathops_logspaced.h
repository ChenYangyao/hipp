#ifndef _HIPPALGORITHM_MATHOPS_LOGSPACED_H_
#define _HIPPALGORITHM_MATHOPS_LOGSPACED_H_
#include "mathops_base.h"
namespace HIPP::ALGORITHM {

template<typename T>
class LogSpaced {
public:
    typedef std::ptrdiff_t diff_t;
    typedef T value_t;

    LogSpaced(value_t start, diff_t n, value_t stride = value_t(1)) noexcept;

    template<typename It>
    const LogSpaced & operator>>(It b) const;

    template<typename Container>
    const LogSpaced & get_to(Container &c) const;

    template<typename Container = vector<value_t> >
    Container get() const;
protected:
    value_t _start, _stride;
    diff_t _n;
};

#define _HIPP_TEMPHD template<typename T>
#define _HIPP_TEMPARG LogSpaced<T>::

_HIPP_TEMPHD
_HIPP_TEMPARG LogSpaced(value_t start, diff_t n, value_t stride ) noexcept 
:_start(start), _stride(stride), _n(n)
{}

_HIPP_TEMPHD 
template<typename It>
auto _HIPP_TEMPARG operator>>(It b) const -> const LogSpaced & {
    value_t cur_val = std::pow(10.0, _start), inc = std::pow(10.0, _stride);
    for(diff_t i=0; i<_n; ++i){
        *b++ = cur_val;
        cur_val *= inc;
    }
    return *this;
}

_HIPP_TEMPHD 
template<typename Container>
auto _HIPP_TEMPARG get_to(Container &c) const -> const LogSpaced & {
    c.clear();
    value_t cur_val = std::pow(10.0, _start), inc = std::pow(10.0, _stride);
    for(diff_t i=0; i<_n; ++i){
        c.push_back( cur_val );
        cur_val *= inc;
    }
    return *this;
}

_HIPP_TEMPHD 
template<typename Container>
Container _HIPP_TEMPARG get() const {
    Container c;
    get_to<Container>(c);
    return c;
}

#undef _HIPP_TEMPHD
#undef _HIPP_TEMPARG
    
} // namespace HIPP::ALGORITHM
#endif	//_HIPPALGORITHM_MATHOPS_LOGSPACED_H_