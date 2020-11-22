I/O the Arrays of Numeric Types
===============================================================

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
and load the data into a vector by ``read(vector_name)``. The library automatically resize 
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

.. code-block::
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
Using ``h5dump`` in the command line prompt, you can view and verify the content of 
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
        vector<ArrayType> vector_of_structs.

    However, in such cases you need to take the pointer to the underlying data and 
    cast it into a proper numeric type::

        dataset.write((double *)&vector_of_arrays[0]);
        dataset.write((float *)&vector_of_structs[0]);
