/**
create: Yangyao CHEN, 2020/12/22
    [write   ] 
    GaussianRandomNumber<> - generator of random numbers for normal 
        distribution.
*/ 

#ifndef _HIPPNUMERICAL_RANNUM_GAUSSIAN_H_
#define _HIPPNUMERICAL_RANNUM_GAUSSIAN_H_
#include "rannum_base.h"
namespace HIPP::NUMERICAL {

/**
GaussianRandomNumber - generator of Normal Distribution random numbers.
*/
template<typename RealT = double, 
    typename EngineT = DefaultRandomEngine>
class GaussianRandomNumber {
public:
    typedef RealT result_t;
    typedef EngineT engine_t;
    typedef std::normal_distribution<result_t> rng_t;
    typedef typename rng_t::param_type param_t;
    typedef typename engine_t::result_type seed_t;

    /**
    @mean, stddev: parameters of normal distribution. Instead a ``param_t`` can
        be passed.
    @engine: an engine of random number. A pointer is passed, and its lifetime
        is controlled by the user.
    */
    explicit GaussianRandomNumber(
        result_t mean = 0., result_t stddev = 1., 
        engine_t *engine = &global_random_engine);
    explicit GaussianRandomNumber(const param_t &param, 
        engine_t *engine = &global_random_engine);
    ~GaussianRandomNumber();

    /** 
    The copy for parameter is deep, but engine is shared. 
    */
    GaussianRandomNumber(const GaussianRandomNumber &o);
    GaussianRandomNumber(GaussianRandomNumber &&o);
    GaussianRandomNumber & operator=(const GaussianRandomNumber &o);
    GaussianRandomNumber & operator=(GaussianRandomNumber &&o);
    
    /**
    Setters.
    reset_state(): reset the internal state so that the subsequent random 
        numbers do not depend on the previous ones.
    reset_param(): reset the parameters.
    reset_engine(): reset the random number engine. Note that its life-time 
        is controlled by the user.
    seed(): reseed the random engine.
    */
    GaussianRandomNumber & reset_state();
    GaussianRandomNumber & reset_param(
        result_t mean = 0., result_t stddev = 1.);
    GaussianRandomNumber & reset_param(
        const param_t &param);
    GaussianRandomNumber & reset_engine(engine_t *engine);
    GaussianRandomNumber & seed(seed_t seed = engine_t::default_seed);

    /**
    Geters.
    mean(), stddev() : the parameters of the distribution.
    param() : the packet of ``mean`` and ``stddev``.
    min(), max() : minimum and maximum value that can be returned by the random
        number generator.
    range() : the pair {min(), max()}.
    engine() : the random engine. 
    rng() : the internal random number generator.
    */
    result_t mean() const;
    result_t stddev() const;
    param_t param() const;
    result_t min() const;
    result_t max() const;
    std::pair<result_t, result_t> range() const;
    engine_t * engine() const;
    rng_t & rng();
    const rng_t & rng() const;

    /**
    Get a single random number.

    1. using current parameters.
    2. using new parameters ``param``.
    3. using new parameters ``mean`` and ``stddev``.
    */
    result_t operator()();
    result_t get(const param_t &param);
    result_t get(result_t mean, result_t stddev);
    
    /**
    Get a series of random numbers.

    1. Get ``n`` numbers, returned a container (must have push back).
    2. Directly pushing back into container ``c`` (``clear()`` is not called).
    3. Write into a range specified by a pair of iterator.
    */
    template<template<typename> typename Container = std::vector>
    Container<result_t> operator()(std::size_t n);

    template<typename Container> 
    void operator()(Container &c, std::size_t n);

