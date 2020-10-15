Read/write array of built-in types
===============================================================

Write/read 1-d array of data to a file
--------------------------------------------------------------

The basic requirement will be writing and reading 1-d uniform array with built-in datatype from a std::vector<T>.::

    std::string filename = "***.h5"; // This is the target file
    std::vector<double> v(521);

    // do something and store the value in v;

    HIPP::IO::H5File o_h5(filename, "w"); // The "w" flag means create file, truncate if exists.
    o_h5.create_dataset<double>("data_name", {v.size()}).write(v);

For data not in a vector, as long as it has a continuous memory layout, the writing process is still convenient. For
example, we store the data in a dynamically allocated array.::

    auto data_ptr = std::make_shared<int>(521);
    o_h5.create_dataset<int>("another_data", {521}).write(data_ptr.get());

The reading to a vector is especially easy since HIPP::IO will automatically resize the vector according to the
data space property.::

    std::string filename = "***.h5"; // This is the target file
    std::vector<double> v;
    HIPP::IO::H5File i_h5(filename, "r"); // The "r" flag means read-only
    i_h5.open_dataset("data_name").read(v); // The vector will be automatically resized.

For non-vector, we need to manualy allocate the appropriate memory. For example::

    auto data_ds = i_h5.open_dataset("another_data"); // read the dataset
    auto data_ptr = std::make_shared<int>(data_ds.dataspace().dims()[0]); // here we assume it is 1-d array
    data_ds.read(data_ptr.get());

.. warning::
   The data to be writen must be in a continuous memory layout.

Write/read 2-d array of data to a file
--------------------------------------------------------------

The STL does not specifically support multi-dimensional array, but we can use the 1-d array to store the data and
convert it to multi-dimension in the writing::
    
    std::size_t dim0 = 10;
    std::size_t dim1 = 12;
    std::size_t dim2 = 5;
    std::vector<double> vec(dim0 * dim1 * dim2);

    // write
    o_h5.create_dataset<double>("nd_data", {dim0, dim1, dim2}).write(vec.data());
    // read
    std::vector<double> vec_new;
    auto pos_ds = i_h5.open_dataset("nd_data");
    std::size_t size = 1;
    for(int i = 0; i < pos_ds.dataspace().ndims(); ++i)
        size *= pos_ds.dataspace().dims()[i];
    vec_new.resize(size);
    pos_ds.read(vec_new.data());

.. warning::
   vector<vector<double>> does not have continuous memory layout. Do not use it to write or accept data.
