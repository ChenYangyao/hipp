Parallel I/O
====================

.. include:: /global.rst

The following classes are all defined within namespace ``HIPP::MPI``.

.. namespace:: HIPP::MPI

Class File: the Parallel I/O Handler
----------------------------------------

.. class::  File

    .. enum:: @enums: int 

        .. enumerator:: \
            sSET=MPI_SEEK_SET
            sCUR=MPI_SEEK_CUR
            sEND=MPI_SEEK_END

            The seek specifications.

    **Memory management methods:**
    
    =================================================== ====================================================
    Method                                              Detail 
    =================================================== ====================================================
    default constructor                                 Not available.
    copy constructor |br| and ``operator=(&&)``         Available; ``noexcept``; shallow copy.
    move constructor |br| and ``operator=(const &)``    Available; ``noexcept``.
    =================================================== ====================================================
    

    .. function:: File( const Comm &comm, const string &name, const string &amode, const Info &info = Info::nullval() )

        Create a new file or open an existing file. 

        :arg comm:  The communicator, consisting of the processes involved in I/O.
        :arg name:  File name. 
        :arg amode: Visiting mode. Could be ``"r"``, ``"w"``, ``"wr"``, ``"c"`` (``"create"``), ``"excl"``, 
                    ``"delete_on_close"``, ``"unique_open"``, ``"sequential"``, ``"a"`` (``"append"``), or 
                    their comma-separated combination, e.g., ``"wr,c"``.
        :arg info:  Hints to the implementation.

    .. function::   ostream &info( ostream &os = cout, int fmt_cntl = 1 ) const
                    friend ostream & operator<<( ostream &os, const File &file )
        
        ``info()`` displays some basic information of the file instance to ``os``.

        :arg fmt_cntl:  Control the display format. 0 for inline information and 1 for a verbose, multiple-line information.
        :return: The argument ``os`` is returned.

        The overloaded ``<<`` operator is equivalent to ``info()`` with default 
        ``fmt_cntl``.

        The returned reference of ``os`` allows you to chain the outputs, such as 
        ``file.info(cout) << " continue printing " << endl``.

    .. function::   void close() noexcept

        Close the file. This can be called on any object (even a null value) or even multiple times.
                    
    .. function::   static void del(const string &name, const Info &info = Info::nullval())

        Delete a file named ``name``.
                        
    .. function:: \
        bool is_null() const
        static File nullval() noexcept

        ``is_null()`` tests whether the instance is a null value.
        
        ``nullval()`` returns a null value (internally ``MPI_FILE_NULL``).

    .. function::\   
        File & set_size( offset_t size )
        File & preallocate( offset_t size )
        offset_t get_size()const

        File size management methods.

        ``set_size(size)`` resizes the file to ``size`` bytes (measured from the beginning of the file). The call 
        is collective; all processes in the group must pass identical values for ``size``. If ``size`` is smaller
        than the current file size, the file is truncated at the position defined by ``size``. If ``size`` is 
        larger than the current size, the values in new regions are undefined.

        ``preallocate(size)`` ensures that storage space is allocated for the first ``size`` bytes. The call is 
        also collective. Regions that have previously been written are unaffacted. Newly allocated region is written 
        with undefined data. If ``size`` is larger than the current file size, file size is increased to ``size``.
        Otherwise the file size is unchanged.

        ``get_size()`` retrives the file size in bytes.

    .. function::   Group get_group()const
                    string get_amode()const

        Querying file parameters.

        ``get_group()`` returns a duplication of the group of the communicator used to open the file. 

        ``get_amode()`` returns the access mode associated with the file.

    .. function::   File & set_info( const Info &info )
                    Info get_info()const
        
        File info management methods.

        ``set_info()`` sets new values for the hints. It is a collective call. The info object 
        may be different on each process, except if the implementation requires. Implementation
        may ignore the hints in this call because some info items cannot be changed after 
        the creation or opening of the file.

        ``get_info()`` returns the info containing all hints that is actually used by the 
        system related to this file.

    .. function::   File & set_view( offset_t disp, const Datatype & etype,  \
                        const Datatype &filetype, const string &datarep="native",  \
                        const Info &info = Info::nullval())
                    void get_view( offset_t &disp, Datatype &etype, Datatype &filetype,  \
                        string &datarep )const

        File views management methods. 

        ``set_view()`` changes that process's view of the data in the file. The start of 
        the view is ``disp`` (in bytes), the data type is ``etype`` and the distribution of 
        data is sete by ``filetype``, and the representation is set to ``datarep``.
        ``set_view()`` also set the indivisual and shared file pointers to zero. 
        This call is collective; ``datarep`` and the extents of ``etype`` must be identical
        on all processes in the group; ``disp``, ``filetype``, ``info`` may vary. The ``etype``
        and ``filetype`` must be committed. User should ensure that all nonblocking calls 
        and split collective calls have been complete - otherwise the call is erroneous.

        ``get_view`` returns the process's view of the data in the file. 

    .. function::\
        aint_t get_type_extent(const Datatype &dtype)const
        File & set_atomicity( int flag )
        int get_atomicity()const
        void sync()

        Methods for file interoperability and consistency semantics.

        ``get_type_extent()`` retunrs the extent of ``dtype`` in the file. The extent will be 
        the same for all processes accessing the file.

        ``set_atomicity()`` sets/unsets the atomic access mode. It is a collective call. 

        ``get_atomicity()`` checks whether the atomic mode is anabled.

        ``sync()`` causes all previous writes to the file by the calling process to be 
        transferred to the storage device.

    .. function:: \
        template<typename ...Args> \
        Status read_at( offset_t offset, Args && ...args )
        template<typename ...Args> \
        Status read_at_all( offset_t offset, Args && ...args )
        template<typename ...Args> \
        Status write_at( offset_t offset, Args && ...args )
        template<typename ...Args> \
        Status write_at_all( offset_t offset, Args && ...args )
        template<typename ...Args> \
        Requests iread_at( offset_t offset, Args && ...args )
        template<typename ...Args> \
        Requests iwrite_at( offset_t offset, Args && ...args )
        template<typename ...Args> \
        void read_at_all_begin( offset_t offset, Args && ...args )
        Status read_at_all_end( void *buf )
        template<typename ...Args> \
        void write_at_all_begin( offset_t offset, Args && ...args )
        Status write_at_all_end( void *buf )
        void seek(offset_t offset, int whence)
        void seek(offset_t offset, const string &whence)
        offset_t get_position()const
        offset_t get_byte_offset( offset_t offset )const
        template<typename ...Args> \
        Status read( Args && ...args )
        template<typename ...Args> \
        Status read_all( Args && ...args )
        template<typename ...Args> \
        Status write( Args && ...args )
        template<typename ...Args> \
        Status write_all( Args && ...args )
        template<typename ...Args> \
        Requests iread( Args && ...args )
        template<typename ...Args> \
        Requests iwrite( Args && ...args )
        template<typename ...Args> \
        void read_all_begin( Args && ...args )
        Status read_all_end( void *buf )
        template<typename ...Args> \
        void write_all_begin( Args && ...args )
        Status write_all_end( void *buf )
        void seek_shared(offset_t offset, int whence)
        void seek_shared(offset_t offset, const string &whence)
        offset_t get_position_shared()const
        template<typename ...Args> \
        Status read_shared( Args && ...args )
        template<typename ...Args> \
        Status read_ordered( Args && ...args )
        template<typename ...Args> \
        Status write_shared( Args && ...args )
        template<typename ...Args> \
        Status write_ordered( Args && ...args )
        template<typename ...Args> \
        Requests iread_shared( Args && ...args )
        template<typename ...Args> \
        Requests iwrite_shared( Args && ...args )
        template<typename ...Args> \
        void read_ordered_begin( Args && ...args )
        Status read_ordered_end( void *buf )
        template<typename ...Args> \
        void write_ordered_begin( Args && ...args )
        Status write_ordered_end( void *buf )

    Data access calls. Please refer to MPI **Standard** for their usage.