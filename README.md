---
description: Under Development
---

# HIPParcos - A C++ HIgh Performance scientific computation Package

HIPParcos (HIPP) is a C++ pacakge for scientific computation. It provides high-level OOP APIs for basic libraries, such as MPI, HDF5, SIMD, GSL. Enhanced features developed bsed on the basic libraries are also provided, such as the master-slave models and locks which are not standard part of MPI but very useful in some cases.

## What can you find in HIPParcos?

### MPI Interface
OOP interface for the Message Passing Interface (MPI) with enhanced tools (e.g., slave-master model, mutex). Examples include

`MPI environment initializtion`: with OOP feature 'RAII' (resource Acquisition Is Initialization), initialize
MPI environment by define an 'Environment' object. No need for calling 'MPI_Finalize' because it is automatically called
after env is destructed.
```c++
int main(int argc, char *argv[]){
    HIPP::MPI::Env env(argc, argv);    // initialize MPI
    auto comm = env.world();           // communication world object
    // ... use MPI
    return 0;                          // MPI automatically finalized
}
```

`Object printing`: it is easy to print the status of a MPI object - very useful
for bug diagnosis.
```c++
auto win = comm.win_create_dynamic();  // create a RMA window
if( comm.rank() == 0 )
    cout << win;
```
Outputs give detail infomation of the window object we have created.
```txt
HIPP::MPI::Win instance [loc=0x7ffe4ce9cfa0, size=16, align=8]
----------
  Process group
    HIPP::MPI::Group instance [0x7ffe4ce9ced0] size: 4, rank: 0
  Memory
    base:          0
    size:          0
    disp_unit:     1
    model:         UNIFIED
    create flavor: DYNAMIC
```

`Process management`: new commnicators can be construted.
```c++
auto new_comm = comm.split( comm.rank()%2 );// split the communicator into two
new_comm.info(cout, 0) << endl;             // print status of new_comm
```
The output is like
```
HIPP::MPI::Comm instance [0x7ffdafeb0430] size: 2, rank: 0, topology: undefined
HIPP::MPI::Comm instance [0x7ffd2f994160] size: 2, rank: 1, topology: undefined
HIPP::MPI::Comm instance [0x7ffe00765300] size: 2, rank: 0, topology: undefined
HIPP::MPI::Comm instance [0x7ffc4e01b6f0] size: 2, rank: 1, topology: undefined
```

`Point-to-point communication`: a pair of processes send messages to each other - 
use blocking send and non-blocking receive.
```C++
int rank = comm.rank();
if( rank < 2 ){
    vector<double> inbuf(5), outbuf(5, rank);
    int dest = rank ? 0: 1;
    auto req = comm.irecv(dest, 0, inbuf);
    comm.send(dest, 0, outbuf);
    req.wait();
    cout << "Rank: " << rank << ", Received: "; 
    HIPP::prt_a(cout, inbuf) << endl;  
}
```
Output is like
```txt
Rank: 0, Received: 1,1,1,1,1
Rank: 1, Received: 0,0,0,0,0
```

`Collective communication`: all processes are involved in the SUM reduction
```c++
int rank = comm.rank();
vector<double> outbuf(5, rank), inbuf(5);
comm.reduce(outbuf.data(), inbuf.data(), 5,
    HIPP::MPI::DOUBLE, "sum", 0);

if( rank == 0 ){
    HIPP::prt_a(cout, inbuf) << endl;
}
```
Output is like (run with 4 processes)
```txt
6,6,6,6,6
```

`Remote memory access`: cyclic PUT, i.e., each process PUTs an array of numbers to its 'next' neighbor.
Synchronization is made by starting the 'fence'. No need for seconding call of fence because of RAII.
It easy to enforce a sequential output by defining a SeqBlock object.

`Sequential scheduling`: sometimes the tasks need to be finished sequentially, one by one, in different
processes. SeqBlock interface make this as easy as a single call.
```c++
constexpr int N = 5;
vector<double> local_arr(N, comm.rank());            // local buffer
auto [win, recv_arr] = comm.win_allocate<double>(N); // RMA window object and buffer

{   int dest = (comm.rank()+1) % comm.size();        // rank of next neighbor
    auto guard = win.fence_g();
    win.put(dest, local_arr, 0);  }                  // put local buffer out
    
{   HIPP::MPI::SeqBlock seq(comm);                   // sequential printing
    cout << "Rank: " << comm.rank() << ", Local Data: ";
    HIPP::prt_a(cout, local_arr) << ", Received Data: ";
    HIPP::prt_a(cout, recv_arr, recv_arr+N) << endl;  }
```
The output is guaranteed ordered:
```text
Rank: 0, Local Data: 0,0,0,0,0, Received Data: 4,4,4,4,4
Rank: 1, Local Data: 1,1,1,1,1, Received Data: 0,0,0,0,0
Rank: 2, Local Data: 2,2,2,2,2, Received Data: 1,1,1,1,1
Rank: 3, Local Data: 3,3,3,3,3, Received Data: 2,2,2,2,2
Rank: 4, Local Data: 4,4,4,4,4, Received Data: 3,3,3,3,3
```
`Master-slave scheduling`: a counter class that supports task distribution. Each process calls get() method to fetch the value of the counter and add 1 to it.
`Synchronization`: the spin lock and mutex classes give simple ways of constructing critical region.
```c++
HIPP::MPI::CounterS<> cnt(comm);            // start a counter
int max_val = 16;
vector<int> vals_got;
while(true){
    if(int val = cnt.get(); val < max_val ) // each process fetch its values
        vals_got.push_back(val);
    else break;
}

HIPP::MPI::SpinLock lk(comm);               // initialize a spin lock
lk.lock();                                  // start of the critical region
cout << "Rank: " << comm.rank() << ", Vals got: ";
HIPP::prt_a(cout, vals_got) << endl;
lk.unlock();                                // end of the critical region
```
The vals_got in each process is printed as (output is guaranteed not entangled):
```txt
Rank: 0, Vals got: 0,1,2,3,5,7,9,10,11,13,14,15
Rank: 2, Vals got: 4
Rank: 3, Vals got: 6,12
Rank: 1, Vals got: 8
```

