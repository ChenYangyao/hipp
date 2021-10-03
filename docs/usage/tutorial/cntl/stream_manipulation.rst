Basic Stream Manipulation
==========================

.. include:: /global.rst

Printing is a common task in C++ programming. The standard library provides 
various facilities for it (e.g. ``std::printf`` and ``std::cout``), which enable 
very detailed controlling of both binary and formatted read/write.

Printing is so common that HIPP decides to provide some shortcuts for it. The 
formatted printing functions and classes are described here, mainly for producing 
log records along the execution of the program. Binary IO for large computation 
data is described in the separate :doc:`IO Module <../io/index>`.

All the example codes in the below sections can be downloaded at:  
:download:`cntl/basic-stream-manip.cpp </../example/tutorial/cntl/basic-stream-manip.cpp>`.

The following example codes assume a namespace declarations::

    using namespace HIPP;
    using namespace std;

which expose all the standard library and HIPP CNTL module interfaces.

.. _tutor-cntl-pstream:

Printing Anything With the Pretty Stream 
-----------------------------------------

As ``std::ostream``, numeric types like ``double``, ``int``, or string type ``std::string``
can be directly put into the Pretty Stream::

    double a = 1.0;
    int b = 2;
    string c = "foo";

    pout << 
        "a = ", a, '\n',
        "b = ", b, '\n',
        "c = ", c, endl;

The whole output sequence is started by ``<<`` on the Pretty Stream :var:`~HIPP::pout`, 
followed by zero or more comma separate arguments. This is a little bit different from the standard 
``std::cout``.

Output:

.. code-block:: text

    a = 1
    b = 2
    c = foo

For STL containers and utilities, you can still put them into Pretty Stream,  such 
as the ``std::vector``, ``std::unordered_map``, ``std::pair``, and ``std::tuple``::

    vector<string> d = {"bar", "baz"};
    unordered_map<string, string> e = {{"a", "f(a)"}, {"b", "f(b)"}};
    pair<string, int> f = {"pair-key", 1};
    tuple<string, int, vector<double> > g = {"tuple-elem", 1, {1.,2.,3.}};

    pout << 
        "d = ", d, '\n',
        "e = ", e, '\n',
        "f = ", f, '\n',
        "g = ", g, endl;

Output:

.. code-block:: text

    d = bar,baz
    e = b:f(b),a:f(a)
    f = pair-key:1
    g = tuple-elem:1:1,2,
    
For raw arrays or iterable ranges, use the ``pout(b, e)`` to convert them 
to a printable object. To get more control of the print format, 
use :class:`~HIPP::PrtArray` class::

    int arr1d[5] = {1,2,3,4,5};
    int arr2d[2][3] = {{1,2,3}, {4,5,6}};

    pout << 
        "arr1d = ", pout(arr1d, arr1d+5), '\n',
        "arr1d = ", PrtArray(arr1d, arr1d+5), '\n',
        "arr2d = \n", PrtArray(arr2d[0], arr2d[0]+6).ncol(3).width(4), endl;

Output:

.. code-block:: text

    arr1d = 1,2,3,4,5
    arr1d = 1,2,3,4,5
    arr2d = 
        1,   2,   3,
        4,   5,   6


Note that the first usage ``pout(b, e)`` is valid for any input 
iterator pair ``[b, e)``. The range referred by them is printed as 
a comma-separated sequence. The :class:`~HIPP::PrtArray` class is more general.
For the 1-d range ``[b, e)``, just use ``PrtArray(b, e)`` to 
format it. For a 2-d range, use method :func:`~HIPP::PrtArray::ncol` to 
specify the number of columns, and use method :func:`~HIPP::PrtArray::width` 
to limit the field width so that elements in the same column are aligned.

