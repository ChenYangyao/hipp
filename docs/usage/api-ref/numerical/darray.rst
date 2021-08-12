Dynamic Arrays 
================

.. include:: /global.rst

The following variables, functions and classes are all defined within the namespace ``HIPP::NUMERICAL``.

.. namespace:: HIPP::NUMERICAL

DArray
---------------

DArrayBase 
""""""""""""""""""

.. class:: DArrayBase

    Base class for all DArray.

    .. class:: size_hint_t

        ``size_hint_t`` - hint the DArray implementation about its dynamical size.
        This type is defined for type matching.
        
        .. member::     size_t size
        
        .. function:: explicit constexpr size_hint_t(size_t _size)

DArray Class Template
""""""""""""""""""""""

.. class:: template<typename ValueT, size_t Rank> DArray : public DArrayBase 

    Dynamic Array with fixed compile-time rank (i.e., the number of dimensions).
    The extents of dimensions (i.e., the shape) is specified at run-time.

    .. type:: ValueT value_t
    
    .. member:: static constexpr size_t RANK = Rank
    
    .. type:: SVec<size_t, RANK> shape_t

        Basic aliases and properties.
        
        ``value_t``: type of the array element.
        
        ``RANK``: number of dimensions.
        
        ``shape_t``: extents of dimensions.

    .. type::   value_t & ref_t
        const value_t & cref_t
        value_t *iter_t
        const value_t *citer_t

        Aliases for member access.
        
        ``ret_t`` and ``cref_t``: reference type to the vector element, and its const 
        counterpart.
    
        ``iter_t`` and ``citer_t``: iterator type to the vector element, and its const
        counterpart.

    .. member:: static constexpr bool IS_INT
        
    .. type:: std::conditional_t<IS_INT, ValueT, int> int_value_t

        ``IS_INT``: tells whether the value type is integer-like (i.e., integer or 
        pointer). 

        ``int_value_t``: defined as ``int`` for non-integer-like, and defined as 
        itself for integer-like.

    .. function:: \
        DArray() noexcept
        explicit DArray(const shape_t &shape)
        DArray(const shape_t &shape, const value_t&value)
        template<typename It> DArray(const shape_t &shape, It b, It e)
        template<typename InputValue> DArray(const shape_t &shape, std::initializer_list<InputValue> il)
        template<typename InputValue> explicit DArray(const DArray<InputValue, Rank> &a)
        DArray(const shape_t &shape, const size_hint_t &size_hint)
        DArray(const shape_t &shape, const size_hint_t &size_hint, const value_t &value)
        template<typename It> DArray(const shape_t &shape, const size_hint_t &size_hint, It b, It e)
        template<typename InputValue> DArray(const shape_t &shape, const size_hint_t &size_hint, std::initializer_list<InputValue> il)

        Constructors
    
        - (1) a valid-state darray, behaving like a moved object. 
          (called an empty state)
        - (2) a darray with specified ``shape``, elements are default-initialized. 
          Note that for fundamental type, the values are uninitialized.
        - (3)-(5) the same as (2) but initialize the elements either by a single 
          ``value`` (to all elements), filled row-major with a range [b, e), 
          or a initializer list. The number of values provided by the range or 
          initializer list must be <= the desired number of elements in the 
          darray. In the case of <, the remaining elements are uninitialized.
        - (6) copy another darray whose value type may be different. The values 
          get statically cast.
        - (7)-(10): the same as (2)-(5) but use a size equal to ``size_hint.size``. The 
          value of size hint must be equal to shape.prod() or otherwise the 
          program is ill-defined.
          These constructors are provided to avoid the overhead in computing 
          the size from the shape.
    
    .. function:: DArray(const DArray &)
        DArray(DArray &&)  noexcept
        DArray & operator=(const DArray &)
        DArray & operator=(DArray &&)  noexcept
        ~DArray() noexcept

        Copy is deep. 

        After move, the moved darray (i.e., source object) is left an empty state,
        which can be destroyed or reconstructed using copy/move-assignment.

        
    .. function:: DArray & operator=(const value_t &value) noexcept

        Copy with a single ``value`` results in a constant-value DArray.


    .. function:: friend void swap (DArray &lhs, DArray &rhs) noexcept

        Deep, all elements are swapped. ``swap`` can be made to DArrays with 
        different sizes or shapes.

    .. function:: \
        value_t * release() noexcept
        void reset() noexcept
        void reset(value_t *ptr, const shape_t &shape) noexcept
        void reset(value_t *ptr, const shape_t &shape, const size_hint_t &size_hint) noexcept

        ``release()`` returns the pointer to the internal data buffer and sets the 
        instance to empty. User is responsible for the life-time management of 
        the buffer. The buffer should be freed with ``delete []`` operator.
        
        ``reset()`` destructs the instance and take over an external buffer. The buffer
        is expected to be allocated with ``new []`` and is managed by the instance.
        ``reset()`` with no parameter gives an empty darray.
        With a size hint, ``size_hint.size`` must be equal to ``shape.prod()``.

    .. function::   friend ostream & operator<< (ostream &os, const DArray &)
                    ostream & info(ostream &os=cout, int fmt_cntl=1) const
    
        ``operator<<()`` prints inline information of the instance.
    
        ``info()`` prints the instance with more controls.
    
        :arg fmt_cntl: ``0`` for an inline short message. ``1`` for a long block message.
    
    .. function:: \
        const value_t *data() const noexcept
        value_t *data() noexcept

    .. function:: \
        size_t size() const noexcept
        const shape_t & shape() const noexcept
        bool empty() const noexcept
    
    .. function:: \
        ref_t operator[](size_t pos) noexcept
        cref_t operator[](size_t pos) const noexcept

    .. function:: \
        ref_t at(size_t pos)
        cref_t at(size_t pos) const

    .. function:: \
        iter_t begin() noexcept
        citer_t begin() const noexcept
        citer_t cbegin() const noexcept
        iter_t end() noexcept
        citer_t end() const noexcept
        citer_t cend() const noexcept

        STL-conforming definitions - treating the array as a row-major linear array.
        
        ``data()`` : return a pointer to the internal storage.
        
        ``size()`` : always returns ``SIZE``.
        
        ``shape()`` : returns the shape, i.e., extents at all dimensions.
        
        ``empty()`` : returns true only if ``SIZE == 0``.
    
        ``operator[]`` and ``at()`` - element access. 
            
          - at() throws on the out-of-range.
        
        ``begin()``, ``end()`` and their const counterparts - iterators.
    
        For an empty object:
        
        - ``data()`` returns ``NULL``, ``size()`` returns 0, shape returns a all-zero vector, 
          ``empty()`` returns true.
        - ``operator[]`` is forbidden, ``at()`` raises exception.
        - ``begin()`` and end()``, and their const counterparts, return a valid iterable 
          range (of length 0).

    .. function:: \
        template<typename ...SizeTs> ref_t operator()(SizeTs &&...ids) noexcept
        template<typename ...SizeTs> cref_t operator()(SizeTs &&...ids) const noexcept

        Visit an element with indices ``ids`` at each dimension.

    .. function:: \
        void reshape(const shape_t &new_shape)
        template<size_t NewRank> DArray<ValueT, NewRank> reshaped(const SVec<size_t, NewRank> &new_shape) const
        
        ``reshape()``: inplace change the shape to ``new_shape``. The number of elements
        (i.e., ``size()``) and the rank cannot be changed.
        
        ``reshaped()``: return a new reshaped darray ranked ``NewRank``. The number of 
        elements cannot be changed. The result of reshape to an empty darray is an empty one.
        
    .. function:: template<typename T = value_t> vector<T> to_vector() const
        
        Return a vector row-majorly filled with the DArray elements.
    
    .. function:: \
        DArray & operator+=(const value_t &rhs) noexcept
        DArray & operator-=(const value_t &rhs) noexcept
        DArray & operator*=(const value_t &rhs) noexcept
        DArray & operator/=(const value_t &rhs) noexcept
        DArray & operator%=(const value_t &rhs) noexcept
        DArray & operator&=(const value_t &rhs) noexcept
        DArray & operator|=(const value_t &rhs) noexcept
        DArray & operator^=(const value_t &rhs) noexcept
    
    .. function:: \
        DArray & operator+=(const DArray &rhs)
        DArray & operator-=(const DArray &rhs)
        DArray & operator*=(const DArray &rhs)
        DArray & operator/=(const DArray &rhs)
        DArray & operator%=(const DArray &rhs)
        DArray & operator&=(const DArray &rhs)
        DArray & operator|=(const DArray &rhs)
        DArray & operator^=(const DArray &rhs)
    
    .. function:: \
        DArray operator+() const
        DArray operator-() const
        DArray operator~() const

        Linear algebra operations. All are element-wise operations.

        .. table:: 
            :class: tight-table fix-width-table
            :widths: 20 20 60

            ======================== ===============================================================  ==========================================================================================================================
            Type of operation        Usage                                                            The operator ``op``
            ======================== ===============================================================  ==========================================================================================================================
            Unary RMW operations     ``arr op= scalar`` |br| ``arr op= arr``                          ``+``, ``-``, ``*``, ``/``, ``%``: arithmetic operation and return ``DArray &``. |br|
                                                                                                      ``&``, ``|``, ``^``: bit-wise logic for each element and returns ``DArray &``.
            Binary operations        ``arr op scalar`` |br| ``scalar op arr`` |br| ``arr op arr``     ``+``, ``-``, ``*``, ``/``, ``%``: arithmetic operations and return ``DArray``. |br|
                                                                                                      ``&``, ``|``, ``^``: bit-wise for each element and return ``DArray``. |br|
                                                                                                      ``<``, ``<=``, ``>``, ``>=``, ``==``, ``!=``: comparison/logic operations and return ``DArray<bool, Rank>``.
                                                                                                      For empty Darray, the result is empty.
            Unary operations         ``op arr``                                                       ``~``, ``+``, ``-`` for bit-wise NOT, arithmetic positate, negate, respectively.
            ======================== ===============================================================  ==========================================================================================================================
            
            
        Caution: interger-lift is used intermediately for small integers, like 
        ``bool``, ``char``, etc., so that ``~true != false``. But ``&``, ``|``, ``^`` work just as
        expected.

    .. function:: \
        template<typename ResT = double> ResT norm() const noexcept;
        template<typename ResT = double> ResT norm(int p) const noexcept
        template<typename ResT = double> ResT squared_norm() const noexcept
    
    .. function:: \
        DArray & normalize() noexcept
        DArray & normalize(int p) noexcept
        DArray normalized() const
        DArray normalized(int p) const

        ``norm()`` : 2-norm (i.e., Frobenius norm).
        ``norm(p)`` : p-norm (treating array as a vector).
        ``squared_norm()`` : square of 2-norm.
        ``normalize()`` : normalize itself, according to 2-norm.
        ``normalize(int p)`` : normalize itself, according to p-norm.
        ``normalized()`` : returns a normalized (according to 2-norm) copy.
        ``normalized(int p)`` : returns a normalized (according to p-norm) copy.

        Note: ``normalize()`` or ``normalized()`` for an integer vector are ill-defined. 
        ``norm()`` with ``ResT`` != floting-point type may have truncation.
        With caution to use.

    .. function:: \
        template<typename ResT = value_t> ResT sum() const noexcept
        template<typename ResT = value_t> ResT prod() const noexcept
        template<typename ResT = value_t> ResT mean() const noexcept
    
    .. function:: \
        value_t min() const noexcept
        value_t max() const noexcept
        std::pair<value_t, value_t> minmax() const noexcept
    
    .. function:: \  
        size_t min_index() const noexcept
        size_t max_index() const noexcept
        std::pair<size_t, size_t> minmax_index() const noexcept

    .. function:: \
        bool all() const noexcept
        bool any() const noexcept

    Reduction operations.

    ``sum()``, ``prod()``, ``mean()`` : the summation, product, and mean of all elements.
    
    ``min()``, ``max()``, ``minmax()`` : as you expect. 
    The indexed-version returns the element index if the corresponding result.

    ``all()``, ``any()`` : all true or any true.

    .. function:: \
        template<typename UnaryOp> DArray & map(UnaryOp op)
        template<typename UnaryOp, typename ResT = std::invoke_result_t<UnaryOp, value_t> > DArray<ResT, Rank> mapped(UnaryOp op) const
        template<typename BinaryOp> void visit(BinaryOp op) const
        template<typename BinaryOp> void visit(BinaryOp op)

        Map and visit operations.

        ``map()`` : for each ``size_t i``, ``self[i] = op(self[i])``.
        ``mapped()`` : returns a mapped copy.
        ``visit()`` : for each ``size_t i``, call ``op(i, self[i])``.

    .. function:: \
        template<typename ResT = int_value_t> DArray<ResT, Rank> floor() const
        template<typename ResT = int_value_t> DArray<ResT, Rank> ceil() const
        template<typename ResT = int_value_t> DArray<ResT, Rank> trunc() const
        DArray abs() const

        Round to floor, ceil, trunc toward zero, and absolute value.
        ``ResT`` can be floating-point or integral type.

        By default, if ``value_t`` is integer or pointer, ``ResT`` is 
        ``value_t`` itself, no conversion bappens. Otherwise ``ResT`` is ``int``, 
        and the conversion is made by std::floor, ceil, trunc and then cast.

Non-member functions 
""""""""""""""""""""""

