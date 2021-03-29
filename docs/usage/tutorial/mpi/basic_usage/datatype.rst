Fun with Datatypes 
========================


.. include:: /global.rst 

One of the unusual features of MPI is the introduction of a datatype argument for 
all messages sent and received. MPI has :ref:`predefined datatypes <api-mpi-predefined-dtype>`
that corresponds to the base datatypes in the host programming language - integers, 
floating-point numbers, etc. MPI also allows users to created derived datatypes 
from those predefined ones. 

Derived Datatypes 
------------------

MPI uses typemap to describe a general datatype. However, it is not so convenient in 
many cases, particularly when the datatype has a large number of entries. MPI provides 
a number of ways to create datatypes without explicitly contructing the typemap, from 
simple to complex:

.. table:: 
    :widths:    25 75
    :class:     fix-width-table

    ======================= ===========================================================================
    Datatype Constructor        Description
    ======================= ===========================================================================
    Contiguous                  Simplest derived datatype - ``count`` elements of an existing datatype.
    Vector                      Slightly generalization of contiguous type. Allow regular gap between 
                                blocks of elements. Blocks has equal lengths. It has 
                                a variant called hvector.
    Indexed_block               More general - the gaps between equal-length blocks may be different.
                                It has a variant called hindexed_block.
    Indexed                     Both the gaps and block lengths may be different.
                                It has a variant called hindexed.
    Struct                      Most general one - the datatypes, lengths of blocks and the gaps 
                                between blocks may be different.
    ======================= ===========================================================================         

Methods in :class:`Datatype <HIPP::MPI::Datatype>` class can be used to construct derived datatypes. 
We will discuss the contiguous type here. The method :func:`Datatype::contiguous <HIPP::MPI::Datatype::contiguous>`
produces a new datatype by making ``count`` copies of an existing one. For example, we can construct a 
datatype consisting of 4 double precision floating-point number by::

    auto &oldtype = HIPP::MPI::DOUBLE;
    auto newtype = oldtype.contiguous(4);

Then, we can use the new type in a send/recv call::

    comm.send(dest, tag, buff, 1, newtype);

which sends ``1 newtype`` data (i.e., 4 double).

In fact, the call of send/recv with ``count`` elementary typed elements as the argument 
is equivalent to construct a contiguous type, and send ``1`` new typed element. So, the 
above send is equivalent to::

    comm.send(dest, tag, buff, 4, HIPP::MPI::DOUBLE);
    
    // Or, as a shortcut:
    comm.send(dest, tag, buff, 4, "double");
                    

Applications 
---------------------------------

The N-body Problem 
"""""""""""""""""""

Many simulations involve computing the interaction between pairs particles, which is called N-body 
problem. Such a problem typically requires :math:`\mathcal{O}(N^2)` computation. Therefore, MPI 
can help to distribute the computation among processes because the overhead is small relative to 
the force computation. What we need is to determine how to send/recv data among processes. The following 
example is taken from Ch-5.2 of [GroppW-UMPIv3]_.

In a C++ application, a particle is typically represented by a struct, and particles can be 
stored in an array e.g.::

    struct Particle {
        double x, y, z;
        double mass;
    };
    vector<Particle> particles(N_PARTICLES);

To send/recv the data, we could just send/recv 4 doubles for each particle. But it makes 
more sense to create a new datatype for the particle type. 
We can create a new datatype consisting of four doubles::

    auto dtype_particle = HIPP::MPI::DOUBLE.contiguous(4);

In the simplest approach, we could just send all particles in a procecss to all of the others,
and compute the force with the received copies of particles. For example, each process could 
do::

    int n_procs = comm.size(), rank = comm.rank();
    for(int i = 0; i < n_procs; ++i) {
        if( i != rank )
            comm.send(i, tag, particles.data(), N_PARTICLES, dtype_particle);
    }

    vector<Particle> recv_particles(N_PARTICLES);
    for(int i = 0; i < n_procs; ++i) {
        if( i != rank )
            comm.recv(i, tag, recv_particles.data(), N_PARTICLES, dtype_particle);
    }

This code has many problems: it does not scale; it may deadlock. We could use 
other sending modes to avoid the deadlock problem, but fortunately, MPI provides 
routines for this common case. We will show how to use allgather and allgatherv 
to efficiently communicate data between procecsses.

Matrix Transposition
"""""""""""""""""""""

The following codes show how to transpose a maxtrix using MPI derived datatypes. 
Users can find more details in Ch-14.5 of [DuZH-MPIv2001]_. 

:download:`mpi/dtype-matrix-transpose.cpp </../example/tutorial/mpi/dtype-matrix-transpose.cpp>`

.. include:: /../example/tutorial/mpi/dtype-matrix-transpose.cpp 
    :code: cpp

The key operation is to define a new datatype ``col_type`` which represents 
a column of the matrix ``mat``. This can be achieved by using the :func:`vector <HIPP::MPI::Datatype::vector>`
method on an existing datatype (:var:`HIPP::MPI::DOUBLE` here). The vector type has 
``nrow`` blocks. Each block has only 1 element. Adjacent blocks are strided by ``ncol`` elements.

The next operation :func:`resized <HIPP::MPI::Datatype::resized>` is called on the vector 
datatype to create a new datatype with correct extent ``sizeof(double)``, which means that adjacent 
vectors are offset by a double value.

The matrix ``mat`` is sent as ``ncol`` column vectors. It is received into ``mat_T`` directly, which means 
the original columns are filled into the new rows. Because the sending and receiving are called 
by the same process, we use :func:`sendrecv <HIPP::MPI::Comm::sendrecv>` method of the communicator 
to avoid deadlock.  

The output is (executed using 1 process):

.. code-block:: text 

    M =
      0,  1,  2,  3,
      4,  5,  6,  7,
      8,  9, 10, 11
    Transposed M =
      0,  1,  2,
      3,  4,  5,
      6,  7,  8,
      9, 10, 11




