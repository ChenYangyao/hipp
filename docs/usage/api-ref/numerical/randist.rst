Probability Distributions
============================

.. include:: /global.rst

.. namespace:: HIPP::NUMERICAL

Introduction
---------------

This component provides subroutines for computing probability distribution functions (i.e., pdf, cdf, etc.).
For any continuous probability distribution represented by a class ``D``, we have the following static 
methods to compute different function values:

- ``D::pdf(x, ...)`` - Probability density function at ``x``.
- ``D::cdf_lower(x, ...)`` - Cumulative probability function at ``x``.
- ``D::cdf_upper(x, ...)`` - Cumulative (from upper bound) probability function at ``x``.
- ``D::invcdf_lower(P, ...)`` - The inverse of cumulative probability function at ``P``.
- ``D::invcdf_lower(Q, ...)`` - The inverse of cumulative (from upper bound) probability function at ``Q``.

Here ``...`` represents the additional arguments specific to the target distribution.

Note that these methods are optional, i.e., for some distributions, not all of these methods are available.

The following variables, functions and classes are all defined within the namespace ``HIPP::NUMERICAL``.

**Example:** we compute the probability density function and cumulative probability function of the Gaussian
distribution by the static methods of class :class:`GaussianDist`::

    using namespace HIPP;
    using namespace HIPP::NUMERICAL;

    double sigma = 2.0;
    pout << "Gaussian distribution p(x) and cumulative P(x):\n";
    for( auto x : {0., 1., 2., 3., 4., 5.} )
        pout << "p(", x, ") = ",  GaussianDist::pdf(x, sigma), 
            ", P(", x, ") = ", GaussianDist::cdf_lower(x, sigma), endl;

The output is:

.. code-block:: text 

    Gaussian distribution p(x) and cumulative P(x):
    p(0) = 0.199471, P(0) = 0.5
    p(1) = 0.176033, P(1) = 0.691462
    p(2) = 0.120985, P(2) = 0.841345
    p(3) = 0.0647588, P(3) = 0.933193
    p(4) = 0.0269955, P(4) = 0.97725
    p(5) = 0.00876415, P(5) = 0.99379


Table of Available Distributions
""""""""""""""""""""""""""""""""

We summary the available distribution class and its methods in the following table:

.. table::
    :class: tight-table fix-width-table
    :widths: 20 30 60
    
    +------------------------------------------+--------------------------------------------------------------------------------+---------------------------------------------------------------------------------------------------------------+
    | Class                                    |   Probability Distribution                                                     |          Methods                                                                                              |
    +==========================================+================================================================================+===============================================================================================================+
    | :class:`UniformRealDist`                 | Uniform real distribution in the range :math:`[a,\,b]`                         | ``pdf(x,a,b)``, ``cdf_lower(x,a,b)``, ``cdf_upper(x,a,b)``, ``invcdf_lower(P,a,b)``, ``invcdf_upper(Q,a,b)``  |
    +------------------------------------------+--------------------------------------------------------------------------------+---------------------------------------------------------------------------------------------------------------+
    | :class:`GaussianDist`                    | Normal/Gaussian distribution with standard deviation                           | ``pdf(x,sigma)``, ``cdf_lower(x,sigma)``, ``cdf_upper(x,sigma)``, ``invcdf_lower(P,sigma)``,                  |
    |                                          | :math:`\sigma`                                                                 | ``invcdf_upper(Q,sigma)``                                                                                     |
    +------------------------------------------+--------------------------------------------------------------------------------+---------------------------------------------------------------------------------------------------------------+
    |                                          | Specific to :math:`\sigma=1`                                                   | ``pdf_unit(x)``, ``cdf_lower_unit(x)``, ``cdf_upper_unit(x)``, ``invcdf_lower_unit(P)``,                      |
    |                                          |                                                                                | ``invcdf_upper_unit(Q)``                                                                                      |       
    +------------------------------------------+--------------------------------------------------------------------------------+---------------------------------------------------------------------------------------------------------------+
    | :class:`GaussianTailDist`                | The gaussian tail distribution for :math:`x>a` with standard                   | ``pdf(x,a,sigma)``                                                                                            |
    |                                          | deviation :math:`\sigma`                                                       |                                                                                                               |
    +------------------------------------------+--------------------------------------------------------------------------------+---------------------------------------------------------------------------------------------------------------+
    |                                          | Specific to :math:`\sigma`                                                     | ``pdf_unit(x,a)``                                                                                             |
    +------------------------------------------+--------------------------------------------------------------------------------+---------------------------------------------------------------------------------------------------------------+
    | :class:`BivariateGaussianDist`           | Bivariate Gaussian distribution with standard deviations                       | ``pdf(x,y,sigma_x,sigma_y, rho)``                                                                             |
    |                                          | :math:`(\sigma_x, \sigma_y)` and correlation coefficient :math:`\rho`          |                                                                                                               |
    +------------------------------------------+--------------------------------------------------------------------------------+---------------------------------------------------------------------------------------------------------------+
    | :class:`LognormalDist`                   | Lognormal distribution for :math:`x > 0`                                       | ``pdf(x,zeta,sigma)``, ``cdf_lower(x,zeta,sigma)``, ``cdf_upper(x,zeta,sigma)``,                              |
    |                                          |                                                                                | ``invcdf_lower(P,zeta,sigma)``, ``invcdf_upper(Q,zeta,sigma)``                                                |
    +------------------------------------------+--------------------------------------------------------------------------------+---------------------------------------------------------------------------------------------------------------+
    | :class:`ExpPowDist`                      | Exponential power distribution functions with scale parameter :math:`a`        | ``pdf(x,a,b)``, ``cdf_lower(x,a,b)``, ``cdf_upper(x,a,b)``                                                    |
    |                                          | and exponential :math:`b`                                                      |                                                                                                               |
    +------------------------------------------+--------------------------------------------------------------------------------+---------------------------------------------------------------------------------------------------------------+
    | :class:`ExponentialDist`                 | Exponential distribution functions with mean :math:`\mu`                       | ``pdf(x,mu)``, ``cdf_lower(x,mu)``, ``cdf_upper(x,mu)``, ``invcdf_lower(P,mu)``, ``invcdf_upper(Q,mu)``       |
    +------------------------------------------+--------------------------------------------------------------------------------+---------------------------------------------------------------------------------------------------------------+
    | :class:`LaplaceDist`                     | Laplace distribution functions with with :math:`a`                             | ``pdf(x,a)``, ``cdf_lower(x,a)``, ``cdf_upper(x,a)``, ``invcdf_lower(P,a)``, ``invcdf_upper(Q,a)``            |
    +------------------------------------------+--------------------------------------------------------------------------------+---------------------------------------------------------------------------------------------------------------+
    | :class:`CauchyDist`                      | Cauchy distribution (also known as Lorentz distribution)                       | ``pdf(x,a)``, ``cdf_lower(x,a)``, ``cdf_upper(x,a)``, ``invcdf_lower(P,a)``, ``invcdf_upper(Q,a)``            |
    |                                          | with scale parameter :math:`a`                                                 |                                                                                                               |
    +------------------------------------------+--------------------------------------------------------------------------------+---------------------------------------------------------------------------------------------------------------+
    | :class:`RayleighDist`                    | Rayleigh distribution with scale parameter :math:`\sigma`                      | ``pdf(x,sigma)``, ``cdf_lower(x,sigma)``, ``cdf_upper(x,sigma)``,                                             |
    |                                          |                                                                                | ``invcdf_lower(P,sigma)``, ``invcdf_upper(Q,sigma)``                                                          |
    +------------------------------------------+--------------------------------------------------------------------------------+---------------------------------------------------------------------------------------------------------------+
    | :class:`RayleighTailDist`                | Tail of Rayleigh distribution with scale parameter :math:`\sigma`              | ``pdf(x,a,sigma)``                                                                                            |
    |                                          | and a lower limit :math:`a`                                                    |                                                                                                               |
    +------------------------------------------+--------------------------------------------------------------------------------+---------------------------------------------------------------------------------------------------------------+


Uniform Real Distribution
----------------------------

.. class:: UniformRealDist

    ``UniformRealDist`` - the distribution function is uniform in :math:`[a, b]`, i.e.,

    .. math::
        
        p(x) = \frac{1}{b-a} .

    .. function:: \
        static double pdf(double x, double a, double b) noexcept
        static double cdf_lower(double x, double a, double b) noexcept
        static double cdf_upper(double x, double a, double b) noexcept
        static double invcdf_lower(double P, double a, double b) noexcept
        static double invcdf_upper(double Q, double a, double b) noexcept

Gaussian Distributions
-----------------------------------------

Gaussian Distribution
"""""""""""""""""""""""""

.. class:: GaussianDist

    The Normal (Gaussian) distribution with standard deviation :math:`\sigma`.

    .. function:: \
        static double pdf(double x, double sigma) noexcept
        static double cdf_lower(double x, double sigma) noexcept
        static double cdf_upper(double x, double sigma) noexcept
        static double invcdf_lower(double P, double sigma) noexcept
        static double invcdf_upper(double Q, double sigma) noexcept

        Calls for the Gaussian dist with standard deviation :math:`\sigma`.

    .. function:: \
        static double pdf_unit(double x) noexcept
        static double cdf_lower_unit(double x) noexcept
        static double cdf_upper_unit(double x) noexcept
        static double invcdf_lower_unit(double P) noexcept
        static double invcdf_upper_unit(double Q) noexcept

        Calls for the unit gaussian dist., i.e., :math:`\sigma = 1`.


Gaussian Tail Distribution
"""""""""""""""""""""""""""

.. class:: GaussianTailDist

    GaussianTailDist - for the gaussian tail distribution, i.e., its pdf is the 
    upper tail of a full normal distribution :math:`\mathcal{N}(x|0, \sigma)`, with :math:`x > a`.

    .. function:: \
        static double pdf(double x, double a, double sigma) noexcept
        static double pdf_unit(double x, double a) noexcept

Bivariate Gaussian Distribution
"""""""""""""""""""""""""""""""""

