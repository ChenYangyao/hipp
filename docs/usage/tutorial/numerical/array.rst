Numerical Arrays
=====================

HIPP defines class templates for numerical arrays (i.e., vectors, matrices, or arrays of higher dimensionality).
These types provide both expressive interface and efficient implementation, 
serving as the back-end of other numerical facilities.

Two favors of arrays are defined:

- :class:`~HIPP::NUMERICAL::SArray`: a static array whose rank and extents (i.e., shape) are determined at compile-time. Examples 
  include a 3D position vector of a particle, a :math:`3\times 3` inertial tensor of a rigid body, a histogram with 
  a fixed number of 10 bins.
- :class:`~HIPP::NUMERICAL::DArray`: a dynamic array whose rank is determined at compile-time, but the extent along each dimension are 
  determined at run-time. Examples include a :math:`128\times 128` single-channel images, 
  a :math:`3\times 128 \times 128` array that describes a 3 channel images, a :math:`512^3` 3D
  spatial scalar field in numerical simulations.

The interface, underlying storage and performance are therefore different for those two types array.

In the following, we assume the declarations of namespace::

    using namespace HIPP;
    using namespace HIPP::NUMERICAL;
    using namespace std;

which expose all the interface in the standard library and HIPP NUMERICAL module.

.. _tutorial-numeric-array-static:

Static Arrays
----------------

Define SArray Instance
""""""""""""""""""""""""

:class:`~HIPP::NUMERICAL::SArray` class holds a static array. To get an instance, pass the scalar type of its element
and extents along dimensions as template parameters::

    SArray<double, 3, 4> a34;       // An array shaped [3][4].
    SArray<double, 3> a3;           // An vector shaped [3].

Because 1D arrays, i.e., vectors, are frequently used, we have a set of aliases:

- ``SVec<t, N>`` class template: defines a vector with scalar type ``t`` and ``N`` elements. This 
  is equivalent to ``SArray<t, N>``.
- ``SVecXt<N>``: a shortcut for ``SVec<t, N>``. Here ``t`` could be ``i`` (for ``int``), 
  ``f`` (for ``float``), ``d`` (for ``double``) and ``b`` (for ``bool``).
- ``SVecNt``: a shortcut for  ``SVecXt<N>``. Here ``N`` could be ``1``, ``2``, ``3`` or ``4``.

For example, the following three declarations all define the same vector type with 3 double-precision floating-point values::

    SVec<double, 3> x3;             // 3 double values.
    SVecXd<3> y3;                       
    SVec3d z3;                  

By default, all array elements are default-initialized (i.e., numerical values may have undefined values).
To initialize the array elements at construction, simply use a initializer list with 
less of equal number of elements than the array::

    SVec3d a3 = {1,2,3}, b3 = {1,2};          // b3[2] is uninitialized.
    pout << "a3 = ", a3, ", b3 = ", b3, endl;

Output:

.. code-block:: text

    a3 = SArray{1,2,3}, b3 = SArray{1,2,2.31271e+35}

The following direct initializers are also acceptable::

    double x[3] = {4,5,6};
    SVec3d c3(1),              // Constant array, {1,1,1}.
        d3(x),                 // Copy from a pointer x.
        e3(x, 2),              // Copy only 2 elements from x.
        f3(a3);                // Copy from another SArray.
    pout << "c3 = ", c3, ", d3 = ", d3, ", e3 = ", e3, ", f3 = ", f3, endl;

Output:

.. code-block:: text

    c3 = SArray{1,1,1}, d3 = SArray{4,5,6}, e3 = SArray{4,5,0}, f3 = SArray{1,2,3}

High-dimensional arrays can be initialized by treating them as 1D vector with 
row-major storage order::

    SArray<int, 2, 3> 
        a23 = {1,2,3,4,5,6},        // Row-majored filling.
        b23(1),                     // Constant array.
        c23(&a23[0]),               // Copy from a contiguous buffer.
        d23(&a23[0], 3),            // Copy only the first 3 elements.
        e23(a23);                   // Copy from another SArray.
    pout << "a23 = ", a23, ", b23 = ", b23, '\n',
        "c23 = ", c23, ", d23 = ", d23, '\n',
        "e23 = ", e23, endl;

Output:

.. code-block:: text
    
    a23 = SArray{1,2,3,4,5,6}, b23 = SArray{1,1,1,1,1,1}
    c23 = SArray{1,2,3,4,5,6}, d23 = SArray{1,2,3,32576,-2005988080,21874}
    e23 = SArray{1,2,3,4,5,6}

