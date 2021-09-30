Basic Communication
===============================================================

The following classes are all defined within namespace ``HIPP::MPI``.

.. namespace:: HIPP::MPI

Class Comm: the Communication Context 
----------------------------------------

.. class:: Comm : public MPIObj<_Comm>

    Comm is the high-level communicator interface for MPI system, which encapsulates
    the group of processes involved in the communication, the topology of them, and 
    also defines an isolated communication context. All point-to-point and collective 
    communications within this communicator are started by calling the method of 
    ``Comm`` instance.

    The instance of ``Comm`` should not be directly constructed. It should be
    returned by other function calls like the communicator management functions
    in ``Comm``, or by the :func:`Env::world` method. After you have a initial
    communicator like the the communication world, it is easy to operate on its 
    process group and use :func:`Comm::create` to create a new communicator.

    The ``Comm`` object can be **copy-constructed**, **copy-assigned**, **move-constructed**
    and **move-assigned**. The copy operation gives a object that refers to the same 
    commnicator (internally the same ``MPI_Comm``). 
    The copy operations, move operations and destructor are ``noexcept``.

    .. type::   std::function<bool(Comm &oldcomm, int keyval, void *extra_state, \
                    void *attr_val, void *&attr_val_out)> copy_attr_fn_t
                std::function<void(Comm &comm, int keyval, void *attr_val, \
                    void *extra_state)> del_attr_fn_t

        The attribute caching copy function and delete function types. See MPI **Standard**
        for the definition of attribute caching. HIPP allows you to provide 
        any function or function-like object (including lambda) as copy and delete 
        functions.
    
    .. member:: static const copy_attr_fn_t  NULL_COPY_FN
                static const copy_attr_fn_t  DUP_FN
                static const del_attr_fn_t  NULL_DEL_FN

        Predefined copy and delete functions for attribute caching. 
        ``NULL_COPY_FN`` does not copy attribute, and ``DUP_FN`` just copy 
        the attribute value. ``NULL_DEL_FN`` does nothing at the delete of 
        the attribute.


    .. function:: void free() noexcept

        Free the communicator and set the instance to the null value 
        (i.e., is_null()->true). 
        
        ``free()`` may be called on any object (even on a null value or other 
        predefined communicators) at any time.

    .. function:: ostream &info( ostream &os = cout, int fmt_cntl = 1 ) const
                  friend ostream & operator<<( ostream &os, const Comm &comm )

        ``info()`` displays some basic information of the communicator instance to ``os``.

        :arg fmt_cntl:  Control the display format. 0 for inline information and 1 for a verbose, multiple-line information. 2 for a exhausted priting, with lots of information to be printed.
        :return: The argument ``os`` is returned.

        The overloaded ``<<`` operator is equivalent to ``info()`` with default 
        ``fmt_cntl``.

        The returned reference of ``os`` allows you to chain the outputs, such as 
        ``comm.info(cout) << " continue printing " << endl``.
        
    .. function::   int size() const
                    int rank() const
                    bool is_null() const
                    bool is_inter() const
                    int remote_size() const

        Return the basic properties of the communicator.
        
        
        ``size``: number of processes in the process group of the communicator.
        
        ``rank``: the identifier of the calling process among all processes. It must
        be in the range ``[0, size)``.
        
        ``is_null``: whether or not the communicator is a null value 
        (internally, ``MPI_PROC_NULL``).
        
        ``is_inter``: whether or not the communicator is an inter-communicator.
        
        ``remote_size``: number of processes in the remote group for an 
        inter-communicator.
        
        For an inter-communicator, ``size`` and ``rank`` are the values of the local group.

    .. _api-mpi-comm-attribute-caching:

    .. function::   static int create_keyval( copy_attr_fn_t copy_attr_fn = NULL_COPY_FN,\
                        del_attr_fn_t del_attr_fn = NULL_DEL_FN, void *extra_state = nullptr )
                    static void free_keyval( int &keyval )
                    Comm & set_attr( int keyval, void *attr_val )
                    bool get_attr( int keyval, void * &attr_val ) const
                    Comm & del_attr( int keyval )
                    template<typename AttrT>\
                    static int create_keyval()
                    template<typename AttrT>\
                    bool get_attr( int keyval, AttrT * &attr_val ) const

        Attribute caching calls (see MPI **Standard** for a detailed description).

        ``create_keyval()`` creates a key value for attribute caching, with args specifying 
        its copy and delete functions, and passing extra state. The created key value is 
        returned. ``free_keyval()`` frees a key values specified by the arg ``keyval`` 
        (user should ensure that the key is not used, possibly by calling ``del_attr()`` 
        on call communicators that use this key). 
        
        If the attribute is set, the copy function is invoked on
        :func:`Comm::dup`, and the delete function is invoked on the destruction (when all 
        instances refering to the internal communicator object are destroyed)
        or ``Comm::set_attr()``.

        ``set_attr()`` sets the attribute corresponding to the ``keyval``. The attribute is 
        a type ``void *`` variable, typically cast from an integer or pointing to an address 
        of a heap object. ``get_attr()`` gets the attribute value corresponding to ``keyval`` into 
        arg ``attr_val`` and returns true. If the attribute is not set yet, returns false.
        ``del_attr()`` removes the attribute, which invoke the delete function. 

        In the templated version, the cached attribute set by user must be a pointer to
        ``AttrT`` which is dynamically allocated with new operator.
        The templated ``create_keyval()`` uses the copy constructor and destructor
        of ``AttrT`` as the copy function and delete function to make new heap object 
        and delete existing heap object, and set ``extra_state = nullptr``.

        Possible usage example::

            struct AttrT{
                int a = 1;
                double b = 2.;
            };

            keyval = HIPP::MPI::Comm::create_keyval<AttrT>(); // Create a key for attribute caching.

            comm.set_attr(keyval, new AttrT);          // Set the attribute to communicator 'comm'.
            auto new_comm = comm.dup();                // At 'dup', new_comm get a copy of attribute.
            AttrT *attr_val;
            if( new_comm.get_attr(keyval, attr_val) )  // Now we print it.
                cout << "a=" << attr_val->a << ", b=" << attr_val->b << endl;

            comm.del_attr(keyval);                     // Delete all attributes, and free the key.
            new_comm.del_attr(keyval);
            HIPP::MPI::Comm::free_keyval(keyval);

        Output (from a single process) is: 

        .. code-block:: text 

            a=1, b=2

    

    .. function::   Comm split( int color, int key = 0 )const
                    Comm dup() const
                    Comm create( const Group &group) const
                    static Comm world() noexcept
                    static Comm selfval() noexcept
                    static Comm nullval() noexcept
                    Comm create_inter( int local_leader, const Comm &peer_comm,\
                        int remote_leader, int tag )
                    Comm merge_inter( int high )
        
        Communicator management functions - get new communicaters or return existing
        communicaters. 

        Because communicater provides the context and topology of the communication,
        it is always a good idea to create new communicators, which helps encapsulate
        your library development and simplify the communication logic.
        
        ``split()`` splits the current group of processes into several disjoints ones,
        and returns communicators that host these new groups. 
        This is a collective operation of the processes in the old group, and the returned 
        communicater is that the caller process resides in.

        :arg color: processes with the same color is grouped into the same new group. If 
            a process does not want a new communicater, set ``color=UNDEFINED``,
            in such a case the split operation returns a null process as 
            returned by :func:`Comm::nullval()`.
        :arg key: specify the rank of processes in the new group. Process has a smaller 
            key will have a smaller rank in the new group. Processes with the same key
            will ordered according to their ranks in the old group.

        ``dup()`` copies the current communicator and retunrs a new one. This is a 
        collective operation of the old communicater. Note that the attribute cahched 
        will also be copied according to the copy function specified in the creation of 
        the key value (see :ref:`API/MPI/Comm Attribute Caching Calls <api-mpi-comm-attribute-caching>`).

        ``create()`` create new communicators according to the group arguments. Processes
        that you want to put in the same new communicater should call with a group argument
        containing them, with same rank order. Pass an empty group as returned by 
        ``Group::emptyvall()`` if a process does not need a new communicater. On return,
        the process that belongs to its group argument get a new communicater, if it does 
        not belongs to its group argument (e.g., an empty group), return a null communicater
        as returned by ``Comm::nullval()``.
        
        ``world()``, ``selfval()`` and ``nullval()`` return the predefined communicators - 
        the world communicater, the communicater that contains only self, and the null 
        communicater, respectively. These calls are local.
        
        ``create_inter()`` creates and returns a new inter-communicater. This call is collective 
        over the union of the local and remote groups.
        This must be called by 
        two groups of processes (two intra-communicaters), 
        and within in each of the group, processes provide 
        the same rank of the local leader. 
        The local leader must specify a peer communicater that contains at least self 
        and the remote leader (for non-leader process, ``remote_leader`` is not significant), 
        and a ``tag`` that is used for point-to-point communication on the creation of 
        the inter-communicator.
        
        ``merge_inter()`` merges the groups in a inter-communicator, and returns a 
        intra communicater.

        Example of creating an inter-communicator, and using it to perform the collective 
        communication::

            // Creation of an inter-communicator that has one master and multiple workers.
            int rank = comm.rank(), local_leader = 0,
                remote_leader = (rank==0)?1:0, tag = 0;
            auto inter_comm = comm.split(rank==0).create_inter(local_leader, 
                comm, remote_leader, tag);

            // Perform inter-collective-communication with the communicator.
            auto &dtype = HIPP::MPI::INT;
            if( rank == 0 ){
                int out_buff = rank, count = 1, root=HIPP::MPI::ROOT;
                vector<int> in_buff(inter_comm.remote_size());

                inter_comm.bcast(&out_buff, count, dtype, root);
                inter_comm.gather(NULL, 0, dtype, 
                    in_buff.data(), count, dtype, root);

                // To avoid output entanglement, use SeqBlock to serialize the following statements. 
                HIPP::MPI::SeqBlock seq(comm);
                cout << "Master " << "sends " << out_buff 
                    << " and recvs "; HIPP::prt_a(cout, in_buff) << endl;
            }else{
                int out_buff = rank, count = 1, in_buff, root = 0;
                inter_comm.bcast(&in_buff, count, dtype, root);
                inter_comm.gather(&out_buff, count, dtype, 
                    NULL, 0, dtype, root);

                HIPP::MPI::SeqBlock seq(comm);
                cout << "Worker " << inter_comm.rank() << " recvs " << in_buff 
                    << " and sends " << out_buff << endl; 
            }

        Here we create a master-slave model. The communication in such a model is perfectly
        described by an inter-communicator. We let rank-0 process in ``comm`` to become master,
        and the remaining processes are slaves/workers. Then, the master broadcast a message 
        to all workers, and workers reply the master by a gather. Note that we use the synchronization extension 
        :class:`MPI::SeqBlock` to avoid entanglement of the output (i.e., outputs will be serialized across processes).

        The output is (run with 5 processes in total)

        .. code-block:: text 

            Master sends 0 and recvs 1,2,3,4
            Worker 0 recvs 0 and sends 1
            Worker 1 recvs 0 and sends 2
            Worker 2 recvs 0 and sends 3
            Worker 3 recvs 0 and sends 4
    
    
    .. function::   const Group group() const
                    Group group()
                    const Group remote_group() const
                    Group remote_group()

            ``group()`` returns the (local) group of processes in the communicator. If this is an 
            inter-communicator, ``remote_group()`` returns the remote group of procecess.


    .. _api-mpi-comm-virtual-topology:

    .. function::   Comm cart_create( const vector<int> &dims, \
                        const vector<int> &periods, int reorder = 1 )const
                    static void dims_create( int nnodes, int ndims, vector<int> &dims )
                    int cartdim_get()const
                    void cart_get( vector<int> &dims, vector<int> &periods, \
                        vector<int> &coords )const
                    int cart_rank( const vector<int> &coords )const
                    vector<int> cart_coords( int rank )const
                    void cart_shift( int direction, int disp, \
                        int &rank_src, int &rank_dest )const
                    Comm cart_sub( const vector<int> &remain_dims )
                    int topo_test()const

        Virtual topology management.
        
        ``cart_create()`` create a new communicator with number of processes at each dimension 
        specified by ``dims``,
        whether periodic at each dimension specified by ``periods``. 
        If ``reorder`` is not zero then 
        implementation is allowed to reorder the ranks of the processes and then put then on 
        cartesian grids (otherwise processes are put by row-major order according to their ranks).
        If size of the origin communicator is larger than needed, processes that is not put 
        on the grids get null communicater as returned by :func:`Comm::nullval()`.

        ``dims_create()`` is a helpful function to determine the number of processes at each dimension
        from the number of process available in total, ``nnodes``, 
        and disired number of dimensions, ``ndims``. ``dims`` serves are both input and output arg, 
        on entry, positive ``dims[i]`` will not changed on exit, zero ``dims[i]`` will be changed to a 
        suitable value. Changed dims will be in an non-increasing order, 
        and they are as close as possible. If on entry, nnodes is not multiple of 
        prod(dims[i]) (for all dims[i] != 0), an error will occur. 

        It is valid to pass a `dims` with length not equal to ``ndims``. ``dims`` is 
        resized to ``ndims`` (padding with 0 if necessary).

        Example: a call of ``Comm::cart_create(nnodes, ndims, dims)`` gives results as 

        ================= ==========================
        nnodes and ndims   input and output dims  
        ================= ==========================
        6, 2              (0,0) -> (3,2)
        7, 2              (0,0) -> (7,1)
        6, 3              (0,3,0) -> (2,3,1)
        7, 3              (0,3,0) -> erroneous call 
        ================= ==========================

        For an communicator with cartesian topology, the following calls inquiry its information. 
        
        ``cartdim_get()`` returns the number of dimensions. ``cart_get()`` returns number of processes 
        in each dimension, whether each dimension is periodic, and the coordinates of the calling 
        process, into args ``dims``, ``periods`` and ``coords``, respectively.

        ``cart_rank()`` accepts coordinates ``coords`` in the topology and return its ``rank`` 
        in the communicator. For periodic dimension, ``coords[i]`` is shifted to valid range, otherwise an out-of-range 
        ``coords[i]`` is erroneous. For zero-dimensional topology, ``coords`` is not significant and the call retunrs 0.

        ``cart_coords()`` convert the ``rank`` in the communicator into the coordinates.

        ``cart_shift()`` find the neighbor ranks of the calling process at dimension specified by 
        ``direction`` and displacement (positive) specified by ``disp``. Return the ranks of the 
        processes offset by ``-disp`` and ``disp`` at this dimension into ``rank_src``, ``rank_dest``, 
        respectively.

        ``cart_sub()`` decompose the original cartesian topology into several sub-cartesian communicators.
        The remaining dimensions are passed as ``remain_dims``, and decomposition happens at the non-remaining 
        direction.

        ``topo_test()`` return the topology type of the communicator. Possible values are 
        :var:`UNDEFINED`, :var:`GRAPH`, :var:`CART`, :var:`DIST_GRAPH` in the ``HIPP::MPI`` namespace.

    .. _api-mpi-comm-rma-window-creation:

    .. function::   Win win_create(void *base, aint_t size, int disp_unit, \
                        const Info &info=Info::nullval()) const
                    Win win_create_dynamic(const Info &info=Info::nullval()) const
                    Win win_allocate(void *&base_ptr, \
                        aint_t size, int disp_unit, const Info &info=Info::nullval()) const
                    template<typename T>\
                    std::pair<Win, T*> win_allocate(size_t n, int disp_unit=sizeof(T), \
                        const Info &info=Info::nullval()) const
                    Win win_allocate_shared(void *&base_ptr, \
                        aint_t size, int disp_unit, const Info &info=Info::nullval()) const
                    template<typename T>\
                    std::pair<Win, T*> win_allocate_shared(size_t n, int disp_unit=sizeof(T), \
                        const Info &info=Info::nullval()) const

        Remote memory access (RMA) window creation functions.
        
        Those functions create RMA window by different 'flavors'. ``win_create()`` create 
        a RMA window by attaching a memory buffer starting at ``base`` with ``size`` bytes.
        ``win_allocate()`` does similar thing, but instead of attach user's buffer, it allocate 
        a memory buffer and return its address by ``base_ptr``. ``win_allocate_shared()`` is similar
        to ``win_allocate()`` but require the allocated memory can be directly load/store by 
        other processes (which is only possible for processes in a shared-memory system).

        The templated version of ``win_allocate()`` and ``win_allocate_shared()`` are usually 
        more convient than the non-templated version. They allocate memory for ``n`` type ``T``
        variables (i.e., sizeof(T)*n bytes).

        The common args of these functions are:

        :arg info: info object to 'hint' the implementation. A null info (as returned by 
            :func:`Info::nullval()` is always valid). See MPI **Standard** for 
            which hints are defined. See also the implementation 
            manual for implementation-specific hints.

        :arg disp_unit: specify the displacement unit in the RMA operation started by 
            any 'remote' process. It suggested to set 'disp_unit' to the size of the data 
            type if memory buffer
            is occupied by elements of a single type, or set to 1 otherwise.
    
        Example of creation and usage of the RMA window object::

            constexpr int N = 5;
            vector<double> out_buff(N, comm.rank());
            auto [win, in_buff] = comm.win_allocate<double>(N);
            int disp = 0;
            {   
                int dest = (comm.rank()+1) % comm.size();
                auto guard = win.fence_g();           // RMA synchronization call.
                win.put(dest, out_buff, disp);        // RMA PUT call.
            }
            {   
                // Now, sequentially print the local data and those received from 
                // other process.
                HIPP::MPI::SeqBlock seq(comm);        
                cout << "Rank: " << comm.rank() 
                    << " has put out ";
                HIPP::prt_a(cout, out_buff) 
                    << " and received ";
                HIPP::prt_a(cout, in_buff, in_buff+N) << endl; 
            }

        In the above, we create a RMA window and use it to put the local data in ``in_buff``
        to a remote window of the 'next' process in the communicator. With proper fence 
        synchronization, data will be seen by the remote process after exit of the first block.
        Each process is then print the data in its local buffer and those received from 
        other process. Note that we use the synchronization extension 
        :class:`MPI::SeqBlock` to avoid entanglement of the output (i.e., outputs will be serialized across processes).

        The output is (run with 3 processes)

        .. code-block:: text 

            Rank: 0 has put out 0,0,0,0,0 and received 2,2,2,2,2
            Rank: 1 has put out 1,1,1,1,1 and received 0,0,0,0,0
            Rank: 2 has put out 2,2,2,2,2 and received 1,1,1,1,1

    .. _api-mpi-comm-point-to-point:

    .. function:: \
        template<typename ...Args>\
        void send( int dest, int tag, Args && ...args ) const
        template<typename ...Args>\
        void bsend( int dest, int tag, Args && ...args ) const
        template<typename ...Args>\
        void ssend( int dest, int tag, Args && ...args ) const
        template<typename ...Args>\
        void rsend( int dest, int tag, Args && ...args ) const
        template<typename ...Args>\
        Status recv( int src, int tag, Args && ...args ) const
        template<typename ...Args>\
        Requests isend( int dest, int tag, Args && ...args ) const
        template<typename ...Args>\
        Requests ibsend( int dest, int tag, Args && ...args ) const
        template<typename ...Args>\
        Requests issend( int dest, int tag, Args && ...args ) const
        template<typename ...Args>\
        Requests irsend( int dest, int tag, Args && ...args ) const
        template<typename ...Args>\
        Requests irecv( int src, int tag, Args && ...args ) const
        
        Point-to-point communication calls - the core functions of MPI.

        ``send()``, ``bsend()``, ``ssend()``, ``rsend()``: blocking sendings in 
        standard mode, buffered mode, synchronous mode and ready mode. 
        Refer to the MPI standard for their semantics. The standard mode is usually 
        the first choice.
        
        ``recv()``: blocking receiving.
        
        Methods with prefix "i" are the corresponding non-blocking calls. They 
        return a :class:`~HIPP::MPI::Requests` object for later test, 
        cancellation, or completion.

        ``recv()`` returns a :class:`Status` object containing the meta-infomation of 
        the received message. 

        Any data buffer passed to these calls must not be pr-value - its life time 
        must last at least to the return of blocking calls or the finish of 
        non-blocking calls.

        Arguments are:

        - ``dest``, ``src``: rank of target process in the group that forming the 
          communicator. Receiving calls may use ``ANY_SOURCE`` for the matching
          of any source rank. All calls may use ``PROC_NULL`` so that the
          method returns immediately and has no effect.
        - ``tag``: matching tag. Receiving calls may use wildcard tag ``ANY_TAG`` for
          the matching of arbitrary tags.
        - ``args``: data to be sent/received. ``args`` are perfect-forwarded to 
          constructing a :class:`ConstDatapacket` object (for sending calls) or 
          :class:`Datapacket` object (for receiving calls) from which the buffer 
          specification is extracted and passed to the underlying communication 
          calls (see below examples).

        **Examples:** It is always valid to use the standard MPI buffer specification 
        style, i.e., by the ``buffer address, size, datatype`` triplet::
        
            int a[3] = {0, 1, 2};
            int dest = 1, tag = 0;
            
            comm.send(dest, tag, &a[0], 3, INT);      // Use the standard MPI style.

        In HIPP, any object or objects that can construct a Datapacket is also 
        allowed. So the above call is equivalent to::

            comm.send(dest, tag, a);                  // Use the HIPP style.

        The underlying implementation is like::

            auto [buff, size, dtype] = Datapacket {a};      // Construct the Datapacket and extract buffer specificaton.
        
            assert(buff == (void *)&a[0]);
            assert(size == 3);
            assert(dtype.raw() == MPI_INT);

            comm.send(dest, tag, buff, size, dtype);        // Forward the buffer specification.
            

    .. function:: \
        Status sendrecv(const Datapacket &send_dpacket, int dest, int sendtag, \
            const Datapacket &recv_dpacket, int src, int recvtag)
        Status sendrecv(const Datapacket &send_dpacket, int dest, int sendtag, \ 
            void *recvbuf, int src, int recvtag)
        Status sendrecv_replace(const Datapacket &dpacket, int dest, int sendtag, \ 
            int src, int recvtag)

        Perform one send and one receive in a single call.

        The second overload assumes the receiving buffer size and datatype are the 
        same with those of the sending buffer.

        The third overload uses a single buffer, i.e., received data replace the 
        sending data. Note that the underlying implementation may use additional 
        buffer.

    .. function:: \
            Status probe(int src, int tag) const
            Status iprobe(int src, int tag, int &flag) const
            std::pair<Status, Message> mprobe(int src, int tag) const
            std::pair<Status, Message> improbe(int src, int tag, int &flag) const

        The probe operations allow incoming messages to be checked for, without actually receiving them.
        In all probe calls, ``src`` and ``tag`` specify the target message to be checked for 
        (which can be wildcards), 
        in the calling communicators. The blocking version ``probe()`` and ``mprobe()`` wait until one 
        message is found, while the non-blocking version ``iprobe()`` and ``improbe()`` return immediately, 
        with the ``flag`` indicating whether the message is found.
        
        A :class:`Status` object is returned to allow the check of message details. 
        A **matched** version ``mprobe()`` or ``improbe()`` also return a :class:`Message` object
        to allow receiving calls precisely applied to the matched message, which may be helpful in a 
        threaded program.
        

    **Collective communication/computation calls**: According to MPI standard, some collective communication functions can
    apply to both inter and intra communicators. If inter-communicators
    are used, then only the 'all to all' functions are bi-directional, others
    are uni-directional.

    Some recv/send buffer can be specified with a :var:`IN_PLACE`, this
    is exactly the same as the **Standard** MPI_IN_PLACE.
    
    The non-blocking version here returns a :class:`Requests` object 
    for later testing
    and completion. The requests object should not be freed manually before
    completion.
    
    In all cases, the datatype argument mush be exactly a :class:`Datatype` 
    instance or
    an array of such. This is different from the point-to-point
    communication, where you can pass a string to indicate a basic type. One
    exception is ``alltoallw()`` and ``ialltoallw()``, in which the datatype 
    arguments is an array of original MPI datatype as returned by method :func:`Datatype::raw()` 
    (this design avoid the problem when using non-blocking collective operation, and also avoid 
    overhead in converting the datatype from high-level instance to MPI 
    original one).

    Please refer to the **Standard** for the detailed semantics of these collective calls.
    
    .. function::   \
        void barrier() const
        void bcast( void *buf, int count, const Datatype &dtype, int root) const
        void bcast(const Datapacket &dpacket, int root) const
        
    .. function::   \
        void gather( const void *sendbuf, int sendcount, const Datatype &sendtype, void *recvbuf, int recvcount, const Datatype &recvtype, int root) const
        void gather(const void *sendbuf, void *recvbuf, \
            int count, const Datatype &dtype, int root) const
        void gather(const ConstDatapacket &send_dpacket, \
            void *recvbuf, int root) const
        void gather(const ConstDatapacket &send_dpacket, \
            const Datapacket &recv_dpacket, \
            int root) const

        Gather calls.
        
        (1): MPI standard-compliant.
        
        (2)：Send and recv share the same datatype and count.
        
        (3,4): Same as (2) but the datatype and count are taken from 
        ``send_dpacket``.
        
        ``recvbuf``, ``recvcount``, ``recvtype`` - only significant at root. The send buffer 
        signature must match the recv buffer signature at root.

        For intra-communicator, ``sendbuf = IN_PLACE`` at root means in-place 
        sending. Then the ``sendcount`` and ``sendtype`` are ignored.

        For inter-communicator, in group A, root process passes ``root=ROOT``, other
        processes pass ``root=PROC_NULL``. In group B, all pass ``root`` eq to the 
        rank of root in A.
        
    .. function::   \
        void gatherv(const void *sendbuf, int sendcount, const Datatype &sendtype, void *recvbuf, const int recvcounts[], const int displs[], const Datatype &recvtype, int root ) const
        void gatherv(const ConstDatapacket &send_dpacket, void *recvbuf, \
            ContiguousBuffer<const int> recvcounts, \
            ContiguousBuffer<const int> displs,\
            const Datatype &recvtype, int root) const
        void gatherv(const ConstDatapacket &send_dpacket, \
            const Datapacket &recv_dpacket, \
            ContiguousBuffer<const int> recvcounts, \
            ContiguousBuffer<const int> displs, int root) const

        Variant of ``gather``, allowing processes sending different number of items.
        
        (1): MPI standard-compliant.
        
        (2): Use abstract concept arguments - Datapacket and ContiguousBuffer.
        
        (3): Same as (2) but recv datatype is inferred from the datapacket.

        ``recvbuf``, ``recvcounts``, ``displs``, ``recvtype`` specify the place to put the received data - 
        significant only at root.
        
        ``sendbuf = IN_PLACE`` is still available.

    .. function::   \
        void scatter(const void *sendbuf, int sendcount, const Datatype &sendtype, void *recvbuf, int recvcount, const Datatype &recvtype, int root )const
        void scatter(const void *sendbuf, void *recvbuf, int count, const Datatype &dtype, int root) const 
        void scatter(const void *sendbuf, const Datapacket &recv_dpacket, int root) const
        void scatter(const ConstDatapacket &send_dpacket,\
            const Datapacket &recv_dpacket, int root) const

        Scatter calls.

        (1): MPI standard-compliant.
        
        (2): Send and recv share the same datatype and count.
        
        (3,4): Same as (2) but datatype and count are taken from ``recv_dpacket``.
        
        ``sendbuf``, ``sendcount``, ``sendtype`` - only significant at root. The send buffer
        signature at root must match each recv buffer signature. Every 
        location in the send buffer cannot be read more than once.

        For intra-communicator, ``recvbuf = IN_PLACE`` at root mean in-place 
        sending. Then ``recvbuf`` and ``recvcount`` are ignored.

        For inter-communicator, in group A, root process passes ``root=ROOT``, other
        processes pass ``root=PROC_NULL``. In group B, all pass ``root`` eq to the 
        rank of root in A.
        
    .. function::   \
        void scatterv(const void *sendbuf, const int sendcounts[], const int displs[], const Datatype &sendtype, void *recvbuf, int recvcount, const Datatype &recvtype, int root) const
        void scatterv(const void *sendbuf, ContiguousBuffer<const int> sendcounts, ContiguousBuffer<const int> displs, const Datatype &sendtype, const Datapacket &recv_dpacket, int root) const
        void scatterv(const ConstDatapacket &send_dpacket, \
            ContiguousBuffer<const int> sendcounts, \
            ContiguousBuffer<const int> displs,\
            const Datapacket &recv_dpacket, int root) const

        Variants of scatter, allowing sending to processes different number of 
        items.
        
        (1): MPI standard-compliant.
        
        (2): Use abstract concept arguments - Datapacket and ContiguousBuffer.
        
        (3): Same as (2) but send datatype is inferred from the datapacket.

        The send buffer arguments are ignored at all processes except the root.
        ``recvbuf = IN_PLACE`` is still available.

    .. function::   \
        void allgather( const void *sendbuf, int sendcount, \
            const Datatype &sendtype,\
            void *recvbuf, int recvcount, const Datatype &recvtype ) const
        void allgather(const void *sendbuf, void *recvbuf, \
            int count, const Datatype &dtype) const
        void allgather(const ConstDatapacket &send_dpacket,\
            void *recvbuf) const
        void allgather(const ConstDatapacket &send_dpacket,\
            const Datapacket &recv_dpacket) const
        void allgatherv(\
            const void *sendbuf, int sendcount, const Datatype &sendtype, \
            void *recvbuf, const int recvcounts[], const int displs[],\
            const Datatype &recvtype ) const
        void allgatherv(\
            const ConstDatapacket &send_dpacket, void *recvbuf, \
            ContiguousBuffer<const int> recvcounts, \
            ContiguousBuffer<const int> displs,\
            const Datatype &recvtype) const
        void allgatherv(\
            const ConstDatapacket &send_dpacket, const Datapacket &recv_dpacket,\
            ContiguousBuffer<const int> recvcounts, \
            ContiguousBuffer<const int> displs) const

        All-variant to the gather/gatherv call - all processes received the data.

    .. function::   \
        void alltoall( const void *sendbuf, int sendcount, const Datatype &sendtype,\
            void *recvbuf, int recvcount, const Datatype &recvtype ) const
        void alltoall(const void *sendbuf, void *recvbuf, \
            int count, const Datatype &dtype) const
        
        All-to-all calls.
        
        (1): MPI standard-compliant.
        
        (2): send buffer and recv buffer share the same count and datatype.

        If send buffer is ``IN_PLACE``, the data are taken from and replace the recv
        buffer.

    
    .. function::   \
        void alltoallv( const void *sendbuf, const int sendcounts[], \
            const int senddispls[], const Datatype &sendtype,\
            void *recvbuf, const int recvcounts[], const int recvdispls[], \
            const Datatype &recvtype ) const
        void alltoallw( const void *sendbuf, const int sendcounts[], \
            const int senddispls[], const Datatype::mpi_t sendtypes[],\
            void *recvbuf, const int recvcounts[], const int recvdispls[], \
            const Datatype::mpi_t recvtypes[] ) const
        
    .. function::   \
        void reduce( const void *sendbuf, void *recvbuf, int count, \
            const Datatype &dtype, const Oppacket &op, int root ) const
        void reduce(const ConstDatapacket &send_dpacket, void *recvbuf,\
            const Oppacket &op, int root) const
        void reduce(const void *sendbuf, const Datapacket &recv_dpacket,\
            const Oppacket &op, int root) const
        void reduce(const ConstDatapacket &send_dpacket, \
            const Datapacket &recv_dpacket,\
            const Oppacket &op, int root) const
        void allreduce( const void *sendbuf, void *recvbuf, int count, \
            const Datatype &dtype, const Oppacket &op ) const
        void allreduce(const ConstDatapacket &send_dpacket, void *recvbuf, \
            const Oppacket &op ) const
        void allreduce(const void *sendbuf, const Datapacket &recv_dpacket,\
            const Oppacket &op ) const
        void allreduce(const ConstDatapacket &send_dpacket, \
            const Datapacket &recv_dpacket, const Oppacket &op) const

        Reduce calls.
        
        (1): MPI standard-compliant.
        
        (2,4): count and datatype are taken from ``send_dpacket``.
        
        (3): count and datatype are taken from ``recv_dpacket``.

        The same applies to ``allreduce()``.

        Set ``sendbuf=IN_PLACE`` at root implies in-place reduction, i.e., at root,
        data are taken from recv buffer and the results overwrite it.

    .. function::   \
        static void reduce_local(const ConstDatapacket &in_dpacket, void *inoutbuf, \
            const Oppacket &op)
        static void reduce_local(const ConstDatapacket &in_dpacket, \
            const Datapacket &inout_dpacket, const Oppacket &op)

        Reduce_local calls.
        
        (1): MPI standard-compliant.
        
        (2,3): count and datatype are taken from ``in_dpacket``.
        
    .. function::   \
        void reduce_scatter_block( const void *sendbuf, void *recvbuf, \
            int recvcount, const Datatype &dtype, const Oppacket &op ) const
        void reduce_scatter( const void *sendbuf, void *recvbuf, \
            const int recvcounts[], const Datatype &dtype, \
            const Oppacket &op )const
        void scan( const void *sendbuf, void *recvbuf, \
            int count, const Datatype &dtype, const Oppacket &op ) const
        void exscan( const void *sendbuf, void *recvbuf, \
            int count, const Datatype &dtype, const Oppacket &op ) const

    .. function::   \
        Requests ibarrier() const
        Requests ibcast( \
            void *buf, int count, const Datatype &dtype, int root) const
        Requests ibcast(const Datapacket &dpacket, int root) const
    
    .. function::   \
        Requests igather( \
            const void *sendbuf, int sendcount, const Datatype &sendtype, \
            void *recvbuf, int recvcount, const Datatype &recvtype, int root) const
        Requests igather(const void *sendbuf, void *recvbuf, \
            int count, const Datatype &dtype, int root) const
        Requests igather(const ConstDatapacket &send_dpacket, \
            void *recvbuf, int root) const
        Requests igather(const ConstDatapacket &send_dpacket, \
            const Datapacket &recv_dpacket, int root) const
        Requests igatherv(\
            const void *sendbuf, int sendcount, const Datatype &sendtype, \
            void *recvbuf, const int recvcounts[], const int displs[],\
            const Datatype &recvtype, int root ) const
        Requests igatherv(\
            const ConstDatapacket &send_dpacket, void *recvbuf, \
            ContiguousBuffer<const int> recvcounts, \
            ContiguousBuffer<const int> displs, \
            const Datatype &recvtype, int root ) const
        Requests igatherv(\
            const ConstDatapacket &send_dpacket, const Datapacket &recv_dpacket,\
            ContiguousBuffer<const int> recvcounts, \
            ContiguousBuffer<const int> displs, int root) const

    .. function::   \
        Requests iscatter(\
            const void *sendbuf, int sendcount, const Datatype &sendtype,\
            void *recvbuf, int recvcount, const Datatype &recvtype, int root )const
        Requests iscatter(const void *sendbuf, void *recvbuf, \
            int count, const Datatype &dtype, int root) const
        Requests iscatter(const void *sendbuf, \
            const Datapacket &recv_dpacket, int root) const
        Requests iscatter(const ConstDatapacket &send_dpacket,\
            const Datapacket &recv_dpacket, int root) const
        Requests iscatterv(\
            const void *sendbuf, const int sendcounts[], const int displs[], \
            const Datatype &sendtype,\
            void *recvbuf, int recvcount, const Datatype &recvtype, int root) const
        Requests iscatterv(\
            const void *sendbuf, ContiguousBuffer<const int> sendcounts, \
            ContiguousBuffer<const int> displs, const Datatype &sendtype,\
            const Datapacket &recv_dpacket, int root) const
        Requests iscatterv(\
            const ConstDatapacket send_dpacket, ContiguousBuffer<const int> sendcounts, \
            ContiguousBuffer<const int> displs,\
            const Datapacket &recv_dpacket, int root) const

    .. function::   \
        Requests iallgather( const void *sendbuf, int sendcount, \
            const Datatype &sendtype,\
            void *recvbuf, int recvcount, const Datatype &recvtype ) const
        Requests iallgather(const void *sendbuf, void *recvbuf, int count, \
            const Datatype &dtype) const
        Requests iallgather(const ConstDatapacket &send_dpacket, void *recvbuf) const
        Requests iallgather(const ConstDatapacket &send_dpacket, \
            const Datapacket &recv_dpacket) const
        Requests iallgatherv(\
            const void *sendbuf, int sendcount, const Datatype &sendtype, \
            void *recvbuf, const int recvcounts[], const int displs[],\
            const Datatype &recvtype ) const
        Requests iallgatherv(\
            const ConstDatapacket &send_dpacket,\
            void *recvbuf, ContiguousBuffer<const int> recvcounts, \
            ContiguousBuffer<const int> displs,\
            const Datatype &recvtype ) const
        Requests iallgatherv(\
            const ConstDatapacket &send_dpacket, const Datapacket &recv_dpacket, \
            ContiguousBuffer<const int> recvcounts, \
            ContiguousBuffer<const int> displs) const

    .. function::   \
        Requests ialltoall( const void *sendbuf, int sendcount, \
            const Datatype &sendtype,\
            void *recvbuf, int recvcount, const Datatype &recvtype ) const
        Requests ialltoall(const void *sendbuf, void *recvbuf, int count, \
            const Datatype &dtype) const
        Requests ialltoallv( const void *sendbuf, const int sendcounts[], \
            const int senddispls[], const Datatype &sendtype,\
            void *recvbuf, const int recvcounts[], const int recvdispls[], \
            const Datatype &recvtype ) const
        Requests ialltoallw( const void *sendbuf, const int sendcounts[], \
            const int senddispls[], const Datatype::mpi_t sendtypes[],\
            void *recvbuf, const int recvcounts[], const int recvdispls[], \
            const Datatype::mpi_t recvtypes[] ) const

    .. function::   \
        Requests ireduce( const void *sendbuf, void *recvbuf, int count, \
            const Datatype &dtype, const Oppacket &op, int root ) const
        Requests ireduce( const ConstDatapacket &send_dpacket, void *recvbuf, \
            const Oppacket &op, int root ) const
        Requests ireduce(const void *sendbuf, const Datapacket &recv_dpacket, \
            const Oppacket &op, int root ) const
        Requests ireduce(const ConstDatapacket &send_dpacket, \
            const Datapacket &recv_dpacket, const Oppacket &op, int root ) const
        Requests iallreduce( const void *sendbuf, void *recvbuf, int count, \
            const Datatype &dtype, const Oppacket &op ) const
        Requests iallreduce( const ConstDatapacket &send_dpacket, void *recvbuf, \
            const Oppacket &op ) const
        Requests iallreduce(const void *sendbuf, const Datapacket &recv_dpacket,\
            const Oppacket &op ) const
        Requests iallreduce(const ConstDatapacket &send_dpacket, \
            const Datapacket &recv_dpacket,\
            const Oppacket &op ) const
        Requests ireduce_scatter_block( const void *sendbuf, void *recvbuf, \
            int recvcount, const Datatype &dtype, const Oppacket &op ) const
        Requests ireduce_scatter( const void *sendbuf, void *recvbuf, \
            const int recvcounts[], const Datatype &dtype, \
            const Oppacket &op )const
        Requests iscan( const void *sendbuf, void *recvbuf, \
            int count, const Datatype &dtype, const Oppacket &op ) const
        Requests iexscan( const void *sendbuf, void *recvbuf, \
            int count, const Datatype &dtype, const Oppacket &op ) const


