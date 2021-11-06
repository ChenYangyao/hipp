***********************
Binary I/O
***********************

.. include:: /global.rst

.. namespace:: HIPP::IO::H5

I/O with HDF5 Format
======================

Using the Library
------------------

To use HIPP's I/O module, include the header ``<hippio.h>``. A minimal cpp source
file is like::

    // h5-minimal-example.cpp
    #include <hippio.h>

    using namespace HIPP;
    using namespace std;
    namespace H5 = HIPP::IO::H5;

    int main(int argc, char const *argv[]) {
        // use the library
        return 0;
    }

Note that we adopt the above namespace declarations for clarity throughout this 
tutorial.

To generate executables, just compile and link the libraries. Then run it

.. code-block:: bash

    g++ -std=c++17 h5-minimal-example.cpp -lhippio -lhippcntl -lhdf5
    ./a.out


Files and Groups
------------------

To create a new HDF5 file, just construct a :class:`File` instance by providing 
a file name and an access mode flag::

    H5::File f("file1.h5", "w");                // Create a new file (truncate if existing).

To open an existing file, use the ``"a"`` (appending) flag::

    H5::File f("file1.h5", "a");                // Open an existing file.

A data group is created with :expr:`Group::create_group` method of a file or 
another group::

    H5::Group g1 = f.create_group("g1");        // Create a new data group.
    auto g2 = g1.create_group("g2-in-g1");      // Use "auto" for type deduction.

Existing group can be re-opened by using :expr:`Group::open_group` method of 
a file or another group::
    
    auto g_root = f.open_group("/");
    auto g2 = f.open_group("g1/g2-in-g1");
    auto g2 = g1.open_group("g2-in-g1");

Note that the root group (path ``"/"``) is auto-created by the library when 
creating the file.

The file content can be navigated by HDF5 command line tool, e.g., ``h5ls``:

.. code-block:: bash 

    $ h5ls -r file1.h5 
    /                        Group
    /g1                      Group
    /g1/g2-in-g1             Group    

Datasets
----------

To create datasets and put contents into them, the simplest way is to use the 
:class:`DatasetManager` of a file or a group::

    H5::DatasetManager dsets = f.datasets();    // Get the dataset manager of the file

The dataset manager provides shortcuts for common used I/O tasks, which is 
sufficient in most cases.

Now, suppose you have the following data to be put into a file, which shall be 
numerical scalar, ``std::vector`` or numerical scalar, 
raw-array-like object of numerical scalar in any dimensionality 
(e.g., ``std::array<std::array<int, 3>, 4>``, ``int [3][4]``), 
or even ``std::vector`` of the raw-array-like object::

    short s;
    vector<double> d10(10);
    int i5[5];
    long l34[3][4];
    array<float, 3> f3;
    vector<array<int, 3> > i83(8);

Using the :expr:`DatasetManager::put` method by providing a dataset name and data 
object to be written, the library creates dataset with suitable datatype and 
shape, and then writes the data into it. Note that if the dataset already exists, 
the library just overwrites it (in such case the data to be put must have
consistent type and shape)::

    dsets.put("s",      s);                     // Create a new dataset and write to it
    dsets.put("d10",    d10);
    dsets.put("i5",     i5);
    dsets.put("l34",    l34);
    dsets.put("f3",     f3);
    dsets.put("i83",    i83);

To load back the data from the file, just call :expr:`DatasetManager::get`::

    dsets.get("s",      s);                     // Read back the data.
    dsets.get("d10",    d10);
    dsets.get("i5",     i5);
    dsets.get("l34",    l34);
    dsets.get("f3",     f3);
    dsets.get("i83",    i83);

In the ``std::vector`` cases (i.e., ``d10`` and ``i83``), the vector is auto-resized
to exactly fit the data to be loaded.

To see the meta-info of a :class:`Dataset`, open it by :expr:`DatasetManager::open` method::

    auto dset = dsets.open("l34");

The :class:`Datatype` and :class:`Dataspace` of the dataset can be retrieved by 
the corresponding methods::

    auto dspace = dset.dataspace();
    auto dtype = dset.datatype();
    
    pout << "Dataspace: ndims = ",      dspace.ndims(), 
            ", dims = ",                dspace.dims(), '\n',
            "Datatype: size = ",        dtype.size(), 
            ", class = ",               dtype.get_class(), endl;

The output is:

.. code-block:: text 

    Dataspace: ndims = 2, dims = Dimensions{3,4}
    Datatype: size = 8, class = 0

The datasets in the file can be navigated in command line by ``h5dump``:

.. code-block:: bash

    $ h5dump file1.h5 
    HDF5 "file1.h5" {
    GROUP "/" {
        DATASET "d10" {
            DATATYPE  H5T_IEEE_F64LE
            DATASPACE  SIMPLE { ( 10 ) / ( 10 ) }
            DATA { ... }
        }
        DATASET "f3" {
            DATATYPE  H5T_IEEE_F32LE
            DATASPACE  SIMPLE { ( 3 ) / ( 3 ) }
            DATA { ... }
        }
        DATASET "i5" {
            DATATYPE  H5T_STD_I32LE
            DATASPACE  SIMPLE { ( 5 ) / ( 5 ) }
            DATA { ... }
        }
        DATASET "l34" {
            DATATYPE  H5T_STD_I64LE
            DATASPACE  SIMPLE { ( 3, 4 ) / ( 3, 4 ) }
            DATA { ... }
        }
        DATASET "s" {
            DATATYPE  H5T_STD_I16LE
            DATASPACE  SCALAR
            DATA { ... }
        }
    }
    }

