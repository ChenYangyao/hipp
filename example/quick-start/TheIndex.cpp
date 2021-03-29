#include <hippio.h>
//#include <hippmpi.h>
#include <hippalgorithm.h>
using namespace std;
using namespace HIPP::IO;
//using namespace HIPP::MPI;
using namespace HIPP::ALGORITHM;


int main(int argc, char *argv[]){
    int dset_data[4][6];
    H5File("file.h5", "w").create_dataset<int>("array", {4,6}).write(&dset_data[0][0]);

    /*
    Env env(argc, argv);
    auto comm = env.world();


    Comm new_comm = comm.create(comm.group().incl({0,1,2}));

    SeqBlock sb(comm);
    cout << new_comm << endl;
    sb.end();
    */

    vector<vector<float> > poses = { 
        {0.25, 1.9}, {1.5, 2.}, {.5, .5}, {1.,1.}, {1.5, 0.75}, {2., 1.}, {.6, 1.7}, {1.6, 2.1} };
    vector<KDTreeif<3, 4>::point_t> points;
    for(const auto &pos: poses){
        points.emplace_back( pos, int(&pos-poses.data()) );
    }
    KDTreeif<3, 4> tree(points);

    cout << tree << endl;

    return 0;
}
