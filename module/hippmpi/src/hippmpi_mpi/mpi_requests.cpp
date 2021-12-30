#include <mpi_requests.h>

namespace HIPP::MPI {
    
Requests::Requests()
: _obj_base_t( std::make_shared<_obj_raw_t>() ) {}

ostream & Requests::info( ostream &os, int fmt_cntl ) const {
    if(fmt_cntl == 0 ){
        prt( os, 
            HIPPCNTL_CLASS_INFO_INLINE(HIPP::MPI::Requests),
            "no. of requests: ", size() );
    }
    if(fmt_cntl >= 1){
        prt( os, HIPPCNTL_CLASS_INFO(HIPP::MPI::Requests) );
        int cntnull = 0;
        for(int i=0; i<size(); ++i) cntnull += is_null(i);
        prt( os, "  Requests (no.=", size(), ", no. null=", cntnull, ")") 
            << endl;
    }
    return os;
}

void Requests::free(){
    *this = nullval();
}

void Requests::clear(){
    _obj_ptr = std::make_shared<_obj_raw_t>();
}

Requests::mpi_t Requests::raw(int i)const { 
    return _obj_ptr->raw(i); 
}

bool Requests::is_null() const { 
    return _obj_ptr->is_null(); 
}

bool Requests::is_null(int i) const { 
    return _obj_ptr->is_null(i); 
}

Requests Requests::nullval() noexcept { 
    return _from_raw( _obj_raw_t::nullval(), 0 ); 
}

int Requests::size() const { 
    return _obj_ptr->size(); 
}

bool Requests::empty() const { 
    return _obj_ptr->empty(); 
}

void Requests::put( Requests & rqs){ 
    return put(std::move(rqs)); 
}

void Requests::put( Requests && rqs){
    _obj_ptr->put( std::move(*rqs._obj_ptr) );
}

Requests & Requests::operator+=( Requests & rqs ){ 
    put(rqs); return *this; 
}

Requests & Requests::operator+=( Requests && rqs ) { 
    put(std::move(rqs)); return *this; 
}

Requests Requests::get( int i ){
    auto ptr = std::make_shared<_obj_raw_t>();
    _obj_ptr->get( i, *ptr );
    return Requests(ptr);
}

Requests Requests::get( int b, int e ){
    auto ptr = std::make_shared<_obj_raw_t>();
    _obj_ptr->get( b, e, *ptr );
    return Requests(ptr);
}

void Requests::start() {
    _obj_ptr->start();
}

void Requests::start(int i) {
    _obj_ptr->start(i);
}

void Requests::startall() {
    _obj_ptr->startall();
}

Status Requests::wait() { 
    return Status(_obj_ptr->wait()); 
}

Status Requests::wait(int i) { 
    return Status(_obj_ptr->wait(i)); 
}

Status Requests::test(int &flag) { 
    return Status(_obj_ptr->test(flag)); 
}

Status Requests::test(int i, int &flag) { 
    return Status(_obj_ptr->test(i, flag)); 
}

Status Requests::status(int &flag)const { 
    return Status(_obj_ptr->status(flag)); 
}

Status Requests::status(int i, int &flag)const { 
    return Status(_obj_ptr->status(i, flag)); 
}

Status Requests::waitany(int &index) { 
    return Status(_obj_ptr->waitany(index)); 
}

Status Requests::testany(int &index, int &flag) { 
    return Status(_obj_ptr->testany(index, flag)); 
}

void Requests::waitall(ContiguousBuffer<Status> statuses) {
    auto n = size(); 
    auto [p, n_got] = statuses;
    if(  n_got != static_cast<size_t>(n) )
        ErrLogic::throw_(ErrLogic::eLENGTH, emFLPFB, 
        "  ... size of statuses (", n_got, 
        ") does not match requests (", n, ")\n" );
    _obj_ptr->waitall(reinterpret_cast<Status::mpi_t *>(p));
}

void Requests::waitall(vector<Status> &statuses) {
    auto n = size(); 
    statuses.resize(n);
    _obj_ptr->waitall(reinterpret_cast<Status::mpi_t *>(statuses.data()));
}

void Requests::waitall() {
    _obj_ptr->waitall(MPI_STATUSES_IGNORE);
}

void Requests::testall(int &flag, ContiguousBuffer<Status> statuses) {
    auto n = size(); 
    auto [p, n_got] = statuses;
    if(  n_got != static_cast<size_t>(n) )
        ErrLogic::throw_(ErrLogic::eLENGTH, emFLPFB, 
        "  ... size of statuses (", n_got, 
        ") does not match requests (", n, ")\n" );
    _obj_ptr->testall(flag, reinterpret_cast<Status::mpi_t *>(p));
}

void Requests::testall(int &flag, vector<Status> &statuses) {
    auto n = size(); 
    statuses.resize(n);
    _obj_ptr->testall(flag, reinterpret_cast<Status::mpi_t *>(statuses.data()));
}

void Requests::testall(int &flag) {
    _obj_ptr->testall(flag, MPI_STATUSES_IGNORE);
}

void Requests::waitsome( int &count, ContiguousBuffer<int> indices, 
    ContiguousBuffer<Status> statuses) 
{
    auto n = size(); 
    auto [p, n_got] = statuses;
    auto [pi, ni_got] = indices;
    if(  n_got != static_cast<size_t>(n) || ni_got != static_cast<size_t>(n) )
        ErrLogic::throw_(ErrLogic::eLENGTH, emFLPFB, 
        "  ... size of indices (", ni_got, ") or statuses (", n_got, 
        ") does not match requests (", n, ")\n" );
    _obj_ptr->waitsome(
        count, pi, reinterpret_cast<Status::mpi_t *>(p));
}

void Requests::waitsome(int &count, vector<int> &indices, 
    vector<Status> &statuses) 
{
    auto n = size(); 
    statuses.resize(n);
    indices.resize(n);
    _obj_ptr->waitsome(count, indices.data(),
        reinterpret_cast<Status::mpi_t *>(statuses.data()));

}

void Requests::waitsome(int &count, ContiguousBuffer<int> indices) {
    auto n = size(); 
    auto [pi, ni_got] = indices;
    if(  ni_got != static_cast<size_t>(n) )
        ErrLogic::throw_(ErrLogic::eLENGTH, emFLPFB, 
        "  ... size of indices (", ni_got,
        ") does not match requests (", n, ")\n" );
    _obj_ptr->waitsome(
        count, pi, MPI_STATUSES_IGNORE);
}

void Requests::waitsome(int &count, vector<int> &indices) {
    auto n = size(); 
    indices.resize(n);
    _obj_ptr->waitsome(count, indices.data(), MPI_STATUSES_IGNORE);
}

void Requests::testsome(int &count, ContiguousBuffer<int> indices, 
    ContiguousBuffer<Status> statuses) 
{
    auto n = size(); 
    auto [p, n_got] = statuses;
    auto [pi, ni_got] = indices;
    if(  n_got != static_cast<size_t>(n) || ni_got != static_cast<size_t>(n) )
        ErrLogic::throw_(ErrLogic::eLENGTH, emFLPFB, 
        "  ... size of indices (", ni_got, ") or statuses (", n_got, 
        ") does not match requests (", n, ")\n" );
    _obj_ptr->testsome(
        count, pi, reinterpret_cast<Status::mpi_t *>(p));
}

void Requests::testsome(int &count, vector<int> &indices, 
    vector<Status> &statuses) 
{
    auto n = size(); 
    statuses.resize(n);
    indices.resize(n);
    _obj_ptr->testsome(count, indices.data(),
        reinterpret_cast<Status::mpi_t *>(statuses.data()));   
}

void Requests::testsome(int &count, ContiguousBuffer<int> indices) {
    auto n = size(); 
    auto [pi, ni_got] = indices;
    if(  ni_got != static_cast<size_t>(n) )
        ErrLogic::throw_(ErrLogic::eLENGTH, emFLPFB, 
        "  ... size of indices (", ni_got,
        ") does not match requests (", n, ")\n" );
    _obj_ptr->testsome(
        count, pi, MPI_STATUSES_IGNORE);
}

void Requests::testsome(int &count, vector<int> &indices) {
    auto n = size(); 
    indices.resize(n);
    _obj_ptr->testsome(count, indices.data(), MPI_STATUSES_IGNORE);
}

void Requests::cancel(){ 
    _obj_ptr->cancel(); 
}

void Requests::cancel(int i){ 
    _obj_ptr->cancel(i); 
}

} // namespace HIPP::MPI
