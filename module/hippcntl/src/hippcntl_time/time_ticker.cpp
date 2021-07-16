#include <time_ticker.h>
#include <hippcntl_stream/stream.h>
namespace HIPP {

ostream & Ticker::record_t::info(ostream &os, int fmt_cntl) const {
    HIPP::prt_f(os, "%8g", dur.count());
    if( !rec_str.empty() )
        os << ", " << rec_str;
    return os;
}

ostream & Ticker::summary_t::info(ostream &os, int fmt_cntl) const {
    PStream ps(os);
    ps << n_recs, " records, duration ", dur_mean, " +/- ", dur_sd;
    return os;
}

Ticker::Ticker(): _last_point(clock_t::now())
{}
Ticker::~Ticker(){ }

Ticker::Ticker(const Ticker &o): 
_records(o._records), _last_point(o._last_point)
{}

Ticker::Ticker(Ticker &&o): 
_records(std::move(o._records)), _last_point(std::move(o._last_point))
{}

Ticker & Ticker::operator=(const Ticker &o){
    _records = o._records;
    _last_point = o._last_point;
    return *this;
}
Ticker & Ticker::operator=(Ticker &&o){
    _records = std::move(o._records);
    _last_point = std::move(o._last_point);
    return *this;
}

void Ticker::tick(int add_entry) {
    auto cur_point = clock_t::now();
    if( add_entry ){
        _records.emplace_back( 
            std::chrono::duration_cast<dur_t>(cur_point-_last_point), "" );    
    }
    _last_point = cur_point;
}
void Ticker::tick(const string &s) {
    auto cur_point = clock_t::now();
    _records.emplace_back(
        std::chrono::duration_cast<dur_t>(cur_point-_last_point), s);
    _last_point = cur_point;
}
auto Ticker::query(index_t index) -> record_t & {
    return _records.at(index);
}
auto Ticker::query_last( ) -> record_t & {
    return _records.at(_records.size()-1);
}
auto Ticker::query_all() noexcept -> vector<record_t> & {
    return _records;
}
auto Ticker::query(index_t index) const -> const record_t & {
    return _records.at(index);
}
auto Ticker::query_last( ) const -> const record_t & {
    return _records.at(_records.size()-1);
}
auto Ticker::query_all() const noexcept -> const vector<record_t> & {
    return _records;
}
auto Ticker::summary() const noexcept -> summary_t {
    index_t n_recs = 0; 
    double dur_total = 0., dur_sq_total = 0.;
    for(auto &r: _records){
        double dur = r.dur.count();
        ++n_recs;
        dur_total += dur;
        dur_sq_total += dur*dur;
    }
    double dur_mean = 0., dur_sd = 0.;
    if( n_recs > 0 ){
        dur_mean = dur_total / double(n_recs);
        dur_sd = dur_sq_total / double(n_recs) - dur_mean*dur_mean;
    }
    return {n_recs, dur_total, dur_mean, dur_sd};
}

ostream & Ticker::info(ostream &os, int fmt_cntl) const {
    HIPP::PStream ps(os);
    if( fmt_cntl == 0 ){
        ps <<   HIPPCNTL_CLASS_INFO_INLINE(Ticker),
                _records.size(), " records found.";
        return os;
    }
    ps << HIPPCNTL_CLASS_INFO(Ticker), summary(), '\n';
    for(std::size_t i=0; i<_records.size(); ++i){
        ps << std::setw(4), i, ": ";
        _records[i].info(os) << '\n';
    }
    ps << std::flush;
    return os;
}

} // namespace HIPP