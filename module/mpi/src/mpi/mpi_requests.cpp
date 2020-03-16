#include "mpi_requests.h"
namespace HIPP{
namespace MPI{
    
Requests::Requests(): _obj_base_t( std::make_shared<_obj_raw_t>() ){}

ostream & Requests::info( ostream &os, int fmt_cntl ) const{
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
Requests::mpi_t Requests::raw(int i)const { return _obj_ptr->raw(i); }
bool Requests::is_null() const { return _obj_ptr->is_null(); }
bool Requests::is_null(int i) const { return _obj_ptr->is_null(i); }
Requests Requests::nullval() noexcept 
{ return _from_raw( _obj_raw_t::nullval(), 0 ); }
int Requests::size() const { return _obj_ptr->size(); }
bool Requests::empty() const { return _obj_ptr->empty(); }

void Requests::put( Requests & rqs){ return put(std::move(rqs)); }
void Requests::put( Requests && rqs){
    _obj_ptr->put( std::move(*rqs._obj_ptr) );
}
Requests & Requests::operator+=( Requests & rqs ){ put(rqs); return *this; }
Requests & Requests::operator+=( Requests && rqs )
    { put(std::move(rqs)); return *this; }
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

Status Requests::wait()
{ return Status(_obj_ptr->wait()); }
Status Requests::wait(int i)
{ return Status(_obj_ptr->wait(i)); }
Status Requests::test(int &flag)
{ return Status(_obj_ptr->test(flag)); }
Status Requests::test(int i, int &flag)
{ return Status(_obj_ptr->test(i, flag)); }
Status Requests::status(int &flag)const
{ return Status(_obj_ptr->status(flag)); }
Status Requests::status(int i, int &flag)const
{ return Status(_obj_ptr->status(i, flag)); }
Status Requests::waitany(int &index)
{ return Status(_obj_ptr->waitany(index)); }
Status Requests::testany(int &index, int &flag)
{ return Status(_obj_ptr->testany(index, flag)); }
void Requests::waitall(vector<Status> &statuses){
    statuses.resize( size() );
    _obj_ptr->waitall( (Status::mpi_t *)statuses.data() );
}
void Requests::testall(int &flag, vector<Status> &statuses){
    statuses.resize( size() );
    _obj_ptr->testall( flag, (Status::mpi_t *)statuses.data() );
}
void Requests::waitsome( 
    int &count, vector<int> &indices, vector<Status> &statuses){
    indices.resize( size() );
    statuses.resize( size() );
    _obj_ptr->waitsome( count, 
        indices.data(), (Status::mpi_t *)statuses.data() );
}
void Requests::testsome( int &count, 
    vector<int> &indices, vector<Status> &statuses){
    indices.resize( size() );
    statuses.resize( size() );
    _obj_ptr->testsome( count, 
        indices.data(), (Status::mpi_t *)statuses.data() );
}


} // namespace MPI
} // namespace HIPP
