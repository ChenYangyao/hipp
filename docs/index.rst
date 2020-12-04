.. HIPParcos documentation master file, created by
   sphinx-quickstart on Wed Oct 14 04:53:30 2020.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

HIPP: Modern C++ Toolkit for HPC
======================================================================

|badge1| |badge2| |badge3|

.. |badge1| image:: https://img.shields.io/github/v/tag/ChenYangyao/hipp
.. |badge2| image:: https://img.shields.io/github/license/ChenYangyao/hipp
.. |badge3| image:: https://img.shields.io/github/last-commit/ChenYangyao/hipp

**HIPP** is still under active development. If you love C++ programming and scientific computation, you 
are welcome to join us. We are happy to receive any comment or suggestion from you. The codes are hosted 
on `Github <https://github.com/ChenYangyao/hipp>`_.

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

(1)   For **I/O**, you possibly know the `HDF5 <https://www.hdfgroup.org/solutions/hdf5/>`_ library. Read/Write a single array could be as lengthy as the example 
      `here <https://raw.githubusercontent.com/HDFGroup/hdf5/develop/examples/h5_rdwt.c>`_ .
      With HIPP, you just need one line::
      
         H5File("file.h5", "w").create_dataset<int>("array", {4,6}).write(&dset_data[0][0]);

(2)   When using the **MPI** (`Massage Passing Interface <https://www.mpi-forum.org/>`_) for parallelism, 
      you may need different 
      communicators to deal with different tasks. Creation of a communicator in MPI would be lengthy if the 
      communicating pattern is complex. With HIPP, things would be as easy as one line::

          Comm new_comm = comm.create(comm.group().incl({0,1,2}));

(3)   When processing **spatial data** (e.g., particles in N-body simulation, galaxies from observation), 
      spatial searching algorithm would be necessary for computing local statistical quantities. With 
      HIPP, you can build a tree such as KDTree by using just one line::

         KDTreeif<3, 4> tree(points);

      Note that the tree is in 3-dimensional space with 4 bytes padded to hold any associated data for points. Such 
      a design is extremely efficient and flexible.

(4)   HIPP is devoted to free you from the painful **BUGs**. With full-OOP interface, you do not have to 
      deal with the bitter resource managements by your own. With a unified exception model, you can 
      choose to ignore, catch or fix any exception thrown in the calling of libraries. With user-friendly
      priting system, you can monitor the status of any HIPP object by just priting it, e.g., for the 
      two objects defined above::

         cout << new_comm;
         cout << tree;

(5)   HIPP is **rigorly tested**. We are trying using `Google Test <https://github.com/google/googletest>`_ to validate
      all of our library components. Some of the modules yet cannot fit into the Google Test framework. If you have any 
      idea please help us. 

(6)   HIPP has a stack of **abstraction layers**. By using HIPP, you can finish almost all tasks that can be 
      achieved by the underlying libraries. However, sometimes you still want to go back to the underlying
      libraries, possibly because the extreme demanding of performance or flexibility. To enable this, 
      HIPP uses abstraction layers, one on the top of another one, to allow the access of the underlying 
      libraries directly in your programming.

Contributors
----------------

- Yangyao Chen (Tsinghua, DOA, `yangyaochen.astro@foxmail.com <mailto:yangyaochen.astro@foxmail.com>`_)
- Kai Wang (Tsinghua, DOA, `wkcosmology@gmail.com <mailto:wkcosmology@gmail.com>`_)


.. toctree::
   :maxdepth: 1
   :caption: Get Started

   usage/get-start/installation
   usage/get-start/quickstart

.. toctree::
   :maxdepth: 1
   :caption: Tutorial

   usage/tutorial/cntl/index
   usage/tutorial/simd/index
   usage/tutorial/mpi/index
   usage/tutorial/io/index

.. toctree::
   :maxdepth: 1
   :caption: API Reference

   usage/api-ref/cntl/index
   usage/api-ref/mpi/index
   usage/api-ref/io/index
   usage/api-ref/simd/index

.. role:: bash(code)
   :language: bash