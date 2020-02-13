# Basic Point-to-point Communication

MPI standard interface allows users to perform point-to-point communication by calling one of its point-to-point subroutines. In the following, we describe how to use HIPP MPI to achieve this by a simple example. The complete interfaces are introduced in the [User's Guide](../../users-guide.md).

To perform a point-to-point communication, you also need to include the header &lt;hippmpi.h&gt; and initialize the MPI environment by declare a HIPP::MPI::Env object, just as we did in the ['Hello World' Example](./). Now if we want to send a message, e.g., a vector type containing 10 integers, from rank-0 process to the rank-1 process, then for the sending process \(rank = 0\) you may write as simple as

```cpp
HIPP::MPI::Env env;
auto world = env.world();
if( world.rank() == 0 ){
    vector<int> sendbuf(10, 100);    // [1] declare a vector of 10 integers.
    world.send( 1, 0, sendbuf );     // [2] send it to process 1, with tag=0.   
}
```

Just like in the standard MPI interface, for a send call MPI\_Send\(\) you must specify the message envelop and the message buffer. The envelop in the standard MPI interface is a triplet: &lt;dest, tag, comm&gt;, which is the destination process rank, a tag for message matching, and the communicator context. The message buffer is also a triplet: &lt;buf, size, datatype&gt;, which is the starting address, number of data items \(in the unit of datatype, not byte\), and the data item type. In the HIPP MPI, the message envelop should be explicitly specified. The call of `world.send(1, 0, ...)` in the above example is equivalent to specify the envelop triplet as &lt;1, 0, MPI\_COMM\_WORLD&gt; in the standard MPI. The data buffer is simpler, you can just pass the vector instance into send call, which is equivalent to specify &lt;&sendbuf\[0\], sendbuf.size\(\), MPI\_INT&gt; in the standard MPI.

The receive call is also much similar to the standard MPI\_Recv. We can directly write it as

```cpp
if( world.rank() == 1 ){
    vector<int> recvbuf(10);
    world.recv( 0, 0, recvbuf );
}
```

Note that in this case, the recvbuf must be initialized with a appropriate length, 10 in this example, so that when you pass it to the recv call, it will be convert into a suitable MPI standard call with data buffer triplet &lt;&recvbuf\[0\], recvbuf.size\(\), MPI\_INT&gt;. It is erroneous that the recv buffer size is smaller than the send buffer size, but it is correct that if larger.

{% hint style="success" %}
**Elaboration:** In the receive call, you might be interested in where does the MPI\_Status go. Actually the world.recv\(\) call returns a Status object, so that you can call its source\(\), tag\(\), error\(\) and count\(\) method to retrive the receiving status. The detail is documented in the [User's Guide](../../users-guide.md).
{% endhint %}



