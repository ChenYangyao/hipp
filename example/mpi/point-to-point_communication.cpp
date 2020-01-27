#include <hippmpi.h>
#include <unistd.h>         // sleep() system call


int main(int argc, char const *argv[]){
    /**
     * Init the MPI environment, and get the communication world.
     */
    HIPP::MPI::Env env;
    auto world = env.world();

    /**
     * The rank-0 process in the communication world send a messeage to each
     * of other processes in the same communicator. After sending/receiving,
     * each process prints a line of information indicating the source/target
     * of the message and its content.
     */
    int buf;
    if( world.rank() == 0 ){
        for(int i=1, n=world.size(); i<n; ++i){
            sleep(1);
            buf = i;
            world.send( i, 0, &buf, 1, "int" );
            printf("sended: 0 -> %d [ data %d ] \n", i, buf);
        }
    }else{
        world.recv( 0, 0, &buf, 1, "int" );
        printf("received: 0 -> %d [ data %d ] \n", world.rank(), buf);
    }

    return 0;
}
