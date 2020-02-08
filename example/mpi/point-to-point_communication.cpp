#include <hippmpi.h>
#include <unistd.h>         // sleep() system call


int main(int argc, char const *argv[]){
    /**
     * Init the MPI environment, and get the communication world.
     */
    HIPP::MPI::Env env;
    auto world = env.world();

    /**
     * Rank-0 process sends a message containing 10 integers to rank-1 process.
     * Then rank-1 process prints the received data.
     * The barrier ensures the output is not entangled.
     */
    if( world.rank() == 0 ){
        std::vector<int> sendbuf(10, 100);
        world.send( 1, 0, sendbuf );
    }else if( world.rank() == 1 ){
        std::vector<int> recvbuf(10);
        world.recv( 0, 0, recvbuf );

        printf("  received: 1 <- [ data ");
        for(int i=0; i<9; ++i) printf("%d, ", recvbuf[i]); 
        printf("%d ]\n", recvbuf[9]);
    }
    world.barrier();

    /**
     * The rank-0 process in the communication world sends a messeage to each
     * of other processes in the same communicator. After sending/receiving,
     * each process prints a line of information indicating the source/target
     * of the message and its content.
     */
    int buf;
    if( world.rank() == 0 ){
        for(int i=1, n=world.size(); i<n; ++i){
            buf = i;
            world.send( i, 0, &buf, 1, "int" );
            
            sleep(1);
            printf("sended: 0 -> [ data %d ] \n", buf);
        }
    }else{
        world.recv( 0, 0, &buf, 1, "int" );
        
        sleep(1);
        printf("  received: %d <- [ data %d ] \n", world.rank(), buf);
    }

    return 0;
}
