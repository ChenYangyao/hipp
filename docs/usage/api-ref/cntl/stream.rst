Stream Manipulation
==========================

.. include:: /global.rst

The following variables, functions and classes are all defined within the namespace ``HIPP``.

.. namespace:: HIPP 

Predefined Variables
------------------------

.. var::    extern PStream pout
            extern PStream perr

    Pretty output streams for standard output and standard error. See below :class:`PStream` for the detail.

.. var::    extern PLogStream plog

    Stream for producing pretty log records. See below :class:`PLogStream` for the detail.


Formatted IO
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
            :class: fix-width-table tight-table
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

    ``PrtArray`` handles the formatted IO for array-like objects (e.g., 
    raw array, ``std::vector``, etc.).

    ``PrtArray`` can be copy/move-constructed, copy/move-assigned, where the 
    format controllers and the iterators are copied/moved. The destructor and 
    move operations are  ``noexcept``.

    Several internal format specifiers/controllers are used to control the IO
    of the array:

    .. table::
        :class: fix-width-table tight-table
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

Logger
---------

.. class:: PLogStream: protected PStream

    ``PLogStream`` - pretty stream for producing log.

    ``PLogStream`` is like PStream, has overloaded ``<<`` operator which can then be 
    chained using the comma operator.

    ``PLogStream`` has extra methods to produce log records. Its features include

    - Log entries are nested with indents hinting the stack height, i.e., ``PLogStream`` 
      allows entering/leaving scopes, and produces extra indents in the more inner
      scope.
    - The type of information is printed can be controlled, i.e., log entries can be 
      filtered according to its priority level.


    **Memory management methods:**

    .. table:: 
        :class: tight-table
    
        =================================================== ==================================================
        Method                                              Detail 
        =================================================== ==================================================
        default constructor                                 Not available.
        copy and move constructors                          Defined; deep-copy.
        ``operator=(&&)`` |br| and ``operator=(const &)``   Deleted.
        destructor                                          ``noexcept``.
        =================================================== ==================================================

    .. type:: _hippcntl_stream_pretty_log_helper::Guard guard_t
    .. type:: _hippcntl_stream_pretty_log_helper::StreamOperand stream_op_t

    .. enum:: @priority_level_enum : int

        .. enumerator:: \
            LV_EMERG=0
            LV_ALERT=1  
            LV_CRIT=2 
            LV_ERR=3 
            LV_WARNING=4
            LV_NOTICE=5 
            LV_INFO=6
            LV_DEBUG=7 
            LV_MIN=-1
            LV_MAX=100

    .. enum :: @other_constants : size_t

        .. enumerator:: ENTRY_PREFIX_MAXLEN=32

    .. function:: \ 
        PLogStream(ostream &os, int level=LV_NOTICE, int level_used=LV_NOTICE, int stack_height=0) noexcept

        Constructor.

        :arg os: which the log output are directed to.
        :arg level:         the current priority level.
        :arg level_used:    under which level the information should be printed.
        :arg stack_height:  the depth of nesting.
    
    .. function:: \
        PLogStream & set_level(int level=LV_NOTICE) noexcept
        PLogStream & set_level_used(int level_used=LV_NOTICE) noexcept
        PLogStream & set_indent(int indent=2) noexcept
        PLogStream & set_entry_prefix(const string &entry_prefix="|- ") noexcept

        Attribute setters.
        
        - ``set_level()`` - current priority level.
        - ``set_level_used()`` - used priority level. Log entries with 
          `level <= level_used` will be printed.
        - ``set_indent()`` - number of extra spaces padded at the front of each log entry.
        - ``set_entry_prefix()`` - prefix padded at the front of each log entry. The 
          prefix should be no longer than :enumerator:`ENTRY_PREFIX_MAXLEN`-1. Otherwise
          it is truncated.
    
    .. function:: \
        int level() const noexcept
        int level_used() const noexcept
        int indent() const noexcept
        string entry_prefix() const noexcept
        int stack_height() const noexcept

        Attribute getters.
        
        - ``stack_height()`` - current stack height. If the height is non-zero,
          ``indent*stack_height`` spaces and ``entry_prefix`` are printed for each 
          entry. Otherwise nothing is padded.

        Other attributes are explained in the corresponding setters.

    .. function:: \
        stream_op_t operator<< (ostream& (*pf)(ostream&))
        stream_op_t operator<< (std::ios& (*pf)(std::ios&))
        stream_op_t operator<< (std::ios_base& (*pf)(std::ios_base&))
        template<typename T> stream_op_t operator<<(T &&x)

        Output an object to this stream.
        The object can be an standard formatter ``pf``, or any object ``x`` that is 
        printable to PStream.
    
        The returned :type:`stream_op_t` object allows chaining the subsequent outputs
        using comma operator, e.g., ``plog << "the object ", x, endl;``.
    
        The contents output by ``<<`` and ``,`` in a single statement is viewed as 
        a single log entry. Paddings, as controlled by ``indent`` and ``entry_prefix``
        is added.
        
    .. function:: \
        template<typename ...Args> void push(Args &&...args)
        void pop(bool hint=false)

        ``push()`` increases the stack height by 1, while ``pop()`` decreases it by 1.
        These two operations are used to increse the indent of log entry.

        It is always recommended to use the guarded version :func:`push_g()` and 
        :func:`push_at()` instead of the direct ``push()`` and ``pop()``. (RAII style)

        For ``push()``, if ``args`` are non-empty, they are ouput into the stream, with
        ``indent*stack_height`` spaces padding at front. Then, stack height is 
        increased.

        For ``pop()``, if `hint` is true, an entry is output, and then the stack height
        is decreased.
    
    .. function:: \
        template<typename ...Args> guard_t push_g(Args &&...args)
        template<typename ...Args> guard_t push_at(int level, Args &&...args)

        The guarded versions of :func:`push()`
        
        ``push_g()`` - call :func:`push()` and returns a guard.
        
        ``push_at()`` - change the level to ``level``, call :func:`push()`, and returns a guard.

        The guard is responsible for restoring the ``stack_height`` on its destruction
        (similar to calling :func:`pop()`). The ``level`` is also restored if its was changed.

    .. function:: template<typename It> stream_op_t::it_pair_t<It> operator()(It b, It e)

        Objects in an iterable range defined by iterators [b, e) can be printed 
        by, e.g., ``pout << "{", pout(b, e), "}", endl;``

        This is particularly useful for the raw array, like::

            int a[N];
            int *a = new int [N];

    .. function:: ostream & get_stream() const noexcept

        Return a reference to the internal ``std::ostream`` object.


    **Examples:**

    To perform logging, just use ``<<`` to put content into the stream, and chain 
    the subsequent contents with ``,``::

        plog << "Begin computation", endl;
        int sum = 0;
        for(int i=0; i<5; ++i)
            sum += i;
        plog << "Sum from 0 to 4 is ", sum, endl;

    The output is 

    .. code-block:: text

        Begin computation
        Sum from 0 to 4 is 10

    To enter a new scope, call :func:`push_g()` and name the returned guard object. 
    On the destruction of the guard object, we automatically leave the scope. The 
    following codes show a function ``outer()``, with a nested ``inner()`` inside 
    it::

        double inner(double x) {
            auto g = plog.push_g(emFF);
            plog << "Begin computation for x=", x, "", endl;
            double y = std::sin(x);
            plog << "Got result ", y, endl;
            return x;
        }

        void outer() {
            auto g = plog.push_g(emFF, "- the outer loop");
            vector<double> xs = {1., 2., 3.}, ys;
            plog << "Begin computation for ", xs.size(), " values" , endl;
            plog << "Inputs = {", xs, "}", endl;
            for(size_t i=0; i<xs.size(); ++i){
                ys.push_back(inner(xs[i]));
            }
            plog << "Got all results {", ys, "}", endl;
        }

    Here ``inner()`` just computes the sine of the input. :c:macro:`emFF` is 
    a shortcut macro which gives the current source file name ("plog.cpp") and the function name ("outer").

    A call of ``outer()`` then outputs

    .. code-block:: text 

        [file] plog.cpp, [func] outer - the outer loop
          |- Begin computation for 3 values
          |- Inputs = {1,2,3}
          [file] plog.cpp, [func] inner 
            |- Begin computation for x=1
            |- Got result 0.841471
          [file] plog.cpp, [func] inner 
            |- Begin computation for x=2
            |- Got result 0.909297
          [file] plog.cpp, [func] inner 
            |- Begin computation for x=3
            |- Got result 0.14112
          |- Got all results {1,2,3}

    To specify the priority level in the scope, call :func:`push_at()` with the 
    first argument specifying the desired level. The default level is :enumerator:`LV_NOTICE`.
    The following codes show how to enter the :enumerator:`LV_INFO` and :enumerator:`LV_DEBUG` 
    levels, respectively. Those two levels have lower priority than the NOTICE level::

        void log_with_filter() {    
            plog << "With filter", endl;
            plog << "Log entries with low priority levels are not printed", endl;
            {
                auto _ = plog.push_at(plog.LV_INFO, "Lower-priority block");
                plog << "Printed only when the INFO level is required", endl;
                plog << "Begin execution", endl;
                /* work on a task */
                {
                    auto _ = plog.push_at(plog.LV_DEBUG, "Even lower-priority block");
                    plog << "Printed only when the DEBUG level is required", endl;
                    plog << "Begin more detail of the task", endl;
                    /* work on detail */
                    plog << "End successfully", endl;
                }
                plog << "End execution", endl;
            }
            plog << "End of subroutine", endl;
        }

    A call of ``log_with_filter()`` outputs 

    .. code-block:: text

        With filter
        Log entries with low priority levels are not printed
        End of subroutine

    Where the log entries with priority lower than :enumerator:`LV_NOTICE` are not printed.
    To allow printing the INFO entries, call :func:`set_level_used()`::

        plog.set_level_used(plog.LV_INFO);
        log_with_filter();

    Then the output give all logs with priority equal or higher than the INFO level:

    .. code-block:: text
    
        With filter
        Log entries with low priority levels are not printed
        Lower-priority block
        |- Begin execution
        |- End execution
        End of subroutine


    If the :enumerator:`LV_DEBUG` is turned on::

        plog.set_level_used(plog.LV_DEBUG);
        log_with_filter();

    Then, more logs are printed

    .. code-block:: text

        With filter
        Log entries with low priority levels are not printed
        Lower-priority block
        |- Printed only when the INFO level is required
        |- Begin execution
        Even lower-priority block
            |- Printed only when the DEBUG level is required
            |- Begin more detail of the task
            |- End successfully
        |- End execution
        End of subroutine


