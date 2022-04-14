/**
create: Yangyao CHEN, 2020/12/22
    [write   ] 
    UniformRealRandomNumber - generator of floating-point random numbers of 
          uniform distribution
*/ 

#ifndef _HIPPNUMERICAL_RANNUM_UNIFORM_REAL_H_
#define _HIPPNUMERICAL_RANNUM_UNIFORM_REAL_H_
#include "rannum_base.h"
namespace HIPP::NUMERICAL {

/**
UniformRealRandomNumber - generator of floating-point random numbers of 
uniform distribution.
*/
template<typename RealT = double, typename EngineT = DefaultRandomEngine>
class UniformRealRandomNumber {
public:
    /**
    The result type, random number engine type, underlying random number 
    generator type, parameter type, and seed type.
    */
    typedef RealT result_t;
    typedef EngineT engine_t;
    typedef std::uniform_real_distribution<result_t> rng_t;
    typedef typename rng_t::param_type param_t;
    typedef typename engine_t::result_type seed_t;

    /**
    @a, b: the minimum and maximum of the distribution. Instead a param_t can
        be passed.
    @engine: an engine of random number. A pointer is passed, and its lifetime
        is controlled by the user.
    */
    explicit UniformRealRandomNumber(result_t a = 0., result_t b = 1.,
        engine_t *engine = &global_random_engine);
    explicit UniformRealRandomNumber(const param_t &param, 
        engine_t *engine = &global_random_engine);
    ~UniformRealRandomNumber();

    /** 
    The copy for parameter is deep, but engine is shared. 
    */
    UniformRealRandomNumber(const UniformRealRandomNumber &o);
    UniformRealRandomNumber(UniformRealRandomNumber &&o);
    UniformRealRandomNumber & operator=(const UniformRealRandomNumber &o);
    UniformRealRandomNumber & operator=(UniformRealRandomNumber &&o);

    /**
    Setters.
    reset_state(): reset the internal state so that the subsequent random 
        numbers do not depend on the previous ones.
    reset_param(): reset the parameters.
    reset_engine(): reset the random number engine. Note that its life-time 
        is controlled by the user.
    seed(): reseed the random engine.
    */
    UniformRealRandomNumber & reset_state();
    UniformRealRandomNumber & reset_param(result_t a = 0., result_t b = 1.);
    UniformRealRandomNumber & reset_param(const param_t &param);
    UniformRealRandomNumber & reset_engine(engine_t *engine);
    UniformRealRandomNumber & seed(seed_t seed = engine_t::default_seed);

    /**
    Geters.
    a(), b() - the parameter a, b.
    param() - the packet of a and b.
    min(), max() - minimum and maximum value that can be returned by the random
        number generator. Equal to a and b, respectively.
    range() - the pair {min(), max()}.
    engine() - the random engine. 
    rng() - the internal random number generator.
    */
    result_t a() const;
    result_t b() const;
    param_t param() const;
    result_t min() const;
    result_t max() const;
    std::pair<result_t, result_t> range() const;
    engine_t *engine() const;
    rng_t & rng();
    const rng_t & rng() const;

    /**
    Get a single random number.
    1. using current parameters.
    2. using new parameters 'param'.
    3. using new parameters 'a' and 'b'.
    */
    result_t operator()();
    result_t get(const param_t &param);
    result_t get(result_t a, result_t b);

