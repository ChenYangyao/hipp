Stream Manipulation
==========================

The following variables, functions and classes are all defined within the namespace ``HIPP``.

.. namespace:: HIPP 

Predefined Variables
------------------------

.. var::    extern PStream pout
            extern PStream perr

    Pretty output streams for standard output and standard error. See below :class:`PStream` for the detail.


Classes for formated IO
--------------------------

.. class:: PStream

    ``PStream`` (Pretty Stream) is a wrapper for ``std::ostream``. It provides shortcuts which 
    enable elegant output.

    .. type::   stream_op_t

        The internal type that actually performs output.


    .. function:: \
        PStream(ostream &os) noexcept
        
        Constructor. Wrap the stream ``os``. The output is then made on it.
        
        An internal reference is kept to ``os``. So, make sure it does not destroyed
        on the output calls of ``PStream``.

    .. function:: \
        ostream & get_stream() const noexcept

        Retrive the internal reference to ``os`` passed in the constructor.

    .. function:: \
        stream_op_t & operator<< (ostream& (*pf)(ostream&))
        stream_op_t & operator<< (std::ios& (*pf)(std::ios&))
        stream_op_t & operator<< (std::ios_base& (*pf)(std::ios_base&))
        template<typename T> stream_op_t & operator<<(T &&x)

        Output a specifier ``pf`` (e.g., ``std::endl``, ``std::setw``) or an object ``x`` to 
        the stream. The first output should be made with ``operate<<``, and the subsequent ones 
        should be made with the comma operator (see below examples).

        Possible types of ``x`` are:

        .. table:: 
            :class: fix-width-table
            :widths: 30 70 

            =========================== ==================================================================================
            Standard container          ``array``, ``deque``, ``forward_list``, ``list``, ``map``, ``multimap``, 
                                        ``unordered_map``, ``unordered_multimap``, ``set``, 
                                        ``multiset``, ``unordered_set``, ``unordered_multiset``, ``vector``.
            Pair of input iterator      e.g., ``pout(b, e)``. See below examples.               
            Standard composite type     ``pair``, ``tuple``
            Object with ``operator<<``  
            =========================== ==================================================================================

        If the objects satisfying the above constraints make up of a nested object, it can also be put into the 
        stream, e.g. ``vector<vector<double>>`` and ``tuple<string, int, vector<double>>``.

    .. function:: template<typename It> stream_op_t::it_pair_t<It> operator()(It b, It e)

        Wrap a pair of input iterators to an object which can be passed to ``PStream`` for output.   

    **Example:**
    
    Here we output some objects to :var:`pout`, which is a wrapper of ``std::cout``::

        using HIPP::pout;

        int a[3] = {1,2,3};
        pout << "a has three elements, a[0]=", a[0], ", a[1]=", a[1], ", a[2]=", a[2], '\n',
                "a can be directly put into stream: ", pout(a, a+3), endl;
        
        vector<double> b = {1.1, 2.1, 3.1};
        std::unordered_map<string, double> c = {{"bar", 10.0}, {"foo", 20.0}, {"baz", 30.0}};
        pout << "b=(", b, "), c={", c, "}", endl;

        std::tuple<string, int, vector<double>> d = {"a string", 1, {2.0,3.0, 4.0}};
        pout << "d=", d, endl;

    The output is 

    .. code-block::

        a has three elements, a[0]=1, a[1]=2, a[2]=3
        a can be directly put into stream: 1,2,3
        b=(1.1,2.1,3.1), c={foo:20,baz:30,bar:10}
        d=a string:1:2,3,4