## Install

### Requirements

* GNU compatible C++ compiler \(standard &gt;= 11\)
* CMake - the compiling system generator \(version &gt;= 3.9\)

The followings are optional:

* MPI implementation \(headers, libraries, compiler wrappers, MPI-luanchers; must with standard &gt;= 3.0\)
* HDF5 implementation \(headers, libraries; version &gt;=1.8.0\)

License: HIPP is distributed under GPL version 3. See the LICENSE file for the detail license terms.

### Quick Install

Clone or download this repository, then enter the root directory of this repository and execute

```bash
cmake ./ -Dprefix=./
make
make install
```

These configure, build and install HIPP in the current directory. If successful, two folders named `lib`, `include` appear, which include the libraries and header files, respectively. You can now modify the environment lists \(e.g., `LIBRARY_PATH`, `LD_LIBRARY_PATH` and `CPLUS_INCLUDE_PATH`\) to allow the compiler system searching the installed package, e.g.,

```bash
export CPLUS_INCLUDE_PATH="/path/to/repo/include:${CPLUS_INCLUDE_PATH}"
export LIBRARY_PATH="/path/to/repo/lib:${LIBRARY_PATH}"
export LD_LIBRARY_PATH="/path/to/repo/lib:${LD_LIBRARY_PATH}"
```

where you should replace the "/path/to/repo" with the directory of this repositoty. You may add these lines into you  shell init scripts, such as `${HOME}/.bashrc` or `${HOME}/.bash_profile`, then it will set the environment when you login the shell.

### Test the Installed Library

Once you successfully install the HIPP, you can enter the `example/cntl` directory in the repository.  Try

```bash
make print.out; ./print.out
```

to compile and run the example. If error, you may specify the compiling command by you self, such as `g++ -o print.out print.cpp` and run the executive by `./print.out`. If still error, you may check that if the lib and include environment is set in the install step. If successful, on the output device it shows

```text
Sum of 1 and 2 and 3 is 6
Up to precision 10, a is 1.0000000000
The array is 1,2,3,4,5
```

which means the install is successful. Enjoy using HIPP!

### Building and Installation Options

To change the build direcoty, first create a new directory, enter it, and invokde cmake with `/path/to/repository`.  This will use the repository '/path/to/repository' to configure the build system in the current directory. Then you may use `make; make install` to finish the build and installation. 

To change installation directory, use `-Dprefix`.  By default, if you do not specify `prefix`,  HIPP will installed into the system directory, e.g. /usr/local. 

To speed up the building with multiple processors, use `make -j num_of_processes`.

```bash
cmake /path/to/repository -Dprefix=/path/to/install
make -j N
make install
```

The above commands configure and build HIPP in the current directory, use the source code in the /path/to/repository, with N processes, and install HIPP into the path '/path/to/install'. 

The -D option is additive. You may specify multiple -D options.

HIPP provides some optional functions. If your system has the depencency it needs, You may install them  by specifying the options when running the cmake command.

| Options | Dependency | Decription |
| :--- | :--- | :--- |
| -Denable-mpi=ON | MPI \(&gt;=3.0\) | The HIPP MPI Module |
| -Denable-hdf5=ON | HDF5 \(&gt;=1.8.0\) | The HIPP IO Module |
| -Denable-simd=ON | Intel x86 architecture | The HIPP SIMD Module |
| -Dcxx=any\_compiler | Any C++ compiler \(&gt;=11\) | Use other C++ compiler instead  of the default |

## Documentation

The documentation of HIPP is organized into three layers, with increasing depth.  

* The [Tutorial](documentation/tutorial/) provides a basic guide of how to use HIPP to perform basic enhancement of C++ language, perform parallel computation with OOP style MPI interface,  or doing I/O with the IO interface. 
* The [User's Guide](documentation/users-guide.md) gives a more deep description about the design, the structure and implementation detail of HIPP.
* The [Interfaces by Modules](documentation/interfaces-by-modules/) provides a introduction of HIPP categorized by Modules and Submodules.

The documentation now available at [Gitbook](https://hipp.gitbook.io/hipparcos/).



