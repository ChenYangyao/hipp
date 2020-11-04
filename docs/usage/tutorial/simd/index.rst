************************************
Tutorial on the SIMD Module
************************************

The ``HIPP::SIMD`` module provides the OOP interface to single-instruction-multiple-data (SIMD) operations.
Currently it only supports the Intel x86 platform where AVX vector instructions are implemented 
(see `Intel's Guide <https://software.intel.com/sites/landingpage/IntrinsicsGuide/>`_ 
and check the `Wiki <https://en.wikipedia.org/wiki/Advanced_Vector_Extensions>`_ page to see 
the processors that supports SIMD).


In this tutorial we will show some examples of how to use the ``HIPP::SIMD`` module to accelerate 
your program. 
We will begin with an ordinary program using the non-SIMD instructions. 
Then we show how to convert it to the SIMD version.
Sometimes it is useful to go back to the SIMD intrinsics 
(i.e., those defined in ``<x86intrin.h>``), 
so we also show how to implement the program 
with only the intrinsics and how HIPP's OOP interface interacts with the intrinsics. 

A Simple Example 
================================

Let us begin with a simple example: the addition of two arrays, "arr2 = arr1 + arr2";
This is a trival task, but it shows the basic ideas of how to vectorize your operations.
It demonstrates three parts that are used in (almost) every SIMD program: 
(1) memory management, 
(2) load/store operations, and 
(3) arithmetic operations.

The non-SIMD version 
-------------------------
The following code sample demonstrates the array addition without any SIMD operation::

    void vecadd_nonSIMD(){
        const int N = 1024*8;
        float *arr1 = new float[N], 
            *arr2 = new float[N];       // allocate memory for 8192 floats

        for(int i=0; i<N; ++i){         // initialize arr1 & arr2 (store)
            arr1[i] = float(i);
            arr2[i] = float(N-i);
        }

        for(int i=0; i<N; ++i)          // Make vector addition (including
            arr2[i] += arr1[i];         // load/store and arithmetics).

        // print the first 40 elements
        HIPP::prt_a(cout, arr2, arr2+40) << endl;

        delete [] arr2; delete [] arr1;
    }

There is nothing new in the code. We list the significant operations that make 
up of this code sample:

* As usual, a large chunk of memory can be 
  allocated in the heap memory using the ``new`` operator (do not forget to 
  release it with ``delete`` at final). 
* Elements in the array are accessed by ``[]`` operator. For example, the 
  i-th element in the array ``arr1`` can be accessed (read or write) by 
  ``arr1[i]``. Equivalently, you can do ``*(arr1+i)`` which makes use of 
  the pointer arithmetics. 
* You add two elements by the ``+`` operator, and assign the result to another, 
  say, ``arr2[i] = arr1[i] + arr[2]``, or equivalently,  ``arr2[i] += arr1[i]``.

After the addition operation, ``arr1`` is not changed and ``arr2`` becomes a 
constant array (each element is 8192.0f). We print the first 40 elements of ``arr2`` into ``cout`` using 
``HIPP::prt_a``.

Keeping these three parts in mind, we now convert them into the vectorized version 
using HIPP's SIMD library.

The SIMD version 
-------------------------

Logically, SIMD code is nothing different from the ordinary code. We just replace
the above three parts by SIMD operations.

The memory is required aligned in many SIMD operations. The reason for that is 
performance - the more regularity the design is, the faster the machine would be.
Intel does provide the unaligned version, but it usually has higher CPI.

To allocate aligned memory, POSIX standard provides some library functions (see the `GNU manual <https://www.gnu.org/software/libc/manual/html_node/Aligned-Memory-Blocks.html>`_, for example). 
C++17 also standardize the ``std::aligned_alloc`` (see the `cpp reference <https://en.cppreference.com/w/cpp/memory/c/aligned_alloc>`_, for example).
You may use any of them, but the most convinient way is to use HIPP's memory interface.
To replace the two ``new`` operations in the non-SIMD version, you write::

    #include <hippsimd.h>                           // HIPP's SIMD library
    #include <hippcntl.h>                           // MemObj and prt_a

    typedef HIPP::SIMD::Vec<float, 8> f8_t;
    typedef HIPP::MemObj<f8_t> mem_t;   

    const int N = 1024;
    f8_t *arr1 = mem_t::alloc_a(N, 1),              // allocate aligned memory
        *arr2 = mem_t::alloc_a(N, 1);

The two header files are necessary here and in the following - they provide the 
interface to HIPP's SIMD classes and other useful functions, respectively.
The SIMD classes are defined in the namespace ``HIPP::SIMD``. Other useful functions 
are defined in the namespace ``HIPP``. 

All HIPP SIMD vectors are defined through the class template ``Vec<scalar_type, n>``, which 
hosts an array of ``n`` elements each typed ``scalar_type``. We alias ``f8_t`` 
as a vector that consists of 8 single-precision floats.
To allocate aligned memory for the vector type, you define the memory allocator 
``MemObj`` of it (we alias it as ``mem_t``). Then the call of ``addr = mem_t::alloc(N, A)``
allocates space for ``N`` vectors (i.e., ``N*8`` floats) which is aligned at ``A`` minimal 
alignments of ``f8_t`` (i.e., ``A * alignof(f8_t)`` ), and returns its address into ``addr``. 
The call of ``mem_t::dealloc(addr)`` release the memory allocated by the allocator.