Shortcuts for formatted IO and Factories of Strings 
----------------------------------------------------

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

    

Helpers
--------------

The following classes are defined as helpers for the main classes. 
Users generally do not use them directly, and should get access to 
them by calling the methods of the main classes.


.. class:: _hippcntl_stream_pretty_log_helper::Guard

    Guard of the push operation of :class:`PLogStream`.

    On the calling of a guarded push, the :class:`PLogStream` releases a guard object. 
    On the destruction of the guard object, the side effect of push operation is
    restored.

    **Memory management methods:**

    .. table:: 
        :class: tight-table

        =================================================== ====================================================
        Method                                              Detail 
        =================================================== ====================================================
        default constructor                                 Not avaiable.
        copy constructor |br| and ``operator=(&&)``         Deleted.
        move constructor |br| and ``operator=(const &)``    Available; ``noexcept``.
        destructor                                          ``noexcept``.
        =================================================== ====================================================

    .. function:: Guard(PLogStream &pls, bool hint_pop=false)

        Constructor. Get a guard to the current scope for a stream ``pls``.
        
        :arg hint_pop: see the attribute setter :func:`hint_pop_on()` and :func:`hint_pop_off()`.

    .. function:: \
        void hint_pop_on() noexcept
        void hint_pop_off() noexcept
        
        Switch on/off the hint on pop.

        If on, an extra entry is output to the :class:`PLogStream` at the destruction of 
        the guard, similar to calling :func:`PLogStream::pop()` with ``true``.

    .. function:: \
        bool hint_pop() const noexcept
    
        Attribute getter - whether the hint on pop is turned on.

    .. function:: \
        void set_level(int level, bool scoped=true) noexcept

        Change the `level` of the :class:`PLogStream`. 
        
        :arg scoped: if true, the guard object is responsible for restoring the old 
            ``level``. Otherwise the ``level`` is really changed - the guard does not
            restore it.


