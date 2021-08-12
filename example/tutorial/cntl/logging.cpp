#include <hippcntl.h>

using namespace HIPP;

int find_sum(const vector<double> &v) {
    auto _ = plog.push_g(emFF);
    plog << "Begin computing the sum of {", v, "}", endl;
    auto sum = std::accumulate(v.begin(), v.end(), 0);
    plog << "Done. Get ", sum, endl;
    return sum;
}

void priority_level() {
    plog << "A NOTICE", endl;
    {
        auto _ = plog.push_at(plog.LV_INFO, "A new scope with INFO priority");
        plog << "An INFO", endl;
        plog << "Another INFO", endl;
    }
    plog << "Another NOTICE", endl;
}

int main(int argc, char const *argv[])
{
    /** basic logging */
    vector<int> v{1,2,3,4};
    plog << "Begin computing the sum of {", v, "}", endl;
    auto sum = std::accumulate(v.begin(), v.end(), 0);
    plog << "Done. Get ", sum, endl;

    /** use stack feature of PLogStream */
    auto sum1 = find_sum({1,2,3}),
        sum2 = find_sum({4,5,6});
    plog << "Get sums ", sum1, " and ", sum2, endl;

    /** change used priority level */
    priority_level();

    plog.set_level_used(plog.LV_INFO);
    priority_level();

    return 0;
}
