Synchronizations
====================================================

The following classes are all defined within namespace ``HIPP::MPI``.

.. namespace:: HIPP::MPI

SpinLock and Mutex
--------------------------------

.. class:: SpinLock 

    A spinlock or a set of spinlocks for mutual exclusion.

    The ``SpinLock`` object cannot be **copy-constructed** and **copy-assigned**.
    But it can be **move-constructed** or **move-assigned**. 
    The move operations and the destructor are ``noexcept``. 
    After moving, the source spinlock instance 
    can be correctly destructed, but cannot be used in any futher synchronization.
    It is forbidden to move from or move to a spinlock that is already accquired 
    (through the lock operation or the guarded version).

    .. function::   SpinLock(const Comm &comm, int n_locks=1)

        Initialize a set of ``n_locks`` spinlocks within the communicator ``comm``.
        Processes in the communicator can use the locks to construct critical regions 
        for mutually exclusive execution.

        The initializer is a collective operation which must be called in all 
        of the processes within the communicator, with the same ``n_locks`` arguments. 

    .. function::   void lock(int lock_id=0)
                    bool try_lock(int lock_id=0)
                    void unlock(int lock_id=0)

        A process accquires the i-th spinlock within the set of locks by calling ``lock(i)``,
        and releases it by calling ``unlock(i)``.

        ``lock()`` is a blocking operation. The variant ``try_lock()`` is non-blocking
        and returns true if the lock is successfully accquired. 

    .. function::   guard_t lock_g(int lock_id=0)
                    guard_t try_lock_g(int lock_id=0)

        The guarded version of ``lock()`` and ``try_lock()``. 
        
        ``lock_g()`` is the same with ``lock()`` except that it returns a guard object.
        THE lock is automatically released on the destruction of the guard object.

        ``try_lock_g()`` is the same with ``try_lock()`` except that is returns a guard
        object. If the lock is successfully accquired, the lock will be automatically
        released on the destruction of the guard object. Otherwise the guard object
        is an "empty" object which does nothing on destruction.

    **Examples:**

    **Using spinlock:** the following example shows how to make mutually exclusion 
    execution among a group of processes.

    The rank-0 process creates a new file named "of.txt" and other processes open it.
    Then a spinlock is declared. Each process prints some text into the file in 
    the critical region protected by the spinlock. Because of the protection,
    the output is not entangled in the file.

    .. code-block::

        #include <fstream>              // ofstream
        #include <unistd.h>             // sleep (Unix-specific)

        ofstream of;                    // open a common file
        if( rank == 0 ) 
            of.open("of.txt", of.trunc);
        comm.barrier();
        if( rank != 0 )
            of.open("of.txt", of.app);
        
        HIPP::MPI::SpinLock lk(comm);   // initialize a spin lock
        for(int i=0; i<3; ++i){
            lk.lock();                  // enter the critical region
            of.seekp(0, of.end);
            of << "Process " << rank;
            sleep(1);
            of << " has a long task ";
            sleep(1);
            of << "to finish" << endl;
            lk.unlock();                // exit the critical region
        }

    The output to the file "of.txt" is (run with 3 processes; output order may change in each run)

    .. code-block:: text

        Process 0 has a long task to finish
        Process 1 has a long task to finish
        Process 0 has a long task to finish
        ... (the remainings are ignored)

    **Using the guard:** In the above example, the lock/unlock operations are necessarily paired. 
    If user forgets to release the lock, or an exception is thrown before 
    the unlock operation has chance to be executed, the behavior of the 
    whole application is undefined (possibly deadlocked). To avoid this, 
    the guarded versions are defined.

    The guarded version :func:`SpinLock::lock_g` is the same with :func:`SpinLock::lock`,
    except that it returns a guard object. The guard takes over the responsibility
    to release the lock, which means the lock will be released on the destruction of 
    the guard, no matter what is the reason of such destruction. Even if 
    an exception is thrown, the lock will be released in the stack unwinding.
    By using the guarded version, the above example can written as

    .. code-block:: 
        :emphasize-lines: 5

        HIPP::MPI::SpinLock lk(comm);
        for(int i=0; i<3; ++i){
            auto guard = lk.lock_g();   // a guard is returned
            // output to the ofstream 
        }                               // no need to call 'unlock'

    **Working with multiple spinlocks:** the :class:`SpinLock` class allows initializing 
    multiple locks (in the same communicator) by defining one class instance. 
    User simply passes the argument ``n_locks`` into the constructor to initialize a 
    set of ``n_locks`` spinlocks， and uses ``lock(i)`` and ``unlock(i)`` to accquire 
    and release the i-th lock.

    For example, there are 5 trunks of data (e.g., a shared array), and therefore 5 locks are 
    needed to protect them separately. 
    User may want to perform an atomic operation on a subset, say, trunk 1 and trunk 2. 
    In this case, multiple locks should be accquired::
    
        int n_locks = 5;
        HIPP::MPI::SpinLock lks(comm, n_locks);
        lks.lock(1); lks.lock(2);
        // operate on data trunk 1 and 2 atomically
        lks.unlock(2); lks.unlock(1);

    .. note:: 

        The content output to the file "of.txt" are serialized in the execution
        environment on the writter's single-node computer.

        However, on a computer cluster that has shared file system linked by the 
        network system or other hardwares, the content may entangle because 
        the lock in the example code only ensures that the time when the text is output to the kernel
        buffer cache of the file is ordered. The network system may still need 
        asynchronous steps to transfer the content from the buffer to the real file.

        Even in a single-node computer with lock protection, if the write operation is called on the 
        standard output/error stream ``cout``/``cerr`` and the messages are printed
        on the screen, it is still possible that they are entangled. This is because 
        the data transfer from the kernel buffer cache to the screen may be asynchronous.

        The detail behavir of the standard C/C++ IO library depends on the 
        platform. User may use the MPI parallel IO library with shared file pointer (class :class:`File`) 
        to ensure the IO is serialized or even ordered.