.. class:: template<typename InputIterator> PrtArray

    ``PrtArray`` handles the formated IO for array-like objects (e.g., 
    raw array, ``std::vector``, etc.).

    ``PrtArray`` can be copy/move-constructed, copy/move-assigned, where the 
    format controllers and the iterators are copied/moved. The destructor and 
    move operations are  ``noexcept``.

    Several internal format specifiers/controllers are used to control the IO
    of the array:

    .. table::
        :class: fix-width-table 
        :widths: 20 30 50
        
        ==================== =========================== ===============================================================
        Format specifier     Default value               Effect
        ==================== =========================== ===============================================================
        ``indent``           ``ind=0`` (no indent)       the indent at each line-start.
        ``width``            ``w=-1`` (no padding)       the width of each printed item. 
        ``ncol``             ``nc=-1`` (no line-break)   break the array into several rows, each has ``ncol`` elements.
        ``coloffset``        ``coff=0``                  offset of the first element
        ``sep``              ``sep=","``                 separation to print between elements 
        ``endline``          ``el="\n"``                 what to print at the end of each row 
        ``endlast``          ``elast=""``                what to print after the last elements  
        ==================== =========================== ===============================================================

    Given an array ``a``, the final output is (where ``sep`` is determined by ``width``)::

        ind pad a[0]      sep pad a[1]       ...   sep pad a[ncol-1]   sep el 
        ind pad a[ncol]   sep pad a[ncol+1]  ...   sep pad a[2*ncol-1] sep el 
        ... pad item[n-2] sep pad a[n-1]     elast


    **Examples:**

    .. code-block:: 

        vector<double> arr1 = {1,2,3,4,5,6,7,8};
        PrtArray p_arr1(arr1);
        cout << p_arr1 << endl;

    The output is 
    
    .. code-block:: text

        1,2,3,4,5,6,7,8

    Now we set the number of columns ``ncol`` to 3::

        p_arr1.ncol(3);
        cout << p_arr1 << endl;

    The output is 

    .. code-block:: text

        1,2,3,
        4,5,6,
        7,8

    Now we set the field width ``width`` to 4::

        p_arr1.width(4);
        cout << p_arr1 << endl;

    The output is 

    .. code-block:: text 

        # width is 4 for elements
            1,   2,   3,
            4,   5,   6,
            7,   8
    
    Now we print two arrays together using the ``coloffset`` for the second array::

        cout << p_arr1;
        p_arr1.coloffset( arr1.size()%3 );
        cout << p_arr1 << endl;


    The output is 

    .. code-block:: text 

        # width is 4 for elements; two arrays are joined
            1,   2,   3,
            4,   5,   6,
            7,   8,   1,
            2,   3,   4,
            5,   6,   7,
            8

    .. type:: InputIterator iter_t

        The input iterator type which is used as the source of printing. 

    .. function:: \
        template<typename Container> explicit PrtArray( const Container &array )
        PrtArray( iter_t b, iter_t e )

        The constructors.

        ``PrtArray`` is constructed by a pair of input interators ``b`` and ``e``. 
        The range of elements refered by them are then printed on the call of 
        the function :func:`operator<<` or the method :func:`prt`. Two overloads
        are:

        1.  using an array-like object ``array`` - ``std::begin()`` and 
            ``std::end()`` are used to extract the range iterators.
        2.  using directly the pair of iterators. 

    .. function:: \
        ostream & prt( ostream &os = cout ) const
        friend ostream & operator<< ( ostream &os, const PrtArray &arr )

        Print the content specified by the range iterators into the stream ``os``.
        
    .. function:: \
        std::pair<iter_t, iter_t> get_iter() const
        std::ptrdiff_t indent() const noexcept
        const string & endline() const noexcept
        const string & sep() const noexcept
        const string & endlast() const noexcept
        std::ptrdiff_t ncol() const noexcept
        std::ptrdiff_t coloffset() const noexcept
        std::ptrdiff_t width() const noexcept

        Retrive the current range iterators or the format specifiers.

    .. function:: \ 
        template<typename Container> PrtArray & set_array( const Container &array )
        PrtArray & set_array( iter_t b, iter_t e )
        PrtArray & indent( std::ptrdiff_t value )
        PrtArray & endline( const string &value )
        PrtArray & sep( const string &value )
        PrtArray & endlast( const string &value )
        PrtArray & ncol( std::ptrdiff_t value )
        PrtArray & coloffset( std::ptrdiff_t value )
        PrtArray & width( std::ptrdiff_t value )
        PrtArray & reset_fmt()

        Set the range iterators or the format specifiers.

        ``set_array()`` can be called with either a single ``array`` (where ``std::begin()`` and ``std::end()``
        are used to extract the range iterators) or a pair of iterators. 

        ``reset_fmt()`` resets all the format specifiers to their default values.

    


