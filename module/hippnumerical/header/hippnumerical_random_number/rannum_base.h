/**
create: Yangyao CHEN, 2021/08/12
    [write   ] Random number engine types and objects.
*/

#ifndef _HIPPNUMERICAL_RANNUM_BASE_H_
#define _HIPPNUMERICAL_RANNUM_BASE_H_

#include <random>
#include <hippcntl.h>

namespace HIPP::NUMERICAL {

struct RandomEngine {
    /**
    Predefined random number generator types.
    */
    using minstd_rand0_t          = std::minstd_rand0;
    using minstd_rand_t           = std::minstd_rand;
    using mt19937_t               = std::mt19937;
    using mt19937_64_t            = std::mt19937_64;
    using ranlux24_base_t         = std::ranlux24_base;
    using ranlux48_base_t         = std::ranlux48_base;
    using ranlux24_t              = std::ranlux24;
    using ranlux48_t              = std::ranlux48;
    using knuth_b_t               = std::knuth_b;
    using default_t               = std::default_random_engine;
    
    /**
    Non-deterministic random number generator types.
    */
    using device_t                = std::random_device;
};

/**
The default one chosen by the implementation. Generally the best in most cases.
*/
typedef RandomEngine::default_t DefaultRandomEngine;


/**
The default random engine objects used by HIPP.

The thread local one is thread-safe, but threads may have the same random
sequence. This is the default choice in the construction of distribution
generators.

The process global one is shared among threads. It is not thread-safe
and must be protected carefully.
*/
inline thread_local static 
DefaultRandomEngine global_random_engine;

inline static 
DefaultRandomEngine process_global_random_engine;
    
} // namespace HIPP::NUMERICAL


#endif	//_HIPPNUMERICAL_RANNUM_BASE_H_