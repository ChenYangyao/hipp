*************************************
IO Module
*************************************

.. _api-io-usage:

All the definitions in the IO module can be accessed by including the header file **<hippio.h>**.
Classes, functions and variables are all defined within the ``HIPP::IO`` namespace. 
Code that uses the IO module would be like ::

    /* src.cpp */

    #include <hippio.h>                         // include all definitions in HIPP IO 

    int main(int argc, char const *argv[]){
        HIPP::IO::H5File file("list.h5", "w");  // create a new file of HDF5 format

        // perform IO on the file instance
    }

To generate the executable binary, the links to **"libhippio.so"** and **"libhippcntl.so"** are necessary.
Depending on your installation of the HDF5 library, the link to **libhdf5.so** would also be required.
The compiling command in an Unix-like system would be like

.. code-block:: bash 

    $ g++ -std=c++17 src.cpp -lhippio -lhippcntl -lhdf5

.. _api-io-convention:

To avoid confusion, we list the convetions used in this API reference:

* The term **Standard** means the HDF5 standard C APIs (types, functions, variables, macros, etc.)
  defined by the `HDF5 Group <https://www.hdfgroup.org/solutions/hdf5/>`_. E.g., the **Standard**
  ``hid_t`` means the generic object type defined by HDF5 C API.
* The HIPP IO library has three components: the **High-level API**, the **Extended API**, and the 
  **Intermediate-level API** (see below for the detail API references of each component).
* The documentation text of one definition would probably refer to other definitions. If they are in the 
  same namespace (``HIPP::IO`` here), we drop the namespace specifiers for clarity 
  (e.g., class :class:`H5File <HIPP::IO::H5File>` means class :class:`HIPP::IO::H5File`). 
* The example code that uses C++ standard library may ignore the ``std::`` namespace specifier (namely, assume 
  a global ``using namespace std`` is used). This is not good in practice, but makes the example code more compact. 

.. toctree::
    :maxdepth: 2
    :caption: High-level API 

    high-level/file 
    high-level/dataset 

.. toctree::
    :maxdepth: 2
    :caption: Extended API 

    ext/tabular

.. toctree::
    :maxdepth: 2
    :caption: Intermediate-level API

    intermediate-level/index 

