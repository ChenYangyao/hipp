HIPP: Modern C++ Toolkit for HPC
======================================================================

[![GitHub release (latest by date including pre-releases)](https://img.shields.io/github/v/release/ChenYangyao/hipp?include_prereleases)](https://github.com/ChenYangyao/hipp/releases)
[![Documentation Status](https://readthedocs.org/projects/hipp/badge/?version=latest)](https://hipp.readthedocs.io/en/latest/?badge=latest)
[![GitHub Workflow Status](https://img.shields.io/github/actions/workflow/status/ChenYangyao/HIPP/cmake.yml)](https://github.com/ChenYangyao/hipp/actions/workflows/cmake.yml)
[![GitHub last commit (branch)](https://img.shields.io/github/last-commit/ChenYangyao/HIPP/master)](https://github.com/ChenYangyao/hipp)
[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://github.com/ChenYangyao/hipp/blob/master/LICENSE)
[![ascl:2301.030](https://img.shields.io/badge/ascl-2301.030-blue.svg?colorB=262255)](https://ascl.net/2301.030)


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

**Flexible IO Library**: For example, scalar or array-like data can be written 
into HDF5 format by a single call of `put`:
```c++
short s;
vector<double> d10(10);
int i5[5];
long l34[3][4];
array<float, 3> f3;
vector<array<int, 3> > i83(8);

H5::DatasetManager dsets = H5::File("file1.h5", "w").datasets();

dsets.put("s",      s);
dsets.put("d10",    d10);
dsets.put("i5",     i5);
dsets.put("l34",    l34);
dsets.put("f3",     f3);
dsets.put("i83",    i83);
```

Array of more complex structured type can also be written by a single call:
```c++
struct S {
    int a;
    double b[3];
    float c[2][3];
    array<array<long, 3>, 4> d;
} data[10];

HIPPIO_H5_XTABLE(S, a, b, c, d).write(data, "file.h5");
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
