Random Number Generators
==================================

.. _api-numerical-rannum:

.. include:: /global.rst

The following variables, functions and classes are all defined within the namespace ``HIPP::NUMERICAL``.

.. namespace:: HIPP::NUMERICAL

.. _api-numerical-rannum-predefined-obj:

Predefined Random Number Generators
--------------------------------------

The following table lists the predefined random number generator objects for quick-and-dirty 
random number generation. Note that they are all static thread-local variable and all share 
a global (thread-local) random engine :var:`global_random_engine` with default seed, 
i.e., the calling ``seed()`` of one affects the others.

.. table::
    :class: tight-table fix-width-table
    :widths: 20 20 60

    ==================== =============================================== ==========================================================
    Object                   Type                                        Description
    ==================== =============================================== ==========================================================
    :var:`randn`         :expr:`GaussianRandomNumber<>`                  standard normal dist
    :var:`rand`          :expr:`UniformRealRandomNumber<>`               uniform real dist in ``[0,1]``
    :var:`randfloat`     :expr:`UniformRealRandomNumber<float>`          uniform real dist in ``[0,1]``, using ``float``
    :var:`randchar`      :expr:`UniformIntRandomNumber<char>`            uniform char in ``[0, max char]``
    :var:`randshort`     :expr:`UniformIntRandomNumber<short>`           uniform short in ``[0, max short]``
    :var:`randint`       :expr:`UniformIntRandomNumber<>`                uniform int in ``[0, max int]``
    :var:`randuint`      :expr:`UniformIntRandomNumber<unsigned int>`    uniform unsigned int in ``[0, max unsigned int]``
    :var:`randlong`      :expr:`UniformIntRandomNumber<long>`            uniform long in ``[0, max long]``
    :var:`randlonglong`  :expr:`UniformIntRandomNumber<long long>`       uniform long long in ``[0, max long long]``
    :var:`randsize`      :expr:`UniformIntRandomNumber<std::size_t>`     uniform size_t in ``[0, max size_t]``
    ==================== =============================================== ==========================================================

**Examples:**

The following codes demonstrate how to generate standard Gaussian random numbers using the predefined object :var:`randn`::

    using namespace HIPP;
    using namespace HIPP::NUMERICAL;

    /* Call the object to get a random number or a sequence of them. */
    double r1 = randn();                    /* return a single random number */

    vector<double> r2 = randn(3);           /* return a vector of 3 numbers */

    double r3[3];
    randn(r3, r3+3);                        /* fill 3 number into a range */

    /* Call get() method to use different parameters. */
    double r4 = randn.get(1.0, 3.0);        /* a random number with mean=1, stddev=3 */

    pout << r1, '\n',
        r2, '\n',
        pout(r3,r3+3), '\n',
        r4, endl;

The output is (may depend on the implementation and variate among runs):

.. code-block:: text

    -0.121966
    -1.08682,0.68429,-1.07519
    0.0332695,0.744836,0.0336061
    -0.57991

