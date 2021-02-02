Communicators and Parallel Libraries
=======================================

MPI provides an unusual feature - communicator, which is missing in many other 
parallel libraries. In many parallel libraries and applications, using the 
global "world" communicator (``MPI_COMM_WORLD`` in **Standard** MPI, or :func:`HIPP::MPI::Env::world` in HIPP)
is sufficient. However, it is always recomended not to use the global communicator
in any library development, just like that, in any programming, we should avoid using 
global varibles unless really necessary.

MPI's communicator is an encapsulation of 

- the **context** of point-to-point and collective communication, 
- the **process group** which participates the communication, 
- the **logic topology** of processes, 
- the **naming** of each process, 
- and the caching of context-specific **attributes**. 

We will explore all these features by examples.

Creating and Using Communicators
----------------------------------

New communicators can be created from any existing, old communicator. 
In the following, we demonstrate the creation and usage of user-defined communicator
with an example - computing :math:`\pi` by a Monte-Carlo algorithm. This example 
is taken from Ch-3.8 of [GroppW-UMPIv3]_. The full implementation can be 
found at :download:`mpi/pi-monte-carlo-computation.cpp </../example/tutorial/mpi/pi-monte-carlo-computation.cpp>`.

The algorithm is very famous, usually used as the first example of Monte-Carlo 
algorithm in many textbooks. It includes the following steps:

- Generate a set of :math:`N` random points. Each point :math:`(x,\,y)` is in the 2-dimensional square :math:`[-1,\,1]\times[-1,\,1]`.
- Count the number, :math:`N_{\rm in}`, of points in the unit circle :math:`\{(x,\,y)|x^2+y^2<1\}`. 
- Approximate :math:`\pi` by :math:`\pi = 4 N_{\rm in}/N`. 

The key step is to generate the random points. However, in a parallel implementation, the 
random number generation is a non-trival task. Here, just to demonstrate the 
communicators, we use a brute-force method - generate all the random numbers in 
a single process (referred as the master process thereafter), and spread them into other processes 
(referred as the slave processes). 
Note that this requires communication between the master and each slave.

For the slaves, once they determine their local :math:`N_{\rm in}`, a reduction operation is necessary
to sum up all the local :math:`N_{\rm in}`, and find the global estimate of :math:`\pi`. 
This can be achieved by a MPI "gather" operation from all slaves. Since the gather operation 
is a collective operation and all processes in a communicator must participate in it,
we must define a new communicator which excludes the master from the world communicator.
The communicators and communication calls can be described by the following figure:

.. _fig-tutor-mpi-basic-comm-pi-calc:
.. figure:: img/comm-pi-calc.png
    :figwidth: 50%
    :align: center

    **The "world" communicator and the "slave" communicator.** Each open circle represents 
    a single process.


As before, we get the world communicator from the :class:`HIPP::MPI::Env` object. We
let the first process (``rank==0``) to be the master which is responsible for 
random number generation::

    HIPP::MPI::Env env;
    auto world_comm = env.world();
    int rank = world_comm.rank(), master_rank = 0;

The slaves need a separate communicator, which can be created from the existing world
communicator by excluding the master::

    auto slaves_group = world_comm.group().excl({master_rank});
    auto slaves_comm = world_comm.create(slaves_group);

Here, the call ``world.group()`` returns an instance of :class:`HIPP::MPI::Group`, 
which represents the group of processes in the world 
communicator. The method :func:`excl <HIPP::MPI::Group::excl>` excludes a subset 
of the processes from the group (the master process in the example), and returns 
the new group. The :func:`create <HIPP::MPI::Comm::create>` method of the old communicator
returns a new communicator whose group of procecsses is specified by the argument.
The master process just gets a null communicator, because it is not in the slave group.

After setting the communicators done, it is easy to write the code for the master 
and for the slaves. The master just uses the world communicator. It waits on 
the communicator until receiving a request from other process. Then, it generates 
a sequence of ``chunk_size`` random number using class :class:`HIPP::NUMERICAL::UniformRealRandomNumber`
(use your own random number generator if the numerical module is not installed), and 
sents it back. If a stop signal is received, the master returns::

    void master_do(long long chunk_size, HIPP::MPI::Comm &world_comm){

        HIPP::NUMERICAL::UniformRealRandomNumber rng(-1., 1.);
        vector<double> rands(chunk_size);
        /* Wait for request. If request != 0, send back random numbers. */
        while(true){
            int request;
            auto status = world_comm.recv(HIPP::MPI::ANY_SOURCE, 0, 
                &request, 1, "int");
            if( !request ) break;
            rng(rands.begin(), rands.end());
            world_comm.send(status.source(), 0, rands);
        }
    }

The codes for the slaves are longer. Each slave sends a request to the master using 
the world communicator, and receives the random number sequence. Then it counts
the number of points in the unit circle according to the Monte-Carlo algorithm 
described above. The results from all slaves are "sum"-reduced using 
:func:`allgather <HIPP::MPI::Comm::allreduce>` method of the slave communicator.
If the precision satisfies the constraint given by ``eps``, or the maximal 
number of points is achieved, all slaves return and one of the slaves sends a 
stop signal to the master::

    const double REAL_PI = 3.141592653589793238462643;

    void slave_do(long long chunk_size, long long max_n_points, double eps, 
        HIPP::MPI::Comm &world_comm, HIPP::MPI::Comm &slaves_comm)
    {
        long long n_in = 0, n_out = 0;
        vector<double> rands(chunk_size);
        int request = 1;
        while(request){
            /* Request random numbers. */
            world_comm.send(0, 0, &request, 1, "int");
            world_comm.recv(0, 0, rands);
            
            /* Computing PI using Monte-Carlo method. Reduce the result into one 
            process. */
            for(long long i=0; i<chunk_size; i+=2){
                double x = rands[i], y = rands[i+1];
                if( x*x+y*y < 1 ) ++ n_in;
                else ++ n_out;
            }
            long long n_inout[2] = {n_in, n_out}, total_inout[2];
            slaves_comm.allreduce({n_inout, 2, "long long"}, total_inout, "+");
            double pi = (4.0*total_inout[0]) / (total_inout[0]+total_inout[1]);

            /* See if convergent. If it is, send a stop signal. */
            bool done = ( std::fabs(pi-REAL_PI) < eps ) 
                || (total_inout[0]+total_inout[1] > max_n_points);
            request = done ? 0 : 1;
            if( slaves_comm.rank() == 0 ){
                HIPP::pout << "pi=", pi, endl;
                if( done )
                    world_comm.send(0, 0, &request, 1, "int");
            }
        }
    }

Executing the code with 6 processes gives the output 

.. code-block:: text 

    pi=3.14707
    pi=3.14603
    pi=3.14185