# Basic Point-to-point Communication

MPI standard interface allows users to perform point-to-point communication by calling one of its point-to-point subroutines \(e.g., MPI\_Send\(\) or MPI\_Recv\(\), or their non-blocking counterparts\). In the following, we describe how to use HIPP MPI to achieve this by a simple example. The complete interfaces are introduced in the [User's Guide](../../users-guide.md).

## Send and Receive Messages

To perform a point-to-point communication, you also need to include the header &lt;hippmpi.h&gt; and initialize the MPI environment by declare a HIPP::MPI::Env object, just as we did in the ['Hello World' Example](./). Now if we want to send a message, e.g., a vector type containing 10 integers, from rank-0 process to the rank-1 process, then for the sending process \(rank = 0\) you may write as simple as

```cpp
#include <hippmpi.h>

HIPP::MPI::Env env;
auto world = env.world();

if( world.rank() == 0 ){
    vector<int> sendbuf(10, 100.0);  // [1] declare a vector of 10 integers.
    int dest = 1, tag = 0;
    world.send( dest, tag, sendbuf );// [2] send it to process 1, with tag=0.   
}
```

Just like in the standard MPI interface, for a send call MPI\_Send\(\) you must specify the message envelop and the message buffer. The envelop in the standard MPI interface is a triplet: &lt;dest, tag, comm&gt;, which is the destination process rank, a tag for message matching, and the communicator context. The message buffer is also a triplet: &lt;buf, size, datatype&gt;, which is the starting address, number of data items \(in the unit of datatype, not byte\), and the data item type. In the HIPP MPI, the message envelop should be explicitly specified. The call of `world.send(dest, tag, ...)`  at \[2\] in the above example is equivalent to specify the envelop triplet as &lt;dest, tag, MPI\_COMM\_WORLD&gt; in the standard MPI. The data buffer is simpler, you can just pass the vector instance into send call, which is equivalent to specify &lt;&sendbuf\[0\], sendbuf.size\(\), MPI\_INT&gt; in the standard MPI.

The receive call is also much similar to the standard MPI\_Recv. We can directly write it as

```cpp
if( world.rank() == 1 ){
    vector<int> recvbuf(10);
    int src = 0, tag = 0;
    world.recv( src, tag, recvbuf );// [3] recv the message from process 0
}
```

Note that in this case, the recvbuf must be initialized with a appropriate length, 10 in this example, so that when you pass it to the recv call, it will be convert into a suitable MPI standard call with data buffer triplet &lt;&recvbuf\[0\], recvbuf.size\(\), MPI\_INT&gt;. It is erroneous that the recv buffer size is smaller than the send buffer size, but it is correct that if larger.

{% hint style="success" %}
**Elaboration:** In the receive call, you might be interested in where does the MPI\_Status go. Actually the world.recv\(\) call returns a Status object, so that you can call its source\(\), tag\(\), error\(\) and count\(\) method to retrive the receiving status. The detail is documented in the [User's Guide](../../users-guide.md).
{% endhint %}

### More on the Arguments

As said in the [Begin with HIPP MPI Module](./),  HIPP MPI provides maximum flexibility, to allow you not only easily finishing the majority of works \(just like the example above\), but also going back to the standard MPI style that can handle the most subtle case. For the send/receive operation pair, HIPP MPI provides the following four ways of specifying the data buffer

```cpp
// hippmpi.h
class Comm{
public:
    /**
     * standard way of specifying data buffer, using data buffer triplet
     * <buff, count, dtype>, where dtype can be any pre-defined data type
     * such as CHAR, FLOAT, DOUBLE, ...
     */
    void send(int dest, int tag, 
        const void *buff, int count, Datatype dtype) const;
    /**
     * using a string to dynamically specify the datatype. Where the `dtype`
     * can be "byte", "char", "short", "int", "long", "long long", "float",
     * or "double".
     */
    void send(int dest, int tag, 
        const void *buff, int count, const string &dtype) const;
    /**
     * using a string `s` as buffer. This is equivalent to the triplet
     * <s.c_str(), s.size(), CHAR>.
     * If calling this interface, the recv/send pair must have `s` with 
     * same size.
     */
    void send(int dest, int tag, const string &s) const;
    template<typename T, typename A>
    /**
     * using a vector `v` as buffer. This is equivalent to the triplet
     * <v.data(), v.size(), _TypeCvt<T>>, where _TypeCvt converts the C native
     * type T into a HIPP MPI Datatype, e.g. when T = double, then 
     * _TypeCvt<T> = DOUBLE.
     */
    void send(int dest, int tag, const vector<T,A> &v) const;
    
    /**
     * Receive calls are similar, but returns a Status object, and all the 
     * buffers (`buff`, `s`, `v` in above send interfaces) are non-const.
     */
};
```

For example, if you want to pass a vector of 10 values, say `vector<int> sendbuf(10, 100.0)`, you can use the following four different but equivalent ways:

```cpp
world.send(dest, tag, sendbuf);
world.send(dest, tag, sendbuf.data(), sendbuf.size(), "int");
world.send(dest, tag, sendbuf.data(), sendbuf.size(), HIPP::MPI::INT);
```

We give a complete code example below to demonstrate how to make point-to-point communication.

```cpp
/**
 * Example:
 * This example demonstrates the point-to-point communication interface of
 * HIPP MPI.
 */
#include <hippmpi.h>
#include <unistd.h>
using namespace std;

int main(int argc, char const *argv[]){
    HIPP::MPI::Env env;
    auto world = env.world();

    /**
     * Rank-0 process sends a message containing 10 integers to rank-1 
     * process.
     * Then rank-1 process prints the received data.
     * The barrier ensures the output is not entangled.
     */
    if( world.rank() == 0 ){
        vector<int> sendbuf(10, 100.0);         
        HIPP::prt( cout, 
            "sent: 0 -> 1, data = 10 integers") << endl;
        world.send( 1, 0, sendbuf );              // [1.1] send with a vector
    }else if( world.rank() == 1 ){
        vector<int> recvbuf(10);
        world.recv( 0, 0, recvbuf );              // [1.2] receive a vector
        HIPP::prt( cout, "  received: 1 <- 0, data = " );
        HIPP::prt_a( cout, recvbuf ) << endl;
    }
    world.barrier();
    

    /**
     * The rank-0 process in the communication world sends a messeage of an 
     * integer to each to other processes in the same communicator. 
     * After sending/receiving, each process prints a line of information 
     * indicating the source/target of the message and its content.
     */
    if( world.rank() == 0 ){
        for(int i=1, n=world.size(); i<n; ++i){
            int sendbuf = i;
            HIPP::prt( cout, "sent: 0 -> ", i, 
                ", data = ", sendbuf) << endl;
            world.send( i, 0, &sendbuf,1,"int" ); // [2] send with triplet.
            sleep(1);
        }
    }else{
        int recvbuf;
        auto status = world.recv( 0, 0, 
            &recvbuf, 1, HIPP::MPI::INT );        // [3] receive with triplet.
        HIPP::prt( cout, "  received: ", status.source(), 
            " <- 0, data = ", recvbuf ) << endl;
    }
    world.barrier();

    /**
     * In this case, rank-0 process sends a string to rank-1 process.
     * Note that the sending string size must match the receiving string size.
     */
    if( world.rank() == 0 ){
        string sendbuf = "hello rank 1";
        HIPP::prt( cout, 
            "sent: 0 -> 1, data = ", sendbuf) << endl;
        world.send( 1, 0, sendbuf );             // [4.1] send a string.
    }else if( world.rank() == 1 ){
        string recvbuf( 12, '\0' );
        world.recv( 0, 0, recvbuf );             // [4.2] receive a string.
        HIPP::prt( cout, 
            "  received: 1 <- 0, data = ", recvbuf ) << endl;
    }

    return 0;
}
```

At \[1.1\] and \[1.2\], process 0 sends a vector of 10 intergers to process 1, using the data buffer specification with only a vector type. At \[2\], process 0 sends a integer to each other process with triplet specification &lt;buff, count, dtype&gt; where dtype is a string specifiction. At \[3\] each other process receive  the integer also using the triplet &lt;buff, count, dtype&gt; where dtype is a Datatype object, which is the standard-MPI-way. At \[4.1\] and \[4.2\] process 0 sends a string to process 1. The barrier operation `world.barrier()` synchronize all the processes in the communication world, so that the message output will not be entangled.

The output on the screen \(run with 4 proesses\) is

```text
sent: 0 -> 1, data = 10 integers
  received: 1 <- 0, data = 100,100,100,100,100,100,100,100,100,100
sent: 0 -> 1, data = 1
  received: 0 <- 0, data = 1
sent: 0 -> 2, data = 2
  received: 0 <- 0, data = 2
sent: 0 -> 3, data = 3
  received: 0 <- 0, data = 3
sent: 0 -> 1, data = hello rank 1
  received: 1 <- 0, data = hello rank 1
```

{% hint style="success" %}
**Elaboration:** MPI standard specifies 4 types of point-to-point communication mode - the standard mode, the buffer mode, the synchronous mode, and the ready mode. Each mode has blocking and non-blocking send. All together we have 8 sends and 2 receives: send/isend, bsend/ibsend, ssend/issend, rsend/irsend, recv/irecv \(All defined in HIPP::MPI::Comm object\). Four all these calls we provide four different ways of specifying the data buffer. The detail is documented in the [User's Guide](../../users-guide.md).
{% endhint %}



