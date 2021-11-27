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
    H5::File f12("f1.h5", "ac");    // Create, or open if existing

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
    auto g12 = f1.create_group("g1", "ac");     // Create, or open if existing

    auto g2 = g1.create_group("g2", "ac");      // Create /g1/g2.
    
On successful creation and/or open, the returned :class:`Group` object serves
as a handler to the group.

An existing group can be opened by :class:`Group::open()` on a file or group.
The argument is either an absolute path or a relative path interpreted w.r.t.
the caller object::

    auto g13 = f1.open_group("g1");             // Open /g1.
    auto g21 = f1.open_group("g1/g2");          // Open /g1/g2.
    g21 = g1.open_group("g2");                  // Open /g1/g2 via g1.


Using ``h5ls -r`` to see the contents of this file, it prints:

.. code-block:: text

    /                        Group
    /g1                      Group
    /g1/g2                   Group


.. _tutor-io-h5-file-link-creation:

Link Creation
----------------

A HDF5 group is an analog to the directory in the filesystem.
It is a collector that connects to other objects.  The connections 
are stored as "links" internally. The linked target can be any group, dataset, 
or (named-)datatype. HDF5 provide three types of links:

- A **hard link** points to an existing object.
- A **soft link** stores a path to a (possible) object in the same file.
- An **external link** stores a path to a (possible) object in any file.

The hard link contributs the reference count of the target object. Once the 
reference count decreases to zero, the object is no longer accessible and 
the storage space is marked as free space which may or may not be reused
by the HDF5 library. The later two types are "symbolic links" which do not 
contribute to the reference count - they are resolved only when you use them
to visit objects.

On the creation of a named object, the link is automatically added to its 
parent object. For example, the following codes create two groups and 
two datasets::

    auto g1 = f1.create_group("g1"), 
         g2 = f1.create_group("g2");
    auto d1 = f1.create_dataset_scalar<double>("d1"),
         d2 = g1.create_dataset_scalar<int>("d2");

The layout of the file is like:

.. code-block:: text

    f1.h5/
        |- g1/
            |- d2
        |- g2/
        |- d1

Now we create three links to ``"d2"``:

- A hard link under ``"g2"``.
- A soft link under ``"f1"``.
- A external link under ``"f1"`` (here, the linked file is the file itself).

.. code-block:: cpp

    g2.create_hard_link("d2", g1, "d2");
    f1.create_soft_link("d2", "/g1/d2");
    f1.create_external_link("d2-ext", "f1.h5", "/g1/d2");

Now, the layout of the file is like:

.. code-block:: text

    f1.h5/
        |- g1/
            |- d2   (the link target)
        |- g2/
            |- d2   (hard link to the target)
        |- d1
        |- d2       (soft link to /g1/d2)
        |- d2-ext   (external link to f1.h5//g1/d2)

Since all these three links points to a valid object, you may use either of 
them to visit the target.


Group Navigation
------------------

A group may contain various links to other objects. The links may have 
different "link types" and the pointed objects may also have different "object types". 
In this section, we demonstrate the methods to retrieve these "meta-info" of 
HDF5 objects, including 

- Get the number of links under a group.
- Discover the name and type of each link.
- Check whether or not a link resolves to an object, and get the type of that 
  object.

Consider the file layout in the above example (:ref:`tutor-io-h5-file-link-creation`):
the file itself has 5 links (three hard to ``"g1"``, ``"g2"``, and ``"d1"``, 
1 soft, and 1 external), group ``"g1"`` and ``"g2"`` both have 1 hard link.

To get the number of links in a group, call :expr:`Group::get_info()` to retrieve 
the "group meta-info" and read its attribute ``nlinks``::

    size_t nlinks_f1 = f1.get_info().nlinks,
        nlinks_g1 = g1.get_info().nlinks,
        nlinks_g2 = g2.get_info().nlinks;

    pout << "nlinks of f1,g1,g2 =", 
        nlinks_f1, ',', nlinks_g1, ',', nlinks_g2, endl;

Output:

.. code-block:: text

    nlinks of f1,g1,g2 =5,1,1

To get the names of the links, call :expr:`Group::get_link_name()` by passing 
a sub-group path (``"."`` means the current group itself) and 
the link index::

    vector<string> names(nlinks_f1);
    for(size_t i=0; i<nlinks_f1; ++i)
        names[i] = f1.get_link_name(".", i);
    pout << "links in f1 = {", names, "}\n";

Output:

.. code-block:: text
    
    links in f1 = {d2-ext,d1,d2,g1,g2}

To retrieve the type of a link, call :expr:`Group::get_link_info()` by passing 
the link name::

    assert( f1.get_link_info("d1").type == g1.tHARD );
    assert( f1.get_link_info("d2").type == g1.tSOFT );
    assert( f1.get_link_info("d2-ext").type == g1.tEXTERNAL );

A symbolic link may be dangled, referring to no object. We can
verify that it resolves to an object using :expr:`Group::object_exists()`.
This method returns ``false`` if the link does not exist, or the link exists but 
does not resolve to an object.

If the link does resolve to an object, the call of :expr:`Group::get_object_info()`
returns the "object meta-info", whose ``type`` field indicates the object type.

For example::

    for( auto path: {"d1", "d2", "d2-ext", "g1", "g2", "g1/d2", "g2/d2", "not-existing"} ) {
        if( ! f1.object_exists(path)  ) {
            pout << path, " is dangled\n";
            continue;
        }
    
        auto type = f1.get_object_info(path).type;
        if( type == f1.tGROUP )
            pout << path, " is a group\n";
        else if( type == f1.tDATASET )
            pout << path, " is a dataset\n";
        else 
            pout << path, " has other type\n";
    }

Output:

.. code-block:: text

    d1 is a dataset
    d2 is a dataset
    d2-ext is a dataset
    g1 is a group
    g2 is a group
    g1/d2 is a dataset
    g2/d2 is a dataset
    not-existing is dangled


Iterating Under a Group
""""""""""""""""""""""""

As demonstrated above, to visit all links under a group, we get the number of 
links using group meta-info, then get names of links by their indices, and 
finally get meta-info of those links. 

Visiting all objects under a group is similar except that 
we further verify that the each link does resolve to an object. If it does, 
we retrieve its object meta-info.

Because such iterations are frequently used, the HDF5 library provides several 
shortcuts for them. To iterate over all links right under a group, call 
:expr:`Group::link_iterate(idx, cb)`, where 

- ``idx`` is an in-out argument. On entry, it specifies the index (zero-based)
  of the first link to operate at. On exit, it is the index of the 
  next-to-operate link.
  Usually we start at ``idx = 0`` and end with ``idx = no. of links in the group``
  if no shortcut failure of success.
- ``cb`` is a callback function. It must accept the link information typed
  :expr:`Group::link_iter_arg_t &`, and return ``herr_t``. The type 
  :type:`Group::link_iter_arg_t` has several methods which give link names,
  link meta-info, etc. The return value of the callback may be negative to 
  indicate a shortcut failure, or positive to indicate a shortcut success, 
  or ``0`` to allow iteration continues.

For example, working at the file ``f1`` in the above example 
:ref:`tutor-io-h5-file-link-creation`, the following callback prints the names and 
types of all links under the root group of ``f1``::

    auto link_cb = [&](H5::Group::link_iter_arg_t &link) -> herr_t {
        pout << "Link ", link.name(), " has type ", link.info().type, '\n';
        return 0;
    };

    hsize_t idx = 0;
    f1.link_iterate(idx, link_cb);
    pout << "no. of links = ", idx, endl;

Output: 

.. code-block:: text 

    Link d1 has type 0
    Link d2 has type 1
    Link g1 has type 0
    Link g2 has type 0
    Link d2-ext has type 64
    no. of links = 5

Another method, :expr:`Group::link_visit(cb)`, is slightly different: it
recursively applies the callback to all links under the group and its 
sub group, sub-sub group, etc::

    f1.link_visit(link_cb);

Output: 

.. code-block:: text 

    Link d1 has type 0
    Link d2 has type 1
    Link g1 has type 0
    Link g1/d2 has type 0
    Link g2 has type 0
    Link g2/d2 has type 0
    Link d2-ext has type 64


The method, :expr:`Group::object_visit(cb)`, is similar, but it only visits 
all hard-linked objects::

    auto obj_cb = [&](H5::Group::obj_iter_arg_t &obj) -> herr_t {
        pout << "Object ", obj.name(), " has type ", obj.info().type, '\n';
        return 0;
    };
    f1.object_visit(obj_cb);

Output: 

.. code-block:: text 

    Object . has type 0
    Object d1 has type 1
    Object g1 has type 0
    Object g1/d2 has type 1
    Object g2 has type 0

Note that the callback of :expr:`Group::object_visit()` accepts 
:expr:`Group::obj_iter_arg_t &`. 
Also note that the group itself is visited.







