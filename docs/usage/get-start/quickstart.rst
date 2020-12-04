Quick Start
=====================

Write and Build an Application that Use HIPP: A Minimal Example  
-----------------------------------------------------------------

.. code-block::

    /* printing.cpp */
    #include <hippcntl.h>
    using namespace std;

    int main(int argc, char const *argv[]) 
    {
        double a = 1;
        int b = 2;
        string s = "The sum of ";

        /* print arguments of any type to standard out */
        HIPP::pout << s, a, " and ", b, " is ", a+b, endl;

        /* print any STL container that supports iteration into stream */
        vector<int> arr {1,2,3,4,5};
        HIPP::prt_a(cout, arr) << endl;

        /* transform variables into string */
        string s_joined = HIPP::str(s, a, " and ", b, " is ", a+b, '\n')
            + HIPP::str_a(arr) + '\n';
        HIPP::pout << s_joined;

        return 0;
    }

.. code-block:: bash 

    c++ -std=c++17 -O3 -Wall  -o printing.out printing.cpp -lhippcntl
    ./printing.out 

.. code-block:: text 

    The sum of 1 and 2 is 3
    1,2,3,4,5
    The sum of 1 and 2 is 3
    1,2,3,4,5


Using the MPI Module
-------------------------------

.. code-block:: 

    /* p2p-comm.cpp */
    #include <hippcntl.h>
    #include <hippmpi.h>
    using namespace std;

    int main(int argc, char *argv[]) 
    {
        HIPP::MPI::Env env(argc, argv);             // Init the MPI environment
        auto comm = env.world();

        if( comm.rank() == 0 ){
            int tag = 0, dest_rank = 1;
            vector<int> out_arr {1,2,3,4,5};
            comm.send(dest_rank, tag, out_arr);     // Process 0 sends to 1
        }else if( comm.rank() == 1 ){
            int tag = 0, src_rank = 0;
            vector<int> in_arr(5);
            comm.recv(src_rank, tag, in_arr);       // Process 1 sends to 0
            HIPP::prt_a(cout, in_arr) << endl;
        }

        return 0;
    }

.. code-block:: bash 

    mpicxx -std=c++17 -O3 -Wall -o p2p-comm.mp.out p2p-comm.cpp -lhippmpi -lhippcntl
    mpirun -n 4 ./p2p-comm.mp.out

.. code-block:: text 

    1,2,3,4,5

Using the IO Module 
------------------------------------

.. code-block::

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

        vector<double> floats(32);
        /* write 32 floats as an 1-d array into dataset "floats"*/
        out_file.create_dataset<double>("floats", {32}).write(floats);

        vector<Person> persons(8);
        HIPP::IO::H5XTable<Person> tbl_manip (
            "age",      &Person::age,
            "name",     &Person::name,
            "height",   &Person::height);
        /* write 8 objects into a group "persons" as separate datasets for fields */
        tbl_manip.write(persons, out_file.create_group("persons"));
        /* or, write them as records into a single dataset "person_records" */
        tbl_manip.write_records(persons, out_file, "person_records");

        return 0;
    }

.. code-block:: bash 

    c++ -std=c++17 -O3 -Wall -o io-arrays.out io-arrays.cpp -lhippio -lhippcntl -lhdf5
    ./io-arrays.out

.. code-block:: text 

    HDF5 "arrays.h5" {
        GROUP "/" {
        DATASET "floats" {
            DATATYPE  H5T_IEEE_F64LE
            DATASPACE  SIMPLE { ( 32 ) / ( 32 ) }
            DATA { ... }
        }
        DATASET "person_records" {
            DATATYPE  H5T_COMPOUND {
                H5T_IEEE_F64LE "height";
                H5T_ARRAY { [32] H5T_STD_I8LE } "name";
                H5T_STD_I32LE "age";
            }
            DATASPACE  SIMPLE { ( 8 ) / ( 8 ) }
            DATA { ... }
        GROUP "persons" {
            DATASET "age" {
                DATATYPE  H5T_STD_I32LE
                DATASPACE  SIMPLE { ( 8 ) / ( 8 ) }
                DATA { ... }
            }
            ...

