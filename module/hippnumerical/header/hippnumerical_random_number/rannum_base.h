#ifndef _HIPPNUMERICAL_RANNUM_BASE_H_
#define _HIPPNUMERICAL_RANNUM_BASE_H_

#include <random>
#include <hippcntl.h>

namespace HIPP::NUMERICAL {

typedef std::default_random_engine DefaultRandomEngine;

/**
 * The default random engine objects for HIPP.
 * 
 * The thread local one is thread-safe, but threads may have the same random
 * sequence. This is the default choice in the construction of distribution
 * generators.
 * 
 * The process global one is shared among threads. It is not thread-safe
 * and must be protected carefully.
 */
inline thread_local static 
DefaultRandomEngine global_random_engine;

inline static 
DefaultRandomEngine process_global_random_engine;
    
} // namespace HIPP::NUMERICAL


#endif	//_HIPPNUMERICAL_RANNUM_BASE_H_