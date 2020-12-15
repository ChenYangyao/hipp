Remote Memory Access
=========================

.. include:: /global.rst

The following classes are all defined within namespace ``HIPP::MPI``.

.. namespace:: HIPP::MPI

Class Win: the RMA Window
-------------------------------

.. class:: Win

    .. enum:: @Enums: int 

        ..  enumerator:: \
            LOCK_SHARED = MPI_LOCK_SHARED
            LOCK_EXCLUSIVE = MPI_LOCK_EXCLUSIVE

            Types of lock operation.
        
        ..  enumerator:: \
            MODE_NOSTORE = MPI_MODE_NOSTORE
            MODE_NOPUT = MPI_MODE_NOPUT
            MODE_NOPRECEDE = MPI_MODE_NOPRECEDE
            MODE_NOSECCEED = MPI_MODE_NOSUCCEED
            MODE_NOCHECK = MPI_MODE_NOCHECK

            Synchronization modes.
        
        ..  enumerator:: \
            BASE = MPI_WIN_BASE
            SIZE = MPI_WIN_SIZE
            DISP_UNIT = MPI_WIN_DISP_UNIT
            CREATE_FLAVOR = MPI_WIN_CREATE_FLAVOR
            MODEL = MPI_WIN_MODEL

            Attributes of the window object.
        
        ..  enumerator:: \ 
            UNIFIED = MPI_WIN_UNIFIED
            SEPARATE = MPI_WIN_SEPARATE 

            Memory models.
        

        ..  enumerator:: \
            FLAVOR_CREATE = MPI_WIN_FLAVOR_CREATE 
            FLAVOR_ALLOCATE = MPI_WIN_FLAVOR_ALLOCATE
            FLAVOR_DYNAMIC = MPI_WIN_FLAVOR_DYNAMIC
            FLAVOR_SHARED = MPI_WIN_FLAVOR_SHARED

            Window creation flavors.

    **Memory management methods:**

    =================================================== ==================================================
    Method                                              Detail 
    =================================================== ==================================================
    default constructor                                 Not available.
    copy constructor |br| and ``operator=(&&)``         Defined; ``noexcept``.
    move constructor |br| and ``operator=(const &)``    Defined; ``noexcept``.
    =================================================== ==================================================

    .. function::   ostream &info(ostream &os = cout, int fmt_cntl = 1)const
                    friend ostream & operator<<(ostream &os, const Win &win)

        ``info()`` displays some basic information of the window instance to ``os``.

        :arg fmt_cntl:  Control the display format. 0 for inline information and 1 for a verbose, multiple-line information.
        :return: The argument ``os`` is returned.

        The overloaded ``<<`` operator is equivalent to ``info()`` with default 
        ``fmt_cntl``.

        The returned reference of ``os`` allows you to chain the outputs, such as 
        ``win.info(cout) << " continue printing " << endl``.

    
    
    .. function::   void free() noexcept

        Free the current window object, and set it to a null value as returned by
        :func:`nullval`.
        ``free()`` can be called at any time and even multiple times.

    .. function::   bool is_null() const
                    void * shared_query(int rank, aint_t &size, int &disp_unit) const
                    bool get_attr(int keyval, void * &attr_val) const
                    Group get_group() const
                    void * get_base() const
                    aint_t get_size() const
                    int get_disp_unit() const
                    int get_create_flavor() const
                    int get_model() const
                    void set_info(const Info &info)
                    Info get_info()
                    void attach(void *base, aint_t size)
                    void detach(const void *base)
                    static Win nullval() noexcept


        Inquery the information of the instance.
        
        ``is_null()`` tests whether this is a null window object (internally, 
        ``MPI_WIN_NULL``).

        ``shared_query()`` - for window created with shared memory, we can query
        the base pointer (return value), its size and displacement unit, given
        the rank of the remote window.

        ``get_attr()`` gets cache attribute, including the predefined values.
        The predefined attribute can also be access by ``get_base()``, ``get_size()``,
        get_disp_unit(), get_create_flavor(), and get_model().

        ``get_group()`` returns the processes group associated with this window.

        ``get_info()`` and ``set_info()`` - get used hints and set new hints.

        ``attach()`` and ``detach()`` - attach and detach dynamic memory to the local 
        window. Only valid if the window is created with dynamic flavor.

        ``nullval()`` - returun a nullval (internally ``MPI_WIN_NULL``).
    
    .. function::   void put(int target_rank, const Datapacket &origin_dpacket, const Datapacket &target_dpacket)
                    void put(int target_rank, const Datapacket &origin_dpacket, aint_t target_disp)
                    void get(int target_rank, const Datapacket &origin_dpacket, const Datapacket &target_dpacket)
                    void get(int target_rank, const Datapacket &origin_dpacket, aint_t target_disp)
                    void accumulate(int target_rank, const Oppacket &op, const Datapacket &origin_dpacket, const Datapacket &target_dpacket)
                    void accumulate(int target_rank, const Oppacket &op, const Datapacket &origin_dpacket, aint_t target_disp)
                    void get_accumulate(int target_rank, const Oppacket &op, \
                        const Datapacket &result_dpacket, \
                        const Datapacket &origin_dpacket, \
                        const Datapacket &target_dpacket)
                    void get_accumulate(int target_rank, const Oppacket &op, \
                        const Datapacket &result_dpacket, const void *origin_addr, \
                        aint_t target_disp)
                    void fetch_and_op(int target_rank, const Oppacket &op, \
                        const Datatype &dtype, \
                        void *result_addr, const void *origin_addr, aint_t target_disp)
                    template<typename T> \
                    void fetch_and_op(int target_rank, const Oppacket &op, \
                        T &result, const T &origin, aint_t target_disp)
                    void compare_and_swap(int target_rank, const Datatype &dtype, \
                        void *result_addr, const void *compare_addr, \
                        const void *origin_addr, aint_t target_disp)
                    template<typename T>\
                    void compare_and_swap(int target_rank, T &result, const T &compare, \
                        const T &origin, aint_t target_disp)
                    Requests rput(int target_rank, const Datapacket &origin_dpacket, \
                        const Datapacket &target_dpacket)
                    Requests rput(int target_rank, const Datapacket &origin_dpacket, \
                        aint_t target_disp)
                    Requests rget(int target_rank, const Datapacket &origin_dpacket, \
                        const Datapacket &target_dpacket)
                    Requests rget(int target_rank, const Datapacket &origin_dpacket, \
                        aint_t target_disp)
                    Requests raccumulate(int target_rank, const Oppacket &op, \
                        const Datapacket &origin_dpacket, const Datapacket &target_dpacket)
                    Requests raccumulate(int target_rank, const Oppacket &op, \
                        const Datapacket &origin_dpacket, aint_t target_disp)
                    Requests rget_accumulate(int target_rank, const Oppacket &op,\ 
                        const Datapacket &result_dpacket, \
                        const Datapacket &origin_dpacket, \
                        const Datapacket &target_dpacket)
                    Requests rget_accumulate(int target_rank, const Oppacket &op, \
                        const Datapacket &result_dpacket, const void *origin_addr, \
                        aint_t target_disp)

        RMA communication calls. 

        Here the datapacket of the origin buffer/result buffer should be 
        specified by either triplet, or ``std::vector``, or ``std::string`` (see class definition 
        of :class:`Datapacket`). 
        The triplet is in the form of ``((aint_t)displacement, (int)size, (Datatype)datatype)``.
        Since all RMA calls are not guaranteed blocking, make sure your buffer is 
        not released until the synchronization calls are made.
        
        ``put()`` puts data in origin buffer to target window. 
        
        ``get()`` gets the data in the target window into origin buffer.
        
        ``accumulate()`` accumulates the origin buffer, with operation 'op', to 
        the target window.
        
        ``get_accumulate()`` is similar to accumulate but also fetch the data before
        accumulating.
        The second version of each call accepts only a ``target_disp``, which means 
        the size and datatype are the same as the ``origin_dpacket``.
        
        ``fetch_and_op()`` is a simplified version of ``get_accumulate()`` which assume
        size = 1. The template version can be applied to :ref:`predefined numeric types <api-mpi-dpacket-predefined-type>`.
        
        ``compare_and_swap()`` is a important synchronization call, which compare
        the data in ``compare`` with the data in target window. If the same, write
        the data in ``origin_addr`` to the target window. The data in target window
        (before writing) is always returned in ``result_addr``.
        The template version accepts predefined datatypes.
        
        The non-blocking version (``rput()``, ``rget()``, ...) returns a request object which
        can be waited or tested later. Non-blocking version is only valid in 
        passive target access.
   
    .. function::   void fence(int assert=0)
                    void start(const Group &group, int assert=0)
                    void complete()
                    void post(const Group &group, int assert=0)
                    void wait()
                    bool test()
                    void lock(int lock_type, int rank, int assert=0)
                    void unlock(int rank)
                    void lock_all(int assert=0)
                    void unlock_all()
                    void flush(int rank)
                    void flush_all()
                    void flush_local(int rank)
                    void flush_local_all()
                    void sync()
                    sync_guard_t fence_g(int begin_assert=0, int end_assert=0)
                    sync_guard_t start_g(const Group &group, int assert=0)
                    sync_guard_t post_g(const Group &group, int assert=0)
                    sync_guard_t lock_g(int lock_type, int rank, int assert=0)
                    sync_guard_t lock_all_g(int assert=0)

        RMA synchronization calls.
        
        These calls either finish the RMA operations or synchoronize the public
        and private buffer (if in a separate memory model). Look at the MPI standard
        specifications before using them.
        
        The "guard" version (with a suffix "g") adopts RAII convention. When
        called, it returns a guard object. The corresponding epoch-end operation 
        (fence, complete, wait, ...) is automatically called at the destruction 
        of the guard object (user may end the epoch in advanced by calling 
        release() of the guard object).


