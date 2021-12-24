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
    :class:     fix-width-table tight-table

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
                    

Pack and Unpack
----------------
If communications of a large number of small messages are necessary in a application, 
it may be better to pack them into a single message buffer, and send the buffer by 
one MPI call. Possible reasons for that include 

- To be compatible with old communication libraries where pack/unpack is manually done 
  by the user.
- Multiple pieces of data may depend on each other. In this case a derived datatype 
  does not help.
- Controlling the buffer manually to override the default buffering strategy used by 
  the MPI implementation. This is useful when developping a communication library 
  on the top of MPI.

In the following of this section, we show how to pack different data items into a 
single buffer, send them to another procecss, and unpack it. 
The full code can be downloaded at :download:`mpi/dtype-pack.cpp </../example/tutorial/mpi/dtype-pack.cpp>`.

As usual, we define the MPI environment at the entry of the program::
    
    HIPP::MPI::Env env;
    auto comm = env.world();
    int rank = comm.rank();
    int tag = 0;

Here, ``tag`` will be used to match the point-to-point communication.

Assume, at process 0, we have different data items to be sent. They may be scalar, vector, or 
any other data layout that can be used in MPI communications::

        int x = 0; 
        double y[3] = {1.,2.,3.}; 
        vector<float> z = {4.0f, 5.0f, 6.0f};
        string s = "Hello World";
        int nz = z.size(), ns = s.size();

Here we have a integer scalar ``x``, a fixed-length array ``y``, a variable length vector ``z``, and 
a string ``s``. Because the lengths of ``z`` and ``s`` are usually determined at runtime, we 
have to also send their lengths to another process.

To send them by one call of MPI, we have to pack them. We first declare a :class:`Pack <HIPP::MPI::Pack>`
instance, and then :func:`push <HIPP::MPI::Pack::push>` data items into it, and finally we send it 
by a single :func:`send <HIPP::MPI::Comm::send>` call::

        HIPP::MPI::Pack pack(comm);
        pack.push(x).push(y).push(nz).push(z).push(ns).push(s);
        comm.send(1, tag, pack.as_sendbuf());

Here, the ``comm`` argument is necessary for the :class:`Pack <HIPP::MPI::Pack>`, because 
different communicator may use different data representation in the commnication. Note that 
the :func:`push <HIPP::MPI::Pack::push>` call can be chained to push data items one by one.
Each data item passed into :func:`push <HIPP::MPI::Pack::push>` is converted to 
a :class:`Datapacket <HIPP::MPI::Datapacket>` instance, like for the data buffer
we used in the point-to-point or collective commnication. Finally, 
by calling :func:`as_sendbuf <HIPP::MPI::Pack::as_sendbuf>`
we convert the packed data items into a instance that can be used in any MPI commnication, 
and send it to process 1.

In process 1, we first prepare the data items into which the message is unpacked::

    int x; double y[3]; vector<float> z;
    vector<char> s;
    int nz, ns;

Then, we :func:`probe <HIPP::MPI::Comm::probe>` the message length, and use
this length to initialize the :class:`Pack <HIPP::MPI::Pack>` instance. This 
allows the :class:`Pack <HIPP::MPI::Pack>` instance pre-allocates space 
for the incoming message. By calling :func:`as_recvbuf <HIPP::MPI::Pack::as_recvbuf>`, 
the :class:`Pack <HIPP::MPI::Pack>` instance is converted to a receiving buffer 
that can be used in any commnication::

    HIPP::MPI::Pack pack(comm.probe(0, tag).count("packed"), comm);
    comm.recv(0, tag, pack.as_recvbuf());

Once the receiving call is finished, we unpack the buffer into data items by 
:func:`pop <HIPP::MPI::Pack::pop>`. Note that ``z`` and ``s`` both have variable lengths,
so we have to unpack their lengths first, resize them, and unpack the real data into them::

    pack.pop(x).pop(y);
            
    pack.pop(nz);
    z.resize(nz);
    pack.pop(z);

    pack.pop(ns);
    s.resize(ns+1);
    pack.pop(s); 
    s.back() = '\0';

We can print the received data items by::

    HIPP::pout << "x=", x, 
        ", y=(", HIPP::pout(y,y+3), ")", 
        ", z=(", z, ")", 
        ", s='", s.data(), "'\n";

The output is

.. code-block:: text 

    x=0, y=(1,2,3), z=(4,5,6), s='Hello World'


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