    /**
    Get a series of random numbers.
    1. Get 'n' numbers, returned a container (must have push back).
    2. Directly pushing back into container 'c' (clear() is not called).
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
#define _HIPP_TEMPARG UniformRealRandomNumber<RealT, EngineT>::

_HIPP_TEMPHD
_HIPP_TEMPARG UniformRealRandomNumber(result_t a, result_t b, engine_t *engine)
:_engine(engine), _rng(a,b)
{}

_HIPP_TEMPHD
_HIPP_TEMPARG UniformRealRandomNumber(const param_t &param, engine_t *engine)
:_engine(engine), _rng(param)
{}

_HIPP_TEMPHD
_HIPP_TEMPARG ~UniformRealRandomNumber(){}

_HIPP_TEMPHD
_HIPP_TEMPARG UniformRealRandomNumber(const UniformRealRandomNumber &o)
:_engine(o._engine), _rng(o._rng.param()){ }

_HIPP_TEMPHD
_HIPP_TEMPARG UniformRealRandomNumber(UniformRealRandomNumber &&o)
:_engine(o._engine), _rng(o._rng.param()){ }

_HIPP_TEMPHD
auto _HIPP_TEMPARG operator=(const UniformRealRandomNumber &o) 
->UniformRealRandomNumber &
{
    _engine = o._engine;
    _rng.param(o._rng.param());
    return *this;
}
_HIPP_TEMPHD
auto _HIPP_TEMPARG operator=(UniformRealRandomNumber &&o) -> UniformRealRandomNumber &
{
    _engine = o._engine;
    _rng.param(o._rng.param());
    return *this;
}

_HIPP_TEMPHD
auto _HIPP_TEMPARG reset_state() -> UniformRealRandomNumber &{
    _rng.reset();
    return *this;
}

_HIPP_TEMPHD
auto _HIPP_TEMPARG reset_param(result_t a, result_t b) 
-> UniformRealRandomNumber &
{
    _rng.param( param_t(a, b) );
    return *this;
}

_HIPP_TEMPHD
auto _HIPP_TEMPARG reset_param(const param_t &param) 
-> UniformRealRandomNumber &
{
    _rng.param(param);
    return *this;
}

_HIPP_TEMPHD
auto _HIPP_TEMPARG reset_engine(engine_t *engine) 
-> UniformRealRandomNumber & 
{
    _engine = engine;
    return *this;
}

_HIPP_TEMPHD
auto _HIPP_TEMPARG seed(seed_t seed) 
-> UniformRealRandomNumber & 
{
    _engine->seed(seed);
    return *this;
}

_HIPP_TEMPHD
auto _HIPP_TEMPARG a() const -> result_t{ 
    return _rng.a(); 
}
_HIPP_TEMPHD
auto _HIPP_TEMPARG b() const -> result_t {
    return _rng.b();
}
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
auto _HIPP_TEMPARG engine() const -> engine_t *{
    return _engine;
}
_HIPP_TEMPHD
auto _HIPP_TEMPARG rng() -> rng_t & {
    return _rng;
}
_HIPP_TEMPHD
auto _HIPP_TEMPARG rng() const -> const rng_t &{
    return _rng;
}
_HIPP_TEMPHD
auto _HIPP_TEMPARG operator()() -> result_t {
    return _rng(*_engine);
}
_HIPP_TEMPHD
auto _HIPP_TEMPARG get(const param_t &param) -> result_t {
    return _rng(*_engine, param);
}
_HIPP_TEMPHD
auto _HIPP_TEMPARG get(result_t a, result_t b) -> result_t {
    return get(param_t(a,b));
}

_HIPP_TEMPHD
template<typename Container>
auto _HIPP_TEMPARG operator()(std::size_t n) -> Container {
    Container c;
    (*this)(c, n);
    return c;
}

_HIPP_TEMPHD
template<typename Container>
void _HIPP_TEMPARG operator()(Container &c, std::size_t n) {
    for(std::size_t i=0; i<n; ++i) c.push_back(_rng(*_engine));
}

_HIPP_TEMPHD
template<typename InputIt>
void _HIPP_TEMPARG operator()(InputIt b, InputIt e) {
    while( b != e ) *b++ = _rng(*_engine);
}


#undef _HIPP_TEMPARG
#undef _HIPP_TEMPHD

} // namespace HIPP::NUMERICAL
#endif	//_HIPPNUMERICAL_RANNUM_UNIFORM_REAL_H_