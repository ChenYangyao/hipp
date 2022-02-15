#ifndef _HIPPMPI_MPI_RAW_FILE_H_
#define _HIPPMPI_MPI_RAW_FILE_H_
#include "mpi_base.h"
#include "mpi_error.h"
namespace HIPP{
namespace MPI{
class _File{
public:
    typedef MPI_File mpi_t;

    typedef MPI_Datarep_extent_function datarep_extent_fn_t;
    typedef MPI_Datarep_conversion_function datarep_conversion_fn_t;
    inline static const datarep_conversion_fn_t 
        * const CONVERSION_FN_NULL = MPI_CONVERSION_FN_NULL;

    typedef MPI_File_errhandler_function errhandler_fn_t;
    enum : size_t{ MAX_DATAREP_STRING=MPI_MAX_DATAREP_STRING };
    enum : int { 
        MODE_RDONLY =    MPI_MODE_RDONLY, 
        MODE_RDWR =      MPI_MODE_RDWR,
        MODE_WRONLY =    MPI_MODE_WRONLY, 
        
        MODE_CREATE =    MPI_MODE_CREATE,
        MODE_EXCL =      MPI_MODE_EXCL,
        
        MODE_DELETE_ON_CLOSE =   MPI_MODE_DELETE_ON_CLOSE,
        MODE_UNIQUE_OPEN =       MPI_MODE_UNIQUE_OPEN,
        MODE_SEQUENTIAL =        MPI_MODE_SEQUENTIAL,
        MODE_APPEND =            MPI_MODE_APPEND };
    
    _File( mpi_t val, int state ): _val(val), _state(state){ }
    ~_File() noexcept {
        if( _state != 0 && !is_null() ){
            if( MPI_File_close(&_val) != MPI_SUCCESS )
                ErrMPI::abort(1, emFLPFB);
        }
    }
    static mpi_t open( MPI_Comm comm, const char *name, 
        int amode, MPI_Info info ){
        mpi_t fh;
        ErrMPI::check(
            MPI_File_open( comm, name, amode, info, &fh ), emFLPFB, 
                "  ... open file ", name, 
                " with access mode ", amode, " failed\n");
        return fh;
    }
    static void close( mpi_t &fh ){
        ErrMPI::check( MPI_File_close(&fh), emFLPFB );
    }
    static void del( const char *name, MPI_Info info ){
        ErrMPI::check( MPI_File_delete( name, info ), emFLPFB, 
            "  ... delete file ", name, " failed\n" );
    }

    bool is_null() const noexcept { return _val == nullval(); }
    static mpi_t nullval() noexcept { return MPI_FILE_NULL; }
    
    void set_size( offset_t size ){
        ErrMPI::check( MPI_File_set_size( _val, size ), 
            emFLPFB, "  ... set size ", size, " failed\n" );
    }
    void preallocate( offset_t size ){
        ErrMPI::check( MPI_File_preallocate(_val, size), 
            emFLPFB, "  ... pre-allocate ", size, " failed\n" );
    }
    offset_t get_size()const{
        offset_t size;
        ErrMPI::check( MPI_File_get_size(_val, &size),
            emFLPFB );
        return size;
    }
    MPI_Group get_group()const{
        MPI_Group group;
        ErrMPI::check( MPI_File_get_group( _val, &group ), emFLPFB );
        return group;
    }
    int get_amode()const{
        int amode;
        ErrMPI::check( MPI_File_get_amode( _val, &amode ), emFLPFB );
        return amode;
    }

    void set_info( MPI_Info info ){
        ErrMPI::check( MPI_File_set_info( _val, info ), emFLPFB );
    }
    MPI_Info get_info() const {
        MPI_Info info_used;
        ErrMPI::check( MPI_File_get_info( _val, &info_used ), emFLPFB );
        return info_used;
    }
    void set_view(offset_t disp, MPI_Datatype etype, MPI_Datatype filetype,
        const char *datarep, MPI_Info info){
        ErrMPI::check( MPI_File_set_view( _val, disp, etype,
            filetype, datarep, info ), emFLPFB );
    }
    void get_view(offset_t &disp, MPI_Datatype &etype, MPI_Datatype &filetype,
        string &datarep) const{
        char _datarep[MPI_MAX_DATAREP_STRING];
        ErrMPI::check( MPI_File_get_view(_val, &disp, &etype, &filetype, 
            _datarep), emFLPFB );
        datarep.assign(_datarep );
    }
    aint_t get_type_extent(MPI_Datatype datatype)const{
        aint_t ext;
        ErrMPI::check( MPI_File_get_type_extent(_val, datatype, &ext), emFLPFB);
        return ext;
    }
    void set_atomicity( int flag ){
        ErrMPI::check( MPI_File_set_atomicity(_val, flag), emFLPFB, 
            "  ... flag=", flag, '\n' );
    }
    int get_atomicity()const{
        int flag;
        ErrMPI::check( MPI_File_get_atomicity(_val, &flag), emFLPFB );
        return flag;
    }
    void sync(){
        ErrMPI::check(MPI_File_sync(_val), emFLPFB);
    }

