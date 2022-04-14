/**
create: Yangyao CHEN, 2021/08/18
    [write   ] ExtremeValueRandomNumber - RNG for extreme value distribution.
*/
#ifndef _HIPPNUMERICAL_RANNUM_EXTREME_VALUE_H_
#define _HIPPNUMERICAL_RANNUM_EXTREME_VALUE_H_
#include "rannum_base.h"

namespace HIPP::NUMERICAL {

/**
Floating-point value x, distributed according to the extreme value (also known
as Gumbel Type I, log weibull, Fisher-Tippett Type I) probability density 
function:
    p(x | a,b) = (1/b) exp( ( (a-x)/b ) - exp( (a-x)/b ) ),
where a is shape parameter and b the scale parameter.
*/

template<typename RealT = double, 
    typename EngineT = DefaultRandomEngine>
class ExtremeValueRandomNumber {
public:
    typedef RealT result_t;
    typedef EngineT engine_t;
    typedef std::extreme_value_distribution<result_t> rng_t;
    typedef typename rng_t::param_type param_t;
    typedef typename engine_t::result_type seed_t;

    /**
    @a, b: parameters of extreme value distribution. Instead a ``param_t`` can
        be passed.
    @engine: an engine of random number. A pointer is passed, and its lifetime
        is controlled by the user.
    */
    explicit ExtremeValueRandomNumber(
        result_t a = 0.0, result_t b = 1.0,
        engine_t *engine = &global_random_engine);
    explicit ExtremeValueRandomNumber(const param_t &param, 
        engine_t *engine = &global_random_engine);
    ~ExtremeValueRandomNumber();

    /** 
    The copy for parameter is deep, but engine is shared. 
    */
    ExtremeValueRandomNumber(const ExtremeValueRandomNumber &o);
    ExtremeValueRandomNumber(ExtremeValueRandomNumber &&o);
    ExtremeValueRandomNumber & operator=(const ExtremeValueRandomNumber &o);
    ExtremeValueRandomNumber & operator=(ExtremeValueRandomNumber &&o);

    /**
    Setters.
    reset_state(): reset the internal state so that the subsequent random 
        numbers do not depend on the previous ones.
    reset_param(): reset the parameters.
    reset_engine(): reset the random number engine. Note that its life-time 
        is controlled by the user.
    seed(): reseed the random engine.
    */
    ExtremeValueRandomNumber & reset_state();
    ExtremeValueRandomNumber & reset_param(result_t a = 0.0, result_t b = 1.0);
    ExtremeValueRandomNumber & reset_param(const param_t &param);
    ExtremeValueRandomNumber & reset_engine(engine_t *engine);
    ExtremeValueRandomNumber & seed(seed_t seed = engine_t::default_seed);
    
    /**
    Geters.
    a(), b() : the parameters of the distribution.
    param() : the packet of ``a`` and ``b``.
    min(), max() : minimum and maximum value that can be returned by the random
        number generator.
    range() : the pair {min(), max()}.
    engine() : the random engine. 
    rng() : the internal random number generator.
    */
    result_t a() const;
    result_t b() const;
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
    3. using new parameters ``a`` and ``b``.
    */
    result_t operator()();
    result_t get(const param_t &param);
    result_t get(result_t a, result_t b);


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
#define _HIPP_TEMPCLS ExtremeValueRandomNumber _HIPP_TEMPARG
#define _HIPP_TEMPRET _HIPP_TEMPHD inline auto _HIPP_TEMPCLS::
#define _HIPP_TEMPNORET _HIPP_TEMPHD inline _HIPP_TEMPCLS::

_HIPP_TEMPNORET
ExtremeValueRandomNumber(result_t a, result_t b, engine_t *engine) 
: _engine(engine), _rng(a, b)
{ }

_HIPP_TEMPNORET
ExtremeValueRandomNumber(const param_t &param, engine_t *engine)
: _engine(engine), _rng(param)
{ }

_HIPP_TEMPNORET
~ExtremeValueRandomNumber() 
{ }

_HIPP_TEMPNORET
ExtremeValueRandomNumber(const ExtremeValueRandomNumber &o) 
:_engine(o.engine()), _rng(o.param()) 
{ }

_HIPP_TEMPNORET
ExtremeValueRandomNumber(ExtremeValueRandomNumber &&o) 
:_engine(o.engine()), _rng(o.param()) 
{}

_HIPP_TEMPRET operator=(const ExtremeValueRandomNumber &o)
-> ExtremeValueRandomNumber &
{
    _engine = o.engine();
    _rng.param(o.param());
    return *this;
}

_HIPP_TEMPRET operator=(ExtremeValueRandomNumber &&o)
-> ExtremeValueRandomNumber &
{
    _engine = o.engine();
    _rng.param(o.param());
    return *this;
}

_HIPP_TEMPRET reset_state()
-> ExtremeValueRandomNumber &
{
    _rng.reset();
    return *this;
}

_HIPP_TEMPRET reset_param(result_t a, result_t b)
-> ExtremeValueRandomNumber &
{
    return reset_param( param_t(a, b) );
}

_HIPP_TEMPRET reset_param(const param_t &param)
-> ExtremeValueRandomNumber &
{
    _rng.param( param );
    return *this;
}

_HIPP_TEMPRET reset_engine(engine_t *engine)
-> ExtremeValueRandomNumber &
{
    _engine = engine;
    return *this;
}

_HIPP_TEMPRET seed(seed_t seed)
-> ExtremeValueRandomNumber &
{
    _engine->seed(seed);
    return *this;
}

_HIPP_TEMPRET a() const -> result_t {
    return _rng.a();
}

_HIPP_TEMPRET b() const -> result_t {
    return _rng.b();
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

_HIPP_TEMPRET get(result_t a, result_t b) -> result_t {
    return get( param_t(a, b) );
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

#endif	//_HIPPNUMERICAL_RANNUM_EXTREME_VALUE_H_