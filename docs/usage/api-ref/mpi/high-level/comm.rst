High-level API - Process Group and Communication
===============================================================

.. namespace:: HIPP::MPI

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

    .. type::   std::function<bool(Comm &oldcomm, int keyval, void *extra_state, void *attr_val, void *&attr_val_out)> copy_attr_fn_t
                    std::function<void(Comm &comm, int keyval, void *attr_val, void *extra_state)> del_attr_fn_t

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

        free the current communicator instance, and set it to a null 
        communicator as returned by :func:`Comm::nullval`.
        ``free`` can be called at any time and repeatly, 
        even for the pre-defined 
        communicators and null communicators.

    .. function:: ostream &info( ostream &os = cout, int fmt_cntl = 1 ) const
                  friend ostream & operator<<( ostream &os, const Comm &comm )

        ``info()`` displays some basic information of the communicator instance to ``os``.

        :arg fmt_cntl:  Control the display format. 0 for inline information and 1 for a verbose, multiple-line information. 2 for a exhausted priting, with lots of information to be printed.
        :return: The argument ``os`` is returned.

        The overloaded `<<` operator is equivalent to info() with the default 
        ``fmt_cntl``.

        The returned reference of ``os`` allows you to chain the outputs, such as 
        ``comm.info(cout) << " continue printing " << std::endl``.
        
    .. function::    int size() const
                    int rank() const
                    bool is_null() const
                    bool is_inter() const
                    int remote_size() const
        
        Return the basic information of the communicater instance: ``size()`` returns the number of
        processes in the group that forms this communicater. ``rank()`` returns the identifier of 
        the caller process, which is in the range of [0, size()). ``is_null()`` tests whether this 
        is a NULL communicater (internally MPI_PROC_NULL). ``is_inter()`` tests whether this is 
        an inter-communicater, if it is, ``remote_size()`` returns the number of processes in the 
        remote group of the inter-communicater.

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
            a process does not want a new communicater, set ``color=HIPP::MPI::UNDEFINED``,
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

    .. function::   template<typename ...Args>\
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
        
        Point-to-point communication functions - send messages and receive messages. These are traditional calls lying in the heart of MPI.
        
        The MPI **Standard** defines both blocking and non-blocking versions 
        (the later is started with an 'i'). For each version, four send modes are 
        provided: the standard mode (``send()/isend()``), the buffered mode (``bsend()/ibsend()``),
        the synchronous mode (``ssend()/issend()``) and the ready mode (``rsend()/irsend()``).
        If you are not sure about the semantics of the communication modes, choose the standard 
        mode. Otherwise it is suggested to carefully read the **Standard** specifications 
        before using non-standard modes. 

        The blocking ``recv()`` returns 
        a :class:`Status` object containing the information that has been received. The 
        non-blocking isends and ``irecv()`` return :class:`Requests` object for 
        handling the completion/testing of the communication.

        Common arguments of these calls are: 

        :arg dest: rank of the target process of the communication, i.e., 
            source of a recv call and target of a send call. Recv calls can use 
            the wildcard :var:`HIPP::MPI::ANY_SOURCE` to match message from any process. 
            Both recv and send calls 
            can specify :var:`HIPP::MPI::PROC_NULL` as target rank, then the call 
            has no effect and returns immediately.
        :arg tag:  a tag for matching the send/recv operation pairs. 
            Wildcard :var:'HIPP::MPI::ANY_TAG' are allowed for recv calls to match any tags.
        :arg args: specify the data buffer to be sent/received. Four cases are valid, see below.

        The valid ``args`` are: 
        
        - ``(const) void *buff, int size, HIPP::MPI::Datatype dtype``: the most **Standard** way of specifying a buffer in MPI, the starting address ``buff``, number of elements ``size`` of type ``dtype``.  

        - ``(const) void *buff, int size, const std::string dtype``: similar to the first, but use a string to specify the datatype. Only predefined datatypes are allowed, such as int, float, etc (see :class:`HIPP::MPI::Datatype`).   

        - ``(const) vector<T, A> & v`` where T are any predefined types, such as int, float, etc (see :class:`HIPP::MPI::Datatype`): send from/recv to the vector of elements, which is equivalent to the triplet ``v.data(), v.size(), "T"``.   

        - ``const std::string &s``: send the string of characters. Only send call can use this signature, because a `std::string` is not writable.

        The first three cases accept both const and non-const version. Due to the semantics 
        of point-to-point communication, it is erroneous to pass a pure-right-value or x-value 
        (i.e., a temporary variable) as ``v`` or ``s``, because, user must ensure not 
        using the buffer before the completion of communication.
        
    
    .. function::   void barrier() const
                    void bcast( void *buf, int count, const Datatype &dtype, int root) const
                    void gather( const void *sendbuf, int sendcount, const Datatype &sendtype, \
                        void *recvbuf, int recvcount, const Datatype &recvtype, int root) const
                    void gatherv(\
                        const void *sendbuf, int sendcount, const Datatype &sendtype, \
                        void *recvbuf, const int recvcounts[], const int displs[],\
                        const Datatype &recvtype, int root ) const
                    void scatter(\
                        const void *sendbuf, int sendcount, const Datatype &sendtype,\
                        void *recvbuf, int recvcount, const Datatype &recvtype, int root )const
                    void scatterv(\
                        const void *sendbuf, const int sendcounts[], const int displs[], \
                        const Datatype &sendtype,\
                        void *recvbuf, int recvcount, const Datatype &recvtype, int root) const
                    void allgather( const void *sendbuf, int sendcount, \
                        const Datatype &sendtype,\
                        void *recvbuf, int recvcount, const Datatype &recvtype ) const
                    void allgatherv(\
                        const void *sendbuf, int sendcount, const Datatype &sendtype, \
                        void *recvbuf, const int recvcounts[], const int displs[],\
                        const Datatype &recvtype ) const
                    void alltoall( const void *sendbuf, int sendcount, const Datatype &sendtype,\
                        void *recvbuf, int recvcount, const Datatype &recvtype ) const
                    void alltoallv( const void *sendbuf, const int sendcounts[], \
                        const int senddispls[], const Datatype &sendtype,\
                        void *recvbuf, const int recvcounts[], const int recvdispls[], \
                        const Datatype &recvtype ) const
                    void alltoallw( const void *sendbuf, const int sendcounts[], \
                        const int senddispls[], const Datatype::mpi_t sendtypes[],\
                        void *recvbuf, const int recvcounts[], const int recvdispls[], \
                        const Datatype::mpi_t recvtypes[] ) const
                    void reduce( const void *sendbuf, void *recvbuf, int count, \
                        const Datatype &dtype, const Oppacket &op, int root ) const
                    void allreduce( const void *sendbuf, void *recvbuf, int count, \
                        const Datatype &dtype, const Oppacket &op ) const
                    static void reduce_local( const void *inbuf, void *inoutbuf, int count, \
                        const Datatype &dtype, const Oppacket &op )
                    void reduce_scatter_block( const void *sendbuf, void *recvbuf, \
                        int recvcount, const Datatype &dtype, const Oppacket &op ) const
                    void reduce_scatter( const void *sendbuf, void *recvbuf, \
                        const int recvcounts[], const Datatype &dtype, \
                        const Oppacket &op )const
                    void scan( const void *sendbuf, void *recvbuf, \
                        int count, const Datatype &dtype, const Oppacket &op ) const
                    void exscan( const void *sendbuf, void *recvbuf, \
                        int count, const Datatype &dtype, const Oppacket &op ) const
                    Requests ibarrier() const
                    Requests ibcast( \
                        void *buf, int count, const Datatype &dtype, int root) const
                    Requests igather( \
                        const void *sendbuf, int sendcount, const Datatype &sendtype, \
                        void *recvbuf, int recvcount, const Datatype &recvtype, int root) const
                    Requests igatherv(\
                        const void *sendbuf, int sendcount, const Datatype &sendtype, \
                        void *recvbuf, const int recvcounts[], const int displs[],\
                        const Datatype &recvtype, int root ) const
                    Requests iscatter(\
                        const void *sendbuf, int sendcount, const Datatype &sendtype,\
                        void *recvbuf, int recvcount, const Datatype &recvtype, int root )const
                    Requests iscatterv(\
                        const void *sendbuf, const int sendcounts[], const int displs[], \
                        const Datatype &sendtype,\
                        void *recvbuf, int recvcount, const Datatype &recvtype, int root) const
                    Requests iallgather( const void *sendbuf, int sendcount, \
                        const Datatype &sendtype,\
                        void *recvbuf, int recvcount, const Datatype &recvtype ) const
                    Requests iallgatherv(\
                        const void *sendbuf, int sendcount, const Datatype &sendtype, \
                        void *recvbuf, const int recvcounts[], const int displs[],\
                        const Datatype &recvtype ) const
                    Requests ialltoall( const void *sendbuf, int sendcount, \
                        const Datatype &sendtype,\
                        void *recvbuf, int recvcount, const Datatype &recvtype ) const
                    Requests ialltoallv( const void *sendbuf, const int sendcounts[], \
                        const int senddispls[], const Datatype &sendtype,\
                        void *recvbuf, const int recvcounts[], const int recvdispls[], \
                        const Datatype &recvtype ) const
                    Requests ialltoallw( const void *sendbuf, const int sendcounts[], \
                        const int senddispls[], const Datatype::mpi_t sendtypes[],\
                        void *recvbuf, const int recvcounts[], const int recvdispls[], \
                        const Datatype::mpi_t recvtypes[] ) const
                    Requests ireduce( const void *sendbuf, void *recvbuf, int count, \
                        const Datatype &dtype, const Oppacket &op, int root ) const
                    Requests iallreduce( const void *sendbuf, void *recvbuf, int count, \
                        const Datatype &dtype, const Oppacket &op ) const
                    Requests ireduce_scatter_block( const void *sendbuf, void *recvbuf, \
                        int recvcount, const Datatype &dtype, const Oppacket &op ) const
                    Requests ireduce_scatter( const void *sendbuf, void *recvbuf, \
                        const int recvcounts[], const Datatype &dtype, \
                        const Oppacket &op )const
                    Requests iscan( const void *sendbuf, void *recvbuf, \
                        int count, const Datatype &dtype, const Oppacket &op ) const
                    Requests iexscan( const void *sendbuf, void *recvbuf, \
                        int count, const Datatype &dtype, const Oppacket &op ) const

        Collective communication functions for data movement or group computation.
    
        According to MPI standard, some collective communication functions can
        apply to both inter and intra communicators. If inter-communicators
        are used, then only the 'all to all' functions are bi-directional, others
        are uni-directional.

        Some recv/send buffer can be specified with a ``HIPP::MPI::IN_PLACE``, this
        is exactly the same as the **Standard** MPI_IN_PLACE.
        
        The non-blocking version here returns a :class:`HIPP::MPI::Requests` object 
        for later testing
        and completion. The requests object should not be freed manually before
        completion.
        
        In all cases, the datatype argument mush be exactly a :class:`HIPP::MPI::Datatype` 
        instance or
        an array of such. This is different from the point-to-point
        communication, where you can pass a string to indicate a basic type. One
        exception is ``alltoallw()`` and ``ialltoallw()``, in which the datatype 
        arguments is an array of original MPI datatype as returned by method :func:`HIPP::MPI::Datatype::raw()` 
        (this design avoid the problem when using non-blocking collective operation, and also avoid 
        overhead in converting the datatype from high-level instance to MPI 
        original one).

        Please refer to the **Standard** for the detailed semantics of these collective calls.

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


        