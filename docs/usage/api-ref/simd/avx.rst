AVX (256-bit) Vectors 
==============================

The following classes are all defined within namespace ``HIPP::SIMD``.

.. namespace:: HIPP::SIMD

Vec<double, 4>
-----------------------------

.. class:: template<> Vec<double, 4>
    
    A vector of four double-precision (256 bits in total) values. ``Vec<double, 4>`` can be 
    **copied**, **copy-constructed**, **moved**, and **move-constructed**. The copy and move 
    operations and destructor are all ``noexcept``.

    ``Vec<double, 4>`` is binary-compatible with the intrinsic type  ``__m256d``, i.e., 
    they have the same length and alignment.


    .. type::   double scal_t 
                float scal_hp_t
                __m256d vec_t 
                __m128d vec_hc_t 
                __m128 vec_hp_t
                int64_t iscal_t 
                int32_t iscal_hp_t
                __m256i ivec_t
                __m128d ivec_hp_t
                __mmask8 mask8_t

        Type aliases. ``scal_t`` is the scalar type (i.e., element type) of the SIMD vector. 
        ``scal_hp_t`` is half-precision scalar type. 
        ``vec_t`` is the intrinsic SIMD vector, ``vec_hc_t`` and ``vec_hp_t`` represent 
        the half-precision and half-count types, respectively.

        Scalar and vector types for integers are also defined.  

    .. enum::   @simd_vecd4enum : size_t 

        .. enumerator:: NPACK=4
                        NBIT=256
                        VECSIZE=sizeof(vec_t)
                        SCALSIZE=sizeof(scal_t)

        ``NPACK`` is the number of scalars in the vector, ``NBIT`` is the number of bits of the 
        vector register. ``VECSIZE`` and ``SCALSIZE`` are size in bytes of the vector and scalar. 

    .. function::   Vec() noexcept
                    Vec( scal_t e3, scal_t e2, scal_t e1, scal_t e0 ) noexcept
                    explicit Vec( scal_t a ) noexcept
                    explicit Vec( caddr_t mem_addr ) noexcept
                    Vec( const vec_t &a ) noexcept
                    Vec( const scal_t *base_addr, \
                        ivec_t vindex, const int scale=SCALSIZE ) noexcept
                    Vec( vec_t src, const scal_t *base_addr, \ 
                        ivec_t vindex, vec_t mask, const int scale=SCALSIZE ) noexcept
        
        Initializers.
        
        (1) Default Initializer: ``Vec()`` gives an un-initialized vector.
        (2) ``Vec(e3, e2, e1, e0)`` constructs a vector of four given elements, from higher address value ``e3``, to 
            lower address value ``e0``.
        (3) ``Vec(scal_t a)`` constructs a vector of four repeated scalar value ``a``.
        (4) ``Vec(mem_addr)``: the four elements are loaded from the memory address ``mem_addr`` (must be aligned at 32-byte boundary).
        (5) ``Vec(const vec_t &a)``: copy the intrinsic vector ``a``.
        (6) ``Vec(base_addr, vindex, scale)``: load using :func:`gather`.
        (7) ``Vec(src, base_addr, vindex, mask, scale)``: load using :func:`gatherm`.

        The address type ``caddr_t`` can be either ``const double *``,
        ``const vec_t *`` or ``const Vec<double, 4> *``.

    .. function::   ostream & info( ostream &os = cout, int fmt_cntl = 1 ) const
                    friend ostream & operator<<( ostream &os, const Vec &v )

        ``info()`` displays the content of the vector to ``os``.

        :arg fmt_cntl:  Control the display format. 0 for an inline printing and 1 for a verbose, multiple-line version.
        :return: The argument ``os`` is returned.

        The overloaded `<<` operator is equivalent to ``info()`` with default 
        ``fmt_cntl``.

        The returned reference to ``os`` allows you to chain the outputs, such as 
        ``vec.info(cout) << " continue printing " << std::endl``.

    .. function::   const vec_t & val() const noexcept
                    vec_t & val() noexcept
                    const scal_t & operator[]( size_t n )const noexcept
                    scal_t & operator[]( size_t n ) noexcept

        ``val()`` return the intrinsic vector value. ``operator[](n)`` takes the n-th scalar element
        from the vector. 

    .. function::   Vec & load( caddr_t mem_addr ) noexcept
                    Vec & loadu( caddr_t mem_addr ) noexcept
                    Vec & loadm( caddr_t mem_addr, ivec_t mask ) noexcept
                    Vec & load1( const scal_t *mem_addr ) noexcept
                    Vec & bcast( const scal_t *mem_addr ) noexcept
                    Vec & bcast( const vec_hc_t *mem_addr ) noexcept
                    Vec & gather( const scal_t *base_addr, ivec_t vindex, \
                        const int scale=SCALSIZE ) noexcept
                    Vec & gatherm( vec_t src, const scal_t *base_addr, ivec_t vindex, \
                        vec_t mask, const int scale=SCALSIZE ) noexcept
                    Vec & gather_idxhp( const scal_t *base_addr, ivec_hp_t vindex, \
                        const int scale=SCALSIZE ) noexcept
                    Vec & gatherm_idxhp( vec_t src, const scal_t *base_addr, ivec_hp_t vindex, \ 
                        vec_t mask, const int scale=SCALSIZE ) noexcept

        Load operations: load data from memory. The address type ``caddr_t`` can be either ``const double *``,
        ``const vec_t *`` or ``const Vec<double, 4> *``.

        (1) ``load()`` loads a pack of 4 double precision floating-point scalar values into the calling 
            instance from the aligned address ``mem_addr``.
        (2) ``loadu()`` allows that ``mem_addr`` is not aligned.
        (3) ``loadm()`` uses ``mask`` (elements are zeroed out when the highest
            bit of the corresponding element is not set).
        (4) ``load1()`` load a single scalar value and repeats it four times to make a vector.
        (5) ``bcast(const scal_t *)`` is the same as ``load1()``.
        (6) ``bcast(const vec_hc_t *)`` loads two scalar values and repeats them twice to make a vector.
        (7) ``gather()`` loads 4 scalar values from address starting at ``base_addr``, each offset by 
            the corresponding 64-bit element in ``vindex`` (in bytes, and scaled by ``scale``; ``scale`` can be 1, 2, 4, or 8).
        (8) ``gatherm()`` is the same as ``gather()`` but using ``mask`` (elements are copied from src when the 
            highest bit is not set in the corresponding element). 
        (9) ``gather_idxhp()`` is like ``gather()`` but uses 32-bit offset.
        (10) ``gatherm_idxhp()`` us like ``gatherm()`` but uses 32-bit offset.

    .. function::   const Vec & store( addr_t mem_addr ) const noexcept
                    const Vec & storeu( addr_t mem_addr ) const noexcept
                    const Vec & storem( addr_t mem_addr, ivec_t mask ) const noexcept
                    const Vec & stream( addr_t mem_addr ) const noexcept
                    const Vec & scatter( void *base_addr, \
                        ivec_t vindex, int scale=SCALSIZE ) const noexcept
                    const Vec & scatterm( void *base_addr, mask8_t k,\
                        ivec_t vindex, int scale=SCALSIZE ) const noexcept
                    const Vec & scatter_idxhp( void *base_addr, \
                        ivec_hp_t vindex, int scale=SCALSIZE ) const noexcept
                    const Vec & scatterm_idxhp( void *base_addr, mask8_t k,\
                        ivec_hp_t vindex, int scale=SCALSIZE ) const noexcept
                    Vec & store( addr_t mem_addr ) noexcept
                    Vec & storeu( addr_t mem_addr ) noexcept
                    Vec & storem( addr_t mem_addr, ivec_t mask ) noexcept
                    Vec & stream( addr_t mem_addr ) noexcept
                    Vec & scatter( void *base_addr, \
                        ivec_t vindex, int scale=SCALSIZE ) noexcept
                    Vec & scatterm( void *base_addr, mask8_t k,\
                        ivec_t vindex, int scale=SCALSIZE ) noexcept
                    Vec & scatter_idxhp( void *base_addr, \
                        ivec_hp_t vindex, int scale=SCALSIZE ) noexcept
                    Vec & scatterm_idxhp( void *base_addr, mask8_t k,\
                        ivec_hp_t vindex, int scale=SCALSIZE ) noexcept
        
        Store operations: store element from the current instance to a memory location. The address type 
        ``addr_t`` can be either ``double *``, ``vec_t *`` or ``Vec<double, 4> *``.

        Each store operation has a non-``const`` version used for a non-constant instance.

        All the store operations return the reference to the instance itself.

        (1) ``store()`` stores 4 double precision floating-point scalar values into the aligned address ``mem_addr``.
        (2) ``storeu()`` does not need the address to be aligned.
        (3) ``storem()`` uses the ``mask`` (elements are not stored when the highest bit is not set in the corresponding element).
        (4) ``stream()`` uses a non-temporal memory hint. ``mem_addr`` must be aligned. 
        (5) ``scatter()`` stores elements into the address starting at ``base_addr`` and offset by 
            each 64-bit element in ``vindex`` (in byte, and scaled by ``scale``; ``scale`` can be 1, 2, 4, or 8).
        (6) ``scatterm()`` is the same as ``scatter()`` but uses a ``mask`` (elements are not stored when 
            the corresponding mask bit is not set).
        (7) ``scatter_idxhp()`` is the same as ``scatter()`` but uses 32-bit offset.
        (8) ``scatterm_idxhp()`` is the same as ``scatterm()`` but uses 32-bit offset. 

    .. function::   scal_t to_scal( )const noexcept
                    int movemask( ) const noexcept
                    Vec movedup( ) const noexcept

        ``to_scal()`` returns the lower double-precision floating-point scalar value. 
        ``movemask()`` sets each bit of the returned value based on the corresponding most 
        significate bit in each double precision floating-point scalar value.
        ``movedup()`` duplicates even-indexed scalar values.

    .. function::   Vec & set( scal_t e3, scal_t e2, scal_t e1, scal_t e0 ) noexcept
                    Vec & set1( scal_t a ) noexcept
                    Vec & set1( vec_hc_t a ) noexcept
                    Vec & set() noexcept
                    Vec & setzero() noexcept
                    Vec & undefined() noexcept
                    
        Set the scalar values of the calling instance.

        (1) ``set(e3,e2,e1,e0)`` sets each elements from the higher address value ``e3`` to lower address value ``e0``.
        (2) ``set1(scal_t a)`` repeats a scalar value 4 times.
        (3) ``set1(vec_hc_t a)`` repeats the lower scalar value of ``a`` 4 times.
        (4) ``set()`` is the same as ``setzero()``.
        (5) ``setzero()`` set all bits to zero.
        (6) ``undefined()`` set scalars to undefined values.

    
    .. function::   Vec operator+( const Vec &a ) const noexcept
                    Vec operator-( const Vec &a ) const noexcept
                    Vec operator*( const Vec &a ) const noexcept
                    Vec operator/( const Vec &a ) const noexcept
                    Vec operator++(int) noexcept
                    Vec & operator++() noexcept
                    Vec operator--(int) noexcept
                    Vec & operator--() noexcept
                    Vec & operator+=( const Vec &a ) noexcept
                    Vec & operator-=( const Vec &a ) noexcept
                    Vec & operator*=( const Vec &a ) noexcept
                    Vec & operator/=( const Vec &a ) noexcept
                    Vec hadd( const Vec &a ) const noexcept
                    Vec hsub( const Vec &a ) const noexcept

        Arithmetic operations. All of the above operations are element-wise.

        ``hadd()`` performs horizontal addtion, i.e., the result of a.hadd(b) is { a[0]+a[1], b[0]+b[1], a[2]+a[3], b[2]+b[3] }.
        ``hsub()`` performs horizontal subtration, i.e., the result of a.hsub(b) is { a[0]-a[1], b[0]-b[1], a[2]-a[3], b[2]-b[3] }.

    .. function::   Vec operator&( const Vec &a ) const noexcept
                    Vec andnot( const Vec &a ) const noexcept
                    Vec operator|( const Vec &a ) const noexcept
                    Vec operator~()const noexcept
                    Vec operator^( const Vec &a ) const noexcept
                    Vec & operator&=( const Vec &a ) noexcept
                    Vec & operator|=( const Vec &a ) noexcept
                    Vec & operator^=( const Vec &a ) noexcept

        Bitwise Logic operations.
    
    .. function::   Vec operator==( const Vec &a ) const noexcept
                    Vec operator!=( const Vec &a ) const noexcept
                    Vec operator<( const Vec &a ) const noexcept
                    Vec operator<=( const Vec &a ) const noexcept
                    Vec operator>( const Vec &a ) const noexcept
                    Vec operator>=( const Vec &a ) const noexcept

        Relation (comparison) operations. The comparision is element-wise for each scalar.
        If true, all the bits are set in the corresponding result element. 

    .. function::   Vec blend( const Vec &a, const int imm8 ) const noexcept
                    Vec blend( const Vec &a, const Vec &mask ) const noexcept

        Blend two vectors using control mask ``imm8``. For each bit in ``imm8``, if set, taken 
        the corresponding result element from ``b``, otherwise from ``a``.

        The second version uses a vector ``mask``, i.e., each mask bit is taken from the highest 
        bit of the corresponding 64-bit elements.

    .. function::   Vec sqrt() const noexcept
                    Vec ceil() const noexcept
                    Vec floor() const noexcept
                    Vec round( const int rounding ) const noexcept
                    Vec max( const Vec &a ) const noexcept
                    Vec min( const Vec &a ) const noexcept
                    Vec sin() const noexcept
                    Vec cos() const noexcept
                    Vec log() const noexcept
                    Vec exp() const noexcept
                    Vec pow( const Vec &a ) const noexcept

        Elementary math functions. ``sin()``, ``cos()``, ``log()``, ``exp()``, ``pow()`` may not 
        be serialized, depending on the compiler.


