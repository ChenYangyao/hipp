I/O Arrays of Numeric Types
===============================================================

All of the HIPP IO APIs can be accessed by including the header ``<hippio.h>``. The detailed conventions
and compiling options are described in the :ref:`API Reference <api-io-usage>`.

Write/Read Arrays
--------------------------------------------------------------

In C++ programming, an array of a numeric type (e.g., double, int, ...) is usually stored 
in the STL vector. Or it can be manualy stored in raw memory buffer such as those allocated by ``new``
operator or ``std::malloc()``. Examples of such arrays are::

    const size_t N = 5;
    vector<double> arr1 = {0.,1.,2.,3.,4.};                     // a vector of 5 elements 
    
    double *arr2 = new double [N];                              // raw buffer with 5 doubles

To write them into a file of HDF5 format, you simply create a file by defining a 
:class:`HIPP::IO::H5File` instance with desired filename and a "w" mode flag 
("w" means create and truncate the file). Then, a call of 
``create_dataset<T>(dataset_name, dims)`` creates a dataset in the root group of the 
file, with ``T`` specifying its element data type, ``dataset_name`` specifying its 
name and ``dims`` specifying its length in every dimension. 
Finally, a call of ``write(arr1)`` writes the vector ``arr1`` into the dataset::

    HIPP::IO::H5File file("arrays.h5", "w");                    // create a file           
    auto dset1 = file.create_dataset<double>("arr1", {N} );     // create a dataset 
    dset1.write(arr1);                                          // write out an array

The ``write()`` method also accepts a pointer to raw array. So, a call of 
``write(arr2)`` writes the data in ``arr2`` into the dataset. 
It is also allowed to chain the ``create_dataset()`` and ``write()`` operations
in one line of code::

    file.create_dataset<double>("arr2", {N}).write(arr2);       // write out from raw buffer

Note that the size of the dataset (specified in ``create_dataset()``) must be 
compatible with the argument passed in ``write()``, or otherwise the result is 
undefined.

To read the the dataset back into the memory, just open the dataset by ``open_dataset(dataset_name)``,
and load the data into a vector by ``read(vector_name)``. The library automatically resizes 
the vector according to the dataspace in the file::

    vector<double> arr1_in;
    file.open_dataset("arr1").read(arr1_in);                    // read into the vector

If you want to load the data into a raw buffer, you open the dataset, manually 
get its dataspace with ``dataspace()``, and find the size of the dataspace 
(total number of scalar elements) by ``size()``. Then you use the size to properly
allocate the memory buffer, and again use ``read()`` to load data::

    auto dset2 = file.open_dataset("arr2");
    size_t n_elems = dset2.dataspace().size();
    double *arr2_in = new double [n_elems];
    file.open_dataset("arr2").read(arr2_in);

For multi-dimensional array (stored in row-major order in contiguous memory), 
the only difference is that when ``create_dataset()``, you specify each of the dimensions
of that array. Then the write/read operations are the same as those of one-dimensional array

.. code-block:: cpp
    :emphasize-lines: 3

    const size_t n0=2, n1=3, n2=4;
    vector<int> arr3(n0*n1*n2);
    file.create_dataset<int>("arr3", {n0, n1, n2}).write(arr3);
    file.open_dataset("arr3").read(arr3);

    /**
     * Again, for a reading into raw buffer, use dataspace() to retrive the 
     * dataspace, and use size() to get the total number of elements.
     */
    auto dset3 = file.open_dataset("arr3");
    int *arr3_in_buff = new int[ dset3.dataspace().size() ];
    dset3.read(arr3_in_buff);

After the output operations, a file named "arrays.h5" is created in the OS's file system.
Using ``h5dump arrays.h5`` in the command line prompt, you can view and verify the content of 
each dataset