**Examples:**

A typical point-to-point communication is displayed. The process with rank 0 send 
a vector of values to each of the other processes::

    int rank = comm.rank(), size = comm.size();
    HIPP::MPI::Mutex mtx(comm);    // Initialize a mutex for exclusive printing.

    constexpr int count = 5, tag = 0;
    if( rank == 0 ){
        // Process 0 sends a vector of values to each of the other processes.
        for(int i=1; i<size; ++i){      
            vector<double> out_buff(count, i);
            comm.send(i, tag, out_buff);
        }
    }else{
        // Receive the vector of values from rank-0 process. Then print it.
        vector<double> in_buff(count);
        comm.recv(0, tag, in_buff.data(), count, HIPP::MPI::DOUBLE);

        mtx.lock();                // Avoid the entanglement of output.
        cout << "Rank " << rank << " receives ";
        HIPP::prt_a(cout, in_buff) << endl;
        mtx.unlock();
    }

We display two ways of specifying the communication buffer by using either 
a single vector or a standard MPI triplet ``(address, count, datatype)``.
Four ways of specifying buffer are avaiable, see :ref:`API/MPI/Point-to-point Communication <api-mpi-comm-point-to-point>`.
The output is (order may be different at runs)

.. code-block:: text

    Rank 4 receives 4,4,4,4,4
    Rank 2 receives 2,2,2,2,2
    Rank 1 receives 1,1,1,1,1
    Rank 3 receives 3,3,3,3,3

