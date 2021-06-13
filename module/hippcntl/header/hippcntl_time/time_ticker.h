#ifndef _HIPPCNTL_TIME_TICKER_H_
#define _HIPPCNTL_TIME_TICKER_H_
#include "../hippcntl_incl/incl.h"
#include <chrono>
namespace HIPP {

class Ticker {
public:
    typedef std::chrono::steady_clock clock_t;
    typedef clock_t::time_point point_t;
    typedef std::chrono::duration<double> dur_t;
    typedef std::size_t index_t;
    
    struct record_t {
        dur_t dur;
        string rec_str;
        record_t(const dur_t &d, const string &r): dur(d), rec_str(r){ }
        ostream & info(ostream &os = cout, int fmt_cntl=1) const;
        friend ostream & operator<<(ostream &os, const record_t &r);
    };
    struct summary_t {
        index_t n_recs;
        double dur_total, dur_mean, dur_sd;
        ostream & info(ostream &os = cout, int fmt_cntl=1) const;
        friend ostream & operator<<(ostream &os, const summary_t &s);
    };
    
    Ticker();
    ~Ticker();

    Ticker(const Ticker &o);
    Ticker(Ticker &&o);
    Ticker & operator=(const Ticker &o);
    Ticker & operator=(Ticker &&o);

    void tick(int add_entry=1);
    void tick(const string &s);

    /* query() and query_last() throw on illegal access  */
    record_t & query(index_t index);
    record_t & query_last();
    vector<record_t> & query_all() noexcept;
    const record_t & query(index_t index) const;
    const record_t & query_last( ) const;
    const vector<record_t> & query_all() const noexcept;

    /**
     * Return a printable summary instance for statistics.
     * e.g., 
     * Ticker tk;
     * cout << tk.summary() << endl;
     */
    summary_t summary() const noexcept;
    
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