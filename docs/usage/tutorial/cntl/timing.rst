Timing
================

.. include:: /global.rst

The examples in this section assume the following inclusion and namespace
declaration::

    #include <hippcntl.h>   /* The CNTL module of HIPP */
    #include <unistd.h>     /* sleep() */
    using namespace HIPP;

This includes and exposes all the declarations in the CNTL module. For 
detailed compilation guide, see the API-ref :ref:`api-cntl-usage`. 
All the example codes in this section can be downloaded 
at :download:`cntl/timing.cpp </../example/tutorial/cntl/timing.cpp>`.


The Ticker Class 
-------------------

HIPP provides class :class:`~HIPP::Ticker` for timing the events in the 
execution of a program.
At each call of the method :func:`~HIPP::Ticker::tick`, 
the ticker produces a record which contains the time duration since the last call 
of :func:`~HIPP::Ticker::tick`. For example::

    Ticker tk;
    for(int i=0; i<4; ++i){
        tk.tick(0);     
        // Perform some time-consuming computation, e.g.,
        sleep(1);
        tk.tick(1);
    }

Where a ticker instance is defined. In the ``for``-loop, ``tick(0)`` updates
the internal clock of the ticker. ``tick(1)`` updates the internal clock and 
produces a record.

A user-passed message can be attached with the record. Just pass a ``std::string``
object instead of ``1`` to ``tick()``. For example::

    tk.tick(0);
    sleep(1);
    tk.tick("sleep more");

To view the records, you can print a short summary for them::

    pout << tk.summary(), endl;

This outputs:

.. code-block:: text

    5 records, duration 1.00037 +/- 5.95633e-08

More detailed information can be inspected by directly printing the ticker 
object itself::

    pout << tk;

This outputs:

.. code-block:: text

    Ticker instance [loc=0x7ffc6bf2cbc0, size=32, align=8]
    ----------
    5 records, duration 1.00025 +/- 4.77032e-09
       0:  1.00029
       1:  1.00036
       2:  1.00023
       3:  1.00022
       4:  1.00016, sleep more

:func:`~HIPP::Ticker::query_all` returns a vector 
of all the records. :func:`~HIPP::Ticker::query` returns a individual 
record by its index. :func:`~HIPP::Ticker::query_last` returns the last 
record. From them you can loop at the detail of each record. For example::

    auto & all_records = tk.query_all();
    pout << all_records.size(), " records found\n",
        "The first => ", all_records[0], " sec\n",
        "The second => ", tk.query(1), " sec\n";

    auto & record = tk.query_last();
    pout << "The last, duration=", record.dur.count(), " sec",
        ", message=", record.rec_str, endl; 

The output is:

.. code-block:: text

    5 records found
    The first =>  1.00011 sec
    The second =>  1.00061 sec
    The last, duration=1.00022 sec, message=sleep more


