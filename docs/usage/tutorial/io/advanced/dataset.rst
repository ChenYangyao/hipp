Working with Datasets
=========================

.. include:: /global.rst

.. namespace:: HIPP::IO::H5


Using the Dataset Manager
--------------------------

The method :expr:`Group::datasets()` returns a dataset manage for 
the caller group. For example, we create a new file and get a group manager 
for it (i.e., its root group)::

    H5::File f1("f1.h5", "w");
    auto dsets = f1.datasets();

The dataset manager can tell whether or not a dataset of given name exists under 
the group. For example, because ``f1`` is empty now, the following assertion 
passes::

    assert( !dsets.exists("dataset_A") );

The following parts demonstrate how to use the manager to

- Writing/reading data of either numerical types or strings.
- Subsetting the data to write/read.

Put/Get Numerical Data
""""""""""""""""""""""""

Dataset managers support various types of data, including 

- single scalar value, such as ``int``, ``float``;
- :class:`GeneralArrayTraits` compliant container of scalar values, such as ``std::vector`` with scalar ``value_type``;
- :class:`RawArrayTraits` compliant object of scalar values, such as raw arrays (``int [3]``, ``float [2][3]``), or 
  ``std::array`` with arbitrary nesting depth (``std::array<int, 3>``, ``std::array<std::array<int, 3>, 2>``);
- :class:`GeneralArrayTraits` compliant container of :class:`RawArrayTraits`, ``such as std::vector< std::array<int, 3> >``.

For example, consider the following objects to be written into a file::

    short s;
    vector<double> d10(10);
    int i5[5];
    long l34[3][4];
    array<float, 3> f3;
    vector<array<int, 3> > i83(8);

    // We initialize them, for example, for i83
    for(size_t i=0; i<8; ++i)
        for(size_t j=0; j<3; ++j)
            i83[i][j] = i*3+j;

To write those objects into a file, simply call the method :expr:`DatasetManager::put(name, data)`
where ``name`` is the dataset name in the file and ``data`` is an object to be written::

    dsets.put("s",      s);                     // Create a new dataset and write to it
    dsets.put("d10",    d10);
    dsets.put("i5",     i5);
    dsets.put("l34",    l34);
    dsets.put("f3",     f3);
    dsets.put("i83",    i83);

The call automatically creates the dataset and write data into it. If it already exists, 
the call overwrite it. In the later case the data in memory must have consistent datatype 
and dataspace to the existing dataset.

To load data from the file, call :expr:`DatasetManager::get(name, data)` where the dataset content is 
written into ``data``. If the outer-most container is a ``std::vector`` , it is resized to exactly 
fit the dataset. If the resize is not feasible, an :class:`HIPP::ErrorLogic` exception is thrown::

    dsets.get("s",      s);                     // Read back the data.
    dsets.get("d10",    d10);
    dsets.get("i5",     i5);
    dsets.get("l34",    l34);
    dsets.get("f3",     f3);
    dsets.get("i83",    i83);

The ``get`` method has an additional single-argument version::

    s = dsets.get<short>("s");
    f3 = dsets.get<array<float, 3> >("f3");

In this version, an object whose type is given by the template argument is default-constructed, 
passed into the two-argument version to get its data, and returned.

The content of file shown by ``h5ls -r f1.h5`` is like:

.. code-block:: text

    /d10                     Dataset {10}
    /f3                      Dataset {3}
    /i5                      Dataset {5}
    /i83                     Dataset {8, 3}
    /l34                     Dataset {3, 4}
    /s                       Dataset {SCALAR}

Put/Get String 
""""""""""""""""""

Strins have very special types. C-style strings are usually stored in raw arrays of 
characters, if their lengths have a predefined upper bound, or otherwise they are stored
in heap memory segments and manipulated by pointers. In C++, strings are usually represented 
by ``std::string``. 

For the sake of simplicity, the dataset manager supports the following types of string data:

- Single string, such as ``std::string``
  ``(const) char [N]``, ``(const) char *`` (null-terminated).
  The single string is represented by ATOMIC STRING datatype in the file.
- Strings: either a ``std::vector<std::string> >`` instance or ``(const) char[N_STR][STR_LEN]``.
  The strings are represented by an 1-D array of ATOMIC STRING datatype in the file.

For example, consider the following "string object"::

    string str = "foo";
    vector<string> str4 = {"top", "bottom", "left", "right"};
    char raw_str[16] = "foo";
    char raw_str4[4][16] = {"top", "bottom", "left", "right"};

To write them into the file, call :expr:`DatasetManager::put_str(name, data)`. 
The library will figure out the correct datatype and dataspace for each of them::
    
    dsets.put_str("str", str);
    dsets.put_str("str4", str4);
    dsets.put_str("raw_str", raw_str);
    dsets.put_str("raw_str4", raw_str4);

To load back the data, call :expr:`DatasetManager::get(name, data)` instead::

    dsets.get_str("str", str);
    dsets.get_str("str4", str4);
    dsets.get_str("raw_str", raw_str);
    dsets.get_str("raw_str4", raw_str4);

Only in the ``std::string`` and ``std::vector< std::string >`` cases, the objects 
are auto-resized to fit the dataset. In other cases, the object must be large enough 
to hold the dataset.

The ``std::string`` or ``std::vector< std::string >`` case also has single-argument version, where 
the loaded string or strings are returned::

    str = dsets.get_str<string>("str");
    str4 = dsets.get_str<vector<string> >("str4");

The content of the file shown by ``h5ls -r f1.h5`` is like:

.. code-block:: text

    /raw_str                 Dataset {SCALAR}
    /raw_str4                Dataset {4}
    /str                     Dataset {SCALAR}
    /str4                    Dataset {4}

Subsetting the Data
""""""""""""""""""""""

The dataset manager provides three methods for the case that only a subset of the 
whole dataset is the target of reading.

The call of :expr:`DatasetManager::at(name, data, coord)` loads a single element 
at given coordinate ``coord`` in the dataset named ``name`` into ``data``. 
For example, the following codes load an element at position ``(1,2)``::

    int i_value;
    dsets.at("i83", i_value, {1,2});

    pout << i_value, endl;            // => 5

The call :expr:`DatasetManager::ats(name, data, coords)` loads a set of points 
into a container object ``data``. The coordinates of the points is given by 
``coords``, which is typed :class:`Points` and can be initialized with the 
number of points and the starting address of the coordinate array::

    vector<int> i_values;
    vector<hsize_t> coords = {
        0,0, 
        1,1, 
        2,2};
    
    dsets.ats("i83", i_values, {3, coords.data()});
    
    pout << i_values, endl;           // => 0,4,8

The call of :expr:`DatasetManager::slab(name, data, slab)` loads a hyperslab.
The geometry of the hyperslab is specified by ``slab`` that is typed 
:class:`Hyperslab` and can be initialized by the starting coordinate and 
the number of elements at all dimensions.
For example, the following codes load a hyperslab ``[1:(1+2)][0:(0+3)]``::

    dsets.slab("i83", i_values, {{1, 0}, {2, 3}});
    
    pout << i_values, endl;           // => 3,4,5,6,7,8

For ``ats`` and ``slab``, the argument ``data`` must has consistent size to 
the selected part of data. If it is a ``std::vector`` of numerical or RawArrayTraits 
compliant type, it is auto-resized to fit the selection.

For writing, we have :expr:`DatasetManager::put_at`, :expr:`DatasetManager::put_ats`,
and :expr:`DatasetManager::put_slab`. The three calls require that 
the dataset is already existing in the file, and overwrite the selected part.