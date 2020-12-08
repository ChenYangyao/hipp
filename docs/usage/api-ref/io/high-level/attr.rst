Attribute and Property List Manipulation
================================================

The following classes are all defined within namespace ``HIPP::IO``.

.. namespace:: HIPP::IO 

Class H5Attr 
-------------------------

.. class:: H5Attr

    HDF5 objects (e.g., file, group and dataset) use attributes to store small piece 
    of informations (often called meta-data). ``H5Attr`` manipulates these attributes.

    ``H5Attr`` instance can be copy-constructed, copy-assigned, move-constructed and move-assigned.
    The copy, move operations and destructor are all noexcept. The copy operations are shallow-copy, i.e., 
    instances of source and target refer to the same underlying attribute object (like a ``shared_ptr``).

    .. function::   H5Dataspace dataspace()
                    const H5Dataspace dataspace() const
                    H5Datatype datatype()
                    const H5Datatype datatype() const

        Retrive the information (i.e., dataspace and datatype) of the attribute instance. 

    .. function::   template<typename T, typename A>\
                    void write( const vector<T, A> &buff )
                    template<typename T>\
                    void write( const T*buff )
                    void write( const string &buff )
                    template<typename T, typename A>\
                    void write( const vector<T, A> &buff, const H5Datatype &memtype )
                    template<typename T>\
                    void write( const T *buff, const H5Datatype &memtype )
        
        Write data in ``buff`` into the attribute.

        The template parameters and argument list are the same with :func:`H5Dataset::write`
        and its variants. The difference is that the dataspace in memory and file and property list 
        for transfering cannot be adjusted.

    .. function::   template<typename T, typename A>\
                    void read( vector<T, A> &buff ) const
                    template<typename T>\
                    void read( T*buff ) const
                    void read( string &buff ) const
                    template<typename T, typename A>\
                    void read( vector<T, A> &buff, const H5Datatype &memtype ) const
                    template<typename T>\
                    void read( T *buff, const H5Datatype &memtype ) const

        Read data in the attribute into ``buff``. 

        The template parameters and argument list are the same with :func:`H5Dataset::read`
        and its variants. The difference is that the dataspace in memory and file and property list 
        for transfering cannot be adjusted.


Class H5Proplist
----------------------------

.. class::  H5Proplist

    .. var::    static const H5Proplist defaultval

    .. function::   H5Proplist & set_layout( const string &layout )
                    string layout() const
        
        Set and retrive the layout of dataset on the dataset creation property list.
        ``layout`` can be either "contiguous", "chunked", "compact" or "virtual".


    .. function::   H5Proplist & set_chunk( vector<hsize_t> dims )
                    int chunk_ndims() const
                    vector<hsize_t> chunk() const

        Set and retrive the chunk size for a chunk-layout dataset. 

        ``dims`` must have the same rank (i.e., number of dimensions) as the dataset.
        ``dims`` cannot be larger than the dims of a fixed-size dataset, and cannot be larger 
        than "2^32-1". The size of each chunk cannot be larger than 4GB.

        If the layout of the dataset is not ``"chucnk"``, ``set_chunk()`` change the 
        layout to ``"chunk"``. 

    .. function::   H5Proplist & set_chunk_cache( \
                        size_t nslot, size_t nbytes, double w0 )
                    void chunk_cache( size_t &nslot, \
                        size_t &nbytes, double &w0 ) const

        Set and retrive the cache method of the dataset on the dataset access property list.
        If set by user, override the cache method of the file.
    
    .. function::   H5Proplist & set_cache( size_t nslot, \
                        size_t nbytes, double w0 )
                    void cache( size_t &nslot, size_t &nbytes, double &w0 ) const

        Set and retrive the cache method of all datasets of a file on the dataset 
        access property list.

    
