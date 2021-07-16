.. HIPParcos documentation master file, created by
   sphinx-quickstart on Wed Oct 14 04:53:30 2020.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

HIPP: Modern C++ Toolkit for HPC
======================================================================

|badge1| |badge2| |badge3| |badge4| |badge5|

.. |badge1| image:: https://img.shields.io/github/v/release/ChenYangyao/hipp?include_prereleases
   :target: https://github.com/ChenYangyao/hipp/releases
   :alt: Release 

.. |badge2| image:: https://readthedocs.org/projects/hipp/badge/?version=latest
   :target: https://hipp.readthedocs.io/en/latest/?badge=latest
   :alt: Documentation Status

.. |badge3| image:: https://img.shields.io/github/workflow/status/ChenYangyao/hipp/CMake
   :target: https://github.com/ChenYangyao/hipp/actions?query=workflow%3ACMake
   :alt: Build CMake

.. |badge4| image:: https://img.shields.io/github/last-commit/ChenYangyao/hipp
   :target: https://github.com/ChenYangyao/hipp/commits/master
   :alt: Commits 

.. |badge5| image:: https://img.shields.io/github/license/ChenYangyao/hipp
   :target: https://github.com/ChenYangyao/hipp/blob/master/LICENSE
   :alt: License


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

**Flexible IO Library**: for example, a structure array can be stored to a HDF5 file by::

   struct Particle {
      long id;
      double pos[3]; 
      float tidal[3][3]; 
      std::array<int, 3> links;
   };
   vector<Particle> particles;

   H5XTable<Particle>( 
      "id", &Particle::id,        "pos", &Particle::pos, 
      "tidal", &Particle::tidal,  "links", &Particle::links
   ).write(particles, "file.h5");

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
   usage/tutorial/simd/index
   usage/tutorial/mpi/index
   usage/tutorial/io/index

.. toctree::
   :maxdepth: 1
   :caption: Getting Started
   :hidden:

   usage/api-ref/cntl/index
   usage/api-ref/mpi/index
   usage/api-ref/io/index
   usage/api-ref/simd/index
   usage/api-ref/algorithm/index