The value of the whole array can be changed later after construction::

    a3 = c3;                    // Assign from another SArray.
    b3 = 1.0;                   // Assign a scalar to all elements.
    pout << "a3 = ", a3, ", b3 = ", b3, endl;

Output:

.. code-block:: text

    a3 = SArray{1,1,1}, b3 = SArray{1,1,1}


Element Access 
"""""""""""""""""

``SArray`` has tuple-like API defined for **structured binding**. For example, to bind 
a list of variables to elements of a static vector, simply use::

    SVec3i a3 = {1,2,3};
    auto [x0,x1,x2] = a3;                   // Use structured binding.
    pout << "x0,x1,x2 = ", x0, ",", x1, ",", x2, endl;

Output:

.. code-block:: text

    x0,x1,x2 = 1,2,3

Here, ``x0``, ``x1``, ``x2`` are automatically declared as three ``int`` whose values 
are initialized by the three elements of the vector.

Binding as reference is also allowed, by which the variables are declared as 
references to the vector elements. The update on the binding variables are 
reflected to the vector::

    auto &[y0,y1,y2] = a3;                  // Bind as reference.
    y0 += 1; y1 += 2; y2 += 3;  
    pout << "a3 = ", a3, endl;

Output:

.. code-block:: text

    a3 = SArray{2,4,6}

For higher dimensional array, the binding variables are declared as sub-arrays 
along the first dimension of the parent array. For example, the following
code defines two vectors (i.e., ``SVec<int, 3>``) whose values are initialized 
by the two rows of the 2D array::

    SArray<int, 2, 3> a23 = {1,2,3,4,5,6};
    auto [z0, z1] = a23;                    // Bind to the 1st and 2nd rows.
    pout << "z0 = ", z0, ", z1 = ", z1, endl;

Output:

.. code-block:: text

    z0 = SArray{1,2,3}, z1 = SArray{4,5,6}

Again, it is allowed to bind as reference::

    auto &[w0, w1] = a23;
    w0 += a3*10;
    pout << "a23 = ", a23, endl;

Output:

.. code-block:: text

    a23 = SArray{21,42,63,4,5,6}

Like STL, ``SArray`` defines index-based and iterator-based element access 
interface:

- The method ``data()`` returns a pointer to the first element 
  of the array (always in a row-major order). ``size()`` returns the 
  number of elements in the array.
- The method ``raw()`` returns a reference to the raw array.
- ``begin()`` and ``end()`` returns the iterators. ``cbegin()`` and ``cend()``
  are the constant counterparts.
- Operator ``[]`` returns a reference to the element according to 1D index 
  (row-major order). Method ``at()`` is similar except that it throws 
  an :class:ErrLogic`` exception on the out-of-range indexing.
- Operator ``()`` accept N-Dimensional indices for a N-D array. This is the same 
  as ``[]`` for 1D array, but more convenient for higher dimensionality.

For example, for a vector (1D array), its element access can be::

    int *p3 = a3.data();
    int (& a3_raw)[3] = a3.raw();
    for(size_t i=0; i<a3.size(); ++i) {
        p3[i] = i;                          // Visit the buffer by pointer.
        a3_raw[i] += 1;                     // Visit by the raw array reference.
    }    
    for(auto it = a3.begin(); it != a3.end(); ++it)
        *it += 1;                           // Visit by the iterator.

    a3[0] = -1;                             // Visit by operator [] indexing.
    a3.at(1) = -2;                          // Visit by at().
    pout << "a3 = ", a3, endl;

Output:

.. code-block:: text

    a3 = SArray{-1,-2,4}

For the 2D array::

    int *p23 = a23.data();
    int (&a23_raw)[2][3] = a23.raw();
    p23[0] = -1;                // (0,0) element.
    a23_raw[0][1] = -2;         // (0,1) element.
    
    a23[2] = -3;                // (0,2) element.
    a23.at(3) = -4;             // (1,0) element.
    
    *(a23.begin() + 4) = -5;    // (1,1) element.
    a23(1,2) = -6;              // (1,2) element.
    pout << "a23 = ", a23, endl;

Output:

.. code-block:: text

    a23 = SArray{-1,-2,-3,-4,-5,-6}

To help interact with STL, 1D SArray has two methods, ``to_vector()`` and ``to_array``,
which convert the instance to a STL vector and array, respectively::

    vector<int> vec = a3.to_vector();
    array<int, 3> arr = a3.to_array();
    array<double, 3> arr_d = a3.to_array<double>();     // Change the target type.

    pout << "vec = {", vec, "}, arr = {", arr, "}, arr<double> = {", arr_d, "}", endl; 

Output:

.. code-block:: text

    vec = {-1,-2,4}, arr = {-1,-2,4}, arr<double> = {-1,-2,4}


Arithmetic Logic Operations
""""""""""""""""""""""""""""

