.. HIPParcos documentation master file, created by
   sphinx-quickstart on Wed Oct 14 04:53:30 2020.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

HIPP: Modern C++ Toolkit for HPC
======================================================================

|badge1| |badge2| |badge3| |badge4| |badge5| |badge6|

.. |badge1| image:: https://img.shields.io/github/v/release/ChenYangyao/hipp?include_prereleases
   :target: https://github.com/ChenYangyao/hipp/releases
   :alt: GitHub release (latest by date including pre-releases)

.. |badge2| image:: https://readthedocs.org/projects/hipp/badge/?version=latest
   :target: https://hipp.readthedocs.io/en/latest/?badge=latest
   :alt: Documentation Status

.. |badge3| image:: https://img.shields.io/github/actions/workflow/status/ChenYangyao/HIPP/cmake.yml
   :target: https://github.com/ChenYangyao/hipp/actions/workflows/cmake.yml
   :alt: GitHub Workflow Status

.. |badge4| image:: https://img.shields.io/github/last-commit/ChenYangyao/HIPP/master
   :target: https://github.com/ChenYangyao/hipp
   :alt: GitHub last commit (branch)

.. |badge5| image:: https://img.shields.io/badge/License-GPLv3-blue.svg
   :target: https://github.com/ChenYangyao/hipp/blob/master/LICENSE
   :alt: License: GPL v3

.. |badge6| image:: https://img.shields.io/badge/ascl-2301.030-blue.svg?colorB=262255
   :target: https://ascl.net/2301.030
   :alt: ascl:2301.030


**HIPP** is a HPC toolkit written in C++. It is still under active development. 
If you love C++ programming and scientific computation, you 
are welcome to join us. We are happy to receive any comment or suggestion from you. 

The codes are hosted 
on `Github <https://github.com/ChenYangyao/hipp>`_. 
The documentation is available at `Read-the-docs <https://hipp.readthedocs.io/>`_.

Main Features 
-----------------------

**Easy-and-fast MPI OOP Library** : for example, send/recv and collective calls are as simple as::

   int a;
   float b[3];
   vector<double> c(32);

   comm.send(dest, tag, a);
   comm.isend(dest, tag, b).wait();
   comm.bcast(c, root);

**Flexible IO Library**: For example, scalar or array-like data can be written 
into HDF5 format by a single call of ``put``::

   short s;
   vector<double> d10(10);
   int i5[5];
   long l34[3][4];
   array<float, 3> f3;
   vector<array<int, 3> > i83(8);

   H5::DatasetManager dsets = H5::File("file.h5", "w").datasets();
   
   dsets.put("s",      s);
   dsets.put("d10",    d10);
   dsets.put("i5",     i5);
   dsets.put("l34",    l34);
   dsets.put("f3",     f3);
   dsets.put("i83",    i83);

Array of more complex structured type can also be written by a single call::

   struct S {
        int a;
        double b[3];
        float c[2][3];
        array<array<long, 3>, 4> d;
    } data[10];
    
    HIPPIO_H5_XTABLE(S, a, b, c, d).write(data, "file.h5");

**Optimized Algorithm Library**: a KD-Tree can be constructed by::

   KDTreeif<DIM, PAD> tree(points);

**Flexible Numerical Library**: a fixed-length vector can be created and manipulated like::

   SVec<double, 4> v {1, 2, 3, 4};
   v[v>2] += 10;
   double sum = v[v>2].sum();
   double norm = v.norm();

**Other Features** include:

- View the objects by printing them, like ``pout << comm, " and ", tree;``
- Rigorly tested - Unit Tests are made for library components.
- Full stack of abstraction layers - allowing quick-use with high-level API, and controlling library detail with low-level API.

How the Documentation is Organized
------------------------------------

A high-level overview of how it is organized helps you know where to look for certain things:

- :doc:`Installation <usage/get-start/installation>` details how to get HIPP on your environment.
- :doc:`Quick Start <usage/get-start/quickstart>` gives you a first glance at how the library is organized and used.
- The **Tutorial** of each module 
  :doc:`CNTL <usage/tutorial/cntl/index>`,
  :doc:`MPI <usage/tutorial/mpi/index>`,
  :doc:`IO <usage/tutorial/io/index>`,
  :doc:`NUMERICAL <usage/tutorial/numerical/index>`,
  :doc:`ALGORITHM <usage/tutorial/algorithm/index>`,
  :doc:`SIMD <usage/tutorial/simd/index>`
  takes you by hand through a series of steps to work out specific tasks.
- The **API-Ref** of each module 
  :doc:`CNTL <usage/api-ref/cntl/index>`,
  :doc:`MPI <usage/api-ref/mpi/index>`,
  :doc:`IO <usage/api-ref/io/index>`,
  :doc:`NUMERICAL <usage/api-ref/numerical/index>`,
  :doc:`ALGORITHM <usage/api-ref/algorithm/index>`,
  :doc:`SIMD <usage/api-ref/simd/index>`
  lists details of the API, including how to use it and more technical issues.


Motivation of HIPP
-----------------------

There are many well-developed libraries for HPC written in C language. However, modern C++ provides
elegant features to make these function calls more beautiful and more easy-to-use without loss of 
performance. 

**HIPP** (HIgh-Performance Package for scientific computation) is designed to provide such elegant 
interfaces for some well-known HPC libraries. Some libraries
are wrapped with full-OOP interfaces, and many new extensions based on those raw-interfaces are also provided.
With HIPP, the length of your code can be significantly reduced, making your programming life more productive.

Contributors
----------------

- Yangyao Chen (Tsinghua, DOA, `yangyaochen.astro@foxmail.com <mailto:yangyaochen.astro@foxmail.com>`_, `chenyangyao.com <https://www.chenyangyao.com/>`_)
- Kai Wang (Tsinghua, DOA, `wkcosmology@gmail.com <mailto:wkcosmology@gmail.com>`_, `kosmoswalker.com <https://www.kosmoswalker.com/>`_)


.. toctree::
   :maxdepth: 1
   :caption: Getting Started
   :hidden:

   usage/get-start/installation
   usage/get-start/quickstart

.. toctree::
   :maxdepth: 1
   :caption: Tutorial
   :hidden:

   usage/tutorial/cntl/index
   usage/tutorial/mpi/index
   usage/tutorial/io/index
   usage/tutorial/numerical/index
   usage/tutorial/algorithm/index
   usage/tutorial/simd/index

.. toctree::
   :maxdepth: 1
   :caption: API References
   :hidden:

   usage/api-ref/cntl/index
   usage/api-ref/mpi/index
   usage/api-ref/io/index
   usage/api-ref/numerical/index
   usage/api-ref/algorithm/index
   usage/api-ref/simd/index