Formated IO Shortcuts and String Factories
--------------------------------------------------

.. function::   template<typename ...Args> \
                    ostream & prt(ostream &os, Args &&... args)
                template<typename ...Args>\
                    ostream & prt_f( ostream &os, const char *fmt, Args && ...args )
                template<typename Container>\
                    ostream & prt_a( ostream &os, const Container &array )
                template<typename InputIterator>\
                    ostream & prt_a( ostream &os, InputIterator  b, InputIterator  e )
    
    Print arguments into an output stream ``os``. ``os`` is returned.

    ``prt(os, ...args)`` prints any number of aruguments ``args``.
    No padding is added between adjacent arguments or at end. Each argument must have overloaded ``<<`` operator.

    ``prt_f(os, fmt, ...args)`` allows formatted printing of ``args`` using C style formator ``fmt``.

    ``prt_a(os, array)`` or ``prt_a(os, b, e)`` prints an array of elements. The array
    can be specified by a single container ``array`` (``std::begin`` and ``std::end`` are 
    applied to determine its range) or two iterator ``b, e``. Adjacent elements are separated by a comma ",". 
    No padding is added at the beginning or at the end of the printed list.
    For a more fine-tuning controlling, use :class:`PrtArray` class instead.

    **Examples:**

    Use ``prt()`` and its variants to print any number or arguments into ``std::cout``::

        string str = "hello";
        const char *cstr = "world!";
        HIPP::prt(cout, str, ", ", cstr);
        /* equivalent to call cout << str << ", " << c_str; */

        const char *cstr = "world!";
        HIPP::prt_f(cout, "hello, %s", cstr);
        /* equivalent to call printf("hello, %s", cstr) */

        vector<int> arr(5, 1);
        HIPP::prt_a(cout, arr) << endl;
        HIPP::prt_a(cout, arr.begin(), arr.begin()+4) << endl;
        /* In the output devide, printed results would be
           1,1,1,1,1
           1,1,1,1
        */

.. function::   template<typename ...Args>\
                    string str( Args && ...args )
                template<typename ...Args>\
                    string str_f( const char *fmt, Args && ...args )
                template<typename Container>\
                    string str_a( const Container &array )
                template<typename InputIterator>\
                    string str_a( InputIterator  b, InputIterator  e )

    String factories. These functions accept the same arguments as :func:`prt` and its 
    variants do, but return a string instead of printing the content into the stream. 

    **Examples:**

    In the following example we create 10 files whose names are constructed by 
    the :func:`str`::

        for(int i=0; i<10; ++i){
            ofstream ofs( HIPP::str("file.", i, ".txt") );
            /* ... write to ofs ... */
        }
        /* after execution we have 10 files named 
           file.0.txt
           file.1.txt 
           ...
        */

    If the format of number in the string needs carefully control, :func:`prt_f`
    would be helpful. Numeric simulations with multiple snapshot dumped usually 
    name the files this way.
    For example::

        for(int i=0; i<10; ++i){
            double redshift = 0.01 * i;
            ofstream ofs(HIPP::str_f("snapshot.%03d.z%.2f.txt", i, redshift));
            /* ... write to ofs */
        }
        /* after execution we have 10 files named 
           snapshot.000.z0.00.txt
           snapshot.001.z0.01.txt 
           ...
        */

    

    
