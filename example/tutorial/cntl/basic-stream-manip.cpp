#include <hippcntl.h>
using namespace std;

void usage_pretty_stream(){
    double a = 1.0;
    int b = 2;
    string c = "foo";
    vector<string> d = {"bar", "baz"};
    std::unordered_map<string, string> e = {{"a", "f(a)"}, {"b", "f(b)"}};
    std::pair<string, int> f = {"pair-key", 1};
    std::tuple<string, int, vector<double> > g = {"tuple-elem", 1, {1.,2.,3.}};
    int arr1d[5] = {1,2,3,4,5};
    int arr2d[2][3] = {{1,2,3}, {4,5,6}};

    /* Print anything to the pretty stream. */
    HIPP::pout << 
        "a = ", a, '\n',
        "b = ", b, '\n',
        "c = ", c, '\n',
        "d = ", d, '\n',
        "e = ", e, '\n',
        "f = ", f, '\n',
        "g = ", g, '\n',
        "arr1d = ", HIPP::pout(arr1d, arr1d+5), '\n',
        "arr1d = ", HIPP::PrtArray(arr1d, arr1d+5), '\n',
        "arr2d = \n", HIPP::PrtArray(arr2d[0], arr2d[0]+6).ncol(3).width(4), endl;
}

struct Person {
    string first_name, last_name;
    int age;
};
ostream & operator<< (ostream &os, const Person &person) {
    os << person.first_name << ' ' << person.last_name 
        << " (" << person.age << ")";
    return os;
} 

void user_defined_class(){
    Person albert = {"Albert", "Einstein", 120},
        edwin = {"Edwin", "Hubble", 100};
    HIPP::pout << albert, " and ", edwin, endl;
}

void stringstream_and_filestream(){
    ofstream fs("filestream.dat");
    HIPP::PStream ps_for_fs(fs);
    ps_for_fs << "bar", " foo ", "baz", endl;

    ostringstream ss;
    HIPP::PStream ps_for_ss(ss);
    ps_for_ss << "bar", " foo ", "baz", endl;
    
    HIPP::pout << "The content of stringstream is ", ss.str();
}

void Cstyle_print(){
    double a = 3.1415; 
    int b = 111;
    HIPP::prt_f(cout, "a=%10.6f, b=%6d\n", a, b);
}

void string_factories(){
    int snapshot = 100, part_id = 1;
    string filename1 = HIPP::str("snapshot", snapshot, ".", part_id), 
        filename2 = HIPP::str_f("snapshot%.4d.%.4d", snapshot, part_id);

    HIPP::pout << filename1, " and ", filename2, endl;
}

int main(int argc, char const *argv[]) {

    usage_pretty_stream();
    user_defined_class();
    stringstream_and_filestream();

    Cstyle_print();

    string_factories();
    return 0;
}
