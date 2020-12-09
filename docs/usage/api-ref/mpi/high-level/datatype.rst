Datatype and Operation
=================================

.. include:: /global.rst

The following classes are all defined within namespace ``HIPP::MPI``.

.. namespace:: HIPP::MPI

Class Datatype: the Type of Data Element
------------------------------------------

.. class:: Datatype 

    ``Datatype`` describes the type of a data-elements (i.e., its byte-level interpretation and 
    allowed operations on it). ``Datatype`` is used in the communication as a part of the 
    data buffer descriptor.
    
    **Memory management methods:**
    
    =================================================== ====================================================
    Method                                              Detail 
    =================================================== ====================================================
    default constructor                                 Available; ``noexcept``; constructs a null value 
                                                        |br| as returned by :func:`nullval`.
    copy constructor |br| and ``operator=(&&)``         Available; ``noexcept``; shallow copy.
    move constructor |br| and ``operator=(const &)``    Available; ``noexcept``.
    =================================================== ====================================================


    .. function::   ostream &info( ostream &os = cout, int fmt_cntl = 1 ) const
                    friend ostream & operator<<( ostream &os, const Datatype &dtype )

        ``info()`` prints the information of the current instance to the stream 
        ``os``.
    
        :arg fmt_cntl:   control the amount of information to be printed, 0 for a 
                 short and inline priting, 1 for a verbose, multi-line version.
        :return: The argument ``os`` is returned.
    
        The overloaded ``<<`` operator is equivalent to ``info()`` with 
        default ``fmt_cntl``.
    
    .. function:: void free() noexcept

        Free the object and set it to null value as returned by :func:`nullval`.
        It is safe to call this function at any time, or even multiple times, 
        except for exactly the predifined objects (they are `const`, but a 
        non-const copy can be freed). 


    .. function::   bool is_null() const
                    void extent( aint_t &lb, aint_t &ext ) const
                    void true_extent( aint_t &lb, aint_t &ext ) const
                    int size() const

        Inquery the information about this datatype instance.

        ``is_null()`` tests whether this is a null value (an invalid instance 
        for communication).

        ``extent()`` gets the lower bound and extent of the datatype.
        ``true_extent()`` gets the "true" lower bound and extent (ignore the 
        changes made by, e.g., :func:`resized`).

        ``size()`` get the size (i.e., number of bytes required to store 
        the real data).


    .. function::   Datatype dup() const
                    static Datatype nullval() noexcept
                    Datatype resized( aint_t lb, aint_t ext ) const

        ``dup()`` creates a duplication of the current datatype.
        
        ``nullval()`` returns a null value, corresponding to 
        ``MPI_DATATYPE_NULL`` internally.
        
        ``resized()`` creates a resized datatype with new lower bound ``lb`` and extent ``ext``. 


    .. function::   Datatype contiguous( int count ) const
                    Datatype vector( int count, int blklen, int stride ) const
                    Datatype hvector( int count, int blklen, aint_t stride ) const
                    Datatype indexed_block( int blklen, const std::vector<int> &displs ) const
                    Datatype hindexed_block( int blklen, \
                        const std::vector<aint_t> &displs ) const
                    Datatype indexed( const std::vector<int> &blklens, \
                        const std::vector<int> &displs ) const
                    Datatype hindexed( const std::vector<int> &blklens, \
                        const std::vector<aint_t> &displs ) const
                    static Datatype struct_( const std::vector<int> &blklens, \
                        const std::vector<aint_t> &displs, const std::vector<Datatype> &dtypes)

        New datatype creation methods;

        ``contiguous()`` creates a datatype with contiguous ``count`` elements of the current
        datatype. 

        ``vector()`` creates a vector datatype of the current datatype, 
        consisting of ``count`` blocks, each with
        ``blklen`` contiguous elements, separated with stride ``stride`` (in the 
        unit of the current datatype). ``hvector()`` is similar, but the ``stride`` is in bytes. 

        ``indexed_block()`` further allows non-constant stride. The displacements
        of blocks are specified by ``displs`` (in the unit of the current datatype ).
        ``hindexed_block()`` specifies the displacements in bytes. 

        ``indexed()`` allows further flexibility - the blocks can have different lengths specified 
        by ``blklens``.
        ``hindexed()`` uses displacements in bytes;

        ``struct_`` is the most general one - the datatypes of blocks can be different 
        and specified by ``dtypes``.

    .. function::   Datatype darray( int size, int rank, const std::vector<int> &gsizes, \
                        const std::vector<int> &distribs, \
                        const std::vector<int> &dargs,\
                        const std::vector<int> &psizes, int order = ORDER_C )const
                    Datatype subarray( const std::vector<int> &sizes, \
                        const std::vector<int> &subsizes, \
                        const std::vector<int> &starts, int order = ORDER_C )const

        New datatype creation methods for distributed data.

        ``darray()`` creates a datatype which describes a part of a large distributed array.
        ``size`` and ``rank`` are the number of parts and the index of the target part. 
        The array may have any dimension, with ``psizes`` specifying the number of process at each direction, 
        ``gsizes`` describing the size
        of the global array at each direction, ``distribs`` and ``dargs`` describing the 
        distribution method (``distribs`` can be :var:`DISTRIBUTE_BLOCK` or 
        :var:`DISTRIBUTE_CYCLIC` or :var:`DISTRIBUTE_NONE` for each dimension; 
        ``dargs`` is block size, can be :var:`DISTRIBUTE_DFLT_DARG` for nearly uniform distribution or 
        can be a interger).
        ``order`` can be :var:`ORDER_C` or :var:`ORDER_FORTRAN`.

