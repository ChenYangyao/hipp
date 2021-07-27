#include <hippcntl.h>
using namespace HIPP;


void basic_log() {
    plog << "Begin computation", endl;
    int sum = 0;
    for(int i=0; i<5; ++i)
        sum += i;
    plog << "Sum from 0 to 4 is ", sum, endl;
}

double inner(double x) {
    auto g = plog.push_g(emFF);
    plog << "Begin computation for x=", x, "", endl;
    double y = std::sin(x);
    plog << "Got result ", y, endl;
    return x;
}

void outer() {
    auto g = plog.push_g(emFF, "- the outer loop");
    vector<double> xs = {1., 2., 3.}, ys;
    plog << "Begin computation for ", xs.size(), " values" , endl;
    plog << "Inputs = {", xs, "}", endl;
    for(size_t i=0; i<xs.size(); ++i){
        ys.push_back(inner(xs[i]));
    }
    plog << "Got all results {", ys, "}", endl;
}

void with_nested_functions() {
    outer();
}

void log_with_filter() {
    
    plog << "With filter", endl;
    plog << "Log entries with low priority levels are not printed", endl;
    {
        auto _ = plog.push_at(plog.LV_INFO, "Lower-priority block");
        plog << "Printed only when the INFO level is required", endl;
        plog << "Begin execution", endl;
        /* work on a task */
        {
            auto _ = plog.push_at(plog.LV_DEBUG, "Even lower-priority block");
            plog << "Printed only when the DEBUG level is required", endl;
            plog << "Begin more detail of the task", endl;
            /* work on detail */
            plog << "End successfully", endl;
        }
        plog << "End execution", endl;
    }
    plog << "End of subroutine", endl;
}

int main(int argc, char const *argv[])
{
    basic_log();
    with_nested_functions();

    log_with_filter();

    plog.set_level_used(plog.LV_INFO);
    log_with_filter();

    plog.set_level_used(plog.LV_DEBUG);
    log_with_filter();

    return 0;
}