    template<typename InputIt> 
    void operator()(InputIt b, InputIt e);
protected:
    engine_t *_engine;
    rng_t _rng;
};


#define _HIPP_TEMPHD template<typename RealT, typename EngineT>
#define _HIPP_TEMPARG GaussianRandomNumber<RealT, EngineT>::

_HIPP_TEMPHD
_HIPP_TEMPARG GaussianRandomNumber(result_t mean, result_t stddev, 
    engine_t *engine) 
:_engine(engine), _rng(mean, stddev) {}

_HIPP_TEMPHD
_HIPP_TEMPARG GaussianRandomNumber(const param_t &param, 
    engine_t *engine)
:_engine(engine), _rng(param){}

_HIPP_TEMPHD
_HIPP_TEMPARG ~GaussianRandomNumber(){}

_HIPP_TEMPHD
_HIPP_TEMPARG GaussianRandomNumber(const GaussianRandomNumber &o)
:_engine(o.engine()), _rng(o.param()) { }

_HIPP_TEMPHD
_HIPP_TEMPARG GaussianRandomNumber(GaussianRandomNumber &&o)
:_engine(o.engine()), _rng(o.param()) { }

_HIPP_TEMPHD
auto _HIPP_TEMPARG operator=(const GaussianRandomNumber &o)
-> GaussianRandomNumber &
{
    _engine = o.engine();
    _rng.param(o.param());
    return *this;
}

_HIPP_TEMPHD
auto _HIPP_TEMPARG operator=(GaussianRandomNumber &&o) 
-> GaussianRandomNumber &
{
    _engine = o.engine();
    _rng.param(o.param());
    return *this;
}

_HIPP_TEMPHD
auto _HIPP_TEMPARG reset_state() 
-> GaussianRandomNumber &
{
    _rng.reset();
    return *this;
}

_HIPP_TEMPHD
auto _HIPP_TEMPARG reset_param(const param_t &param)
-> GaussianRandomNumber &
{
    _rng.param(param);
    return *this;
}

_HIPP_TEMPHD
auto _HIPP_TEMPARG reset_param(result_t mean, result_t stddev) 
-> GaussianRandomNumber &
{
    _rng.param( param_t(mean, stddev) );
    return *this;
}

_HIPP_TEMPHD
auto _HIPP_TEMPARG reset_engine(engine_t *engine) 
-> GaussianRandomNumber &
{ 
    _engine = engine; 
    return *this;    
}

_HIPP_TEMPHD
auto _HIPP_TEMPARG seed(seed_t seed)
-> GaussianRandomNumber & 
{
    _engine->seed(seed);
    return *this;
}

_HIPP_TEMPHD
auto _HIPP_TEMPARG mean() const -> result_t { return _rng.mean(); }

_HIPP_TEMPHD
auto _HIPP_TEMPARG stddev() const -> result_t { return _rng.stddev(); }

_HIPP_TEMPHD
auto _HIPP_TEMPARG param() const -> param_t {
    return _rng.param();
}
_HIPP_TEMPHD
auto _HIPP_TEMPARG min() const -> result_t {
    return _rng.min();
}
_HIPP_TEMPHD
auto _HIPP_TEMPARG max() const -> result_t {
    return _rng.max();
}
_HIPP_TEMPHD
auto _HIPP_TEMPARG range() const -> std::pair<result_t, result_t> {
    return {min(), max()};
}
_HIPP_TEMPHD
auto _HIPP_TEMPARG engine() const -> engine_t * { return _engine; }

_HIPP_TEMPHD
auto _HIPP_TEMPARG rng() -> rng_t & { return _rng; }

_HIPP_TEMPHD
auto _HIPP_TEMPARG rng() const -> const rng_t & { return _rng; }

_HIPP_TEMPHD
auto _HIPP_TEMPARG operator()() -> result_t { return _rng(*_engine); }


_HIPP_TEMPHD
auto _HIPP_TEMPARG get(const param_t &param) -> result_t { 
    return _rng( *_engine, param ); 
}

_HIPP_TEMPHD
auto _HIPP_TEMPARG get(result_t mean, result_t stddev) -> result_t { 
    return get(param_t(mean, stddev));
}

_HIPP_TEMPHD
template<template<typename> typename Container>
auto _HIPP_TEMPARG operator()(std::size_t n) -> Container<result_t> {
    Container<result_t> c;
    (*this)(c, n);
    return c;
}

_HIPP_TEMPHD
template<typename Container> 
void _HIPP_TEMPARG operator()(Container &c, std::size_t n) {
    for(std::size_t i=0; i<n; ++i) c.push_back( _rng(*_engine) );
}

_HIPP_TEMPHD
template<typename InputIt> void _HIPP_TEMPARG operator()(InputIt b, InputIt e) {
    while( b != e ) *b++ = _rng(*_engine);
}

#undef _HIPP_TEMPHD
#undef _HIPP_TEMPARG

} // namespace HIPP::NUMERICAL
#endif	//_HIPPNUMERICAL_RANNUM_GAUSSIAN_H_