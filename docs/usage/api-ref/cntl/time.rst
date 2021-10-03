Timing 
=========

.. include:: /global.rst

The following variables, functions and classes are all defined within the namespace ``HIPP``.

.. namespace:: HIPP


Ticker
--------

.. class:: Ticker

    ``Ticker`` is used for timing and recording events, and 
    producing summary statistics of them.

    ``Ticker`` is copyable and movable. The copy is deep - new object is independent
    to the old object that gets copied. The move is destructive - old object 
    cannot be used after being moved.

    .. type::   std::chrono::steady_clock clock_t
                clock_t::time_point point_t
                std::chrono::duration<double> dur_t
                std::size_t index_t

        Typedefs.
        
        ``clock_t`` :    clock type for timing.
        
        ``point_t`` :    time point used internally by the clock.
        
        ``dur_t`` :      duration of event used by Ticker.
        
        ``index_t`` :    index of the recorded events.
    
    .. struct:: record_t

        Type used by Ticker to record each event.

        .. member:: dur_t dur
                    string rec_str

            Duration and message string of this event.

            Times are in seconds.

        .. function:: record_t(const dur_t &d, const string &r)

        .. function:: \
            ostream & info(ostream &os = cout, int fmt_cntl=1) const
            friend ostream & operator<<(ostream &os, const record_t &r)

            Print the record. ``fmt_cntl`` is not significant in the current 
            implementation.
            
            ``operator<<()`` is equivalent to ``info()`` with default ``fmt_cntl``.
    
    .. struct:: summary_t

        Summary information of all recorded events.

        .. member:: index_t n_recs
                    double dur_total
                    double dur_mean
                    double dur_sd
            
            Number of records, total time elapsed, mean time elapsed and standard
            deviation of them.

            Times are in seconds.

        .. function:: \
            ostream & info(ostream &os = cout, int fmt_cntl=1) const
            friend ostream & operator<<(ostream &os, const summary_t &s)

            Print the summary statistics. ``fmt_cntl`` is not significant in the 
            current implementation.
            
            ``operator<<()`` is equivalent to ``info()`` with default ``fmt_cntl``.
    
    
    .. function:: Ticker()
    
        Constructor - the internal time point is set to the time point of the 
        construction.

    .. function:: void tick(int add_entry=1)
        void tick(const string &s)

        Update the internal time point. 

        :arg add_entry: If not ``false``, leave a record whose duration is 
            the time elapsed since the last updating of internal time point.

        The second overload is equivalent to set ``add_entry = 1`` and leave an 
        additional information ``s`` for the record.


    .. function:: \
        double duration(int update=1)

        Return the time elapsed in seconds since the last update of the internal 
        clock. 
        
        :arg update: if ``true``, update the internal clock.
    
    .. function:: \
        const point_t & last_time_point() const
        static point_t now()

        ``last_time_point()`` : get the last-updated time point of the internal clock.
        
        ``now()`` : get the time point of now.

    .. function:: \
        record_t & query(index_t index)
        record_t & query_last()
        vector<record_t> & query_all() noexcept
        const record_t & query(index_t index) const
        const record_t & query_last( ) const
        const vector<record_t> & query_all() const noexcept

        Visit the recorded event.
    
        - ``query(index)`` - query event with given ``index``.
        - ``query_last()`` - query the last event.
        - ``query_all()`` - get all records.

        ``query()`` and ``query_last()`` throw on illegal access.

    .. function:: summary_t summary() const noexcept

        Return a printable summary instance for statistics.

        Usage is like::
        
            Ticker tk;
            cout << tk.summary() << endl;
    
    .. function:: \
        ostream & info(ostream &os = cout, int fmt_cntl=1) const
        friend ostream & operator<<(ostream &os, const Ticker &ticker)
    
        Print the ticker.
        
        :arg fmt_cntl: ``0`` for a small, inline information and otherwise for a more 
            detailed information of all records and their summary statistics.

        ``operator<<()`` is equivalent to ``info()`` with the default ``fmt_cntl``.


    **Example:**

    The following codes define a ``Ticker`` instance, use it to record events, and print 
    the summary statistics::

        Ticker tk;

        for(int i=0; i<8; ++i){
            tk.tick(0);     
            // Perform some time-consuming computation, e.g.,
            sleep(1);
            tk.tick(str("Task ", i));
        }

        cout << tk.summary() << endl;

    Output:

    .. code-block:: text

        8 records, duration 1.00041 +/- 2.94286e-08

    If more detailed information is needed, directly print the ticker::

        cout << tk;

    Output:

    .. code-block:: text

        Ticker instance [loc=0x7ffcb94d97f0, size=32, align=8]
        ----------
        8 records, duration 1.00041 +/- 2.94286e-08
           0:  1.00042, Task 0
           1:  1.00038, Task 1
           2:  1.00057, Task 2
           3:  1.00033, Task 3
           4:  1.00036, Task 4
           5:  1.00077, Task 5
           6:  1.00017, Task 6
           7:  1.00028, Task 7