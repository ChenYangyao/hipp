#include <hippcntl.h>
#include <unistd.h>
using namespace HIPP;

int main(int argc, char const *argv[])
{
    Ticker tk;
    for(int i=0; i<4; ++i){
        tk.tick(0);     
        // Perform some time-consuming computation, e.g.,
        sleep(1);
        tk.tick(1);
    }

    tk.tick(0);
    sleep(1);
    tk.tick("sleep more");

    // Print summary statistics.
    pout << tk.summary(), endl;

    // Print all records, as well as the summary statistics.
    pout << tk;

    // Visit each record.
    auto & all_records = tk.query_all();
    pout << all_records.size(), " records found\n",
        "The first => ", all_records[0], " sec\n",
        "The second => ", tk.query(1), " sec\n";

    auto & record = tk.query_last();
    pout << "The last, duration=", record.dur.count(), " sec",
        ", message=", record.rec_str, endl; 

    return 0;
}
