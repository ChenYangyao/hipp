#include <hippcntl.h>
#include <unistd.h>
using namespace HIPP;

int main(int argc, char const *argv[]) {
    Ticker tk;

    for(int i=0; i<8; ++i){
        tk.tick(0);     
        // Perform some time-consuming computation, e.g.,
        sleep(1);
        tk.tick(str("Task ", i));
    }

    cout << tk.summary() << endl;

    cout << tk;

    return 0;
}