The following binary arithmetic and logic functions are defined for :class:`DArray`. 
For details, see, e.g., the description of the corresponding method :func:`DArray::operator+` .

.. function:: \
    template<typename ValueT, size_t Rank> DArray<ValueT, Rank> operator+( const ValueT &lhs, const DArray<ValueT, Rank> &rhs) noexcept
    template<typename ValueT, size_t Rank> DArray<ValueT, Rank> operator-( const ValueT &lhs, const DArray<ValueT, Rank> &rhs) noexcept
    template<typename ValueT, size_t Rank> DArray<ValueT, Rank> operator*( const ValueT &lhs, const DArray<ValueT, Rank> &rhs) noexcept
    template<typename ValueT, size_t Rank> DArray<ValueT, Rank> operator/( const ValueT &lhs, const DArray<ValueT, Rank> &rhs) noexcept
    template<typename ValueT, size_t Rank> DArray<ValueT, Rank> operator%( const ValueT &lhs, const DArray<ValueT, Rank> &rhs) noexcept
    template<typename ValueT, size_t Rank> DArray<ValueT, Rank> operator&( const ValueT &lhs, const DArray<ValueT, Rank> &rhs) noexcept
    template<typename ValueT, size_t Rank> DArray<ValueT, Rank> operator|( const ValueT &lhs, const DArray<ValueT, Rank> &rhs) noexcept
    template<typename ValueT, size_t Rank> DArray<ValueT, Rank> operator^( const ValueT &lhs, const DArray<ValueT, Rank> &rhs) noexcept
    template<typename ValueT, size_t Rank> DArray<bool, Rank> operator<( const ValueT &lhs, const DArray<ValueT, Rank> &rhs) noexcept
    template<typename ValueT, size_t Rank> DArray<bool, Rank> operator<=( const ValueT &lhs, const DArray<ValueT, Rank> &rhs) noexcept
    template<typename ValueT, size_t Rank> DArray<bool, Rank> operator>( const ValueT &lhs, const DArray<ValueT, Rank> &rhs) noexcept
    template<typename ValueT, size_t Rank> DArray<bool, Rank> operator>=( const ValueT &lhs, const DArray<ValueT, Rank> &rhs) noexcept
    template<typename ValueT, size_t Rank> DArray<bool, Rank> operator==( const ValueT &lhs, const DArray<ValueT, Rank> &rhs) noexcept
    template<typename ValueT, size_t Rank> DArray<bool, Rank> operator!=( const ValueT &lhs, const DArray<ValueT, Rank> &rhs) noexcept