Detailed Definitions
""""""""""""""""""""""

.. var:: thread_local static GaussianRandomNumber<> randn            

    Standard normal dist.

.. var:: thread_local static UniformRealRandomNumber<> rand          

    Uniform real dist in [0,1].

.. var:: thread_local static UniformRealRandomNumber<float> randfloat

    Uniform real dist in [0,1], using `float`.

.. var:: thread_local static UniformIntRandomNumber<char> randchar   

    Uniform char in [0, max char].

.. var:: thread_local static UniformIntRandomNumber<short> randshort 

    Uniform short in [0, max short].

.. var:: thread_local static UniformIntRandomNumber<> randint        

    Uniform int in [0, max int].

.. var:: thread_local static UniformIntRandomNumber<unsigned int> randuint

    Uniform unsigned int in [0, max unsigned int].

.. var:: thread_local static UniformIntRandomNumber<long> randlong   

    Uniform long in [0, max long].

.. var:: thread_local static UniformIntRandomNumber<long long> randlonglong     

    Uniform long long in [0, max long long].

.. var:: thread_local static UniformIntRandomNumber<std::size_t> randsize     

    Uniform size_t in [0, max size_t].

Random Number Engines 
----------------------

.. class:: RandomEngine 

    Ensemble of random number engine types.

    .. type:: minstd_rand0_t          = std::minstd_rand0
            minstd_rand_t           = std::minstd_rand
            mt19937_t               = std::mt19937
            mt19937_64_t            = std::mt19937_64
            ranlux24_base_t         = std::ranlux24_base
            ranlux48_base_t         = std::ranlux48_base
            ranlux24_t              = std::ranlux24
            ranlux48_t              = std::ranlux48
            knuth_b_t               = std::knuth_b
            default_t               = std::default_random_engine

        Predefined random number generator types.
    
    .. type:: device_t                = std::random_device

        Non-deterministic random number generator types.


.. type::   RandomEngine::default_t DefaultRandomEngine

    The default engine type chosen by the implementation. Generally the best in most cases.

.. var::    thread_local static DefaultRandomEngine global_random_engine
            static DefaultRandomEngine process_global_random_engine

    The default random engine objects used by HIPP.

    The thread local one is thread-safe, but threads may have the same random
    sequence. This is the default choice in the construction of distribution
    generators.

    The process global one is shared among threads. It is not thread-safe
    and must be protected carefully.

Uniform Distributions
-------------------------

Uniform Distributed Integer 
""""""""""""""""""""""""""""""

.. class:: template<typename IntT = int, typename EngineT = DefaultRandomEngine> UniformIntRandomNumber

    ``UniformIntRandomNumber`` - generator of integer random numbers of 
    uniform distribution.

    .. type:: IntT result_t
        EngineT engine_t
        std::uniform_int_distribution<result_t> rng_t
        typename rng_t::param_type param_t
        typename engine_t::result_type seed_t

        The result type, random number engine type, underlying random number 
        generator type, parameter type, and seed type.

    .. function:: explicit UniformIntRandomNumber(result_t a = 0, result_t b = std::numeric_limits<result_t>::max(), engine_t *engine = &global_random_engine)
                explicit UniformIntRandomNumber(const param_t &param, engine_t *engine = &global_random_engine)
                ~UniformIntRandomNumber()

        Constructors and destructor.

        - ``a``, ``b``: the minimum and maximum of the distribution. Instead a ``param_t`` can
          be passed.
        - ``engine``: an engine of random number. A pointer is passed, and its lifetime
          is controlled by the user.

    .. function::   UniformIntRandomNumber(const UniformIntRandomNumber &o)
        UniformIntRandomNumber(UniformIntRandomNumber &&o)
        UniformIntRandomNumber & operator=(const UniformIntRandomNumber &o)
        UniformIntRandomNumber & operator=(UniformIntRandomNumber &&o)

        The copy for parameter is deep, but engine is shared. 

    .. function:: \
        UniformIntRandomNumber & reset_state()
        UniformIntRandomNumber & reset_param(result_t a = 0, result_t b = std::numeric_limits<result_t>::max())
        UniformIntRandomNumber & reset_param(const param_t &param)
        UniformIntRandomNumber & reset_engine(engine_t *engine)
        UniformIntRandomNumber & seed(seed_t seed = engine_t::default_seed)

        Setters.

        ``reset_state()`` : reset the internal state so that the subsequent random 
        numbers do not depend on the previous ones.
        
        ``reset_param()`` : reset the parameters.
        
        ``reset_engine()`` : reset the random number engine. Note that its life-time 
        is controlled by the user.
        
        ``seed()`` : reseed the random engine.

    .. function:: \
        result_t a() const
        result_t b() const
        param_t param() const
        result_t min() const
        result_t max() const
        std::pair<result_t, result_t> range() const
        engine_t *engine() const
        rng_t & rng()
        const rng_t & rng() const

        Geters.
        
        ``a()``, ``b()`` : the parameter ``a``, ``b``.

        ``param()`` : the packet of ``a`` and ``b``.
        
        ``min()``, ``max()`` : minimum and maximum value that can be returned by the random
        number generator. Equal to a and b, respectively.
        
        ``range()`` : the pair ``{min(), max()}``.
        
        ``engine()`` : the random engine. 
        
        ``rng()`` : the internal random number generator.

    .. function:: \
        result_t operator()()
        result_t get(const param_t &param)
        result_t get(result_t a, result_t b)

        Get a single random number.
        
        1. using current parameters.
        2. using new parameters ``param``.
        3. using new parameters ``a`` and ``b``.

    .. function:: \
        template<template<typename> typename Container = std::vector> Container<result_t> operator()(std::size_t n)
        template<typename Container> void operator()(Container &c, std::size_t n)
        template<typename InputIt> void operator()(InputIt b, InputIt e)

        Get a series of random numbers.

        1. Get ``n`` numbers, returned a container (must have push back).
        2. Directly pushing back into container ``c`` (``clear()`` is not called).
        3. Write into a range specified by a pair of iterator.


Uniform Distributed Floating-point Number
"""""""""""""""""""""""""""""""""""""""""""

.. class:: template<typename RealT = double, typename EngineT = DefaultRandomEngine> UniformRealRandomNumber

    ``UniformRealRandomNumber`` - generator of floating-point random numbers of 
    uniform distribution.


    .. type:: RealT result_t
        EngineT engine_t
        std::uniform_real_distribution<result_t> rng_t
        typename rng_t::param_type param_t
        typename engine_t::result_type seed_t

        The result type, random number engine type, underlying random number 
        generator type, parameter type, and seed type.

    .. function:: \
        explicit UniformRealRandomNumber(result_t a = 0., result_t b = 1., engine_t *engine = &global_random_engine)
        explicit UniformRealRandomNumber(const param_t &param, engine_t *engine = &global_random_engine)
        ~UniformRealRandomNumber()

        Constructors and destructor.

        - ``a``, ``b`` : the minimum and maximum of the distribution. Instead a ``param_t`` can
          be passed.
        - ``engine`` : an engine of random number. A pointer is passed, and its lifetime
          is controlled by the user.

    .. function:: \
        UniformRealRandomNumber(const UniformRealRandomNumber &o)
        UniformRealRandomNumber(UniformRealRandomNumber &&o)
        UniformRealRandomNumber & operator=(const UniformRealRandomNumber &o)
        UniformRealRandomNumber & operator=(UniformRealRandomNumber &&o)

        The copy for parameter is deep, but engine is shared. 

    .. function:: \
        UniformRealRandomNumber & reset_state()
        UniformRealRandomNumber & reset_param(result_t a = 0., result_t b = 1.)
        UniformRealRandomNumber & reset_param(const param_t &param)
        UniformRealRandomNumber & reset_engine(engine_t *engine)
        UniformRealRandomNumber & seed(seed_t seed = engine_t::default_seed)

        Setters.
    
        ``reset_state()`` : reset the internal state so that the subsequent random 
        numbers do not depend on the previous ones.
        
        ``reset_param()`` : reset the parameters.
        
        ``reset_engine()`` : reset the random number engine. Note that its life-time 
        is controlled by the user.
        
        ``seed()`` : reseed the random engine.

    .. function:: \
        result_t a() const
        result_t b() const
        param_t param() const
        result_t min() const
        result_t max() const
        std::pair<result_t, result_t> range() const
        engine_t *engine() const
        rng_t & rng()
        const rng_t & rng() const

        Geters
        
        ``a()``, ``b()`` : the parameter ``a``, ``b``.
        
        ``param()`` : the packet of ``a`` and ``b``.
        
        ``min()``, ``max()`` : minimum and maximum value that can be returned by the random
        number generator. Equal to ``a`` and ``b``, respectively.
        
        ``range()`` : the pair ``{min(), max()}``.
        
        ``engine()`` : the random engine. 
        
        ``rng()`` : the internal random number generator.

    .. function:: \
        result_t operator()()
        result_t get(const param_t &param)
        result_t get(result_t a, result_t b)

        Get a single random number.
        
        1. using current parameters.
        2. using new parameters ``param``.
        3. using new parameters ``a`` and ``b``.

    
    .. function:: \
        template<template<typename> typename Container = std::vector> Container<result_t> operator()(std::size_t n)
        template<typename Container> void operator()(Container &c, std::size_t n)
        template<typename InputIt> void operator()(InputIt b, InputIt e)

        Get a series of random numbers.

        1. Get ``n`` numbers, returned a container (must have push back).
        2. Directly pushing back into container ``c`` (``clear()`` is not called).
        3. Write into a range specified by a pair of iterator.


Gaussian Random Number 
-------------------------

.. class:: template<typename RealT = double, typename EngineT = DefaultRandomEngine> GaussianRandomNumber

    ``GaussianRandomNumber`` - generator of Normal Distribution random numbers.

    .. type:: RealT result_t
        EngineT engine_t
        std::normal_distribution<result_t> rng_t
        typename rng_t::param_type param_t
        typename engine_t::result_type seed_t

    .. function:: \
        explicit GaussianRandomNumber(result_t mean = 0., result_t stddev = 1., engine_t *engine = &global_random_engine)
        explicit GaussianRandomNumber(const param_t &param, engine_t *engine = &global_random_engine)
        ~GaussianRandomNumber()

        - ``mean``, ``stddev`` : parameters of normal distribution. Instead a ``param_t`` can
          be passed.
        
        - ``engine`` : an engine of random number. A pointer is passed, and its lifetime
          is controlled by the user.

    .. function:: \
        GaussianRandomNumber(const GaussianRandomNumber &o)
        GaussianRandomNumber(GaussianRandomNumber &&o)
        GaussianRandomNumber & operator=(const GaussianRandomNumber &o)
        GaussianRandomNumber & operator=(GaussianRandomNumber &&o)

        The copy for parameter is deep, but engine is shared. 
    
    .. function:: \
        GaussianRandomNumber & reset_state()
        GaussianRandomNumber & reset_param(result_t mean = 0., result_t stddev = 1.)
        GaussianRandomNumber & reset_param(const param_t &param)
        GaussianRandomNumber & reset_engine(engine_t *engine)
        GaussianRandomNumber & seed(seed_t seed = engine_t::default_seed)

        Setters.

        ``reset_state()``: reset the internal state so that the subsequent random 
        numbers do not depend on the previous ones.
        
        ``reset_param()``: reset the parameters.
        
        ``reset_engine()``: reset the random number engine. Note that its life-time 
        is controlled by the user.
        
        ``seed()``: reseed the random engine.

    .. function:: \
        result_t mean() const
        result_t stddev() const
        param_t param() const
        result_t min() const
        result_t max() const
        std::pair<result_t, result_t> range() const
        engine_t * engine() const
        rng_t & rng()
        const rng_t & rng() const

        Geters.

        ``mean()``, ``stddev()`` - the parameters of the distribution.
        
        ``param()`` - the packet of ``mean`` and ``stddev``.
        
        ``min()``, ``max()`` - minimum and maximum value that can be returned by the random number generator.
        
        ``range()`` - the pair ``{min(), max()}``.
        
        ``engine()`` - the random engine. 
        
        ``rng()`` - the internal random number generator.

    .. function:: \
        result_t operator()()
        result_t get(const param_t &param)
        result_t get(result_t mean, result_t stddev)

        Get a single random number.
    
        1. using current parameters.
        2. using new parameters ``param``.
        3. using new parameters ``mean`` and ``stddev``.
    
    .. function:: \
        template<template<typename> typename Container = std::vector> Container<result_t> operator()(std::size_t n)
        template<typename Container>  void operator()(Container &c, std::size_t n)
        template<typename InputIt>  void operator()(InputIt b, InputIt e)

        Get a series of random numbers.

        1. Get ``n`` numbers, returned a container (must have push back).
        2. Directly pushing back into container ``c`` (``clear()`` is not called).
        3. Write into a range specified by a pair of iterator.


Poisson Distributions
-------------------------

Poisson Random Number
""""""""""""""""""""""""""""""

.. class:: template<typename IntT = int, typename EngineT = DefaultRandomEngine> PoissonRandomNumber

    PoissonRandomNumber - generator of Poisson Distribution random number.
    It produces non-negative integer value :math:`i`, distributed according to the 
    probability function:

    .. math::
        
        P(i | \mu) = \exp(-\mu) \mu^i / i !,

    where :math:`mu` is the mean number.

    .. type:: IntT result_t
        EngineT engine_t
        std::poisson_distribution<result_t> rng_t
        typename rng_t::param_type param_t
        typename engine_t::result_type seed_t

    
    .. function:: explicit PoissonRandomNumber(double mean = 1.0, engine_t *engine = &global_random_engine)
        explicit PoissonRandomNumber(const param_t &param, engine_t *engine = &global_random_engine)
        ~PoissonRandomNumber()

        Constructors and destructor.

        :arg mean: parameter of Poisson distribution. Instead a ``param_t`` can be passed.
        :arg engine: an engine of random number. A pointer is passed, and its lifetime is controlled by the user.
    
    .. function:: \
        PoissonRandomNumber(const PoissonRandomNumber &o)
        PoissonRandomNumber(PoissonRandomNumber &&o)
        PoissonRandomNumber & operator=(const PoissonRandomNumber &o)
        PoissonRandomNumber & operator=(PoissonRandomNumber &&o)

        The copy for parameter is deep, but engine is shared. 

    .. function:: \
        PoissonRandomNumber & reset_state()
        PoissonRandomNumber & reset_param(double mean = 1.0)
        PoissonRandomNumber & reset_param(const param_t &param)
        PoissonRandomNumber & reset_engine(engine_t *engine)
        PoissonRandomNumber & seed(seed_t seed = engine_t::default_seed)

        Setters.
    
        ``reset_state()`` : reset the internal state so that the subsequent random 
        numbers do not depend on the previous ones.
        
        ``reset_param()`` : reset the parameters.
        
        ``reset_engine()`` : reset the random number engine. Note that its life-time 
        is controlled by the user.
        
        ``seed()`` : reseed the random engine.

    .. function:: \
        double mean() const
        param_t param() const
        result_t min() const
        result_t max() const
        std::pair<result_t, result_t> range() const
        engine_t * engine() const
        rng_t & rng()
        const rng_t & rng() const

        Geters.

        ``mean()`` : the parameter of the distribution.
        
        ``param()`` : the packet of mean.
        
        ``min(),`` ``max()`` : minimum and maximum value that can be returned by the random
        number generator.
        
        ``range()`` : the pair ``{min(), max()}``.
        
        ``engine()`` : the random engine. 
        
        ``rng()`` : the internal random number generator.

    .. function:: \
        result_t operator()()
        result_t get(const param_t &param)
        result_t get(double mean)

        Get a single random number.
    
        1. using current parameters.
        2. using new parameter ``param``.
        3. using new parameter ``mean``.

    .. function:: \
        template<template<typename> typename Container = std::vector> Container<result_t> operator()(std::size_t n)
        template<typename Container>  void operator()(Container &c, std::size_t n)
        template<typename InputIt> void operator()(InputIt b, InputIt e)

        Get a series of random numbers.
    
        1. Get ``n`` numbers, returned a container (must have push back).
        2. Directly pushing back into container ``c`` (clear() is not called).
        3. Write into a range specified by a pair of iterator.

Exponential Random Number
""""""""""""""""""""""""""""""

.. class:: template<typename RealT = double, typename EngineT = DefaultRandomEngine> ExponentialRandomNumber

    Non-negative floating-point value :math:`x`, distributed according to the exponential
    probability density function:

    .. math::

        P(x | \lambda) = \lambda \exp(-\lambda x),
    
    This is the continuous counterpart of geometric distribution.

    .. type:: RealT result_t
        EngineT engine_t
        std::exponential_distribution<result_t> rng_t
        typename rng_t::param_type param_t
        typename engine_t::result_type seed_t

    .. function:: \
        explicit ExponentialRandomNumber(result_t lambda = 1.0, engine_t *engine = &global_random_engine)
        explicit ExponentialRandomNumber(const param_t &param, engine_t *engine = &global_random_engine)
        ~ExponentialRandomNumber()

        Constructors and destructor.

        :arg lambda: parameter of exponential distribution. Instead a ``param_t`` can be passed.
        :arg engine: an engine of random number. A pointer is passed, and its lifetime is controlled by the user.


    .. function:: \
        ExponentialRandomNumber(const ExponentialRandomNumber &o)
        ExponentialRandomNumber(ExponentialRandomNumber &&o)
        ExponentialRandomNumber & operator=(const ExponentialRandomNumber &o)
        ExponentialRandomNumber & operator=(ExponentialRandomNumber &&o)

        The copy for parameter is deep, but engine is shared. 

    .. function:: \
        ExponentialRandomNumber & reset_state()
        ExponentialRandomNumber & reset_param(result_t lambda = 1.0)
        ExponentialRandomNumber & reset_param(const param_t &param)
        ExponentialRandomNumber & reset_engine(engine_t *engine)
        ExponentialRandomNumber & seed(seed_t seed = engine_t::default_seed)

        Setters.
        
        ``reset_state()`` : reset the internal state so that the subsequent random 
        numbers do not depend on the previous ones.

        ``reset_param()`` : reset the parameters.

        ``reset_engine()`` : reset the random number engine. Note that its life-time 
        is controlled by the user.

        ``seed()`` : reseed the random engine.

    .. function:: \
        result_t lambda() const
        param_t param() const
        result_t min() const
        result_t max() const
        std::pair<result_t, result_t> range() const
        engine_t * engine() const
        rng_t & rng()
        const rng_t & rng() const

        Geters.

        ``lambda()`` : the parameter of the distribution.

        ``param()`` : the packet of ``lambda``.
        
        ``min()``, ``max()`` : minimum and maximum value that can be returned by the random
        number generator.
        
        ``range()`` : the pair ``{min(), max()}``.
        
        ``engine()`` : the random engine. 
        
        ``rng()`` : the internal random number generator.


    .. function:: \
        result_t operator()()
        result_t get(const param_t &param)
        result_t get(result_t lambda)

        Get a single random number.

        1. using current parameters.
        2. using new parameter ``param``.
        3. using new parameter ``lambda``.

    .. function:: \
        template<template<typename> typename Container = std::vector> Container<result_t> operator()(std::size_t n)
        template<typename Container> void operator()(Container &c, std::size_t n)
        template<typename InputIt> void operator()(InputIt b, InputIt e)

        Get a series of random numbers.

        1. Get ``n`` numbers, returned a container (must have push back).
        2. Directly pushing back into container ``c`` (``clear()`` is not called).
        3. Write into a range specified by a pair of iterator.

Gamma Random Number
""""""""""""""""""""""""""""""

.. class:: template<typename RealT = double, typename EngineT = DefaultRandomEngine> GammaRandomNumber 

    Positive floating-point value :math:`x`, distributed according to the gamma
    probability density function:

    .. math::
        
        P(x | \alpha, \beta) = \exp(- \frac{x}{\beta} ) x^{\alpha-1} / (\beta^\alpha \Gamma(\alpha) ),

    where :math:`\alpha` is shape parameter and :math:`\beta` the scale parameter.

    .. type:: RealT result_t
        EngineT engine_t
        std::gamma_distribution<result_t> rng_t
        typename rng_t::param_type param_t
        typename engine_t::result_type seed_t

    .. function:: \
        explicit GammaRandomNumber(result_t alpha = 1.0, result_t beta = 1.0,engine_t *engine = &global_random_engine)
        explicit GammaRandomNumber(const param_t &param, engine_t *engine = &global_random_engine)
        ~GammaRandomNumber()

        Constructors and destructor.

        ``alpha``, ``beta`` : parameters of gamma distribution. Instead a ``param_t`` can
        be passed.
        
        ``engine`` : an engine of random number. A pointer is passed, and its lifetime
        is controlled by the user.

    .. function:: \
        GammaRandomNumber(const GammaRandomNumber &o)
        GammaRandomNumber(GammaRandomNumber &&o)
        GammaRandomNumber & operator=(const GammaRandomNumber &o)
        GammaRandomNumber & operator=(GammaRandomNumber &&o)

        The copy for parameter is deep, but engine is shared. 


    .. function:: \
        GammaRandomNumber & reset_state()
        GammaRandomNumber & reset_param(result_t alpha = 1.0, result_t beta = 1.0)
        GammaRandomNumber & reset_param(const param_t &param)
        GammaRandomNumber & reset_engine(engine_t *engine)
        GammaRandomNumber & seed(seed_t seed = engine_t::default_seed)

        Setters.
        
        ``reset_state()`` : reset the internal state so that the subsequent random 
        numbers do not depend on the previous ones.

        ``reset_param()`` : reset the parameters.

        ``reset_engine()`` : reset the random number engine. Note that its life-time 
        is controlled by the user.

        ``seed()`` : reseed the random engine.

    
    .. function:: \
        result_t alpha() const
        result_t beta() const
        param_t param() const
        result_t min() const
        result_t max() const
        std::pair<result_t, result_t> range() const
        engine_t * engine() const
        rng_t & rng()
        const rng_t & rng() const

        Geters.

        ``alpha()``, ``beta()`` : the parameters of the distribution.
        
        ``param()`` : the packet of ``alpha`` and ``beta``.
        
        ``min()``, ``max()`` : minimum and maximum value that can be returned by the random
        number generator.
        
        ``range()`` : the pair ``{min(), max()}``.
        
        ``engine()`` : the random engine. 
        
        ``rng()`` : the internal random number generator.

 
    .. function:: \
        result_t operator()()
        result_t get(const param_t &param)
        result_t get(result_t alpha, result_t beta)

        Get a single random number.

        1. using current parameters.
        2. using new parameters ``param``.
        3. using new parameters ``alpha`` and ``beta``.

    .. function:: \
        template<template<typename> typename Container = std::vector> Container<result_t> operator()(std::size_t n)
        template<typename Container> void operator()(Container &c, std::size_t n)
        template<typename InputIt> void operator()(InputIt b, InputIt e)

        Get a series of random numbers.

        1. Get ``n`` numbers, returned a container (must have push back).
        2. Directly pushing back into container ``c`` (``clear()`` is not called).
        3. Write into a range specified by a pair of iterator.

Weibull Random Number
""""""""""""""""""""""""""""""

.. class:: template<typename RealT = double, typename EngineT = DefaultRandomEngine> WeibullRandomNumber


    Floating-point value :math:`x`, distributed according to the Weibull
    probability density function:

    .. math::

        p(x | a,b) = (\frac{a}{b}) (\frac{x}{b})^{a-1} \exp( -(\frac{x}{b})^a ),
        
    where :math:`a` is shape parameter and :math:`b` the scale parameter.

    .. type:: RealT result_t
        EngineT engine_t
        std::weibull_distribution<result_t> rng_t
        typename rng_t::param_type param_t
        typename engine_t::result_type seed_t

    .. function:: \
        explicit WeibullRandomNumber(result_t a = 1.0, result_t b = 1.0,engine_t *engine = &global_random_engine)
        explicit WeibullRandomNumber(const param_t &param, engine_t *engine = &global_random_engine)
        ~WeibullRandomNumber()

        Constructors and destructor.

        ``a``, ``b`` : parameters of weibull distribution. Instead a ``param_t`` can
        be passed.
        
        ``engine``: an engine of random number. A pointer is passed, and its lifetime
        is controlled by the user.

    .. function:: \
        WeibullRandomNumber(const WeibullRandomNumber &o)
        WeibullRandomNumber(WeibullRandomNumber &&o)
        WeibullRandomNumber & operator=(const WeibullRandomNumber &o)
        WeibullRandomNumber & operator=(WeibullRandomNumber &&o)

        The copy for parameter is deep, but engine is shared. 


    .. function:: \
        WeibullRandomNumber & reset_state()
        WeibullRandomNumber & reset_param(result_t a = 1.0, result_t b = 1.0)
        WeibullRandomNumber & reset_param(const param_t &param)
        WeibullRandomNumber & reset_engine(engine_t *engine)
        WeibullRandomNumber & seed(seed_t seed = engine_t::default_seed)

        Setters.
        
        ``reset_state()`` : reset the internal state so that the subsequent random 
        numbers do not depend on the previous ones.

        ``reset_param()`` : reset the parameters.

        ``reset_engine()`` : reset the random number engine. Note that its life-time 
        is controlled by the user.

        ``seed()`` : reseed the random engine.

    .. function:: \
        result_t a() const
        result_t b() const
        param_t param() const
        result_t min() const
        result_t max() const
        std::pair<result_t, result_t> range() const
        engine_t * engine() const
        rng_t & rng()
        const rng_t & rng() const

        Geters.

        ``a()``, ``b()`` : the parameters of the distribution.
        
        ``param()`` : the packet of ``a`` and ``b``.
        
        ``min()``, ``max()`` : minimum and maximum value that can be returned by the random
        number generator.
        
        ``range()`` : the pair ``{min(), max()}``.
        
        ``engine()`` : the random engine. 
        
        ``rng()`` : the internal random number generator.

    .. function:: \
        result_t operator()()
        result_t get(const param_t &param)
        result_t get(result_t a, result_t b)

        Get a single random number.

        1. using current parameters.
        2. using new parameters ``param``.
        3. using new parameters ``a`` and ``b``.
        
    .. function:: \
        template<template<typename> typename Container = std::vector> Container<result_t> operator()(std::size_t n)
        template<typename Container> void operator()(Container &c, std::size_t n)
        template<typename InputIt> void operator()(InputIt b, InputIt e)

        Get a series of random numbers.

        1. Get ``n`` numbers, returned a container (must have push back).
        2. Directly pushing back into container ``c`` (``clear()`` is not called).
        3. Write into a range specified by a pair of iterator.

Extreme Value Random Number
""""""""""""""""""""""""""""""
.. class:: template<typename RealT = double, typename EngineT = DefaultRandomEngine> ExtremeValueRandomNumber

    Floating-point value :math:`x`, distributed according to the extreme value (also known
    as Gumbel Type I, log weibull, Fisher-Tippett Type I) probability density 
    function:

    .. math::

        p(x | a,b) = \frac{1}{b} \exp( ( \frac{a-x}{b} ) - \exp( \frac{a-x}{b} ) ),

    where :math:`a` is shape parameter and :math:`b` the scale parameter.


    .. type:: RealT result_t
        EngineT engine_t
        std::extreme_value_distribution<result_t> rng_t
        typename rng_t::param_type param_t
        typename engine_t::result_type seed_t

    .. function:: \
        explicit ExtremeValueRandomNumber(result_t a = 0.0, result_t b = 1.0,engine_t *engine = &global_random_engine)
        explicit ExtremeValueRandomNumber(const param_t &param, engine_t *engine = &global_random_engine)
        ~ExtremeValueRandomNumber()

        Constructors and destructor.

        ``a``, ``b``: parameters of extreme value distribution. Instead a ``param_t`` can
        be passed.
        
        ``engine``: an engine of random number. A pointer is passed, and its lifetime
        is controlled by the user.

    .. function:: \
        ExtremeValueRandomNumber(const ExtremeValueRandomNumber &o)
        ExtremeValueRandomNumber(ExtremeValueRandomNumber &&o)
        ExtremeValueRandomNumber & operator=(const ExtremeValueRandomNumber &o)
        ExtremeValueRandomNumber & operator=(ExtremeValueRandomNumber &&o)

        The copy for parameter is deep, but engine is shared. 

    .. function:: \
        ExtremeValueRandomNumber & reset_state()
        ExtremeValueRandomNumber & reset_param(result_t a = 0.0, result_t b = 1.0)
        ExtremeValueRandomNumber & reset_param(const param_t &param)
        ExtremeValueRandomNumber & reset_engine(engine_t *engine)
        ExtremeValueRandomNumber & seed(seed_t seed = engine_t::default_seed)

        Setters.
        
        ``reset_state()`` : reset the internal state so that the subsequent random 
        numbers do not depend on the previous ones.

        ``reset_param()`` : reset the parameters.

        ``reset_engine()`` : reset the random number engine. Note that its life-time 
        is controlled by the user.

        ``seed()`` : reseed the random engine.
    
    .. function:: \
        result_t a() const
        result_t b() const
        param_t param() const
        result_t min() const
        result_t max() const
        std::pair<result_t, result_t> range() const
        engine_t * engine() const
        rng_t & rng()
        const rng_t & rng() const

        Geters.

        ``a()``, ``b()`` : the parameters of the distribution.
        
        ``param()`` : the packet of ``a`` and ``b``.
        
        ``min()``, ``max()`` : minimum and maximum value that can be returned by the random
        number generator.
        
        ``range()`` : the pair ``{min(), max()}``.
        
        ``engine()`` : the random engine. 
        
        ``rng()`` : the internal random number generator.

    
    .. function:: \
        result_t operator()()
        result_t get(const param_t &param)
        result_t get(result_t a, result_t b)

        Get a single random number.

        1. using current parameters.
        2. using new parameters ``param``.
        3. using new parameters ``a`` and ``b``.


    .. function:: \
        template<template<typename> typename Container = std::vector> Container<result_t> operator()(std::size_t n)
        template<typename Container> void operator()(Container &c, std::size_t n)
        template<typename InputIt> void operator()(InputIt b, InputIt e)

        Get a series of random numbers.

        1. Get ``n`` numbers, returned a container (must have push back).
        2. Directly pushing back into container ``c`` (``clear()`` is not called).
        3. Write into a range specified by a pair of iterator.