.. class:: BivariateGaussianDist

    .. function:: \
        static double pdf(double x, double y, double sigma_x, double sigma_y, double rho) noexcept

Lognormal Distribution
"""""""""""""""""""""""""

.. class:: LognormalDist

    ``LognormalDist`` - the lognormal distribution for :math:`x > 0`, i.e.

    .. math::

        p(x) =  \frac{1}{ x \sqrt{ 2 \pi \sigma^2} } \exp( - \frac{( {\rm ln}(x) - \zeta )^2}{2 \sigma^2} ) .
    
    .. function:: \
        static double pdf(double x, double zeta, double sigma) noexcept
        static double cdf_lower(double x, double zeta, double sigma) noexcept 
        static double cdf_upper(double x, double zeta, double sigma) noexcept 
        static double invcdf_lower(double P, double zeta, double sigma) noexcept
        static double invcdf_upper(double Q, double zeta, double sigma) noexcept

Exponential Distributions
--------------------------------

Exponential Power Distribution
"""""""""""""""""""""""""""""""""

.. class:: ExpPowDist

    ``ExpPowDist`` - exponential power distribution functions with scale parameter ``a`` and
    exponential ``b``, i.e.

    .. math::

        p(x) = \frac{1}{ 2 a \Gamma(1+1/b) } \exp( -| x / a |^b ), 
        
    for :math:`x >= 0`.

    Special cases:

    - :math:`b = 1`: Laplace distribution.
    - :math:`b = 2`: Gaussian distribution with :math:`a = \sqrt{2} \sigma`.

    .. function:: \
        static double pdf(double x, double a, double b) noexcept
        static double cdf_lower(double x, double a, double b) noexcept
        static double cdf_upper(double x, double a, double b) noexcept

Exponential Distribution
"""""""""""""""""""""""""""""""""

.. class:: ExponentialDist

    ``ExponentialDist`` - exponential distribution functions with mean :math:`\mu`, i.e.,

    .. math::

        p(x) = \frac{1}{\mu} \exp( - \frac{x}{\mu} ), 
        
    for :math:`x \geqslant 0`.

    .. function:: \
        static double pdf(double x, double mu) noexcept
        static double cdf_lower(double x, double mu) noexcept
        static double cdf_upper(double x, double mu) noexcept
        static double invcdf_lower(double P, double mu) noexcept
        static double invcdf_upper(double Q, double mu) noexcept

Laplace Distribution
----------------------

.. class:: LaplaceDist

    LaplaceDist - laplace distribution functions with with :math:`a`, i.e.,

    .. math::

        p(x) = \frac{1}{2a} \exp( -|x/a| ), 
        
    for :math:`x \in (-\infty, \infty)`.

    .. function:: \
        static double pdf(double x, double a) noexcept
        static double cdf_lower(double x, double a) noexcept
        static double cdf_upper(double x, double a) noexcept
        static double invcdf_lower(double P, double a) noexcept
        static double invcdf_upper(double Q, double a) noexcept

Cauchy Distribution
----------------------

.. class:: CauchyDist

    Cauchy distribution (also known as Lorentz distribution) with scale parameter 
    :math:`a`, i.e.,

    .. math::

        p(x) = \frac{1}{a \pi ( 1 + (x/a)^2 )}, 
        
    for :math:`x \in (-\infty, \infty)`.

    .. function:: \
        double pdf(double x, double a) noexcept
        double cdf_lower(double x, double a) noexcept
        double cdf_upper(double x, double a) noexcept
        double invcdf_lower(double P, double a) noexcept
        double invcdf_upper(double Q, double a) noexcept


Rayleigh Distributions
-----------------------

Rayleigh Distribution
"""""""""""""""""""""""""""""""""

.. class:: RayleighDist

    Rayleigh distribution with scale parameter :math:`\sigma`, i.e.,

    .. math::
    
        p(x) = \frac{x}{\sigma^2} \exp( - \frac{x^2}{2 \sigma^2} ), 
        
    for :math:`x > 0`.

    .. function:: \
        double pdf(double x, double sigma) noexcept
        double cdf_lower(double x, double sigma) noexcept
        double cdf_upper(double x, double sigma) noexcept
        double invcdf_lower(double P, double sigma) noexcept
        double invcdf_upper(double Q, double sigma) noexcept

Rayleigh Tail Distribution
"""""""""""""""""""""""""""""""""

.. class:: RayleighTailDist

    Tail of Rayleigh distribution with scale parameter :math:`\sigma` and a lower limit :math:`a`,
    i.e.,

    .. math::

        p(x) = \frac{x}{\sigma^2} \exp( \frac{a^2-x^2}{2 \sigma^2} ), 
    
    for :math:`x > a`.
  
    .. function:: \
        double pdf(double x, double a, double sigma) noexcept