.. function:: \
    template<typename ValueT, size_t Rank> DArray<ValueT, Rank> operator+( const DArray<ValueT, Rank> &lhs, const ValueT &rhs) noexcept
    template<typename ValueT, size_t Rank> DArray<ValueT, Rank> operator-( const DArray<ValueT, Rank> &lhs, const ValueT &rhs) noexcept
    template<typename ValueT, size_t Rank> DArray<ValueT, Rank> operator*( const DArray<ValueT, Rank> &lhs, const ValueT &rhs) noexcept
    template<typename ValueT, size_t Rank> DArray<ValueT, Rank> operator/( const DArray<ValueT, Rank> &lhs, const ValueT &rhs) noexcept
    template<typename ValueT, size_t Rank> DArray<ValueT, Rank> operator%( const DArray<ValueT, Rank> &lhs, const ValueT &rhs) noexcept
    template<typename ValueT, size_t Rank> DArray<ValueT, Rank> operator&( const DArray<ValueT, Rank> &lhs, const ValueT &rhs) noexcept
    template<typename ValueT, size_t Rank> DArray<ValueT, Rank> operator|( const DArray<ValueT, Rank> &lhs, const ValueT &rhs) noexcept
    template<typename ValueT, size_t Rank> DArray<ValueT, Rank> operator^( const DArray<ValueT, Rank> &lhs, const ValueT &rhs) noexcept
    template<typename ValueT, size_t Rank> DArray<bool, Rank> operator<( const DArray<ValueT, Rank> &lhs, const ValueT &rhs) noexcept
    template<typename ValueT, size_t Rank> DArray<bool, Rank> operator<=( const DArray<ValueT, Rank> &lhs, const ValueT &rhs) noexcept
    template<typename ValueT, size_t Rank> DArray<bool, Rank> operator>( const DArray<ValueT, Rank> &lhs, const ValueT &rhs) noexcept
    template<typename ValueT, size_t Rank> DArray<bool, Rank> operator>=( const DArray<ValueT, Rank> &lhs, const ValueT &rhs) noexcept
    template<typename ValueT, size_t Rank> DArray<bool, Rank> operator==( const DArray<ValueT, Rank> &lhs, const ValueT &rhs) noexcept
    template<typename ValueT, size_t Rank> DArray<bool, Rank> operator!=( const DArray<ValueT, Rank> &lhs, const ValueT &rhs) noexcept

