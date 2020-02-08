#include <hippmpi.h>
#include <unistd.h>


int main(int argc, char const *argv[]){
    HIPP::MPI::Env env;
    const int nrow = 4, ncol = 5;
    double matrix[nrow][ncol], matrix_T[ncol][nrow];
    auto world = env.world();

    /**
     * use derived datatype and point-to-point communication to perform matrix
     * transportation.
     * 
     * The rank-0 process initialize the original matrix, create derived
     * datatype and send the matrix with derived datatype.
     * 
     * The rank-1 process received the transposed matrix with pre-defined
     * datatype, and print it.
     */
    if( world.rank() == 0 ){
        for(int i=0; i<nrow; ++i)
            for(int j=0; j<ncol; ++j)
                matrix[i][j] = i;
        auto col_type = 
            HIPP::MPI::DOUBLE.vector(nrow,1,ncol).resized( 0,sizeof(double) );
        world.send( 1, 0, matrix[0], ncol, col_type );
    }else if( world.rank() == 1 ){
        world.recv( 0, 0, matrix_T[0], ncol*nrow, "double" );
        for(int i=0; i<ncol; ++i){
            for(int j=0; j<nrow; ++j)
                printf("%f\t", matrix_T[i][j]);
            printf("\n");
        }
    }

    return 0;
}
