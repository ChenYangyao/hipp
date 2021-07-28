/**
create: Yangyao CHEN, 2021/07/21
    [write   ] Ticker - timing and recording for events.
*/

#ifndef _HIPPCNTL_TIME_TICKER_H_
#define _HIPPCNTL_TIME_TICKER_H_
#include "../hippcntl_incl/incl.h"
#include <chrono>
namespace HIPP {

/**
Ticker - timing and recording for events.
*/
class Ticker {
public:
    /**
    Typedefs.
    clock_t:    clock type for timing.
    point_t:    time point used internally by the clock.
    dur_t:      duration of event used by Ticker.
    index_t:    index of the recorded events.
    */
    typedef std::chrono::steady_clock clock_t;
    typedef clock_t::time_point point_t;
    typedef std::chrono::duration<double> dur_t;
    typedef std::size_t index_t;
    
    /** Type used by Ticker to record each event. */
    struct record_t {
        /** Duration and message string of this event. Times are in seconds. */
        dur_t dur;
        string rec_str;
        record_t(const dur_t &d, const string &r): dur(d), rec_str(r){ }

        /**
        Print the record. `fmt_cntl` is not significant in the current 
        implementation.
        operator<<() - equivalent to info() with default fmt_cntl.
        */
        ostream & info(ostream &os = cout, int fmt_cntl=1) const;
        friend ostream & operator<<(ostream &os, const record_t &r);
    };
    /** Summary information of all recorded events. */
    struct summary_t {

        /** 
        Number of records, total time elapsed, mean time elapsed and standard
        deviation of them. Times are in seconds.
        */
        index_t n_recs;
        double dur_total, dur_mean, dur_sd;
        /**
        Print the summary statistics. `fmt_cntl` is not significant in the 
        current implementation.
        operator<<() - equivalent to info() with default fmt_cntl.
        */
        ostream & info(ostream &os = cout, int fmt_cntl=1) const;
        friend ostream & operator<<(ostream &os, const summary_t &s);
    };
    
    /**
    Constructor - the internal time point is set to the time point of the 
    construction.
    */
    Ticker();
    ~Ticker();

    /**
    Ticker is copyable and movable. The copy is deep - new object is independent
    to the old object that gets copied. The move is destructive - old object 
    cannot be used after being moved.
    */
    Ticker(const Ticker &o);
    Ticker(Ticker &&o);
    Ticker & operator=(const Ticker &o);
    Ticker & operator=(Ticker &&o);

    /**
    Update the internal time point. 
    @add_entry: If `add_entry` is not false, leave a record whose duration is 
    the time elapsed since the last updating of internal time point.

    The second overload is equivalent to set `add_entry = 1` and leave an 
    additional information `s` for the record.
    */
    void tick(int add_entry=1);
    void tick(const string &s);

    /** 
    Visit the recorded event.
    
    query(index) - query event with given `index`.
    query_last() - query the last event.
    query_all() - get all records.

    query() and query_last() throw on illegal access.
    */
    record_t & query(index_t index);
    record_t & query_last();
    vector<record_t> & query_all() noexcept;
    const record_t & query(index_t index) const;
    const record_t & query_last( ) const;
    const vector<record_t> & query_all() const noexcept;

    /**
    Return a printable summary instance for statistics.
    e.g., 
    Ticker tk;
    cout << tk.summary() << endl;
    */
    summary_t summary() const noexcept;
    
    /**
    Print the ticker.
    @fmt_cntl: 0 for a small, inline information and otherwise for a more 
    detailed information of all records and their summary statistics.

    operator<<() - equivalent to info() with the default `fmt_cntl`.
    */
    ostream & info(ostream &os = cout, int fmt_cntl=1) const;
    friend ostream & operator<<(ostream &os, const Ticker &ticker);
protected:
    vector<record_t> _records;
    point_t _last_point;
};

inline ostream & operator<<(ostream &os, const Ticker &ticker) 
{ return ticker.info(os); }

inline ostream & operator<<(ostream &os, const Ticker::record_t &r) 
{ return r.info(os); }

inline ostream & operator<<(ostream &os, const Ticker::summary_t &s) 
{ return s.info(os); }

} // namespace HIPP
#endif	//_HIPPCNTL_TIME_TICKER_H_