Class Win::sync_guard_t
---------------------------------------------------------------

.. class:: SyncGuard 

    SyncGuard - the Win synchronization guard object. Typically usage is::

        auto win = comm.win_create(...);
        void *outbuf, *inbuf;
        { 
            /* Start a local block. Guard is destructed at the end of the block. */
            auto g = win.fence_g();
            win.put(outbuf);
            win.get(inbuf);   
        }
        /* Reuse the outbuf and inbuf now. */
    
    .. enum:: @enums : int 
    
        ..  enumerator::  \
            syncOVER = 0
            syncFENCE = 1 
            syncSTART = 2 
            syncPOST = 3
            syncLOCK = 4 
            syncLOCKALL = 5

    **Memory management methods:**
    
    =================================================== ==================================================
    Method                                              Detail 
    =================================================== ==================================================
    default constructor                                 Not available.
    copy constructor |br| and ``operator=(&&)``         Not available.
    move constructor |br| and ``operator=(const &)``    Defined.
    =================================================== ==================================================

    .. function::   SyncGuard(Win &win, int sync_type, int rank, int assert)

        Construct a guard on the window, with synchoronization type ``sync_type`` and 
        releasing assertion ``assert``.            

    .. function::     void release()

        Release the lock/fence in the current instance. 
        
        Calling the method on 
        a instance without lock/fence hold results an exception 
        :class:`ErrLogic` with error code :enumerator:`ErrLogic::eDOMAIN`.