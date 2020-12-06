/* io-arrays.cpp */
#include <hippcntl.h>
#include <hippio.h>
using namespace std;

struct Person {
    int age;
    char name[32];
    double height;
};

int main(int argc, char const *argv[]){
    /* create a new file named arrays.h5 */
    HIPP::IO::H5File out_file("arrays.h5", "w");

    /* write 32 floats as an 1-d array into dataset "floats"*/
    vector<double> floats(32);
    out_file.create_dataset<double>("floats", {32}).write(floats);

    /* write 8 objects into a group "persons" as separate datasets for fields */
    vector<Person> persons(8);
    HIPP::IO::H5XTable<Person> tbl_manip (
        "age",      &Person::age,
        "name",     &Person::name,
        "height",   &Person::height);
    tbl_manip.write(persons, out_file.create_group("persons"));

    /* or, write them as records into a single dataset "person_records" */
    tbl_manip.write_records(persons, out_file, "person_records");

    return 0;
}