To show an alternative way of point-to-point communication, 
we use the non-standard mode ('ready' mode here). To use the 'ready' mode, 
the receive side prepares the receive buffer and start a non-blocking 
receive. It then notifies the sender to ask a response::

    if( rank == 0 ){
        /**
        * Process 0 waits for each of the other processes to send a notification.
        * Then it make a response.
        * The notification has no data, so buff = NULL, count = 0, 
        * datatype = any ("char" here).
        * The reponse uses a 'ready' mode because the target must get ready.
        */
        for(int i=1; i<size; ++i){
            // 
            auto status = comm.recv(HIPP::MPI::ANY_SOURCE, tag, NULL, 0, "char");
            vector<double> out_buff(count, i);
            comm.rsend(status.source(), tag, out_buff);
        }
    }else{
        /**
        * Prepare a buffer 'in_buff' and start the non-blocking recv.
        * Then, notify process 0 and wait for response.
        */
        vector<double> in_buff(count);
        auto request = comm.irecv(0, tag, in_buff);
        comm.send(0, tag, NULL, 0, "char");
        request.wait();

        mtx.lock();                 // Avoid the entanglement of output.
        cout << "Rank " << rank << " receives ";
        HIPP::prt_a(cout, in_buff) << endl;
        mtx.unlock();
    }

