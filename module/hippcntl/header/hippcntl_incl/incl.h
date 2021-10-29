#ifndef _HIPPCNTL_INCL_H_
#define _HIPPCNTL_INCL_H_

#include "sys_spec.h"

// Streams
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

// Containers
// ... sequential
#include <array>
#include <vector>
#include <deque>
#include <forward_list>
#include <list>
#include <stack>
#include <queue>
#include <initializer_list>
// ... associated
#include <set>
#include <unordered_set>
#include <map>
#include <unordered_map>
// ... string
#include <string>

// Algorithm
// ... sorts
#include <algorithm>
// ... function-object: less<T>, greater<T> ...
#include <functional>
// ... default_random_engine, uniform_int_distribution<T>
#include <random>
#include <limits>

// Memory
// .. bad_alloc
#include <new>
// ... swap, pair, move, forward
#include <utility>
#include <tuple>
// ... allocator
#include <memory>


// C libraries
#include <cstdlib>
#include <cstdint>
// ... size_t, ptrdiff_t
#include <cstddef>
// ... remove_reference
#include <type_traits>
// ... time(), clock(), clock_t, time_t
#include <ctime>
// ... mathematical functions
#include <cmath>
#include <stdexcept>
// ... macro assert for debug
#include <cassert>



namespace HIPP{
// Streams
// ... Standard IOs and manipulation handlers.
using std::cout; using std::cin; using std::endl; using std::cerr;
using std::ios_base;
// ... IO streams
using std::ostream; using std::istream;
using std::ifstream; using std::ofstream;
using std::istringstream; using std::ostringstream;

// Containers
using std::vector;
using std::string;

// Generic programming
using namespace std::placeholders;
using std::swap;  // always use swap(...);
// Caution: Do not 'using std::move', always call std::move(...);

// Memory and exceptions
using std::bad_alloc;

typedef std::size_t size_t;
typedef std::ptrdiff_t ptrdiff_t;

} // namespace HIPP

#endif	//_HIPPCNTL_INCL_H_
