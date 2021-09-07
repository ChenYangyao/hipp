/**
create: Yangyao CHEN, 2021/08/18
    [write   ] PoissonRandomNumber - generator of Poisson Distribution 
        random number.
*/

#ifndef _HIPPNUMERICAL_RANNUM_POISSON_H_
#define _HIPPNUMERICAL_RANNUM_POISSON_H_

#include "rannum_base.h"

namespace HIPP::NUMERICAL {

/**
PoissonRandomNumber - generator of Poisson Distribution random number.
It produces non-negative integer value i, distributed according to the 
probability function:
    P(i | mu) = exp(-mu) mu^i / i !,
where ``mu`` is the mean number.
*/
template<typename IntT = int, 
    typename EngineT = DefaultRandomEngine>
class PoissonRandomNumber {
public:
    typedef IntT result_t;
    typedef EngineT engine_t;
    typedef std::poisson_distribution<result_t> rng_t;
    typedef typename rng_t::param_type param_t;
    typedef typename engine_t::result_type seed_t;

    /**
    @mean: parameter of Poisson distribution. Instead a param_t can
        be passed.
    @engine: an engine of random number. A pointer is passed, and its lifetime
        is controlled by the user.
    */
    explicit PoissonRandomNumber(
        double mean = 1.0, engine_t *engine = &global_random_engine);
    explicit PoissonRandomNumber(const param_t &param, 
        engine_t *engine = &global_random_engine);
    ~PoissonRandomNumber();

    /** 
    The copy for parameter is deep, but engine is shared. 
    */
    PoissonRandomNumber(const PoissonRandomNumber &o);
    PoissonRandomNumber(PoissonRandomNumber &&o);
    PoissonRandomNumber & operator=(const PoissonRandomNumber &o);
    PoissonRandomNumber & operator=(PoissonRandomNumber &&o);

    /**
    Setters.
    reset_state(): reset the internal state so that the subsequent random 
        numbers do not depend on the previous ones.
    reset_param(): reset the parameters.
    reset_engine(): reset the random number engine. Note that its life-time 
        is controlled by the user.
    seed(): reseed the random engine.
    */
    PoissonRandomNumber & reset_state();
    PoissonRandomNumber & reset_param(double mean = 1.0);
    PoissonRandomNumber & reset_param(const param_t &param);
    PoissonRandomNumber & reset_engine(engine_t *engine);
    PoissonRandomNumber & seed(seed_t seed = engine_t::default_seed);

    /**
    Geters.
    mean() - the parameter of the distribution.
    param() - the packet of mean.
    min(), max() - minimum and maximum value that can be returned by the random
        number generator.
    range() - the pair {min(), max()}.
    engine() - the random engine. 
    rng() - the internal random number generator.
    */
    double mean() const;
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
    2. using new parameter 'param'.
    3. using new parameter 'mean'.
    */
    result_t operator()();
    result_t get(const param_t &param);
    result_t get(double mean);

    /**
    Get a series of random numbers.
    1. Get 'n' numbers, returned a container (must have push back).
    2. Directly pushing back into container 'c' (clear() is not called).
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


#define _HIPP_TEMPHD template<typename IntT, typename EngineT>
#define _HIPP_TEMPARG <IntT, EngineT>
#define _HIPP_TEMPCLS PoissonRandomNumber _HIPP_TEMPARG
#define _HIPP_TEMPRET _HIPP_TEMPHD inline auto _HIPP_TEMPCLS::
#define _HIPP_TEMPNORET _HIPP_TEMPHD inline _HIPP_TEMPCLS::


_HIPP_TEMPNORET PoissonRandomNumber(double mean, engine_t *engine) 
: _engine(engine), _rng(mean)
{ }

_HIPP_TEMPNORET PoissonRandomNumber(const param_t &param, engine_t *engine) 
: _engine(engine), _rng(param)
{ }

_HIPP_TEMPNORET ~PoissonRandomNumber() 
{ }

_HIPP_TEMPNORET PoissonRandomNumber(const PoissonRandomNumber &o) 
: PoissonRandomNumber(o.param(), o.engine())
{ }

_HIPP_TEMPNORET PoissonRandomNumber(PoissonRandomNumber &&o) 
: PoissonRandomNumber(o.param(), o.engine())
{ }

_HIPP_TEMPRET 
operator=(const PoissonRandomNumber &o) -> PoissonRandomNumber & {
    _engine = o.engine();
    _rng.param(o.param());
    return *this;    
}

_HIPP_TEMPRET 
operator=(PoissonRandomNumber &&o) -> PoissonRandomNumber & {
    _engine = o.engine();
    _rng.param(o.param());
    return *this;    
}

_HIPP_TEMPRET 
reset_state() -> PoissonRandomNumber & {
    _rng.reset();
    return *this;    
}

_HIPP_TEMPRET 
reset_param(double mean) -> PoissonRandomNumber & {
    reset_param( param_t(mean) );
    return *this;    
}

_HIPP_TEMPRET 
reset_param(const param_t &param) -> PoissonRandomNumber & {
    _rng.param(param);
    return *this;    
}

_HIPP_TEMPRET 
reset_engine(engine_t *engine) -> PoissonRandomNumber & {
    _engine = engine;
    return *this;    
}

_HIPP_TEMPRET 
seed(seed_t seed) -> PoissonRandomNumber & {
    _engine->seed(seed);
    return *this;    
}

_HIPP_TEMPRET mean() const-> double {
    return _rng.mean();
}

_HIPP_TEMPRET param() const-> param_t {
    return _rng.param();
}

_HIPP_TEMPRET min() const-> result_t {
    return _rng.min();
}

_HIPP_TEMPRET max() const-> result_t {
    return _rng.max();
}

_HIPP_TEMPRET range() const-> std::pair<result_t, result_t> {
    return {min(), max()};
}

_HIPP_TEMPRET engine() const-> engine_t * {
    return _engine;
}

_HIPP_TEMPRET rng()-> rng_t & {
    return _rng;
}

_HIPP_TEMPRET rng() const-> const rng_t & {
    return _rng;
}

_HIPP_TEMPRET operator()()-> result_t {
    return _rng(*_engine);
}

_HIPP_TEMPRET get(const param_t &param) -> result_t {
    return _rng(*_engine, param);
}

_HIPP_TEMPRET get(double mean) -> result_t {
    return get(param_t(mean));
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
    for(size_t i=0; i<n; ++i){
        c.push_back( (*this)() );
    }
}

_HIPP_TEMPHD
template<typename InputIt> 
void _HIPP_TEMPCLS::operator()(InputIt b, InputIt e) {
    while( b != e )
        *b++ = (*this)();
}


#undef _HIPP_TEMPHD
#undef _HIPP_TEMPARG
#undef _HIPP_TEMPCLS
#undef _HIPP_TEMPRET
#undef _HIPP_TEMPNORET

} // namespace HIPP::NUMERICAL

#endif	//_HIPPNUMERICAL_RANNUM_POISSON_H_