The output is similar to the previous example using standard send/recv.


Class Group: the Process Collection
-------------------------------------------

.. class::  Group: public MPIObj<_Group>

    ``Group`` is the high-level interface for group of process.

    As in the **Standard** MPI, a group of processes defines the process 
    name-ranking, which is the basis of
    point-to-point communication. a process group also defines the involved 
    processes of a collective communication.

    An initial group instance should be obtained
    from a communicator through method :func:`Comm::group`. 
    After that, you may apply the group transformation 
    functions (union, intersection, difference, ...) to create new groups based
    on existing groups.

    The life time of the group is manipulated by the instance, and you are not
    necessary to manually control it. However, you may call ``free()`` to 
    free the group instance in advance.

    The ``Group`` object can be **copy-constructed**, **copy-assigned**, **move-constructed**
    and **move-assigned**. The copy operation gives a object that refers to the same 
    process group (internally the same ``MPI_Group``). 
    The copy operations, move operations and destructor are ``noexcept``.

    .. function:: void free() noexcept
        
        free the group instance and set it to a null value as returned by 
        :func:`Group::nullval`.
        
        Calling ``free()`` is not necessary for any group, since the life time 
        is controlled automatically, but you may want to release the resources 
        in advance.
        
        ``free()`` can be called at any time, and even multiple times, and even 
        when the instance is a null value or a predefined value.
    
    
    
    .. function::   ostream &info( ostream &os = cout, int fmt_cntl = 1 ) const
                friend ostream & operator<<( ostream &os, const Group &group )

        ``info()`` prints the information of the current instance to the stream 
        ``os``.
    
        :arg fmt_cntl:   control the amount of information to be printed, 0 for a 
                 short and inline priting, 1 for a verbose, multi-line version.
        :return: The argument ``os`` is returned.
    
        The overloaded ``<<`` operator is equivalent to ``info()`` with 
        default ``fmt_cntl``.
    
    .. function::   int size() const
                int rank() const
                int is_null() const
                vector<int> translate_ranks( \
                    const vector<int> &ranks, const Group &othergroup )const
                int compare( const Group &othergroup )const

    
        Inquery the information of the group instance.

        ``size()`` gives the number of processes in this group.
        ``rank()`` returns the rank of the current process in this group. 
        If the calling process is not in the group, return UNDEFINED.
        
        ``is_null()`` tests whether the group is a null value/null instance. 
        
        ``translate_ranks()`` accepts the ranks of processes in the group instance, returns 
        their ranks in another group ``othergroup``.
        
        ``compare()``
        compares two groups. It may return IDENT, SIMILAR or UNEQUAL. See
        the **Standard** MPI specification for detail.
     
    
    .. function::   Group union_( const Group &othergroup )const
                Group intersection( const Group &othergroup )const
                Group difference( const Group &othergroup )const
                Group incl( const vector<int> &ranks )const
                Group excl( const vector<int> &ranks )const
                Group range_incl( const vector<int> &ranks )const
                Group range_excl( const vector<int> &ranks )const
                static Group emptyval() noexcept
                static Group nullval() noexcept

        Group transformation and creation functions.

        ``union_()``, ``intersection()`` or ``difference()`` operates on the 
        calling group instance and another group ``othergroup``, performs set-like 
        operation, and returned new group instace. The ranks of processes in 
        the new group is ordered according to their ranks in the calling group.
        In the ``union_()`` case,  if a process is not in the calling group, 
        but in ``othergroup``, it is appended after all processes in the calling 
        group and ranked according to its rank in ``other group``. The set 
        operations may give a empty group instance, which is identical
        to the one returned by ``emptyval()`` (i.e., the comparison using :func:`compare` method gives 
        :var:`IDENT`).

        ``incl()`` returns a new group that includes the processes specified by 
        ``ranks`` in the original group. If ``ranks.size()`` is zero, returns 
        a empty group. ``excl()``, on the other hand, excludes processes specified 
        by ``ranks`` in the original group and returns the new group.

        ``range_incl()`` and ``range_excl()`` are similar to ``incl()`` and ``excl()``, respectively.
        But these two calls use triplets to specified the ranks to be included or excluded.
        The argument, ``ranks``, must be ``{b1, e1, stride1, b2, e2, stride2, ...}``, where 
        each triplet ``{bk, ek, stridek}`` specifies processes with ranks ``bk``, ``bk+stridek``, 
        ``bk+2*stridek``, ..., ``bk+floor[(ek-bk)/stridek]*stridek``. It is valid that ``e < b && stridek < 0``,
        but invalid that ``stridek = 0``.

        ``emptyval()`` returns an empty group. ``nullval()`` returns a null group.
        Note that an empty group is different from a null group - the 
        former is  a valid group instance, the later is a **invalid** one that cannot be used 
        as an argument of many functions.


    **Example:**

    The following codes show how to create a new process group from a existing group::

        HIPP::MPI::Env env;
        auto comm = env.world();

        auto group = comm.group();
        auto new_group = group.incl({0,1,2});

        if( comm.rank() == 0 )
            cout << group << new_group;

    Starting from the world communicator returned by :func:`Env::world`, a call 
    of :func:`Comm::group` gives the group that contains all the processes.
    By using :func:`Group::incl`, the first three processes are picked out to give a new group. 
    The information of the old and new groups is printed. Outputs are (run with 6 processes)

    .. code-block:: text

        PP::MPI::Group instance [loc=0x7ffeba41f750, size=16, align=8]
        ----------
        Size info (size=6, rank=0)
        HIPP::MPI::Group instance [loc=0x7ffeba41f760, size=16, align=8]
        ----------
        Size info (size=3, rank=0)

    Note that you can get the same result by using ``auto new_group = group.range_incl({0,2,1})`` 
    instead of the ``incl()``.

