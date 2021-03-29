Work Decomposition 
=====================


The following classes are all defined within namespace ``HIPP::MPI``.

.. namespace:: HIPP::MPI

.. class:: WorkDecomp

.. class:: WorkDecomp1D : public WorkDecomp

    .. function:: \
        static std::pair<size_t, size_t> uniform_block( \
            size_t gsize, size_t n_blocks, size_t rank) noexcept


        Decompose 1-d domain into uniform-distributed blocks.

        :arg gsize: global domain size, i.e., size to be decomposed.
        :arg n_blocks: number of of blocks.
        :arg rank: the rank of the target block. "rank" must be in the 
            range [0, n_blocks).

        :return: beginning and ending indices ``[b, e)`` of the target block are returned. 
        
        **Example:** ``gsize = 21``, ``n_blocks = 6``. Then, for different rank, the result is 

        ======= ===============
        rank    returned values
        ======= ===============
        0       0, 4
        1       4, 8
        2       8, 12
        3       12, 15
        4       15, 18
        5       18, 21
        ======= ===============
        
        The first three blocks has size 4, and the last three blocks has size 3.