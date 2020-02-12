# Begin with HIPP MPI Module

## Hello World Example

{% hint style="info" %}
In the following we assume you have already known the basic ideas of MPI, such as the initialization of MPI environment, the basic point-to-point communication. If you a new to MPI, this tutorial is not suitable for you. We recommend you to learn at least the basic ideas of how to use MPI. Some tutorial material can be found at the [MPI Tutorial](https://mpitutorial.com/tutorials/).

We also assume that you have already install successfully the HIPP with MPI library enabled. If  not, please see the the [home page](../../../) for how to install HIPP with MPI option.
{% endhint %}

We begin with the 'hello world' example as a demonstration of how to use HIPP MPI \(the source code can be found at the example/mpi directory in the repo\).

```bash
#include <hippmpi.h>
using namespace std;

int main(int argc, char *argv[]){
    HIPP::MPI::Env env;          // [1] init the MPI environment
    auto comm = env.world();     // [2] obtain the communicator 

    // [3] print environment and commnicator information
    HIPP::prt(cout, "Hello world from processor ", env.processor_name(),
        ", rank ", comm.rank(), 
        " out of ", comm.size(), " processors") << endl;
    return 0;
}
```

First to notice is the header file `<hippmpi.h>`, which is the only header file that needs to be included in all HIPP MPI interfaces. Some standard library of C++ is also automatically included once &lt;hippmpi.h&gt; is included, so that you can do using the `std` namespace \(altough this is always not recommended, we will expose the standard namespace throughout this tutorial for clarity\).

The second thing is the namespace `HIPP::MPI` where all HIPP MPI class, subroutines and static variable definitions live, such as the Env class you see here. The outer namespace `HIPP` is all HIPP CNTL definitions live \(see the tutorial of the [CNTL module](../begin-with-hipp-cntl-module.md)\), such as the prt\( \) function here.

Almost at the beginning of every standard MPI program, MPI\_Init\(\) should be called. In the HIPP MPI, the only thing you need to do is to declare an environment variable of class HIPP::MPI::Env at \[1\]. In the construction of the `Env` object, the initialization of MPI is finished, just as you call a MPI\_Init\( \). It is also possible to pass arguments into the Env object like what you pass to MPI\_Init\(\), then the Env should be constructed with `HIPP::MPI::Env env(argc, argv)`.

Next you need a communicator which defines the context of the message passing. In the standard MPI you have MPI\_COMM\_WORLD, but here in the HIPP MPI you can get the predefined 'world' communicator by `env.world()` method at \[2\], which is of class HIPP::MPI::Comm and we use 'auto' key word to let the compiler infer its type automatically.

In the next step \[3\], you can inquire the information of the MPI environment and communicator. For example, you may use `env.processor_name()` to get your processor name, `comm.rank()` and `comm.size()` to get the rank of process in the communicator and the size of process group of the communicator. The `HIPP::prt()` function is just a printing function, accepting an stream to print to and any number of arguments to be printed.

Now you can compile and run the program. For example at an Unix-like shell, you call mpicxx -o hello\_world hello\_world.cpp -lhippmpi -lhippcntl; mpirun -n 4 ./hello\_world. These compile and run you program with 4 processors. In the output device it may print

```text
Hello world from processor local-linux, rank 0 out of 4 processors
Hello world from processor local-linux, rank 1 out of 4 processors
Hello world from processor local-linux, rank 2 out of 4 processors
Hello world from processor local-linux, rank 3 out of 4 processors
```

The exact result depends on which platform you run, and the result output from different processes may entangled.

Now you can equip you program with HIPP MPI. As you see, the OOP interface is really simple. What's more, it is the base if you want to want a safe C++ program. Now you can move to [Basic Point-to-point Communication](basic-point-to-point-communication.md) to see how the pass the message.

