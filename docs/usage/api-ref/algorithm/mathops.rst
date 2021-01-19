Mathematical Operations
=============================

The following classes are all defined within the namespace ``HIPP::ALGORITHM``.

.. namespace:: HIPP::ALGORITHM

.. class::  OuterProduct

        .. function:: template<typename It, typename UnaryOp>\
                        static void indices(It b, It e, UnaryOp &&op)

        .. function:: template<typename It, typename UnaryOp, typename ItStk>\
                        static void indices(It b, It e, UnaryOp &&op, ItStk it_stk)


        Outer product of index lists, i.e., :math:`(0,1,...,n_0-1)\times(0,1,...,n_1-1), ..., (0,1,...,n_{K-1}-1)`,
        where :math:`\times` is the Cartesian product. 
        The result is an array of :math:`n=n_0 \times n_1 \times ... \times n_{K-1}` vectors.

        The size :math:`n` can be extremely large. To avoid significant memory consumption, the 
        above functions do not return the results, but accept an operator which acts on each of the 
        result vectors.

        **Arguments:**

        Template paramsters ``It`` and ``ItStk`` must be random accessible iterators 
        referring to signed or unsigned integers.

        ``b`` and ``e`` define the upper limits of indices, i.e., the loop on the range ``[b, e)`` gives
        :math:`n_0, n_1, ..., n_{K-1}` (all must be positive), 
        where ``e-b==K``. If ``b >= e``, the functions have no effect.

        ``op`` is the operator acting on each of the result vector. It is called as ``op(ptr)`` where ``ptr``
        is a random access iterator referring to the first element of the result vector. 
        In the first overload, ``ptr`` is a pointer to an array of elements with the same type as ``*b``.
        In the second overload, ``ptr`` is typed ``ItStk``. 
     
        ``it_stk`` is a buffer. If provided, internal buffer allocation is avoided. The buffer length must be 
        equal or large than ``max(e-b, 0)+1``.  

        **Complexity:**

        The algorithm has a time complexity :math:`\mathcal{O}(n)`. 
        
        In the first overload, a buffer 
        sized ``K+1`` is allocated. In the second overload, no extra space is allocated.

        **Example:**

        .. code-block::

            const int n = 2;
            int maxs[n] = {2, 3};
            auto op = [n](int *p){ pout << '(', pout(p, p+n), ") "; };
            OuterProduct::indices(maxs, maxs+n, op);

        Output are

        .. code-block:: text 

            (0,0) (0,1) (0,2) (1,0) (1,1) (1,2)
