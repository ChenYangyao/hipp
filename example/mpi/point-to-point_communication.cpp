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
     * Rank-0 process sends a message containing 10 integers to rank-1 process.
     * Then rank-1 process prints the received data.
     * The barrier ensures the output is not entangled.
     */
    if( world.rank() == 0 ){
        vector<int> sendbuf(10, 100);
        HIPP::prt( cout, 
            "sent: 0 -> 1, data = 10 integers") << endl;
        world.send( 1, 0, sendbuf );
    }else if( world.rank() == 1 ){
        vector<int> recvbuf(10);
        world.recv( 0, 0, recvbuf );
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
            world.send( i, 0, &sendbuf, 1, "int" );
            sleep(1);
        }
    }else{
        int recvbuf;
        auto status = world.recv( 0, 0, 
            &recvbuf, 1, HIPP::MPI::INT );
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
        world.send( 1, 0, sendbuf );
    }else if( world.rank() == 1 ){
        string recvbuf( 12, '\0' );
        world.recv( 0, 0, recvbuf );
        HIPP::prt( cout, 
            "  received: 1 <- 0, data = ", recvbuf ) << endl;
    }

    return 0;
}