Printing User-defined Classes 
""""""""""""""""""""""""""""""
For any user-defined class, if you want to print it to a Pretty Stream, just 
overload the ``operator<<`` on ``std::ostream``. For example, if you have 
a ``Person`` class, you may define the ``operator<<`` like::

    struct Person {
        string first_name, last_name;
        int age;
    };
    ostream & operator<< (ostream &os, const Person &person) {
        os << person.first_name << ' ' << person.last_name 
            << " (" << person.age << ")";
        return os;
    } 

Then, any ``Person`` instance can be printed into a Pretty Stream. For example, 
you may print it into :var:`~HIPP::pout`::

    Person albert = {"Albert", "Einstein", 120},
        edwin = {"Edwin", "Hubble", 100};
    pout << albert, " and ", edwin, endl;

This gives the output 

.. code-block::

    Albert Einstein (120) and Edwin Hubble (100)

For any other types without ``operator<<`` overloads, HIPP automatically synthesize 
a compiler-dependent formated print. For example::

    struct AnyClass {};
    
    pout << "AnyClass: ", AnyClass(), endl;

Output:

.. code-block:: text

    AnyClass: <8AnyClass instance at 0x7ffebcd287ff>

Using Pretty Stream for Files and Strings
"""""""""""""""""""""""""""""""""""""""""""

C++ standard library uses a unified model for standard output/standard error, 
for file, and for string stream. You can use HIPP's Pretty Stream to 
put formatted content into them.

Once you have a file stream, just construct a :class:`~HIPP::PStream` instance on 
that file::

    ofstream fs("filestream.dat");
    PStream ps_for_fs(fs);

Just like what we did on the standard output, we can put anything to the Pretty 
Stream now::

    ps_for_fs << "bar", " foo ", "baz", endl;

This writes the content into the file. We may look at it:

.. code-block:: bash

    $ cat filestream.dat 
    bar foo baz

The Pretty Stream can be applied to incoorperate with any other ``otream`` object.
For example, the ``std::stringstream``::

    ostringstream ss;
    HIPP::PStream ps_for_ss(ss);
    ps_for_ss << "bar", " foo ", "baz", endl;

The content in that string stream can be obtained by its ``str()`` method::

    pout << "The content of stringstream is ", ss.str();


C-style Formatted Output
--------------------------

Some people think that the C-style formatted print calls (e.g., ``printf``, ``sprintf``) 
are more convenient. In C++ applications, standard streams have type 
``std::ostream`` and we cannot 
call the C-style functions on them. 

HIPP provides some shortcuts for C-style output. 
The function :func:`~HIPP::prt_f` allows formatted printing on a 
C++ stream using C-style formattor. For example::

    double a = 3.1415; 
    int b = 111;
    prt_f(cout, "a=%10.6f, b=%6d\n", a, b);

Which has the same effect as calling ``printf``. :func:`~HIPP::prt_f` also works
when using other streams, like ``std::ofstream`` and ``std::ostringstream``. The output 
of above codes is 

.. code-block:: text 

    a=  3.141500, b=   111

However, the C++ style is still preferred due to its more strict compile-time type checking. 
So, use the C-style priting only when it is really necessary.

String Factories
-----------------

With standard string stream, it is easy enough to create string from other 
objects. However, an inline string factory may be more concise in some cases.
HIPP provides several string factories (i.e., shortcut functions) for that purpose.

For example, in a N-body simulations, particle data at a time step (called "snapshot")
may be too large to be contained in a single file. So, 
we have to create multiple files,
each containing a small "part" of the particles. The filename can be 
created by function :func:`~HIPP::str`, which converts all of its arguments into 
a single string::

    int snapshot = 100, part_id = 1;
    string filename1 = str("snapshot", snapshot, ".", part_id);

where ``filename1`` will be ``"snapshot100.1"``. Sometimes, 
more format controlling is necessary, 
then a C-style formatted string factory :func:`~HIPP::str_f` can be used::

    filename2 = str_f("snapshot%.4d.%.4d", snapshot, part_id);

where ``filename2`` will be ``"snapshot0100.0001"``.