Element-wise arithmetic and logic operations are defined for ``SArray``. Two operands 
can be arrays of the same shapes, or an array with a scalar::

    SVec3d a = {0,1,2}, b = {3,4,5};
    pout << "a+1, a-1, a+b, a-b, a*b+10 => ",
        a+1, ',', a-1, ',', a+b, ',', a-b, ',', a*b+10, endl;

Self-assignment versions of them are also available::

    a += b;
    b += 1;
    pout << "a => ", a, ", b => ", b, endl;

Element-wise comparisions can also be made::

    pout << "a==b, a!=b, a>0 => ",
        a==b, ',', a!=b, ',', a>0, endl;

Output:

.. code-block:: text

    a+1, a-1, a+b, a-b, a*b+10 => SArray{1,2,3},SArray{-1,0,1},SArray{3,5,7},SArray{-3,-3,-3},SArray{10,14,20}
    a => SArray{3,5,7}, b => SArray{4,5,6}
    a==b, a!=b, a>0 => SArray{0,1,0},SArray{1,0,1},SArray{1,1,1}

Reduction 
"""""""""""

Reduction operations can be applied to one or more ``SArray`` instance to produce 
scalar or other ``SArray`` instance.

``norm()``, ``squared_norm()`` and ``norm(p)`` returns the 2-norm (Frobenius norm), square of 2-norm, and p-norm 
of an array::

    SVec3d a = {0,1,2};
    pout << "Norm = ", a.norm(), 
        ", squared norm = ", a.squared_norm(), 
        ", 1 norm = ", a.norm(1), endl;

``normalize()`` perform in-place normalization on the instance. ``normalized()`` returns a normalized copy::

    auto b = a.normalized();            // A normalized copy.
    a.normalize();                      // In-place normalization.
    auto c = a.normalized(1);           // Use 1 norm.
    pout << "a = ", a, ", b = ", b, ", c = ", c, endl;

Output:

.. code-block:: text

    Norm = 2.23607, squared norm = 5, 1 norm = 3
    a = SArray{0,0.447214,0.894427}, b = SArray{0,0.447214,0.894427}, c = SArray{0,0.333333,0.666667}


Minimum, maximum, mean, sum, product can be obtained by corresponding methods::

    pout << "sum = ", a.sum(), 
        ", prod = ", a.prod(), 
        ", mean = ", a.mean(), 
        ", min = ", a.min(), 
        ", max = ", a.max(), 
        ", min and max = ", a.minmax(), 
        ", index of min = ", a.min_index(), endl;

Output:

.. code-block:: text

    sum = 1.34164, prod = 0, mean = 0.447214, min = 0, max = 0.894427, min and max = 0:0.894427, index of min = 0

``any()`` tells whether or not the array has a non-false element, ``all()`` checks if all elements are non-false::

    pout << "any true = ", a.any(), 
        ", all true = ", a.all(), 
        ", any positive = ", (a>0).any(), 
        ", any equal = ", (a==b).any(), endl;

Output:

.. code-block:: text

    any true = 1, all true = 0, any positive = 1, any equal = 1


For 1D ``SArray``, i.e., vector, ``dot()`` and ``cross()`` are defined for dot product and 
cross product::

    pout << "a dot b = ", a.dot(b),
        ", a cross b = ", a.cross(b), endl;

Output:

.. code-block:: text

    a dot b = 1, a cross b = SArray{0,0,0}

Here, because because ``a`` and ``b`` are normalized vectors and parallel to 
each other, the dot product is ``1`` and the cross product is ``0``.


Apply Function to Array Elements
""""""""""""""""""""""""""""""""""""

``SArray`` defines both specific and general methods that map the instance element-wisely to a new one:

- ``floor()``, ``ceil()``, ``trunc()``: return a new integer instance whose elements are from the flooring, ceiling and truncation
  of the original elements, respectively. If applied to an integer array, they simply return a copy.
- ``abs()``: returns a new instance with all elements to be the absolute values of original elements.
- ``mapped(unary_op)``: returns a new instance with each element to be the result of applying the callable ``unary_op`` to the original element.
  ``map(unary_op)`` is similar, but modify the instance in-place.
- ``visit(binary_op)``: call the callable ``binary_op(i, x)`` for each element ``x`` with 1D index ``i``.

The following examples show how to use these methods::

    SVec3d a = {-1.1, 1.1, 2.1};
    pout << "floor = ", a.floor(), 
        ", ceil = ", a.ceil(),
        ", trunc = ", a.trunc(), 
        ", abs = ", a.abs(), endl;

Output:

.. code-block:: text

    floor = SArray{-2,1,2}, ceil = SArray{-1,2,3}, trunc = SArray{-1,1,2}, abs = SArray{1.1,1.1,2.1}

The methods ``mapped`` and ``map()`` enable more general transformation::

    auto sin_a = a.mapped( [](auto x){ return sin(x); } );   // A mapped copy
    a.map( [](double x) {return x+1.0; } );                  // In-place change
    pout << "sin_a = ", sin_a, 
        ", a = ", a, endl;

Output:

.. code-block:: text
    
    sin_a = SArray{-0.891207,0.891207,0.863209}, a = SArray{-0.1,2.1,3.1}

The method ``visit()`` does not return a new instance nor modify the original instance. It simply
applies the user-passed function to the element and its index::

    // Visit each element by index and value
    a.visit( [](size_t i, auto x){ pout << '(', i, ')', x, '\n'; } );

Output:

.. code-block:: text

    (0)-0.1
    (1)2.1
    (2)3.1

View
""""""

HIPP provides "view" object which refers to a subset of elements in a ``SArray``.
Two types of supported views are:

- Boolean view: defined by a mask ``SArray`` whose extents are the same as the target 
  ``SArray`` and whose elements are ``bool``.
- Stride view: defined by a set of triplets in the form ``(begin, end, step)`` 
  at each dimension.

All the views are created by the ``view()`` method of ``SArray``. For convenience, 
a Boolean view can also be created by ``operator[]`` with a bool array passed as mask.

For example, the following code creates a mask array by comparison operator ``>`` 
and then uses it to create a view::

    SVec3d a = {-1, -2, -3};
    auto pos_a = a[ a > 0 ];                // A view of all positive elements.

You may also explicitly create the mask and use it to filter elements::

    SVec3b mask = {true, true, false};
    auto first_2 = a[mask];                 // A view of the first 2 elements.

View object has all arithmetic-logic operations defined as those for ``SArray``.
Reduction operations ``sum()``, ``prod()``, ``mean()``, ``all()`` and ``any()``,
and visitors ``mapped()``, ``map()`` and ``visit()`` are also defined.
When invoked, the corresponding operation is applied to only those selected 
elements.

For example, use the above Boolean view::

    first_2 += 2;
    pout << a, endl;

The code changes only the first 2 elements of the parent SArray.

Output:

.. code-block:: text

    SArray{1,0,-3}

A stride view can be created by the ``view()`` method of any ``SArray``, say, 
``a``. 
To ensure the overload exactly resolved, the first argument must be 
a special object ``a.s_stride``, and the following arguments must be 
special slice object (created from) ``a.s_xxx``. For example, to 
select the range ``[0, 5)`` with step size ``2``, call::

    SVec<double, 5> b = {1,2,3,4,5};
    auto slice_b = b.view(b.s_stride, b.s_range(0,5,2));        // A view of the indexing range [0,5) with stride 2.

Then the view object can be used as::

    slice_b += 10;
    pout << b, endl;

Output:

.. code-block:: text

    SArray{11,2,13,4,15}

For high dimensional array, each dimension needs a slice object. For example, 
to select the first two rows of a 2D array, use::

    SArray<int, 3, 3> c = {1,2,3,4,5,6,7,8,9};
    auto slice_c = c.view(c.s_stride, c.s_head(2), c.s_all);    // The first two rows
    slice_c *= -1;
    pout << c, endl;

Where ``c.s_head(n)`` select the first ``n`` elements at the corresponding dimension,
while ``c.s_all`` selects all.

Output:

.. code-block:: text

    SArray{-1,-2,-3,-4,-5,-6,7,8,9}







.. _tutorial-numeric-array-dynamic:

Dynamic Arrays
------------------

