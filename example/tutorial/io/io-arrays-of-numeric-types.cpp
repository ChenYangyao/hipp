#include <hippio.h>
#include <hippalgorithm.h>
#include <mystd.h>
using namespace MYSTD;

void write_or_read_arrays(){
    const size_t N = 5;
    vector<double> arr1 = {0.,1.,2.,3.,4.};
    
    double *arr2 = new double [N];
    /**
     * Open a file named "arrays.h5", create a dataset named "arr1", and write
     * the vector into that dataset.
     */
    HIPP::IO::H5File file("arrays.h5", "w");                
    auto dset1 = file.create_dataset<double>("arr1", {N} );
    dset1.write(arr1);

    /**
     * You may chain the create_dataset() and write() operations in just one 
     * line of code.
     * The write method also accepts a pointer. So the writing of "arr2" is 
     * as easy as writing "arr1".
     */
    file.create_dataset<double>("arr2", {N}).write(arr2);


    /**
     * To read the data, open the dataset, and read the dataset into a vector.
     * The library resizes the vector automatically.
     */
    vector<double> arr1_in;
    file.open_dataset("arr1").read(arr1_in);

    auto dset2 = file.open_dataset("arr2");
    size_t n_elems = dset2.dataspace().size();
    double *arr2_in = new double [n_elems];
    file.open_dataset("arr2").read(arr2_in);


    /**
     * For multi-dimensional data, you simply specify the correct dimensions
     * in the calling of create_dataset(). Then, use a write() to write the
     * row-major array into the dataset.
     */
    const size_t n0=2, n1=3, n2=4;
    vector<int> arr3(n0*n1*n2);
    file.create_dataset<int>("arr3", {n0, n1, n2}).write(arr3);
    /* The read() operation has no difference from the one-dimensional dataset. */
    file.open_dataset("arr3").read(arr3);

    /**
     * Again, for a reading into raw buffer, use dataspace() to retrive the 
     * dataspace, and use size() to get the total number of elements.
     */
    auto dset3 = file.open_dataset("arr3");
    int *arr3_in_buff = new int[ dset3.dataspace().size() ];
    dset3.read(arr3_in_buff);

    delete [] arr2;
    delete [] arr2_in;
    delete [] arr3_in_buff;
}

using HIPP::IO::hsize_t;

void select_subset(){
    
    /* Write a 2D array into a HDF5 file. */
    vector<hsize_t> file_dims = {4, 5}; 
    vector<int> file_data = HIPP::ALGORITHM::LinSpaced(0, 4*5).get();
    HIPP::pout << "2Darray (file)=\n", 
        HIPP::PrtArray(file_data).ncol(5).width(3), endl;
    HIPP::IO::H5File("data.h5", "w").create_dataset<int>("2Darray", file_dims)
        .write(file_data);
    
    /* Specify the dataspace for file and memory arrays. */
    vector<hsize_t> mem_dims = {4, 6}, 
        file_start = {1, 1}, mem_start = {2, 3},
        count = {2, 3};
    HIPP::IO::H5Dataspace file_space(file_dims), mem_space(mem_dims);
    file_space.select_hyperslab(file_start, count);
    mem_space.select_hyperslab(mem_start, count);

    /* Read a subset from the HDF5 file into a subset of mem_data. */
    vector<int> mem_data(4*6);
    HIPP::IO::H5File("data.h5", "r").open_dataset("2Darray")
        .read(mem_data, mem_space, file_space);
    HIPP::pout << "2Darray (memory)=\n", 
        HIPP::PrtArray(mem_data).ncol(6).width(3), endl;
}

int main(int argc, char const *argv[]){    
    
    write_or_read_arrays();

    select_subset();

    return 0;
}