.. class:: StreamOperand: protected PStream::stream_op_t

    The stream operand of PLogStream. See the description of 
    :func:`PLogStream::operator<<` for the detail usage of the stream operand.

    **Memory management methods:**

    .. table:: 
        :class: tight-table

        =================================================== ====================================================
        Method                                              Detail 
        =================================================== ====================================================
        default constructor                                 Not avaiable.
        copy constructor |br| and ``operator=(&&)``         Deleted. ``noexcept``. The copied one refers to 
                                                            the same stream object.
        move constructor |br| and ``operator=(const &)``    Available; ``noexcept``.
        destructor                                          ``noexcept``.
        =================================================== ====================================================

    .. type:: PStream::stream_op_t parent_t
    .. type:: parent_t::it_pair_t

    .. function:: \
        StreamOperand(ostream &os, bool enabled) noexcept
        StreamOperand(const parent_t & op, bool enabled) noexcept

        Constructor.
        
        :arg os: the operand refers to that stream and the comma operator outputs 
            conntent into it.
        :arg op: the operand refers to the same stream as `op`.
        :arg eanbled: if true, output will show. Otherwise no output is shown.

    .. function:: \
        StreamOperand & operator, (ostream& (*pf)(ostream&))
        StreamOperand & operator, (std::ios& (*pf)(std::ios&)) 
        StreamOperand & operator, (std::ios_base& (*pf)(std::ios_base&))
        template<typename T> StreamOperand & operator,(T &&x)

        The comma operator allows chaining outputs.

    .. function:: \
        ostream & get_stream() const noexcept

        Return a reference to the internal std::ostream object.