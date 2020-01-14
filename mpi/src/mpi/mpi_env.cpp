#include "mpi_env.h"
namespace HIPP{
namespace MPI{
Env::Env(){
    MPI_Init(NULL, NULL);
    _comm = MPI_COMM_WORLD;
    MPI_Comm_set_errhandler(_comm, MPI_ERRORS_RETURN);
}
Env::Env(int &argc, char **&argv ){
    MPI_Init(&argc, &argv);
    _comm = MPI_COMM_WORLD;
    MPI_Comm_set_errhandler(_comm, MPI_ERRORS_RETURN);
}
Env::Env(int &argc, char **&argv, int required, int &provided ){
    MPI_Init_thread( &argc, &argv, required, &provided );
    _comm = MPI_COMM_WORLD;
    MPI_Comm_set_errhandler(_comm, MPI_ERRORS_RETURN);
}
Env::~Env(){
    if( MPI_Finalize() != MPI_SUCCESS )
        ErrMPI::abort(1, emFLPFB);
}
ostream & Env::info( ostream &os, int fmt_cntl ) const{
    int rank, ver, subver, _tagub, _host, _io, _wt;
    ErrMPI::check( MPI_Comm_rank(_comm, &rank), emFLPFB );
    version(ver, subver);
    _tagub = tag_ub(); 
    _host = host(); 
    _io = io(); 
    _wt = wtime_is_global(); 
    
    if( fmt_cntl == 0 ){
        prt(os, HIPPCNTL_CLASS_INFO_INLINE(HIPP::MPI::Env));
        prt_f(os, "Standard: %d.%d", ver, subver) << endl;
    }
    if( fmt_cntl >= 1 ){
        string procname = processor_name();
        prt(os, HIPPCNTL_CLASS_INFO(HIPP::MPI::Env));
        prt_f(os, "  Standard: %d.%d", ver, subver) << endl;
        prt_f(os, "  Runtime Environment (TAG UB=%d, HOST=", _tagub);
        if( _host == MPI_PROC_NULL ) prt(os, "None");
        else prt(os, _host);
        prt(os, ", IO RANK=");
        if( _io == MPI_ANY_SOURCE ) prt(os, "Any");
        else if( _io == MPI_PROC_NULL ) prt(os, "None");
        else if( _io == rank ) prt(os, "Self");
        else prt(os, _io);
        prt(os, ", WTIME GLOBAL=", ( bool(_wt)?"Yes":"No" )) << ")\n";
        prt(os, "  Processor name: ", procname) << endl;
    }
    if( fmt_cntl >= 2 ){
        string libver = library_version();
        prt(os, "Library Version\n----------\n", libver) << endl;
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


} // namespace MPI
} // namespace HIPP
