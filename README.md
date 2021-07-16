HIPP: Modern C++ Toolkit for HPC
======================================================================

[![](https://img.shields.io/github/v/release/ChenYangyao/hipp?include_prereleases)](https://github.com/ChenYangyao/hipp/releases)
[![Documentation Status](https://readthedocs.org/projects/hipp/badge/?version=latest)](https://hipp.readthedocs.io/en/latest/?badge=latest)
[![](https://img.shields.io/github/workflow/status/ChenYangyao/hipp/CMake)](https://github.com/ChenYangyao/hipp/actions?query=workflow%3ACMake)
[![](https://img.shields.io/github/last-commit/ChenYangyao/hipp)](https://github.com/ChenYangyao/hipp/commits/master)
[![](https://img.shields.io/github/license/ChenYangyao/hipp)](https://github.com/ChenYangyao/hipp/blob/master/LICENSE)


**HIPP** is a HPC toolkit written in C++. It is still under active development. 
If you love C++ programming and scientific computation, you 
are welcome to join us. We are happy to receive any comment or suggestion from you. 

The codes are hosted on [Github](https://github.com/ChenYangyao/hipp). 
The documentation is available at [Read-the-docs](https://hipp.readthedocs.io/), which includes
how to install and get started with HIPP.

Main Features 
-----------------------

**Easy-and-fast MPI OOP Library** : for example, send/recv and collective calls are as simple as:
```c++
int a;
float b[3];
vector<double> c(32);

comm.send(dest, tag, a);
comm.isend(dest, tag, b).wait();
comm.bcast(c, root);
```

**Flexible IO Library**: for example, a structure array can be stored to a HDF5 file by:
```c++
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
```

**Optimized Algorithm Library**: a KD-Tree can be constructed by:
```c++
KDTreeif<DIM, PAD> tree(points);
```

**Flexible Numerical Library**: a fixed-length vector can be created and manipulated like:
```c++
SVec<double, 4> v {1, 2, 3, 4};
v[v>2] += 10;
double sum = v[v>2].sum();
double norm = v.norm();
```

**Other Features** include:

- View the objects by printing them, like `pout << comm, " and ", tree;`
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

- Yangyao Chen (Tsinghua, DOA, [yangyaochen.astro@foxmail.com](mailto:yangyaochen.astro@foxmail.com), [chenyangyao.com](https://www.chenyangyao.com/))
- Kai Wang (Tsinghua, DOA, [wkcosmology@gmail.com](mailto:wkcosmology@gmail.com), [kosmoswalker.com](https://www.kosmoswalker.com/))
