#include <hippnumerical.h>

using namespace HIPP;
using namespace HIPP::NUMERICAL;
using namespace std;

void quick_and_dirty(){
    pout << randn(), " and ", randint(), endl;

    vector<double> rvs = randn(5);
    pout << "random variables: {", rvs, "}", endl;
}

void engine(){
    pout << global_random_engine(), endl;
    pout << process_global_random_engine(), endl;

    DefaultRandomEngine e;                  // default constructed
    pout << e(), endl;

    RandomEngine::knuth_b_t e2{10086};      // provide a seed
    pout << e2(), endl;

    e2.seed(10086);                         // re-seed the engine
    pout << e2(), endl;
}

void from_distribution() {
    UniformRealRandomNumber<double> rng {0.0, 100.0};
    pout << rng(), " and ", rng(), endl;

    DefaultRandomEngine e { 10086 };
    UniformRealRandomNumber<double> rng2 {0.0, 1.0, &e};
    pout << rng2(), " and ", rng2(), endl;

    pout << "Minimal and maximal possible values = ", 
        rng.min(), ",  ", rng.max(), '\n';
    pout << "Parameters (a and b for the distribution) = ", 
        rng.a(), ", ", rng.b(), '\n';
    
    
    rng.seed(10086).reset_state();
    pout << "New sequence of random numbers after reseed {", rng(5), "}\n";

    pout << "Temporary U(0, 200) variable ", rng.get(0., 200.), '\n';
    rng.reset_param(0.0, 200.0);           // permanently change the parameters
    pout << rng(), endl;
}

int main(int argc, char const *argv[]) {

    quick_and_dirty();
    engine();
    from_distribution();
    
    return 0;
}