.. function:: \
    template<typename ValueT, size_t Rank> DArray<ValueT, Rank> operator+( const DArray<ValueT, Rank> &lhs, const DArray<ValueT, Rank> &rhs) noexcept
    template<typename ValueT, size_t Rank> DArray<ValueT, Rank> operator-( const DArray<ValueT, Rank> &lhs, const DArray<ValueT, Rank> &rhs) noexcept
    template<typename ValueT, size_t Rank> DArray<ValueT, Rank> operator*( const DArray<ValueT, Rank> &lhs, const DArray<ValueT, Rank> &rhs) noexcept
    template<typename ValueT, size_t Rank> DArray<ValueT, Rank> operator/( const DArray<ValueT, Rank> &lhs, const DArray<ValueT, Rank> &rhs) noexcept
    template<typename ValueT, size_t Rank> DArray<ValueT, Rank> operator%( const DArray<ValueT, Rank> &lhs, const DArray<ValueT, Rank> &rhs) noexcept
    template<typename ValueT, size_t Rank> DArray<ValueT, Rank> operator&( const DArray<ValueT, Rank> &lhs, const DArray<ValueT, Rank> &rhs) noexcept
    template<typename ValueT, size_t Rank> DArray<ValueT, Rank> operator|( const DArray<ValueT, Rank> &lhs, const DArray<ValueT, Rank> &rhs) noexcept
    template<typename ValueT, size_t Rank> DArray<ValueT, Rank> operator^( const DArray<ValueT, Rank> &lhs, const DArray<ValueT, Rank> &rhs) noexcept
    template<typename ValueT, size_t Rank> DArray<bool, Rank> operator<( const DArray<ValueT, Rank> &lhs, const DArray<ValueT, Rank> &rhs) noexcept
    template<typename ValueT, size_t Rank> DArray<bool, Rank> operator<=( const DArray<ValueT, Rank> &lhs, const DArray<ValueT, Rank> &rhs) noexcept
    template<typename ValueT, size_t Rank> DArray<bool, Rank> operator>( const DArray<ValueT, Rank> &lhs, const DArray<ValueT, Rank> &rhs) noexcept
    template<typename ValueT, size_t Rank> DArray<bool, Rank> operator>=( const DArray<ValueT, Rank> &lhs, const DArray<ValueT, Rank> &rhs) noexcept
    template<typename ValueT, size_t Rank> DArray<bool, Rank> operator==( const DArray<ValueT, Rank> &lhs, const DArray<ValueT, Rank> &rhs) noexcept
    template<typename ValueT, size_t Rank> DArray<bool, Rank> operator!=( const DArray<ValueT, Rank> &lhs, const DArray<ValueT, Rank> &rhs) noexcept