Note that you cannot define a SIMD vector of arbitrary size. An AVX-compatible 
processor provide 256-bit "ymm" registors, so you can use a pack of 8 
floats (8*32=256 bits).

The second part we need to change is the memory access operations (i.e., load/store operations).
This can be done through the methods ``load`` and ``store`` of the ``Vec`` type.
``load( (Vec *)addr )`` load a vector of data elements from an aligned address ``addr``
into the ``Vec`` instance, while ``store( (Vec *)addr )`` store the data in the 
instance to the aligned memory address ``addr``. Therefore, the part of the initialization
code can be written as::

    f8_t low {7.0f,6.0f,5.0f,4.0f,3.0f,2.0f,1.0f,0.0f}, // 0, 1, 2, ..., 7
        hi = f8_t(N*8.0f) - low,                        // 8192, 8191, 8190, ..., 8185
        stride = f8_t(8.0);                             // 8, 8, 8, ..., 8
    for(int i=0; i<N; ++i){                         // in GCC:
        low.store(arr1+i);                          // arr1[i] = low
        hi.store(arr2+i);                           // arr2[i] = hi
        low += stride;
        hi -= stride;
    }

Because we need to operate on 8 floats at once, we declare ``low`` 
as a linearly increased vector (0.0, 1.0, ..., 7.0, from low-address to high-address),
and we declare ``hi`` as a linearly decrased vector. Then by a for loop we 
store them into the memory pointed by ``arr1`` and ``arr2``, and update them 
by a ``stride``. After these steps, ``arr1`` and ``arr2`` should the same 
as those in the non-SIMD codes.

In the GCC compiler, the two store operations in the body of for loop can be
written as ``arr1[i]=low`` and ``arr2[i]=hi``, but in other compiling system 
these might not work. 

The final part is to load each element and make the addition operation. We can write::
    
    f8_t temp1, temp2;
    for(int i=0; i<N; ++i){                 
        temp1.load(arr1+i);                         //
        temp2.load(arr2+i);                         // in GCC:
        temp2 += temp1;                             // arr2[i] += arr1[i]
        temp2.store(arr2+i);                        //
    }

Here we load each element from ``arr1`` and ``arr2``, add them, and store back 
into ``arr2``.
Again, in GCC, the four statements in the body of for loop can be condensed 
into one statement ``arr2[i] += arr1[i]``.

The final code that combines these three parts of modification is::

    #include <hippsimd.h>                           // HIPP's SIMD library
    #include <hippcntl.h>                           // MemObj and prt_a

    typedef HIPP::SIMD::Vec<float, 8> f8_t;
    typedef HIPP::MemObj<f8_t> mem_t;   

    void vecadd_SIMD(){
        const int N = 1024;
        f8_t *arr1 = mem_t::alloc_a(N, 1), 
            *arr2 = mem_t::alloc_a(N, 1);           // allocate aligned memory
        
        f8_t low {7.0f,6.0f,5.0f,4.0f,3.0f,2.0f,1.0f,0.0f},
            hi = f8_t(N*8.0f) - low,                       
            stride = f8_t(8.0);
        for(int i=0; i<N; ++i){                     // in GCC: 
            low.store(arr1+i);                      // arr1[i] = low
            hi.store(arr2+i);                       // arr2[i] = hi
            low += stride;
            hi -= stride;
        }

        f8_t temp1, temp2;
        for(int i=0; i<N; ++i){                 
            temp1.load(arr1+i);                     //
            temp2.load(arr2+i);                     // in GCC:
            temp2 += temp1;                         // arr2[i] += arr1[i]
            temp2.store(arr2+i);                    //
        }

        float *p_arr2 = (float *)arr2;
        HIPP::prt_a(cout, p_arr2, p_arr2+40) << endl;

        mem_t::dealloc(arr2);
        mem_t::dealloc(arr1);
    }

To compile, link the library **libhippcntl.so**, and enable the flag **-mavx2** (in GCC; other compiler 
may use different flags):

.. code-block:: bash 

    $ g++ -std=c++17 -O3 -Wall -mavx2 src.cpp   
    $ ./a.out                 

Using the Intrinsics
-------------------------

Sometimes user may want to go back to the SIMD intrinsics (because the HIPP SIMD library may miss 
some rare-used operations). Here we rewrite the above example using the x86 intrinsics:

.. code-block:: 

    #include <x86intrin.h>                              // SIMD instructions 
    #include <hippcntl.h>                               // MemObj and prt_a
    
    void vecadd_intrinsics(){
        const int N = 1024, size_vec = sizeof(__m256);
        __m256 *arr1 = (__m256 *)aligned_alloc(size_vec, size_vec*N),
            *arr2 = (__m256 *)aligned_alloc(size_vec, size_vec*N);
        
        __m256 low = _mm256_set_ps(7.0f,6.0f,5.0f,4.0f,3.0f,2.0f,1.0f,0.0f),
            hi = _mm256_sub_ps( _mm256_set1_ps(float(N*8)), low),
            stride = _mm256_set1_ps(8.0f);
        for(int i=0; i<N; ++i){
            _mm256_store_ps((float *)(arr1+i), low);    // arr1[i] = low
            _mm256_store_ps((float *)(arr2+i), hi);     // arr2[i] = hi
            low = _mm256_add_ps(low, stride);
            hi = _mm256_sub_ps(hi, stride);
        }

        __m256 temp1, temp2;
        for(int i=0; i<N; ++i){           
            temp1 = _mm256_load_ps((float *)(arr1+i)),  //
            temp2 = _mm256_load_ps((float *)(arr2+i));  //
            temp2 = _mm256_add_ps(temp2, temp1);        // arr2[i] += arr1[i]
            _mm256_store_ps((float *)(arr2+i), temp2);  //
        }

        float *p_arr2 = (float *)arr2;
        HIPP::prt_a(cout, p_arr2, p_arr2+40) << endl;

        free(arr2); free(arr1);
    }

In the memory allocation, we use the ``std::aligned_alloc()`` standard function,
which accepts the alignment and size of the desired memory in bytes. The intrinsic 
vector type for a pack of 8 floats is ``__m256``. We use ``_mm256_set_ps()`` to 
set 8 floats to initialize the vector, and use ``_mm256_set1_ps()`` to broadcast 
one float to 8 floats. The load and store operations are ``_mm256_load_ps()`` and 
``_mm256_store_ps()``. The addition and subtraction operations are ``_mm256_add_ps()``
and ``_mm256_sub_ps()``. User can refer to `Intel's Guide <https://software.intel.com/sites/landingpage/IntrinsicsGuide/>`_
for the detail signatures of this functions.


Example: Matrix-matrix Multiplication
=======================================

Matrix-matrix multiplication is another example where data-parallelism can be used 
for acceleration. Given two matrix :math:`A \in \mathbb{R}^{m\times n}` and ::math:`B \in \mathbb{R}^{n\times p}`,
the matrix-matrix multiplication maps them into another matrix :math:`C \in \mathbb{R}^{m\times p}`, where each 
element of :math:`C` is given by 

.. math::

    C_{\rm i, k} = \sum_{j=1}^{n} A_{\rm i,j}B_{\rm j,k}.

Many subroutines have been developed for different types of matrix-matrix multiplication. Here we implement
a simplified version of "DGEMM", the general matrix-matrix multiplication for double-precision floating-points, which 
can be found in the BLAS libraries.

A brute-force implementation simply consists of three nested loops::

    void dgemm_nonSIMD(const double *A, const double *B, double *C, 
        int m, int n, int p)
    {
        for(int i=0; i<m; ++i){
            for(int k=0; k<p; ++k){
                double c = 0.;
                for(int j=0; j<n; ++j){
                    c += A[i*n+j] * B[j*p+k];
                }
                C[i*p+k] = c;
            }
        }
    }

However, this algorithm is not optimized. Many optimization can be applied to accelerate it: using 
vector instructions, using loop expansion, using blocks to localize the memory access, and using multiple 
threads/processes, etc. Here we demonstrate how to use the vector instructions in the HIPP SIMD library to
accelerate the program. Based on the knowledge we have gained in the previous example, we could write::
    
    typedef HIPP::SIMD::Vec<double, 4> d4_t;

    void dgemm_SIMD(const double *A, const double *B, double *C, 
        int m, int n, int p){
        for(int i=0; i<m; ++i){
            for(int k=0; k<p; k+=4){
                d4_t c(0.), a, b;
                for(int j=0; j<n; ++j){
                    c += a.bcast(A+i*n+j) * b.load(B+j*p+k);
                }
                c.store(C+i*p+k);
            }
        }
    }

Here we use the AVX instructions for double precision floating-point values, i.e., we can pack four values in 
a vector.
As a result, we can compute four elements in :math:`C` at once: :math:`C_{\rm i, k}`, :math:`C_{\rm i, k+1}`, :math:`C_{\rm i, k+2}`
and :math:`C_{\rm i, k+3}`. 

For each ``j``, we load the element :math:`A_{\rm i,j}`, repeat it four times, and multiplies it with the four elements: 
:math:`B_{\rm j,k}`, :math:`B_{\rm j,k+1}`, :math:`B_{\rm j,k+2}` and :math:`B_{\rm j,k+3}`. We accumulate the result vector into 
the corresponding location of :math:`C`.

Note that ``load((double *)addr)`` method of a vector loads four contiguous elements at ``addr`` at once (the ``addr`` needs to be aligned),  
and ``bcast((double *)addr)`` loads a single value from ``addr`` and repeat it four times to make a vector. 
Each of these two operations returns the reference to the instance itself, so that it can be put into 
subsequent operations (multiplication in this example).
Finally we use ``store((double *)addr)`` to store a vector into an aligned address ``addr``.