.. code-block:: text 

    HDF5 "arrays.h5" {
    GROUP "/" {
        DATASET "arr1" {
            DATATYPE  H5T_IEEE_F64LE
            DATASPACE  SIMPLE { ( 5 ) / ( 5 ) }
            DATA {
            (0): 0, 1, 2, 3, 4
            }
        }
        DATASET "arr2" ...
        DATASET "arr3" ...

.. warning::

    For both one-dimensional and multi-dimensional cases, the library only accepts 
    arrays with contiguous memory layout. That means the following cases cannot 
    be manipulated by HIPP::

        vector<vector<int> > vector_of_vectors;

        vector<double *> vector_of_pointers_to_buffers;

    The followings are allowed::

        vector<array<double, 3>> vector_of_arrays;

        struct ArrayType {
            float values[3];
        };
        vector<ArrayType> vector_of_structs;

        int raw_array[2][3];

    However, in such cases you need to take the pointer to the underlying data and 
    cast it into a proper numeric type::

        dset.write((double *)&vector_of_arrays[0]);
        dset.write((float *)&vector_of_structs[0]);
        dset.write(&raw_array[0][0]);


Using hyperslab
--------------------------------------------------------------

When dealing with very large dataset, sometimes we just want to take part of
the data. To do that, we need to use the hyperslab feature in hdf5. The `read`
member function for Dataset type has two parameters, `memspace` and
`filespace`, as type of dataspace. These two parameters describe what `read`
do: to move data from `filespace` to `memspace` (`write` has similar member
functions). The hyperslab works by attach some information on these two
parameters using function `select_hyperslab`.

The supported data selection of hyperslab can be described by four parameters,
start, stride, count and block.  For example, in a two dimension array with
shape = (8, 12), we want to select data marked with *

.. code-block:: Text

    0, *, *, 0, *, *, 0, *, *, 0, *, *,
    0, *, *, 0, *, *, 0, *, *, 0, *, *,
    0, *, *, 0, *, *, 0, *, *, 0, *, *,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, *, *, 0, *, *, 0, *, *, 0, *, *,
    0, *, *, 0, *, *, 0, *, *, 0, *, *,
    0, *, *, 0, *, *, 0, *, *, 0, *, *,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

This hyperslab is specified by

* start = (0, 1): starting location of the hyperslab
* stride = (4, 3): number of elememts to separate each block selected
* count = (2, 4): number of blocks to select 
* block = (3, 2): size of block selected

In the following example, we show how to use the hyperslab feature to select
part of the data from a file and how to put it into another buffer with another
hyperslab. Here we want to select a rectangular region, so the stride and block
are set to 1-s by default.

.. code-block:: cpp

    /*
     * In this code, we will show how to use the hyperslab feature in hdf5
     *
     * We have an dataset with shape= (4, 5), and data is
     * 0,  1,  2,  3,  4,
     * 5,  6,  7,  8,  9,
     * 10, 11, 12, 13, 14,
     * 15, 16, 17, 18, 19
     * We only want to take part of the data, the mask is
     * 0, 0, 0, 0, 0,
     * 0, *, *, *, 0,
     * 0, *, *, *, 0,
     * 0, 0, 0, 0, 0
     * This hyperslab can be characterized as start=(1, 1), count=(2, 3)
     * So the data we read is 6, 7, 8, 11, 12, 13
     *
     * Then, we want to put it in a vector with shape = (4, 5) (or equivalently, length=20)
     * And we want to put them in the following way
     * 0, 0, 0, 0, 0,
     * 0, 0, 0, 0, 0
     * 0, 0, *, *, *,
     * 0, 0, *, *, *,
     * So the result should be
     * 0, 0, 0,  0,  0,
     * 0, 0, 0,  0,  0,
     * 0, 0, 6,  7,  8,
     * 0, 0, 11, 12, 13
     *
    */
    #include <hippio.h>
    #include <iostream>
    #include <vector>

    using namespace std;
    using hsize_t=unsigned long long; // default type for index and count in HIPP
    int main(void)
    {
        // create a h5 file with dataset of shape (4, 5)
        HIPP::IO::H5File o_file("./test.h5", "w");
        vector<double> vec;
        for (int i = 0; i < 20; ++i)
            vec.push_back(i);
        auto ds = o_file.create_dataset<double>("data", {4, 5});
        ds.write((double *)vec.data());
        o_file = HIPP::IO::H5File(nullptr);

        // read the part of the data and put it in a small vector
        HIPP::IO::H5File i_file("./test.h5", "r");
        auto dset = i_file.open_dataset("data");
        // create the file_dataspace from the dataset in the file
        auto dspace_file = dset.dataspace();
        vector<hsize_t> offset_file{1, 1}, shape_file{2, 3};
        // create the hyperslab for the file_dataspace with offset and shape
        dspace_file.select_hyperslab(offset_file, shape_file);
        // create the memory dataspace to accept the data
        auto dspace_mem = HIPP::IO::H5Dataspace({4, 5});
        vector<hsize_t> offset_mem{2, 2}, shape_mem{2, 3};
        dspace_mem.select_hyperslab(offset_mem, shape_mem);
        // you can also 
        int vec_size = 20;
        vector<double> vec_recv(vec_size, 0);
        dset.read((double *)vec_recv.data(), dspace_mem, dspace_file);
        for (int i = 0; i < vec_size; ++i) {
            cout << vec_recv[i] << ", " << endl;
        }
        return 0;
    }