Vector<float, 8>
----------------------

.. class::  template<> Vec<float, 8>

    A vector of eight single-precision (256 bits in total) values. ``Vec<float, 4>`` can be 
    **copied**, **copy-constructed**, **moved**, and **move-constructed**. The copy and move 
    operations and destructor are all ``noexcept``.

    ``Vec<float, 4>`` is binary-compatible with the intrinsic type  ``__m256``, i.e., 
    they have the same length and alignment.


    .. type::   float scal_t 
                __m256 vec_t 
                __m128 vec_hc_t 
                int32_t iscal_t 
                __m256i ivec_t
                __mmask8 mask8_t 

        Type aliases. ``scal_t`` is the scalar type (i.e., element type) of the SIMD vector. 
        ``vec_t`` is the intrinsic SIMD vector, ``vec_hc_t`` represents
        the half-count type.

    .. enum::   @simd_vecd4enum : size_t 

        .. enumerator:: NPACK=8
                        NBIT=256
                        VECSIZE=sizeof(vec_t)
                        SCALSIZE=sizeof(scal_t)

        ``NPACK`` is the number of scalars in the vector, ``NBIT`` is the number of bits of the 
        vector register. ``VECSIZE`` and ``SCALSIZE`` are size in bytes of the vector and scalar. 

    .. function::   Vec() noexcept
                    Vec( scal_t e7, scal_t e6, scal_t e5, scal_t e4, \
                        scal_t e3, scal_t e2, scal_t e1, scal_t e0 ) noexcept
                    explicit Vec( scal_t a ) noexcept
                    explicit Vec( caddr_t mem_addr ) noexcept
                    Vec( const vec_t &a ) noexcept
                    Vec( const scal_t *base_addr, ivec_t vindex, \
                        const int scale=SCALSIZE ) noexcept
                    Vec( vec_t src, const scal_t *base_addr, \
                        ivec_t vindex, vec_t mask, const int scale ) noexcept

        Initializers.
        
        (1) Default Initializer: ``Vec()`` gives an un-initialized vector.
        (2) ``Vec(e7, e6, ..., e0)`` constructs a vector of eight given elements from 
            higher address value ``e7``, to lower address value ``e0``.
        (3) ``Vec(scal_t a)`` constructs a vector of eight repeated scalar value ``a``.
        (4) ``Vec(mem_addr)``: the eight elements are loaded from the memory address ``mem_addr`` (must be aligned at 32-byte boundary).
        (5) ``Vec(const vec_t &a)``: copy the intrinsic vector ``a``.
        (6) ``Vec(base_addr, vindex, scale)``: load using :func:`gather`.
        (7) ``Vec(src, base_addr, vindex, mask, scale)``: load using :func:`gatherm`.

        The address type ``caddr_t`` can be either ``const float *``,
        ``const vec_t *`` or ``const Vec<float, 8> *``.

    .. function::   ostream & info( ostream &os = cout, int fmt_cntl = 1 ) const
                    friend ostream & operator<<( ostream &os, const Vec &v )

        ``info()`` displays the content of the vector to ``os``.

        :arg fmt_cntl:  Control the display format. 0 for an inline printing and 1 for a verbose, multiple-line version.
        :return: The argument ``os`` is returned.

        The overloaded `<<` operator is equivalent to ``info()`` with default 
        ``fmt_cntl``.

        The returned reference to ``os`` allows you to chain the outputs, such as 
        ``vec.info(cout) << " continue printing " << std::endl``.

    .. function::   const vec_t & val() const noexcept
                    vec_t & val() noexcept
                    const scal_t & operator[]( size_t n ) const noexcept
                    scal_t & operator[]( size_t n ) noexcept
        
        ``val()`` return the intrinsic vector value. ``operator[](n)`` takes the n-th scalar element
        from the vector. 

    .. function::   Vec & load( caddr_t mem_addr ) noexcept
                    Vec & loadu( caddr_t mem_addr ) noexcept
                    Vec & loadm( caddr_t mem_addr, ivec_t mask ) noexcept
                    Vec & load1( const scal_t *mem_addr ) noexcept
                    Vec & bcast( const scal_t *mem_addr ) noexcept
                    Vec & bcast( const vec_hc_t *mem_addr ) noexcept
                    Vec & gather( const scal_t *base_addr, \
                        ivec_t vindex, const int scale=SCALSIZE ) noexcept
                    Vec & gatherm( vec_t src, const scal_t *base_addr, \
                        ivec_t vindex, vec_t mask, const int scale=SCALSIZE ) noexcept
        
        Load operations: load data from memory. The address type ``caddr_t`` can be either ``const double *``,
        ``const vec_t *`` or ``const Vec<double, 4> *``.

        (1) ``load()`` loads a pack of 8 single precision floating-point scalar values into the calling 
            instance from the aligned address ``mem_addr``.
        (2) ``loadu()`` allows that ``mem_addr`` is not aligned.
        (3) ``loadm()`` uses ``mask`` (elements are zeroed out when the highest
            bit of the corresponding element is not set).
        (4) ``load1()`` load a single scalar value and repeats it eight times to make a vector.
        (5) ``bcast(const scal_t *)`` is the same as ``load1()``.
        (6) ``bcast(const vec_hc_t *)`` loads four scalar values and repeats them twice to make a vector.
        (7) ``gather()`` loads 8 scalar values from address starting at ``base_addr``, each offset by 
            the corresponding 32-bit element in ``vindex`` (in bytes, and scaled by ``scale``; ``scale`` can be 1, 2, 4, or 8).
        (8) ``gatherm()`` is the same as ``gather()`` but using ``mask`` (elements are copied from src when the 
            highest bit is not set in the corresponding element). 
    
    .. function::   const Vec & store( addr_t mem_addr ) const noexcept
                    const Vec & storeu( addr_t mem_addr ) const noexcept
                    const Vec & storem( addr_t mem_addr, ivec_t mask ) const noexcept
                    const Vec & stream( addr_t mem_addr ) const noexcept
                    const Vec & scatter(void *base_addr, ivec_t vindex, \
                        int scale=SCALSIZE) const noexcept
                    const Vec & scatterm(void *base_addr, mask8_t k, ivec_t vindex, \
                        int scale=SCALSIZE) const noexcept
                    Vec & store( addr_t mem_addr ) noexcept
                    Vec & storeu( addr_t mem_addr ) noexcept
                    Vec & storem( addr_t mem_addr, ivec_t mask ) noexcept
                    Vec & stream( addr_t mem_addr ) noexcept
                    Vec & scatter(void *base_addr, ivec_t vindex, \
                        int scale=SCALSIZE) noexcept
                    Vec & scatterm(void *base_addr, mask8_t k, ivec_t vindex, \
                        int scale=SCALSIZE) noexcept

        Store operations: store element from the current instance to a memory location. The address type 
        ``addr_t`` can be either ``double *``, ``vec_t *`` or ``Vec<double, 4> *``.

        Each store operation has a non-``const`` version used for a non-constant instance.

        All the store operations return the reference to the instance itself.

        (1) ``store()`` stores 8 single precision floating-point scalar values into the aligned address ``mem_addr``.
        (2) ``storeu()`` does not need the address to be aligned.
        (3) ``storem()`` uses the ``mask`` (elements are not stored when the highest bit is not set in the corresponding element).
        (4) ``stream()`` uses a non-temporal memory hint. ``mem_addr`` must be aligned. 
        (5) ``scatter()`` stores elements into the address starting at ``base_addr`` and offset by 
            each 32-bit element in ``vindex`` (in byte, and scaled by ``scale``; ``scale`` can be 1, 2, 4, or 8).
        (6) ``scatterm()`` is the same as ``scatter()`` but uses a ``mask`` (elements are not stored when 
            the corresponding mask bit is not set).

    .. function::   scal_t to_scal( )const noexcept
                    int movemask( ) const noexcept
                    Vec movehdup( ) const noexcept
                    Vec moveldup( ) const noexcept

        ``to_scal()`` returns the lower single precision floating-point scalar value. 
        ``movemask()`` sets each bit of the returned value based on the corresponding most 
        significate bit in each single precision floating-point scalar value.
        ``movehdup()`` duplicates odd-indexed scalar values.
        ``moveldup()`` duplicates even-indexed scalar values.

    .. function::   Vec & set( scal_t e7, scal_t e6, scal_t e5, scal_t e4, \
                        scal_t e3, scal_t e2, scal_t e1, scal_t e0 ) noexcept
                    Vec & set1( scal_t a ) noexcept
                    Vec & set1( vec_hc_t a ) noexcept
                    Vec & set() noexcept
                    Vec & setzero() noexcept
                    Vec & undefined() noexcept

        Set the scalar values of the calling instance.

        (1) ``set(e7,e6,...,e0)`` sets each elements from the higher address value ``e7`` to lower address value ``e0``.
        (2) ``set1(scal_t a)`` repeats a scalar value 8 times.
        (3) ``set1(vec_hc_t a)`` repeats the lower scalar value of ``a`` 8 times.
        (4) ``set()`` is the same as ``setzero()``.
        (5) ``setzero()`` set all bits to zero.
        (6) ``undefined()`` set scalars to undefined values.

    .. function::   Vec operator+( const Vec &a ) const noexcept
                    Vec operator-( const Vec &a ) const noexcept
                    Vec operator*( const Vec &a ) const noexcept
                    Vec operator/( const Vec &a ) const noexcept
                    Vec operator++(int) noexcept
                    Vec & operator++() noexcept
                    Vec operator--(int) noexcept
                    Vec & operator--() noexcept
                    Vec & operator+=(const Vec &a) noexcept
                    Vec & operator-=(const Vec &a) noexcept
                    Vec & operator*=(const Vec &a) noexcept
                    Vec & operator/=(const Vec &a) noexcept
                    Vec hadd( const Vec &a ) const noexcept
                    Vec hsub( const Vec &a ) const noexcept

        Arithmetic operations. All of the above operations are element-wise.

        ``hadd()`` performs horizontal addtion, i.e., the result of a.hadd(b) is { a[0]+a[1], a[2]+a[3], b[0]+b[1], b[2]+b[3], ..., b[4]+b[5], b[6]+b[7] }.
        ``hsub()`` performs horizontal subtration, i.e., the result of a.hsub(b) is { a[0]-a[1], a[2]-a[3], b[0]-b[1], b[2]-b[3], ..., b[4]-b[5], b[6]-b[7] }.

    .. function::   Vec operator&( const Vec &a ) const noexcept
                    Vec andnot( const Vec &a ) const noexcept
                    Vec operator|( const Vec &a ) const noexcept
                    Vec operator~()const noexcept
                    Vec operator^( const Vec &a ) const noexcept
                    Vec & operator&=( const Vec &a ) noexcept
                    Vec & operator|=( const Vec &a ) noexcept
                    Vec & operator^=( const Vec &a ) noexcept

        Bitwise Logic operations.
    
    .. function::   Vec operator==( const Vec &a ) const noexcept
                    Vec operator!=( const Vec &a ) const noexcept
                    Vec operator<( const Vec &a ) const noexcept
                    Vec operator<=( const Vec &a ) const noexcept
                    Vec operator>( const Vec &a ) const noexcept
                    Vec operator>=( const Vec &a ) const noexcept

        Relation (comparison) operations. The comparision is element-wise for each scalar.
        If true, all the bits are set in the corresponding result element. 

    .. function::   Vec blend( const Vec &a, const int imm8 ) const noexcept
                    Vec blend( const Vec &a, const Vec &mask ) const noexcept

        Blend two vectors using control mask ``imm8``. For each bit in ``imm8``, if set, taken 
        the corresponding result element from ``b``, otherwise from ``a``.

        The second version uses a vector ``mask``, i.e., each mask bit is taken from the highest 
        bit of the corresponding 64-bit elements.

    .. function::   Vec rcp() const noexcept
                    Vec sqrt() const noexcept
                    Vec rsqrt() const noexcept
                    Vec ceil() const noexcept
                    Vec floor() const noexcept
                    Vec round( const int rounding ) const noexcept
                    Vec max( const Vec &a ) const noexcept
                    Vec min( const Vec &a ) const noexcept

        Elementary math functions.

    .. function::   Vec log2_fast( ) const noexcept
                    Vec log_fast( ) const noexcept
                    Vec log10_fast( ) const noexcept
                    Vec log2_faster( ) const noexcept
                    Vec log_faster( ) const noexcept
                    Vec log10_faster( ) const noexcept

                    Vec pow2_fast() const noexcept
                    Vec exp_fast() const noexcept
                    Vec pow10_fast() const noexcept
                    Vec pow2_faster() const noexcept
                    Vec exp_faster() const noexcept
                    Vec pow10_faster() const noexcept

        Vectorized math functions. These functions are not supported by hardware. 
        They are implemented by approximation algorithms. ``xxx_faster()`` is 
        faster than ``xxx_fast()``, but has lower precision.