Attributes 
-----------

Attributes are "small datasets" attached to HDF5 named objects (file, group, 
dataset, or named datatype). The API of attributes is similar to that of 
dataset. First, get an :class:`AttrManager` of a named object::

    auto attrs = f.attrs();                     // Get the attribute manager of the file

Then, for anything like scalar, raw array or ``std::vector`` of them, call 
:expr:`AttrManager::put` to create a new attribute and  write the data into it::

    short s;
    vector<double> d10(10);
    int i5[5];
    long l34[3][4];
    array<float, 3> f3;
    vector<array<int, 3> > i83(8);

    attrs.put("s",      s);                     // Create a new attribute and write to it
    attrs.put("d10",    d10);
    attrs.put("i5",     i5);
    attrs.put("l34",    l34);
    attrs.put("f3",     f3);
    attrs.put("i83",    i83);

To load data back, call :expr:`AttrManager::get`::

    attrs.get("s",      s);                     // Read back the data.
    attrs.get("d10",    d10);
    attrs.get("i5",     i5);
    attrs.get("l34",    l34);
    attrs.get("f3",     f3);
    attrs.get("i83",    i83);

Meta-info of an attribute can be obtained by first opening it with :expr:`AttrManager::open`,
and the use the corresponding methods to retrieve the information::

    auto attr = attrs.open("l34");
    auto dspace = attr.dataspace();
    auto dtype = attr.datatype();
    
    pout << "Dataspace: ndims = ",      dspace.ndims(), 
            ", dims = ",                dspace.dims(), '\n',
            "Datatype: size = ",        dtype.size(), 
            ", class = ",               dtype.get_class(), endl;

The output is:

.. code-block:: text

    Dataspace: ndims = 2, dims = Dimensions{3,4}
    Datatype: size = 8, class = 0

Structured Data 
---------------

In many applications, data of an object is group as structured type. For example::

    struct S {
        int a;
        double b[3];
        float c[2][3];
        array<array<long, 3>, 4> d;
    } data[10];

The ``struct S`` has four fields. To input/output them with HDF5 format, just 
define a :class:`XTable` object which describes the layout of the structured
type::

    auto xtbl = HIPPIO_H5_XTABLE(S, a, b, c, d);

Note that the order of fields is not significant. If not all fields are needed,
just skip any of them.

To write a contiguous buffer (e.g., raw array, ``std::vector``, ``std::array``)
of records, call :expr:`XTable::write` by providing the data buffer and 
a file name to write in. For example, here we write a raw array of 10 records::

    xtbl.write(data, "file1.h5");                   // Write to a file named "file1.h5".

The four fields are output as four separate datasets under the root group of 
that file. 

To write them to a specific data group, just replace the second argument of ``write``
with a :class:`Group` instance::

    H5::File f("file1.h5", "a");
    xtbl.write(data, f.create_group("S1"));         // Write to a group.
    
The data buffer may be other contiguous buffer object, like ``std::vector``.
You may also pass a pointer plus number of records as the buffer specification::

    vector<S> vec_data(10);
    xtbl.write(vec_data, f.create_group("S2"));     // Write a vector of records.
    xtbl.write(vec_data.data(), 10, f.create_group("S3"));                      
                                                    // Write a raw buffer of records.
    
To read back the datasets, call :expr:`XTable::read` by providing a file name 
or a :class:`Group` instance::
    
    vector<S> in_data = xtbl.read("file1.h5");      // Read from a file named "file1.h5".
    in_data = xtbl.read(f.open_group("S1"));        // Read from a group.

The records are returned in a ``std::vector``. You may also read them into 
an existing vector which is auto-resized to fit the file content::

    xtbl.read(vec_data, f.open_group("S1"));        // Read into an existing vector.
    

To write/read the records as a single dataset with COMPOUND datatype, call :expr:`XTable::write_records`::

    xtbl.write_records(vec_data, f, "S1-records");  // Write to a dataset "S1-records" in f.
    in_data = xtbl.read_records(f, "S1-records");   // Read back.

To write/read the records to/from a subset of all "rows" (i.e., a 1-D hyperslab) 
in the file, just select the desired rows by :expr:`XTable::select_rows`, and then 
perform I/O operations::

    xtbl.select_rows(3, 5);                         // Select rows [3, 3+5) for I/O.
    in_data = xtbl.read("file1.h5");        
    pout << "n_rows loaded = ", in_data.size(), endl;

To write/read only a subset of all fields, for example, the field "a" and "c", just 
call :expr:`XTable::select_fields` and then perform I/O::

    xtbl.select_fields({"a", "b"});                 // Select fields "a" and "b" for I/O.
    in_data = xtbl.read("file1.h5");               

After finishing those subsettings, you may restore the selection::

    xtbl.select_all().select_all_fields();          // Clear the selection.


Advanced Topics
=================

.. toctree::
    :maxdepth: 2

    advanced/file
    advanced/dataset

.. toctree:: 
    :maxdepth: 2
    :caption: Basic usage

    basic_usage/read_write_built_in_data.rst
    basic_usage/user_defined_datatype.rst
    Cooperate with h5py <basic_usage/cooperate_with_h5py.rst>
