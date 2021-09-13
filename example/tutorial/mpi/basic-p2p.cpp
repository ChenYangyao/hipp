/* basic-p2p.cpp */
#include <hippmpi.h>
using namespace HIPP;
using namespace HIPP::MPI;
using namespace std;

void vector_send_recv(Comm comm) {
    int rank = comm.rank();

    double outbuf[5] = {}, inbuf[5];
    int tag = 0;
    if( rank == 0 ) {
        comm.send(1, tag, outbuf);
    }else if( rank == 1 ) {
        comm.recv(0, tag, inbuf);
        pout << "Got {", pout(inbuf, inbuf+5), "}", endl;
    }

    if( rank == 0 ) {
        comm.send(1, tag, outbuf);
    }else if( rank == 1 ) {
        auto status = comm.recv(0, tag, inbuf);
        pout << "Got ", status.count(DOUBLE), " items", endl;
    }
}

void more_on_buffer(Comm comm) {
    int rank = comm.rank();
    int tag = 1;
    double outbuf[5] = {}, inbuf[5];
    if( rank == 0 ) {
        comm.send(1, tag, outbuf, 5, DOUBLE);
        comm.send(1, tag, outbuf, 5, "double");
        comm.send(1, tag, outbuf, 5);

        double x; 
        short a[5];
        string s = "hello"; 
        vector<int> v(5);

        comm.send(1, tag, a);
        comm.send(1, tag, x);
        comm.send(1, tag, s);
        comm.send(1, tag, v);
    }else if(rank == 1) {
        comm.recv(0, tag, inbuf, 5, DOUBLE);
        pout << "Got {", pout(inbuf, inbuf+5), "}", endl;

        comm.recv(0, tag, inbuf, 5, "double");
        pout << "Got {", pout(inbuf, inbuf+5), "}", endl;

        comm.recv(0, tag, inbuf, 5);
        pout << "Got {", pout(inbuf, inbuf+5), "}", endl;

        short a[5];
        double x; vector<char> s(20); vector<int> v(5);
        comm.recv(0, tag, a);
        comm.recv(0, tag, x);
        comm.recv(0, tag, s);
        comm.recv(0, tag, v);
        pout << "Got ", pout(a,a+5), endl;
        pout << "Got ", x, endl;
        pout << "Got ", s, endl;
        pout << "Got ", v, endl;
    }
}


int main(int argc, char const *argv[])
{
    Env env;
    auto comm = env.world();

    vector_send_recv(comm);
    more_on_buffer(comm);
    
    return 0;
}
