Random Number Generators
==========================

HIPP provides a set of random number generators. The details can be found in 
the :ref:`API-Ref <api-numerical-rannum>`. 

All the below examples can be downloaded at :download:`numerical/rannum.cpp </../example/tutorial/numerical/rannum.cpp>`.
In the following, we assume the declarations of namespace::

    using namespace HIPP;
    using namespace HIPP::NUMERICAL;
    using namespace std;

which expose all the interface in the standard library and HIPP NUMERICAL module.

.. _tutorial-numeric-rannum-quick-and-dirty:

Quick-and-Dirty Random Numbers 
--------------------------------

The NUMERICAL module defines a set of global callable objects for quick-and-dirty
random number generations. For example, :var:`~HIPP::NUMERICAL::randn` gives 
standard normal random numbers, :var:`~HIPP::NUMERICAL::randint` gives random 
``int`` variables::

    pout << randn(), " and ", randint(), endl;

Outout:

.. code-block:: text 

    -1.48784 and 282475248

To get a sequence of random numbers, call them repeatly, or simply pass 
a desired number to get a ``std::vector``::
    
    vector<double> rvs = randn(5);
    pout << "random variables: {", rvs, "}", endl;

Output:

.. code-block:: text 

    random variables: {-0.218908,0.459645,0.189132,-0.531634,0.0221783}

Those generators share a common global (thread-local) random engine. The call 
on each of them has global effect, and may not be reproduciable in different runs.

A complete list of such random number generators can be found at :ref:`api-numerical-rannum-predefined-obj`.


Get Random Numbers from Generator Types
----------------------------------------

Random numbers can be generated from either "random engines" or "distributions".

- A random engine instance ``e`` typed ``E`` gives uniformly distributed unsigned integers
  typed ``E::result_type``.
- A distribution instance typed ``D`` is a combination of an engine ``E e`` and a distribution 
  generator ``G g``. It transforms the basic random numbers provided by ``e`` to those satisfying
  the specific distribution function.

Random Engines
""""""""""""""""

HIPP provides two global engine instance :var:`~HIPP::NUMERICAL::global_random_engine` (thread-local) 
and :var:`~HIPP::NUMERICAL::process_global_random_engine` (process-wide, hence shared by threads). They have 
the same type :type:`~HIPP::NUMERICAL::DefaultRandomEngine`. 

To get uniformly distributed integers from them, just call them directly::

    pout << global_random_engine(), endl;
    pout << process_global_random_engine(), endl;

Possible output:

.. code-block:: text

    114807987
    16807

Independent random engines can be declared by user. For example::

    DefaultRandomEngine e;                  // default constructed
    pout << e(), endl;

    RandomEngine::knuth_b_t e2{10086};      // provide a seed
    pout << e2(), endl;

Here ``e`` has type :type:`~HIPP::NUMERICAL::DefaultRandomEngine`, which is 
the default choice and should be good enough for most applications.
More types of engines are defined as the members of :class:`~HIPP::NUMERICAL::RandomEngine`, 
such as the :type:`~HIPP::NUMERICAL::RandomEngine::knuth_b_t`. They can be default 
constructed, or optionally with a seed.

Possible output:

.. code-block:: text

    16807
    2110709517

An engine can be re-seed by the method ``seed()``::

    e2.seed(10086);                         // re-seed the engine
    pout << e2(), endl;

With the same seed, the random engine generates the same sequence output:

.. code-block:: text

    2110709517


Random Number Distributions
""""""""""""""""""""""""""""""

HIPP provides several commonly used classes to generate random numbers 
from specific distribution functions. See the :ref:`API-Ref <api-numerical-rannum>` for details.

For example, to get uniformly distributed floating-point numbers, use 
the class template :class:`~HIPP::NUMERICAL::UniformRealRandomNumber`
with the first template argument specifying the value type. Then, 
random numbers can be drawn from the instance like in :ref:`tutorial-numeric-rannum-quick-and-dirty`::


    UniformRealRandomNumber<double> rng {0.0, 100.0};
    pout << rng(), " and ", rng(), endl;

Here we declare a random number generator which gives uniform random numbers :math:`U(0,100)`.
By default, the generator use the global engine :var:`global_random_engine`.

Possible Output:

.. code-block:: text

    38.3416 and 67.1149

To get a independent random numbers, declare a new engine and pass its pointer 
to the constructor of the distribution class::

    DefaultRandomEngine e { 10086 };
    UniformRealRandomNumber<double> rng2 {0.0, 1.0, &e};
    pout << rng2(), " and ", rng2(), endl;

Possible output:

.. code-block:: text

    0.945231 and 0.690131

The distribution class has methods to query its details. For example, 
the :class:`~HIPP::NUMERICAL::UniformRealRandomNumber` class has methods ``a()``
and ``b()`` returning its lower and upper bound. Most distributions has methods 
``min()`` and ``max()`` returning the minimal and maximal possible output values 
of the generator::

    pout << "Minimal and maximal possible values = ", 
        rng.min(), ",  ", rng.max(), '\n';
    pout << "Parameters (a and b for the distribution) = ", 
        rng.a(), ", ", rng.b(), '\n';

Output:

.. code-block:: text

    Minimal and maximal possible values = 0,  100
    Parameters (a and b for the distribution) = 0, 100

To reseed the distribution instance, call ``seed()`` and pass a new seed to 
it. 
Because a distribution class may remember some intermediate values which depend 
on the former sequence of output, 
call ``reset_state()`` to let it forget its memory. These setters return 
reference to the caller instance itself so that they can be chained to use::

    rng.seed(10086).reset_state();
    pout << "New sequence of random numbers after reseed {", rng(5), "}\n";

Possible output:

.. code-block:: text

    New sequence of random numbers after reseed {69.0131,94.5231,42.2353,2.95343,31.2548}

Distribution parameter can be adjusted by ``reset_param(new_params...)``. 
Or use ``get(params...)`` to temporarily draw samples of given parameters 
without change the distribution::

    pout << "Temporary U(0, 200) variable ", rng.get(0., 200.), '\n';
    rng.reset_param(0.0, 200.0);           // permanently change the parameters
    pout << rng(), endl;

Possible output:

.. code-block:: text

    Temporary U(0, 200) variable 166.706
    138.861