    /**
     * read/write with explicit-offset
     * Both blocking/non-blocking, independent/collective versions are provided.
     */
    MPI_Status read_at( offset_t offset, void *buf, int count, 
        MPI_Datatype datatype ){
        MPI_Status status;
        ErrMPI::check( MPI_File_read_at( _val, offset, buf, count, datatype, 
            &status ), emFLPFB, "  ... offset/buf/count=", 
            offset, '/' ,buf, '/', count, '\n' );
        return status;
    }
    MPI_Status read_at_all(offset_t offset, void *buf, int count, 
        MPI_Datatype datatype){
        MPI_Status status;
        ErrMPI::check( MPI_File_read_at_all(_val, offset, buf, count, datatype, 
            &status), emFLPFB, "  ... offset/buf/count=", 
            offset, '/' ,buf, '/', count, '\n' );
        return status;
    }
    MPI_Status write_at( offset_t offset, const void *buf, int count, 
        MPI_Datatype datatype ){
        MPI_Status status;
        ErrMPI::check( MPI_File_write_at( _val, offset, buf, count, 
            datatype, &status ), emFLPFB, 
            "  ... offset/buf/count=",
            offset, '/', buf, '/', count, '\n' );
        return status;
    }
    MPI_Status write_at_all( offset_t offset, const void *buf, int count, 
        MPI_Datatype datatype ){
        MPI_Status status;
        ErrMPI::check( MPI_File_write_at_all( _val, offset, buf, count, 
            datatype, &status ), emFLPFB, 
            "  ... offset/buf/count=",
            offset, '/', buf, '/', count, '\n' );
        return status;
    }
    MPI_Request iread_at( offset_t offset, void *buf, int count, 
        MPI_Datatype datatype ){
        MPI_Request rq;
        ErrMPI::check( MPI_File_iread_at( _val, offset, buf, count, datatype, 
            &rq ), emFLPFB, "  ... offset/buf/count=", 
            offset, '/' ,buf, '/', count, '\n' );
        return rq;
    }
    /*
    MPI_Request iread_at_all( offset_t offset, void *buf, int count, 
        MPI_Datatype datatype ){
        MPI_Request rq;
        ErrMPI::check( MPI_File_iread_at_all( _val, offset, buf, count, 
            datatype, &rq ), emFLPFB, 
            "  ... offset/buf/count=", 
            offset, '/' ,buf, '/', count, '\n' );
        return rq;
    }*/
    MPI_Request iwrite_at( offset_t offset, const void *buf, int count, 
        MPI_Datatype datatype ){
        MPI_Request rq;
        ErrMPI::check( MPI_File_iwrite_at( _val, offset, buf, count, 
            datatype, &rq ), emFLPFB, 
            "  ... offset/buf/count=", 
            offset, '/' ,buf, '/', count, '\n' );
        return rq;
    }/*
    MPI_Request iwrite_at_all( offset_t offset, const void *buf, int count, 
        MPI_Datatype datatype ){
        MPI_Request rq;
        ErrMPI::check( MPI_File_iwrite_at_all( _val, offset, buf, count, 
            datatype, &rq ), emFLPFB, 
            "  ... offset/buf/count=", 
            offset, '/' ,buf, '/', count, '\n' );
        return rq;
    }*/
    void read_at_all_begin( offset_t offset, void *buf, int count, 
        MPI_Datatype datatype ){
        ErrMPI::check( MPI_File_read_at_all_begin( _val, offset, buf, count, 
            datatype ), emFLPFB, 
            "  ... offset/buf/count=", 
            offset, '/' ,buf, '/', count, '\n' );
    }
    MPI_Status read_at_all_end( void *buf ){
        MPI_Status status;
        ErrMPI::check( MPI_File_read_at_all_end( _val, buf, &status), emFLPFB, 
            "  ... buf=", buf, '\n' );
        return status;
    }
    void write_at_all_begin( offset_t offset, const void *buf, int count, 
        MPI_Datatype datatype ){
        ErrMPI::check( MPI_File_write_at_all_begin( _val, offset, buf, count, 
            datatype ), emFLPFB, 
            "  ... offset/buf/count=", 
            offset, '/' ,buf, '/', count, '\n' );
    }
    MPI_Status write_at_all_end( const void *buf ){
        MPI_Status status;
        ErrMPI::check( MPI_File_write_at_all_end( _val, buf, &status), emFLPFB, 
            "  ... buf=", buf, '\n' );
        return status;
    }
    /**
     * data access with indivisual file pointer
     * Blocking/non-blocking, independent/collective versions are provided.
     */
    void seek(offset_t offset, int whence){
        ErrMPI::check( MPI_File_seek(_val, offset, whence), emFLPFB, 
            "  ... offset/whence=", offset, "/", whence, '\n' );
    }
    offset_t get_position()const{
        offset_t offset;
        ErrMPI::check( MPI_File_get_position(_val, &offset), emFLPFB);
        return offset;
    }
    offset_t get_byte_offset(offset_t offset)const{
        offset_t disp;
        ErrMPI::check( MPI_File_get_byte_offset(_val, offset, &disp),
            emFLPFB, "  ... offset=", offset, '\n' );
        return disp;
    }
    MPI_Status read( void *buf, int count, MPI_Datatype datatype ){
        MPI_Status status;
        ErrMPI::check( MPI_File_read( _val, buf, count, datatype, &status ),
            emFLPFB, "  ... buf/count=", 
            buf, '/', count, '\n' );
        return status;
    }
    MPI_Status read_all( void *buf, int count, MPI_Datatype datatype ){
        MPI_Status status;
        ErrMPI::check( MPI_File_read_all( _val, buf, count, datatype, &status ),
            emFLPFB, "  ... buf/count=", 
            buf, '/', count, '\n' );
        return status;
    }
    MPI_Status write( const void *buf, int count, MPI_Datatype datatype ){
        MPI_Status status;
        ErrMPI::check( MPI_File_write( _val, buf, count, datatype, &status ),
            emFLPFB, "  ... buf/count=", 
            buf, '/', count, '\n' );
        return status;
    }
    MPI_Status write_all( const void *buf, int count, MPI_Datatype datatype ){
        MPI_Status status;
        ErrMPI::check( MPI_File_write_all( _val, buf, count, datatype, &status ),
            emFLPFB, "  ... buf/count=", 
            buf, '/', count, '\n' );
        return status;
    }
    MPI_Request iread( void *buf, int count, MPI_Datatype datatype ){
        MPI_Request rq;
        ErrMPI::check( MPI_File_iread( _val, buf, count, datatype, &rq ),
            emFLPFB, "  ... buf/count=", 
            buf, '/', count, '\n' );
        return rq;
    }
    /*
    MPI_Request iread_all( void *buf, int count, MPI_Datatype datatype ){
        MPI_Request rq;
        ErrMPI::check( MPI_File_iread_all( _val, buf, count, datatype, &rq ),
            emFLPFB, "  ... buf/count=", 
            buf, '/', count, '\n' );
        return rq;
    }*/
    MPI_Request iwrite( const void *buf, int count, MPI_Datatype datatype ){
        MPI_Request rq;
        ErrMPI::check( MPI_File_iwrite( _val, buf, count, datatype, &rq ),
            emFLPFB, "  ... buf/count=", 
            buf, '/', count, '\n' );
        return rq;
    }/*
    MPI_Request iwrite_all( const void *buf, int count, MPI_Datatype datatype ){
        MPI_Request rq;
        ErrMPI::check( MPI_File_iwrite_all( _val, buf, count, datatype, &rq ),
            emFLPFB, "  ... buf/count=", 
            buf, '/', count, '\n' );
        return rq;
    }*/
    void read_all_begin( void *buf, int count, 
        MPI_Datatype datatype ){
        ErrMPI::check( MPI_File_read_all_begin( _val, buf, count, 
            datatype ), emFLPFB, 
            "  ... buf/count=", buf, '/', count, '\n' );
    }
    MPI_Status read_all_end( void *buf ){
        MPI_Status status;
        ErrMPI::check( MPI_File_read_all_end( _val, buf, &status), emFLPFB, 
            "  ... buf=", buf, '\n' );
        return status;
    }
    void write_all_begin( const void *buf, int count, 
        MPI_Datatype datatype ){
        ErrMPI::check( MPI_File_write_all_begin( _val, buf, count, 
            datatype ), emFLPFB, 
            "  ... buf/count=", buf, '/', count, '\n' );
    }
    MPI_Status write_all_end( const void *buf ){
        MPI_Status status;
        ErrMPI::check( MPI_File_write_all_end( _val, buf, &status), emFLPFB, 
            "  ... buf=", buf, '\n' );
        return status;
    }
    /**
     * data access with shared file pointer
     * Blocking/non-blocking, independent/collective(ordered) versions are
     * provided.
     */
    void seek_shared(offset_t offset, int whence){
        ErrMPI::check( MPI_File_seek_shared(_val, offset, whence), emFLPFB, 
            "  ... offset/whence=", offset, "/", whence, '\n' );
    }
    offset_t get_position_shared()const{
        offset_t offset;
        ErrMPI::check( MPI_File_get_position_shared(_val, &offset), emFLPFB);
        return offset;
    }
    MPI_Status read_shared( void *buf, int count, MPI_Datatype datatype ){
        MPI_Status status;
        ErrMPI::check( MPI_File_read_shared( 
            _val, buf, count, datatype, &status ),
            emFLPFB, "  ... buf/count=", 
            buf, '/', count, '\n' );
        return status;
    }
    MPI_Status write_shared( const void *buf, int count, MPI_Datatype datatype ){
        MPI_Status status;
        ErrMPI::check( MPI_File_write_shared( 
            _val, buf, count, datatype, &status ),
            emFLPFB, "  ... buf/count=", 
            buf, '/', count, '\n' );
        return status;
    }
    MPI_Request iread_shared( void *buf, int count, MPI_Datatype datatype ){
        MPI_Request rq;
        ErrMPI::check( MPI_File_iread_shared( 
            _val, buf, count, datatype, &rq ),
            emFLPFB, "  ... buf/count=", 
            buf, '/', count, '\n' );
        return rq;
    }
    MPI_Request iwrite_shared( const void *buf, int count, MPI_Datatype datatype ){
        MPI_Request rq;
        ErrMPI::check( MPI_File_iwrite_shared( 
            _val, buf, count, datatype, &rq ),
            emFLPFB, "  ... buf/count=", 
            buf, '/', count, '\n' );
        return rq;
    }
    MPI_Status read_ordered( void *buf, int count, MPI_Datatype datatype ){
        MPI_Status status;
        ErrMPI::check( MPI_File_read_ordered( 
            _val, buf, count, datatype, &status ),
            emFLPFB, "  ... buf/count=", 
            buf, '/', count, '\n' );
        return status;
    }
    MPI_Status write_ordered( const void *buf, int count, MPI_Datatype datatype ){
        MPI_Status status;
        ErrMPI::check( MPI_File_write_ordered( 
            _val, buf, count, datatype, &status ),
            emFLPFB, "  ... buf/count=", 
            buf, '/', count, '\n' );
        return status;
    }
    void read_ordered_begin( void *buf, int count, 
        MPI_Datatype datatype ){
        ErrMPI::check( MPI_File_read_ordered_begin( _val, buf, count, 
            datatype ), emFLPFB, 
            "  ... buf/count=", buf, '/', count, '\n' );
    }
    MPI_Status read_ordered_end( void *buf ){
        MPI_Status status;
        ErrMPI::check( MPI_File_read_ordered_end( _val, buf, &status), emFLPFB, 
            "  ... buf=", buf, '\n' );
        return status;
    }
    void write_ordered_begin( const void *buf, int count, 
        MPI_Datatype datatype ){
        ErrMPI::check( MPI_File_write_ordered_begin( _val, buf, count, 
            datatype ), emFLPFB, 
            "  ... buf/count=", buf, '/', count, '\n' );
    }
    MPI_Status write_ordered_end( const void *buf ){
        MPI_Status status;
        ErrMPI::check( MPI_File_write_ordered_end( _val, buf, &status), emFLPFB, 
            "  ... buf=", buf, '\n' );
        return status;
    }    

protected:
    mpi_t _val;
    int _state;

    
};  
} // namespace MPI
} // namespace HIPP


#endif	//_HIPPMPI_MPI_RAW_FILE_H_