.. _api-mpi-predefined-dtype:

Predefined Datatypes 
-----------------------------


Class Datapacket: the Data Buffer Descriptor
-----------------------------------------------

.. class::  Datapacket 

    A ``Datapacket`` instance describes a data/message buffer in the communication (i.e., data packet).
    As in the **Standard** MPI interface, a data buffer is described by a 
    triplet ``(addr, size, datatype)``.  ``Datapacket`` encapsulates these three 
    into a single object to make the communication calls more elegant.

    **Memory management methods:**
    
    =================================================== ==================================================
    Method                                              Detail 
    =================================================== ==================================================
    default constructor                                 Not available.
    copy constructor |br| and ``operator=(&&)``         Defined; ``noexcept``; deep copy.
    move constructor |br| and ``operator=(const &)``    Defined; ``noexcept``.
    =================================================== ==================================================


    .. function::   Datapacket(const void *buff, int size, Datatype dtype) noexcept
                    Datapacket(const void *buff, int size, const string &dtype)
                    Datapacket( const string &buff ) noexcept
                    template<typename T, typename A>\
                    Datapacket( const vector<T,A> &buff ) noexcept

        Data packet constructors.
        
        (1)  formally specify the buffer as a triplet, i.e., starting address, number of elements and datatype.
        
        (2)  same with (1), but use a string to describe the datatype. It can be 
             
             - ``"byte"``, ``"char"`` or ``"bool"``;
             - ``"X"``, ``"signed X"`` or ``"unsigned X"``, where ``X`` can be ``char``, ``short``, ``int``, ``long`` or ``long long``;
             - ``"intX_t"`` or ``"uintX_t"``, where ``X`` can be ``8``, ``16``, ``32`` or ``64``;
             - ``"float"``, ``"double"``, ``"long double"``.
        
        (3)  use a string ``buff`` -  equivalent to specify the triplet as ``(void *)buff.data(), buff.size(), "char"``.
        
        (4)  use a vector ``buff`` of type ``T`` - equivalent to specify the triplet as ``(void *)buff.data(), buff.size(), datadype_for_T`` Note that ``T`` can be 

            - ``bool``, ``char``, ``signed char`` or ``unsigned char``;
            - ``X`` or  ``unsigned X``, where ``X`` can be ``short``, ``int``, ``long`` or ``long long``;
            - ``float``, ``double`` or ``long double``;
            - ``std::complex<float>``, ``std::complex<double>`` or ``std::complex<long double>``.

    
    
    .. function::   Datapacket(aint_t disp, int size, Datatype dtype) noexcept
                    Datapacket(aint_t disp, int size, const string &dtype)

        Sometimes, triplet is used to specify a memory segement relative to 
        a base address (e.g., in RMA operations). In such cases, the first 
        part of the triplet is a displacement relative to the base address. 
        
        The ``Datapacket`` type can represent such triplets, too. Internally, the
        displacement is stored by casting into a ``void *``.


Class Op: the Operation
---------------------------



Class Oppacket: the Operation Descriptor
--------------------------------------------


