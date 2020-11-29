Stream Manipulation
==========================

The following functions and classes are all defined within the namespace ``HIPP``.

.. namespace:: HIPP 


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

    

    
