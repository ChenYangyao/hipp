/**
create: Yangyao CHEN, 2021/08/18
    [write   ] ExponentialRandomNumber - RNG for exponential distribution.
*/

#ifndef _HIPPNUMERICAL_RANNUM_EXPONENTIAL_H_
#define _HIPPNUMERICAL_RANNUM_EXPONENTIAL_H_
#include "rannum_base.h"

namespace HIPP::NUMERICAL {

/**
Non-negative floating-point value x, distributed according to the exponential
probability density function:
    P(x | lambda) = lambda exp(-lambda x),
This is the continuous counterpart of geometric distribution.
*/

template<typename RealT = double, 
    typename EngineT = DefaultRandomEngine>
class ExponentialRandomNumber {
public:
    typedef RealT result_t;
    typedef EngineT engine_t;
    typedef std::exponential_distribution<result_t> rng_t;
    typedef typename rng_t::param_type param_t;
    typedef typename engine_t::result_type seed_t;

    /**
    @lambda: parameter of exponential distribution. Instead a ``param_t`` can
        be passed.
    @engine: an engine of random number. A pointer is passed, and its lifetime
        is controlled by the user.
    */
    explicit ExponentialRandomNumber(
        result_t lambda = 1.0, 
        engine_t *engine = &global_random_engine);
    explicit ExponentialRandomNumber(const param_t &param, 
        engine_t *engine = &global_random_engine);
    ~ExponentialRandomNumber();

    /** 
    The copy for parameter is deep, but engine is shared. 
    */
    ExponentialRandomNumber(const ExponentialRandomNumber &o);
    ExponentialRandomNumber(ExponentialRandomNumber &&o);
    ExponentialRandomNumber & operator=(const ExponentialRandomNumber &o);
    ExponentialRandomNumber & operator=(ExponentialRandomNumber &&o);

    /**
    Setters.
    reset_state(): reset the internal state so that the subsequent random 
        numbers do not depend on the previous ones.
    reset_param(): reset the parameters.
    reset_engine(): reset the random number engine. Note that its life-time 
        is controlled by the user.
    seed(): reseed the random engine.
    */
    ExponentialRandomNumber & reset_state();
    ExponentialRandomNumber & reset_param(result_t lambda = 1.0);
    ExponentialRandomNumber & reset_param(const param_t &param);
    ExponentialRandomNumber & reset_engine(engine_t *engine);
    ExponentialRandomNumber & seed(seed_t seed = engine_t::default_seed);

    /**
    Geters.
    lambda() : the parameter of the distribution.
    param() : the packet of ``lambda``.
    min(), max() : minimum and maximum value that can be returned by the random
        number generator.
    range() : the pair {min(), max()}.
    engine() : the random engine. 
    rng() : the internal random number generator.
    */
    result_t lambda() const;
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
    2. using new parameter ``param``.
    3. using new parameter ``lambda``.
    */
    result_t operator()();
    result_t get(const param_t &param);
    result_t get(result_t lambda);

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
#define _HIPP_TEMPARG <RealT, EngineT>
#define _HIPP_TEMPCLS ExponentialRandomNumber _HIPP_TEMPARG
#define _HIPP_TEMPRET _HIPP_TEMPHD inline auto _HIPP_TEMPCLS::
#define _HIPP_TEMPNORET _HIPP_TEMPHD inline _HIPP_TEMPCLS::

_HIPP_TEMPNORET
ExponentialRandomNumber(result_t lambda, engine_t *engine) 
: _engine(engine), _rng(lambda)
{ }

_HIPP_TEMPNORET
ExponentialRandomNumber(const param_t &param, engine_t *engine)
: _engine(engine), _rng(param)
{ }

_HIPP_TEMPNORET
~ExponentialRandomNumber() 
{ }

_HIPP_TEMPNORET
ExponentialRandomNumber(const ExponentialRandomNumber &o) 
:_engine(o.engine()), _rng(o.param()) 
{ }

_HIPP_TEMPNORET
ExponentialRandomNumber(ExponentialRandomNumber &&o) 
:_engine(o.engine()), _rng(o.param()) 
{}

_HIPP_TEMPRET operator=(const ExponentialRandomNumber &o)
-> ExponentialRandomNumber &
{
    _engine = o.engine();
    _rng.param(o.param());
    return *this;
}

_HIPP_TEMPRET operator=(ExponentialRandomNumber &&o)
-> ExponentialRandomNumber &
{
    _engine = o.engine();
    _rng.param(o.param());
    return *this;
}

_HIPP_TEMPRET reset_state()
-> ExponentialRandomNumber &
{
    _rng.reset();
    return *this;
}

_HIPP_TEMPRET reset_param(result_t lambda)
-> ExponentialRandomNumber &
{
    return reset_param( param_t(lambda) );
}

_HIPP_TEMPRET reset_param(const param_t &param)
-> ExponentialRandomNumber &
{
    _rng.param( param );
    return *this;
}

_HIPP_TEMPRET reset_engine(engine_t *engine)
-> ExponentialRandomNumber &
{
    _engine = engine;
    return *this;
}

_HIPP_TEMPRET seed(seed_t seed)
-> ExponentialRandomNumber &
{
    _engine->seed(seed);
    return *this;
}

_HIPP_TEMPRET lambda() const -> result_t {
    return _rng.lambda();
}

_HIPP_TEMPRET param() const -> param_t {
    return _rng.param();
}

_HIPP_TEMPRET min() const -> result_t {
    return _rng.min();
}

_HIPP_TEMPRET max() const -> result_t {
    return _rng.max();
}

_HIPP_TEMPRET range() const -> std::pair<result_t, result_t> {
    return {min(), max()};
}

_HIPP_TEMPRET engine() const -> engine_t * {
    return _engine;
}

_HIPP_TEMPRET rng() -> rng_t & {
    return _rng;
}

_HIPP_TEMPRET rng() const -> const rng_t & {
    return _rng;
}

_HIPP_TEMPRET operator()() -> result_t {
    return _rng(*_engine);
}

_HIPP_TEMPRET get(const param_t &param) -> result_t {
    return _rng(*_engine, param);
}

_HIPP_TEMPRET get(result_t lambda) -> result_t {
    return get( param_t(lambda) );
}

_HIPP_TEMPHD
template<template<typename> typename Container>
auto _HIPP_TEMPCLS::operator()(std::size_t n) -> Container<result_t> {
    Container<result_t> c;
    (*this)(c, n);
    return c;
}
_HIPP_TEMPHD
template<typename Container> 
void _HIPP_TEMPCLS::operator()(Container &c, std::size_t n) {
    for(std::size_t i=0; i<n; ++i) c.push_back( (*this)() );
}

_HIPP_TEMPHD
template<typename InputIt> 
void _HIPP_TEMPCLS::operator()(InputIt b, InputIt e) {
    while( b != e ) *b++ = (*this)();
}

#undef _HIPP_TEMPHD
#undef _HIPP_TEMPARG
#undef _HIPP_TEMPCLS
#undef _HIPP_TEMPRET
#undef _HIPP_TEMPNORET


} // namespace HIPP::NUMERICAL

#endif	//_HIPPNUMERICAL_RANNUM_EXPONENTIAL_H_