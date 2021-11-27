Attribute Management
======================

.. include:: /global.rst

.. namespace:: HIPP::IO::H5

.. _tutor-io-h5-using-attr-manager:

Using the Attribute Manager
-----------------------------

The attribute manager has almost the same interface as the dataset manager 
introduced in the tuturial :ref:`tutor-io-h5-using-dataset-manager`. The 
only difference is that, due to the atomic nature of the attributes, you cannot 
sub-setting them.

The method :expr:`NamedObj::attrs()` returns an attribute manager typed 
:class:`AttrManager` for the caller object. 
The caller object may be any named object such as :class:`File`,
:class:`Group`, :class:`Dataset`, or a committed :class:`Datatype`::

    H5::File f1("f1.h5", "w");
    auto attrs =  f1.attrs();

To tell whether or not an attribute of given ``name`` exists under the named 
object, call :expr:`AttrManager::exists(name)`. For example, the above newly 
created file does not have an attribute named ``"attr_A"``::

    assert( !attrs.exists("attr_A") );

To create a numerical attribute and write data into it, call 
:expr:`AttrManager::put(name, data)`, with desired attribute name ``name``
and the data object ``data``. To read the data from a numerical attribute, 
call :expr:`AttrManager::get(name, data)`. 
The call :expr:`AttrManager::get<T>(name)` is similar but returns the 
data object typed ``T``, instead of writing it into the argument. 
The supported types and the semantics of the put/get operations are 
the same as those for :class:`DatasetManager`.

For example, suppose we have the following data objects::

    short s;
    vector<double> d10(10);
    int i5[5];
    long l34[3][4];
    array<float, 3> f3;
    vector<array<int, 3> > i83(8); 

Put operations can be used to create attributes for them and write data into 
the created attributes::

    attrs.put("s",      s);                     // Create a new attribute and write to it
    attrs.put("d10",    d10);
    attrs.put("i5",     i5);
    attrs.put("l34",    l34);
    attrs.put("f3",     f3);
    attrs.put("i83",    i83);

Get operations can be used to read back the data from the file::

    attrs.get("s",      s);                     // Read back the data.
    attrs.get("d10",    d10);
    attrs.get("i5",     i5);
    attrs.get("l34",    l34);
    attrs.get("f3",     f3);
    attrs.get("i83",    i83);

    s = attrs.get<short>("s");
    f3 = attrs.get<array<float, 3> >("f3");

String-like data (single string or an array of them, in either C++ or C style)
are output/input with :expr:`AttrManager::put_str()` and 
:expr:`AttrManager::get_str()`::

    string str = "foo";
    vector<string> str4 = {"top", "bottom", "left", "right"};
    char raw_str[16] = "foo";
    char raw_str4[4][16] = {"top", "bottom", "left", "right"};

    attrs.put_str("str", str);
    attrs.put_str("str4", str4);
    attrs.put_str("raw_str", raw_str);
    attrs.put_str("raw_str4", raw_str4);

    attrs.get_str("str", str);
    attrs.get_str("str4", str4);
    attrs.get_str("raw_str", raw_str);
    attrs.get_str("raw_str4", raw_str4);

    str = attrs.get_str<string>("str");
    str4 = attrs.get_str<vector<string> >("str4");

Attribute Creation and Opening
--------------------------------

The same as :ref:`tutor-io-h5-dataset-creation-and-opening`, attributes 
are created through methods of three levels, from simple-but-constrained 
to complex-but-general. 

For example, suppose we have the following attributes to be put as attributes 
under a file ``f1``::

    short s;
    vector<double> d10(10);
    int i5[5];

To create attributes for them, simply call 
:expr:`NamedObj::create_attr_for(name, data)`, where ``name`` is the attribute 
name and its datatype and dataspace are auto-deduced from data object ``data``::

    f1.create_attr_for("s",   s);
    f1.create_attr_for("d10", d10);
    f1.create_attr_for("i5",  i5);
    

If the data object is not at hand on the time of creating the attribute, you may 
create the attribute using :expr:`NamedObj::create_attr<T>(name, dataspace)` 
by manually specifying its element type ``T`` and dataspace. For example, 
the above calls are equivalent to::

    f1.create_attr<short>("s",    H5::Dataspace::vSCALAR);
    f1.create_attr<double>("d10", {10});
    f1.create_attr<int>("i5",     {5});

Here, the C++ type ``T`` is converted by the library to a HDF5 datatype. 
If you also want to specify the HDF5 datatype manually, use 
:expr:`NamedObj::create_attr(name, datatype, dataspace)`. Thus, the above 
calls are also equivalent to::

    f1.create_attr("s",   H5::NATIVE_SHORT_T,  H5::Dataspace::vSCALAR);
    f1.create_attr("d10", H5::NATIVE_DOUBLE_T, {10});
    f1.create_attr("i5",  H5::NATIVE_INT_T,    {5});

The method :expr:`NamedObj::open_attr(name)` opens an existing attribute 
of given ``name`` under the named object. For example, the above attribute
``"s"`` is opened like::

    auto attr_s = f1.open_attr("s");

Read/Write on Attribute
------------------------

The same as :ref:`tutor-io-h5-dataset-read-write`, once an attribute is created 
or opened, the I/O operations are performed by :expr:`Attr::read()` and :expr:`Attr::write()`,
respectively. 

For example, suppose we create the following attributes for three 
objects, ``s``, ``d10`` and ``i5``::

    short s;
    vector<double> d10(10);
    int i5[5];

    H5::File f1("f1.h5", "w");
    auto attr_s =   f1.create_attr_for("s",   s),
         attr_d10 = f1.create_attr_for("d10", d10),
         attr_i5 =  f1.create_attr_for("i5",  i5);

To write any of the data objects into the corresponding attribute, 
call :expr:`NamedObj::write()` by passing the object as argument::

    attr_s.write(s);
    attr_d10.write(d10);
    attr_i5.write(i5);

To read them back, use :expr:`Attr::read()`::

    attr_s.read(s);
    attr_d10.read(d10);
    attr_i5.read(i5);

If the data object is a raw buffer, you have to manually specify its datatype. 
For example, take the buffer under ``d10`` as the data to write::

    attr_d10.write(d10.data(), H5::NATIVE_DOUBLE_T);


