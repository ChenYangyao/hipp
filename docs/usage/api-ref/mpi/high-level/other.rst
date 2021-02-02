Other Standard Facilities
===============================

.. include:: /global.rst

The following classes are all defined within namespace ``HIPP::MPI``.

.. namespace:: HIPP::MPI

Class Message: Probing and Matching
----------------------------------------------

.. class::  Message 

    **Memory management methods:**
    
    =================================================== ====================================================
    Method                                              Detail 
    =================================================== ====================================================
    default constructor                                 Available; ``noexcept``; constructs a null value 
                                                        |br| as returned by :func:`nullval`.
    copy constructor |br| and ``operator=(&&)``         Not Available.
    move constructor |br| and ``operator=(const &)``    Available; ``noexcept``.
    =================================================== ====================================================

    .. type::   MPI_Message mpi_t;

    .. function:: \
        explicit Message(mpi_t message) noexcept
    
        Explicit construct a message object from MPI raw message object.

    .. function:: \
        static Message nullval() noexcept
        static Message no_proc_val() noexcept
        bool is_null() const noexcept
        bool is_no_proc() const noexcept

        ``nullval()`` returns a null value (internally ``MPI_MESSAGE_NULL``).
        
        ``no_proc_val()`` returns a value of no-process (internally ``MPI_MESSAGE_NO_PROC``).

        ``is_null()`` tests whether this instance is a null value. 

        ``is_no_proc()`` tests whether this instance is a no-process value.

    .. function:: \
        template<typename ...Args> \
            Status mrecv(Args && ...args)
        template<typename ...Args> \
            Requests imrecv(Args && ...args)

        Matched receiving calls.

        Receive the message pointed by the message object into the buffer ``args``. ``args`` may be 
        any correct arguments that can be used to construct a `datapacket <api-mpi-dpacket-constructor>`_ object.


Class Info: the Hints to MPI Implementation
---------------------------------------------

.. class:: Info 

    
    .. type:: std::pair<string, string> item_t

        The type of info item, i.e., a key-value string pair.

    =================================================== ====================================================
    Method                                              Detail 
    =================================================== ====================================================
    default constructor                                 Available; constructs an empty info object
    copy constructor |br| and ``operator=(&&)``         Available; ``noexcept``; shallow copy.
    move constructor |br| and ``operator=(const &)``    Available; ``noexcept``.
    =================================================== ====================================================

    
    
    .. function:: \
        ostream & info( ostream &os = cout, int fmt_cntl = 1 ) const
        friend ostream & operator<<( ostream &os, const Info &info )

        Display some basic information of the info instance to ``os``.
        
        :arg fmt_cntl:  Control the display format. 0 for inline information and 1
                        for a verbose, multiple-line information including all 
                        key-value pairs.
        :return: The ``os`` object is returned.
        
        The overloaded ``<<`` operator is equivalent to ``info()`` with the default 
        ``fmt_cntl``.

    
    .. function::   void free() noexcept

        Free the current info object and set it to null value as returned by
        ``nullval()``.
        ``free()`` can be called at any time, even for a pre-defined info object.
        
    .. function::   bool is_null() const

        Inquiry whether the info object is a null value.

    .. function::   Info & set( const string &key, const string &val )

        Add (or reset) a key-value pair.

    .. function::   Info & del( const string &key )

        Aelete a key-value pair.


    .. function:: \
        bool get( const string &key, string &val ) const
        string get( const string &key ) const
        item_t get( int n ) const 
        vector<item_t> get() const
        string operator[]( const string &key ) const
        item_t operator[]( int n ) const
        bool get_valuelen( const string &key, int &len ) const
        int get_nkeys() const
        string get_nthkey( int n ) const


        Inquiry key-value pair, or pairs, or information of the pair.
    
        ``get(key, val)`` returns the value into ``val`` corresponding to `key`. If
        the key is not set, return ``false``, and ``val`` is not modified. 
        Otherwise return ``true``.
        
        ``get(key)`` returns the value corresponding to ``key``. If not set, throw an
        :class:`ErrLogic`.
        
        ``get(n)`` returns the n-th key-value pair. ``item_t`` is a ``std::pair<>`` 
        instance with the ``first`` and ``second`` member corresponding to
        key and value, respectively.
        
        ``get()`` returns a vector containing all key-value pairs.
        
        ``operator[](key)`` and ``oprator[](n)`` are aliased to ``get(key)`` and ``get(n)``.
        
        ``get_valuelen()`` returns the lengths of the value into ``len`` (not 
        including the space of null-terminator) and return ``true`` if the key 
        is set. Otherwise return ``false`` and ``len`` is not modified.
        
        ``get_nkeys()`` gets number of keys.
        
        ``get_nthkey()`` returns the n-th key.
        
    
    .. function:: \
        Info dup()
        static Info create()
        static Info nullval() noexcept

        Duplicate the info object, create a new empty info object, or get a 
        null value object.


Memory Management
----------------------

.. function::   void * alloc_mem(aint_t size, const Info &info = Info::nullval())
                void free_mem(void *base)

                ``alloc_mem(size, info)`` allocates ``size`` bytes contiguous memory with hints ``info``.
                The pointer to its starting address is returned.

                The allocated memory must be free by ``free_mem()`` after using, otherwise causing memory leak.

                **Exception:** both of the two functions throw an :class:`ErrMPI` instance on failure. The 
                error code is determined by MPI implementation.


.. function::   void buffer_attach(void *buffer, int size)
                std::pair<void *, int> buffer_detach()

                Buffer management for buffered send operation.

                ``buffer_attach(buffer, size)`` attaches a ``buffer`` with ``size`` bytes to 
                be used for buffered mode send. User may use :var:`BSEND_OVERHEAD`
                to properly determine the overhead associated with each message.

                ``buffer_detach()`` detaches the buffer and returns its base address and size in bytes. 
                This call will blocked until all messages currently in the buffer have been transmitted.

Timer 
-----------------------------

.. function:: \
    double wtime()
    double wtick()

    ``wtime()`` returns the wall-time in seconds. The zero-point is not changed in a process, 
    but different nodes may have different zero-point.

    ``wtick()`` returns the resolution of ``wtime()`` in seconds, i.e., the time between successive 
    clock ticks.