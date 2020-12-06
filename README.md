HIPP: Modern C++ Toolkit for HPC
======================================================================

![](https://img.shields.io/github/v/tag/ChenYangyao/hipp)
![](https://img.shields.io/github/license/ChenYangyao/hipp)
![](https://img.shields.io/github/last-commit/ChenYangyao/hipp)

**HIPP** is still under active development. If you love C++ programming and scientific computation, you 
are welcome to join us. We are happy to receive any comment or suggestion from you.

Installation and Documentation
-------------------------------------

The **Installation** manual, a **Quick-Start Guide**, the **Tutorial**, and the **Full API References** can 
be found at [Read the Docs](<https://hipp.readthedocs.io/en/latest/>).

About HIPP 
-----------------------
There are many well-developed libraries for HPC written in C language. However, modern C++ provides
elegant features to make these function calls more beautiful and more easy-to-use without loss of 
performance. 

**HIPP** (HIgh-Performance Package for scientific computation) is designed to provide such elegant 
interfaces for some well-known HPC libraries. Some libraries
are wrapped with full-OOP interfaces, and many new extensions based on those raw-interfaces are also provided.
With HIPP, the length of your code can be significantly reduced, making your programming life more productive.

Here are some good examples of why to choose HIPP.

-   For **I/O**, you possibly know the [HDF5](https://www.hdfgroup.org/solutions/hdf5/) library. Read/Write a single array 
    could be as lengthy as the  [example here](https://raw.githubusercontent.com/HDFGroup/hdf5/develop/examples/h5_rdwt.c) .
    With HIPP, you just need one line
    ```c++
    H5File("file.h5", "w").create_dataset<int>("array", {4,6}).write(&dset_data[0][0]);
    ```
      
-   When using the **MPI** ([Massage Passing Interface](https://www.mpi-forum.org/)) for parallelism, 
    you may need different 
    communicators to deal with different tasks. Creation of a communicator in MPI would be lengthy if the 
    communicating pattern is complex. With HIPP, things would be as easy as one line
    ```c++
    Comm new_comm = comm.create(comm.group().incl({0,1,2}));
    ```

-   When processing **spatial data** (e.g., particles in N-body simulation, galaxies from observation), 
    spatial searching algorithm would be necessary for computing local statistical quantities. With 
    HIPP, you can build a tree such as KDTree by using just one line
    ```c++
    KDTreeif<3, 4> tree(points);
    ```

    Note that the tree is in 3-dimensional space with 4 bytes padded to hold any associated data for points. Such 
    a design is extremely efficient and flexible.

-   HIPP is devoted to free you from the painful **BUGs**. With full-OOP interface, you do not have to 
    deal with the bitter resource managements by your own. With a unified exception model, you can 
    choose to ignore, catch or fix any exception thrown in the calling of libraries. With user-friendly
    priting system, you can monitor the status of any HIPP object by just priting it, e.g., for the 
    two objects defined above
    ```c++
    cout << new_comm;
    cout << tree;
    ```

-   HIPP is **rigorly tested**. We are trying using [Google Test](https://github.com/google/googletest) to validate
    all of our library components. Some of the modules yet cannot fit into the Google Test framework. If you have any 
    idea please help us. 

-   HIPP has a stack of **abstraction layers**. By using HIPP, you can finish almost all tasks that can be 
    achieved by the underlying libraries. However, sometimes you still want to go back to the underlying
    libraries, possibly because the extreme demanding of performance or flexibility. To enable this, 
    HIPP uses abstraction layers, one on the top of another one, to allow the access of the underlying 
    libraries directly in your programming.

Contributors
----------------

- Yangyao Chen (Tsinghua, DOA, [yangyaochen.astro@foxmail.com](mailto:yangyaochen.astro@foxmail.com))
- Kai Wang (Tsinghua, DOA, [wkcosmology@gmail.com](mailto:wkcosmology@gmail.com))
