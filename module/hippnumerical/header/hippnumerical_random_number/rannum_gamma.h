/**
create: Yangyao CHEN, 2021/08/18
    [write   ] GammaRandomNumber - RNG for gamma distribution.
*/
#ifndef _HIPPNUMERICAL_RANNUM_GAMMA_H_
#define _HIPPNUMERICAL_RANNUM_GAMMA_H_
#include "rannum_base.h"

namespace HIPP::NUMERICAL {

/**
Positive floating-point value x, distributed according to the gamma
probability density function:
    P(x | alpha, beta) = exp(-x/beta) x^(alpha-1) / (beta^alpha Gamma(alpha) ),
where alpha is shape parameter and beta is scale parameter.
*/

template<typename RealT = double, 
    typename EngineT = DefaultRandomEngine>
class GammaRandomNumber {
public:
    typedef RealT result_t;
    typedef EngineT engine_t;
    typedef std::gamma_distribution<result_t> rng_t;
    typedef typename rng_t::param_type param_t;
    typedef typename engine_t::result_type seed_t;

    /**
    @alpha, beta: parameters of gamma distribution. Instead a ``param_t`` can
        be passed.
    @engine: an engine of random number. A pointer is passed, and its lifetime
        is controlled by the user.
    */
    explicit GammaRandomNumber(
        result_t alpha = 1.0, result_t beta = 1.0,
        engine_t *engine = &global_random_engine);
    explicit GammaRandomNumber(const param_t &param, 
        engine_t *engine = &global_random_engine);
    ~GammaRandomNumber();

    /** 
    The copy for parameter is deep, but engine is shared. 
    */
    GammaRandomNumber(const GammaRandomNumber &o);
    GammaRandomNumber(GammaRandomNumber &&o);
    GammaRandomNumber & operator=(const GammaRandomNumber &o);
    GammaRandomNumber & operator=(GammaRandomNumber &&o);

    /**
    Setters.
    reset_state(): reset the internal state so that the subsequent random 
        numbers do not depend on the previous ones.
    reset_param(): reset the parameters.
    reset_engine(): reset the random number engine. Note that its life-time 
        is controlled by the user.
    seed(): reseed the random engine.
    */
    GammaRandomNumber & reset_state();
    GammaRandomNumber & reset_param(result_t alpha = 1.0, result_t beta = 1.0);
    GammaRandomNumber & reset_param(const param_t &param);
    GammaRandomNumber & reset_engine(engine_t *engine);
    GammaRandomNumber & seed(seed_t seed = engine_t::default_seed);

    /**
    Geters.
    alpha(), beta() : the parameters of the distribution.
    param() : the packet of ``alpha`` and ``beta``.
    min(), max() : minimum and maximum value that can be returned by the random
        number generator.
    range() : the pair {min(), max()}.
    engine() : the random engine. 
    rng() : the internal random number generator.
    */
    result_t alpha() const;
    result_t beta() const;
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
    3. using new parameters ``alpha`` and ``beta``.
    */
    result_t operator()();
    result_t get(const param_t &param);
    result_t get(result_t alpha, result_t beta);


    /**
    Get a series of random numbers.

    1. Get ``n`` numbers, returned a container (must have push back).
    2. Directly pushing back into container ``c`` (``clear()`` is not called).
    3. Write into a range specified by a pair of iterator.
    */
    template<typename Container = std::vector<result_t> >
    Container operator()(std::size_t n);

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
#define _HIPP_TEMPCLS GammaRandomNumber _HIPP_TEMPARG
#define _HIPP_TEMPRET _HIPP_TEMPHD inline auto _HIPP_TEMPCLS::
#define _HIPP_TEMPNORET _HIPP_TEMPHD inline _HIPP_TEMPCLS::

_HIPP_TEMPNORET
GammaRandomNumber(result_t alpha, result_t beta, engine_t *engine) 
: _engine(engine), _rng(alpha, beta)
{ }

_HIPP_TEMPNORET
GammaRandomNumber(const param_t &param, engine_t *engine)
: _engine(engine), _rng(param)
{ }

_HIPP_TEMPNORET
~GammaRandomNumber() 
{ }

_HIPP_TEMPNORET
GammaRandomNumber(const GammaRandomNumber &o) 
:_engine(o.engine()), _rng(o.param()) 
{ }

_HIPP_TEMPNORET
GammaRandomNumber(GammaRandomNumber &&o) 
:_engine(o.engine()), _rng(o.param()) 
{}

_HIPP_TEMPRET operator=(const GammaRandomNumber &o)
-> GammaRandomNumber &
{
    _engine = o.engine();
    _rng.param(o.param());
    return *this;
}

_HIPP_TEMPRET operator=(GammaRandomNumber &&o)
-> GammaRandomNumber &
{
    _engine = o.engine();
    _rng.param(o.param());
    return *this;
}

_HIPP_TEMPRET reset_state()
-> GammaRandomNumber &
{
    _rng.reset();
    return *this;
}

_HIPP_TEMPRET reset_param(result_t alpha, result_t beta)
-> GammaRandomNumber &
{
    return reset_param( param_t(alpha, beta) );
}

_HIPP_TEMPRET reset_param(const param_t &param)
-> GammaRandomNumber &
{
    _rng.param( param );
    return *this;
}

_HIPP_TEMPRET reset_engine(engine_t *engine)
-> GammaRandomNumber &
{
    _engine = engine;
    return *this;
}

_HIPP_TEMPRET seed(seed_t seed)
-> GammaRandomNumber &
{
    _engine->seed(seed);
    return *this;
}

_HIPP_TEMPRET alpha() const -> result_t {
    return _rng.alpha();
}

_HIPP_TEMPRET beta() const -> result_t {
    return _rng.beta();
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

_HIPP_TEMPRET get(result_t alpha, result_t beta) -> result_t {
    return get( param_t(alpha, beta) );
}

_HIPP_TEMPHD
template<typename Container>
auto _HIPP_TEMPCLS::operator()(std::size_t n) -> Container {
    Container c;
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

#endif	//_HIPPNUMERICAL_RANNUM_GAMMA_H_