Class Requests: the Non-blocking Handler
-------------------------------------------

.. class:: Requests : public MPIObj<_Requests>

    The high-level MPI requests interface. 
    
    A request is returned by a non-blocking communication call.
    A ``Requests`` object host an array of requests (internally, an array of ``MPI_Requests``). 
    The reason of allowing one object hosting an array of requests, not just a single request ,
    is that the later may cause overhaed in the multiple-completion call on requests.

    The ``Requests`` object can be **copy-constructed**, **copy-assigned**, **move-constructed**
    and **move-assigned**. The copy operation gives a object that refers to the same 
    array of requests. The copy operations, move operations and destructor are ``noexcept``.

    .. function::       Requests()

        Default constructor - construct an empty array of requests. User may later 
        put new requests into the instance by :func:`Requests::put()` or :func:`Requests::operator+=()`.

    .. function::       void free()
                        void clear()

        ``free()`` frees all requests in this instance, and set the current instance 
        to a null value as returned by :func:`Requests::nullval()`. 
        For persistent requests in the array of requests, ``free()`` frees them (so, make sure 
        that they are completed by completion calls). For other types of requests, ``free()``
        requires that they are already completed as become null values.

        ``clear()`` is similar to ``free()``, but it sets the current instance to an empty 
        request array. The difference is that the null value is a length-1 request array, 
        but an empty array is length-0.
    
    .. function::       ostream &info( ostream &os = cout, int fmt_cntl = 1 ) const
                        friend ostream & operator<<( ostream &os, const Requests &rqs )
        

        ``info()`` displays some basic information of the requests instance to ``os``.

        :arg fmt_cntl:  Control the display format. 0 for inline information and 1 for a verbose, multiple-line information.
        :return: The argument ``os`` is returned.

        The overloaded ``<<`` operator is equivalent to ``info()`` with default 
        ``fmt_cntl``.

        The returned reference of ``os`` allows you to chain the outputs, such as 
        ``requests.info(cout) << " continue printing " << std::endl``.

    
    
    .. function::       mpi_t raw(int i)const
                        bool is_null() const
                        bool is_null(int i) const
                        int size() const
                        bool empty() const

        Inquery the information of the current request array.
        ``is_null(i)`` tests whether the i-th request in the array is a null value,
        ``is_null()`` without an argument is equivalent to ``is_null(0)``.
        ``size()`` returns the number of requests in the array.
        ``empty()`` tests whether the array is empty.

    
    
    .. function::       static Requests nullval() noexcept

        Return a null value, which is a length-1 request array with the only 
        element to be a null value (Internally ``MPI_REQUEST_NULL``).

    
    
    .. function::   void put( Requests & rqs)
                    void put( Requests && rqs)
                    Requests & operator+=( Requests & rqs )
                    Requests & operator+=( Requests && rqs )
                    Requests get( int i )
                    Requests get( int b, int e )

        ``put()`` transfers the requests in ``rqs`` into the calling instance 
        (appended at the tail of the; order is kept). `rqs` becomes empty.
        
        Overloaded operator ``+=`` is equivalent to put().
        
        ``get()`` does the opposite thing, extracting the request(s) in the current
        instance and return them. 
        ``get(i)`` returns the i-th request, and get(b, e) returns a range of 
        requests indexed in the range [b, e).
        After the return of ``get()``, the returned requests are removed from the 
        caller instance, the hole is filled by the tail elements remaining in 
        the caller instance (the order may change). 

    .. function::   Status wait()
                    Status wait(int i)
                    Status test(int &flag)
                    Status test(int i, int &flag)
                    Status status(int &flag) const
                    Status status(int i, int &flag) const
                    Status waitany(int &index)
                    Status testany(int &index, int &flag)
                    void waitall(vector<Status> &statuses)
                    void testall(int &flag, vector<Status> &statuses)
                    void waitsome( int &count, vector<int> &indices, vector<Status> &statuses)
                    void testsome( int &count, vector<int> &indices, vector<Status> &statuses)

        Completion calls of the request(s). Please refer to the MPI **Standard** for detailed 
        semantics.

        ``wait()`` without argument is equivalent to ``wait(0)``. 
        ``test(flag)`` is equivalent to ``test(0, flag)``.

        ``status(flag)`` is equivalent to ``status(0, flag)``. The status call returns ``flag=true`` 
        if the communication is complete, and returns the a :class:`Status` object that describes the status
        of such. Otherwise it sets ``flag=false``. The status call differs from the test/wait call in that it 
        does not deallocate or inactivate the request. 

    .. function::   void cancel()
                    void cancel(int i)

        Calls that cancel the posted requests.
        ``cancel()`` is equivalent to ``cancel(0)``.

