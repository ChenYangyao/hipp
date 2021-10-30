File and Group
================

.. include:: /global.rst

.. namespace:: HIPP::IO::H5


File Creation and Opening
-----------------------------

A HDF5 file is created by constructing a :class:`File` object. An existing  
HDF5 file is opened also by constructing a :class:`File` with different 
access mode flag. The usage of the constructors is::

    H5::File f(file_name, access_mode);

To create a file, the ``access_mode`` argument is either ``"w"``, ``"x"`` or ``"ac"``. The 
difference is that how they deal with existing file:

- ``"w"``: truncate it.
- ``"x"``: raise an :class:`ErrH5` exception.
- ``"ac"``: open it.

For example::
    
    H5::File f1 ("f1.h5", "w");     // Old file gets truncated.
    //H5::File f11("f1.h5", "x");   // Will fail because existing.
    H5::File f12("f1.h5", "ac");    // Open the existing file.

On successful return, the file is created and/or opened. The constructed :class:`File` 
object is a handler to that file.

To open an existing file, the ``access_mode`` is either:

- ``"r"``: open with read-only mode.
- ``"a"``: open with read-and-write model.

For example::

    H5::File f14("f1.h5", "r");     // Read-only.
    H5::File f15("f1.h5", "a");     // Read-and-write.

Any of the HDF5 objects is default-constructable, resulting in an "empty"
object that referring to no HDF5 resource. Hence, the HDF5 operations are 
not allowed on such an object. For example::

    H5::File f16;                   // An empty file object.
    assert( ! f16.has_referenced() );

The method :expr:`Obj::has_referenced()` checks whether or not the object 
is non-empty.

You may assign a file object to another. The assigned-to object 
first detaches from the old resource, and then points to the new resource.
For example, assgin a newly-constructed file to an empty file object::

    f16 = H5::File("f1.h5", "a");   // Assign to another file.

To manually detach a HDF5 object to the underlying resource, call 
:expr:`Obj::free()` method::

    f16.free();                     // Again becomes empty.
    if( f16 ) 
        pout << "Non-empty\n"; 
    else 
        pout << "Empty\n";

The object can be used as a boolean value, with its value obtaining from 
:expr:`Obj::has_referenced()`. After detachment, the object becomes empty.
If no other object refers to the resource, the resource is released.


Group Creation and Opening 
------------------------------

HDF5 group is an analog to the directory in the filesystem.
A HDF5 group is a collector which connects to other objects.  The connections 
are stored as "links" internally. The linked target can be any group, dataset, 
or (named-)datatype.

A newly-created HDF5 file has a "root" group which is automatically created.
To create a new group, call :expr:`Group::create_group()` on another group object.
Because a file is also a group (see the inheritance description 
in the API-Ref: :ref:`api-io-h5-objects`), this method can also be called 
on a file object whose root group is used.

The group creation is used as::

    H5::Group new_group = group_or_file.create_group(group_name, access_mode);

The ``access_mode`` controls how to deal with an existing group. It is either:

- ``"x"``: raise an :class:`ErrH5` exception on existing group.
- ``"ac"``: open it if existing.


For example::

    auto g1 = f1.create_group("g1", "x");       // Create /g1.
    // auto g11 = f1.create_group("g1", "x");   // Will fail because existing.
    auto g12 = f1.create_group("g1", "ac");     // Open the existing group.

    auto g2 = g1.create_group("g2", "ac");      // Create /g1/g2.
    
On successful creation and/or open, the returned :class:`Group` object serves
as a handler to the group.

An existing group can be opened by :class:`Group::open()` on a file or group.
The argument is either an absolute path or a relative path interpreted w.r.t.
the caller object::

    auto g13 = f1.open_group("g1");             // Open /g1.
    auto g21 = f1.open_group("g1/g2");          // Open /g1/g2.
    g21 = g1.open_group("g2");                  // Open /g1/g2 via g1.