.. class:: Mutex

    A mutex or a set of mutexes. The mutexes are similar to the spinlocks (class :class:`SpinLock`),
    except that operations on mutexes are slower but more scalable.

    The ``Mutex`` object cannot be **copy-constructed** and **copy-assigned**.
    But it can be **move-constructed** or **move-assigned**. 
    The move operations and the destructor are ``noexcept``. 
    After moving, the source mutex instance 
    can be correctly destructed, but cannot be used in any futher synchronization.
    It is forbidden to move from or move to a mutex that is already accquired 
    (through the lock operation or the guarded version).

    .. function::   Mutex(const Comm &comm, int n_locks=1)
                    void lock(int lock_id=0)
                    bool try_lock(int lock_id=0)
                    void unlock(int lock_id=0)
                    guard_t lock_g(int lock_id=0)
                    guard_t try_lock_g(int lock_id=0)

        All the operations have the same semantics as class :class:`SpinLock`.
        See the API reference and examples of spinlocks for the detail.


Lock Guards
----------------

.. class:: SpinLock::guard_t
    
    .. type:: SpinLock lock_t

    .. function::  SpinLockGuard(lock_t &lock, int lock_id)

        The guard object should never be constructed explicitly. It should be 
        returned by lock operations.

        The guard instance cannot be copied or copy constructed, but it can be 
        moved or move constructed, when, and only when the lock guarded has been 
        released.

        The move operations are ``noexcept``.

    .. function::    void unlock()

        Release the lock. Typically it should be done on the destruction of 
        the guard. But user is allowed to release the lock in advance.

        After release, the guard object is not no longer responsible for the 
        lifetime of the lock.
    
    .. function::   explicit operator bool() const noexcept

        Return ``true`` if the lock is accquired. Return ``false`` if the lock 
        has been released (by :func:`unlock()` method of the guard).

.. class:: Mutex::guard_t

    .. type:: Mutex lock_t


    .. function::   MutexGuard(lock_t &lock, int lock_id)
                    void unlock()
                    explicit operator bool() const noexcept

        Similar to the guard type of the spinlock (class :class:`SpinLock::guard_t`) 
        except that it is returned by the lock operations of the :class:`Mutex`.


Sequential Block
---------------------

.. class:: SeqBlock

    Create a critical region that is sequentially visited by the processes within
    a given communicator.

    The ``SeqBlock`` instance cannot be **copied** or **copy-constructed**, but it can 
    be **moved** or **move-constructed**. The move operations and destructor are 
    ``noexcept``.

    .. function:: SeqBlock( const Comm &comm, int start = 1 )

        Initialize the instance and mark the beginning of the critical region.
        Execution in the critical region is ordered by the rank in the communicator
        ``comm``.
        
        If ``start`` is non-zero, the critical region is started. Otherwise 
        the critical region is not started and user may start it by the method 
        :func:`begin`.

    .. function::   void begin()
                    void end()
        
        ``begin()`` starts a critical region. ``end()`` ends it.

        The begin and end operations must be called in pair (except that the 
        critical region is started on the construction of the instance).

        When the instance is destructed, the critical region is automatically 
        ends.

    .. function:: static void free_cache( const Comm &comm )

        The first time when a :class:`SeqBlock` instance is built on a communicator,
        information is cached in this communicator to reduce overhead in the construction
        of next :class:`SeqBlock` instances on the same communicator. 

        Sometimes user may want to free such cache (although not necessary in 
        most cases), so ``free_cache()`` is provided.

        It is erroneous to free the cache when the critical region is not ended 
        (i.e., before the call of :func:`end`).

    **Examples:** we start a critical region in each process in the communicator ``comm``. 
    Each process prints the rank of self to the screen. The output must be in the order 
    of the rank in the ``comm``::

        #include <unistd.h>                         // sleep (Unix-specific)
        
        {
            HIPP::MPI::SeqBlock sb(comm);           // start the critical region
            cout << "Process " << comm.rank() << " enter ";
            sleep(1);
            cout << "and exit the critical region" << endl;
        }                                           // critical region ends

    Equivalently, we can constrct the :class:`SeqBlock` instance but donot 
    start the critical region. Then we call :func:`SeqBlock::begin` to start
    and call :func:`SeqBlock::end` to end the critical region:

    .. code-block::
        :emphasize-lines: 2,4

        HIPP::MPI::SeqBlock sb(comm, 0);
        sb.begin();
        // print to cout
        sb.end();

    The output is (run with 3 processes):

    .. code-block:: text 

        Process 0 enter and exit the critical region
        Process 1 enter and exit the critical region
        Process 2 enter and exit the critical region