Class Status: the Return Status 
--------------------------------

.. class:: Status

    Communication status record.
    
    The ``Status`` class is binary-compatible with Standard **MPI_Status**, i.e.,
    a conversion from ``Status *`` to ``MPI_Status *`` is always valid. 
    This design is to reduce the overhead when waiting/testing multiple messages 
    in the non-block communications.

    The ``Status`` object can be **copy-constructed**, **copy-assigned**, **move-constructed**
    and **move-assigned**. The copy operation gives a object that has the same communication status 
    record. The copy operations, move operations and destructor are ``noexcept``.

    The default constructor of ``Status`` gives an object with uninitialized status record.
    
    .. function::   int source() const noexcept
                    int tag() const noexcept
                    int error() const noexcept
                    int count( const Datatype &dtype ) const
                    int count( const string &dtype ) const
                    bool test_cancelled() const
    
        Inquery the message properties.
        ``source()`` gives the rank of srouce process, ``tag()`` gives the tag of 
        the matched message, ``error()`` gives the error code, ``count()`` counts the data item,
        and ``test_cancelled()`` returns true if the message request is cancelled.
        
        The error code is set only when a multiple-completion call failed and
        an ``ERR_IN_STATUS`` is returned.
        
        :arg dtype:  pre-defined or derived datatype. Signature of ``dtype`` must match the datatype used in the communication that returns this status. Only pre-defined datatypes support the string version (see :class:`Datatype`).





        