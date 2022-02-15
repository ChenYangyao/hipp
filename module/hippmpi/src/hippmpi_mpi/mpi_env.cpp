#include <mpi_env.h>
namespace HIPP{
namespace MPI{

namespace _mpi_env_helper{
comm_intern_priv_attr_t::comm_intern_priv_attr_t()
:comm_dup(Comm::nullval()), prev(PROC_NULL), next(PROC_NULL)
{}
comm_intern_priv_attr_t::~comm_intern_priv_attr_t() noexcept{}

bool comm_intern_priv_attr_t::has_comm_dup() const{
    return ! comm_dup.is_null();
}
comm_intern_priv_attr_t & comm_intern_priv_attr_t::set_comm_dup( const Comm &comm ){
    comm_dup = comm.dup();
    int rank = comm_dup.rank(), size = comm_dup.size();
    prev = (rank == 0)?PROC_NULL:(rank-1);
    next = (rank == size-1)?PROC_NULL:(rank+1);
    return *this;
}
comm_intern_priv_attr_t & comm_intern_priv_attr_t::del_comm_dup(){
    comm_dup.free();
    prev = next = PROC_NULL;
    return *this;
}
void comm_intern_priv_attr_t::del_attr_fn( Comm &comm, int keyval, 
    void *attr_val, void *extra_state ){
    auto *ptr = (comm_intern_priv_attr_t *)attr_val;
    typedef MemObj<comm_intern_priv_attr_t> mem_t;
    if( ptr ){
        mem_t().destroy( ptr );
        mem_t::dealloc( ptr );
    }
}
} // namespace _mpi_env_helper

int Env::_comm_intern_priv_keyval = KEYVAL_INVALID;

Env::Env(){
    MPI_Init(NULL, NULL);
    _comm = MPI_COMM_WORLD;
    MPI_Comm_set_errhandler(_comm, MPI_ERRORS_RETURN);
    _init_predefined_objects();
    _init_comm_intern_attr();
}
Env::Env(int &argc, char **&argv ){
    MPI_Init(&argc, &argv);
    _comm = MPI_COMM_WORLD;
    MPI_Comm_set_errhandler(_comm, MPI_ERRORS_RETURN);
    _init_predefined_objects();
    _init_comm_intern_attr();
}
Env::Env(int &argc, char **&argv, int required, int &provided ){
    MPI_Init_thread( &argc, &argv, required, &provided );
    _comm = MPI_COMM_WORLD;
    MPI_Comm_set_errhandler(_comm, MPI_ERRORS_RETURN);
    _init_predefined_objects();
    _init_comm_intern_attr();
}
Env::~Env() noexcept{
    try{
        _free_comm_intern_attr();
        _free_predefined_objects();
    }catch(...){
        ErrMPI::abort(1, emFLPFB);
    }
    if( MPI_Finalize() != MPI_SUCCESS )
        ErrMPI::abort(1, emFLPFB);
}
ostream & Env::info(ostream &os, int fmt_cntl, int level) const {
    int rank, ver, subver, _tagub, _host, _io, _wt;
    ErrMPI::check( MPI_Comm_rank(_comm, &rank), emFLPFB );
    version(ver, subver);
    _tagub = tag_ub(); 
    _host = host(); 
    _io = io(); 
    _wt = wtime_is_global(); 
    
    PStream ps(os);
    if( fmt_cntl == 0 ){
        ps << HIPPCNTL_CLASS_INFO_INLINE(Env), 
            "{standard=", ver, ".", subver, "}";
        return os;
    }
    string procname = processor_name();
    auto ind = HIPPCNTL_CLASS_INFO_INDENT_STR(level);
    ps << HIPPCNTL_CLASS_INFO(Env),
        ind, "Standard=", ver, ".", subver, '\n',
        ind, "Runtime Environment {tag upper bound=", _tagub, ", host=";
    if(_host == PROC_NULL) ps << "NULL";
    else ps << _host;
    ps << ", IO rank=";
    if( _io == ANY_SOURCE ) ps << "ANY";
    else if( _io == PROC_NULL ) ps << "NULL";
    else if( _io == rank ) ps << "SELF";
    else ps << _io;
    ps << ", wtime is global=", bool(_wt)?"Yes":"No", "}\n",
        ind, " Processor name=", procname, '\n';
    if( fmt_cntl >= 2 ) {
        string libver = library_version();
        ps << "Library version -- \n", libver, '\n';
    }
    return os;
}
void Env::version( int &version, int &subversion ){   
    ErrMPI::check( 
        MPI_Get_version(&version, &subversion),
        emFLPFB );
}

string Env::library_version( ){
    vector<char> buff( MPI_MAX_LIBRARY_VERSION_STRING, '\0' );
    int reslen = 0;
    int e = MPI_Get_library_version( buff.data(), &reslen);
    ErrMPI::check(e, emFLPFB);
    return string( (const char *)buff.data() );
}
int Env::tag_ub( ) const{
    int value = 0;
    ErrMPI::check( _get_attr( MPI_TAG_UB, value ), emFLPFB);
    return value;
}
int Env::host( ) const{
    int value = 0;
    ErrMPI::check( _get_attr( MPI_HOST, value ), emFLPFB);
    return value;
}
int Env::io( ) const{
    int value = 0;
    ErrMPI::check( _get_attr( MPI_IO, value ), emFLPFB);
    return value;
}
int Env::wtime_is_global( ) const{
    int value = 0;
    ErrMPI::check( _get_attr( MPI_WTIME_IS_GLOBAL, value ), emFLPFB);
    return value;
}
string Env::processor_name(){
    vector<char> _name( MPI_MAX_PROCESSOR_NAME, '\0' );
    int reslen;
    int e = MPI_Get_processor_name( _name.data(), &reslen );
    ErrMPI::check( e, emFLPFB );
    return string(_name.data());
}
int Env::_get_attr( int keyval, int &attrval ) const noexcept {
    int *x = NULL, flag = 0;
    int e = MPI_Comm_get_attr(_comm, keyval, &x, &flag);
    attrval = *x;
    return e;
}
void Env::_init_comm_intern_attr(){
    _comm_intern_priv_keyval = 
        Comm::create_keyval( 
            Comm::NULL_COPY_FN, _comm_intern_priv_attr_t::del_attr_fn );
}
void Env::_free_comm_intern_attr(){
    Comm comm = Comm::world();
    void *attr = nullptr;
    if( comm.get_attr( _comm_intern_priv_keyval, attr ) )
        comm.del_attr( _comm_intern_priv_keyval );
    
    comm = Comm::selfval();
    if( comm.get_attr( _comm_intern_priv_keyval, attr ) )
        comm.del_attr( _comm_intern_priv_keyval );

    Comm::free_keyval( _comm_intern_priv_keyval );
}

void Env::_init_predefined_objects() {
    Datatype::_init_predefined_datatypes();
}

void Env::_free_predefined_objects() {
    Datatype::_free_predefined_datatypes();
}


} // namespace MPI
} // namespace HIPP
