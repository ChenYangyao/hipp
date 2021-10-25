/**
create: Yangyao CHEN, 2020/11/29
    [write   ] _File - Intermediate-level HDF5 file API.
*/ 

#ifndef _HIPPIO_H5_RAW_FILE_H_
#define _HIPPIO_H5_RAW_FILE_H_
#include "h5_raw_base.h"
#include "h5_raw_attr.h"
#include "h5_raw_dataset.h"
#include "h5_raw_proplist.h"
#include <filesystem>

namespace HIPP::IO::H5 {

class _File: public _Group {
public:
    typedef _Group parent_t;
    
    typedef unsigned int acc_flag_t;
    inline static const acc_flag_t
        accTRUNC   = H5F_ACC_TRUNC,
        accEXCL    = H5F_ACC_EXCL,
        accRDONLY  = H5F_ACC_RDONLY,
        accRDWR    = H5F_ACC_RDWR;

    typedef H5F_scope_t scope_t;
    inline static const scope_t 
        scopeLOCAL  = H5F_SCOPE_LOCAL,
        scopeGLOBAL = H5F_SCOPE_GLOBAL;

    explicit _File(hid_t file, int state = stFREE) noexcept;

    /**
    Create a new file or open an existing file.
    @name:  file name
    @flag:  "w" - create and truncate, then open as R/W mode.
            "x" - exclusively create (failed if existing), 
                  then open as R/W mode.
            "ac" | "ca" - 
                  open existing file as R/W mode. Create it if 
                  not existing.
            "r" - open existing file as R mode.
            "a" - open existing file as R/W mode.
    @cprop, aprop:   
            creation and access property lists. If to open existing one, cprop
            is ignored.
    */
    _File(const char *name, const char *flag, 
        hid_t cprop = _Proplist::vDFLT, hid_t aprop = _Proplist::vDFLT);

    virtual ~_File() noexcept override;

    /**
    @flag: ACC_RDONLY or ACC_RDWR.
    */
    static hid_t open(const char *name, acc_flag_t flag, 
        hid_t aprop = _Proplist::vDFLT);

    /**
    @flag: ACC_TRUNC or ACC_RDWR.
    */
    static hid_t create(const char *name, acc_flag_t flag,
        hid_t cprop = _Proplist::vDFLT, hid_t aprop = _Proplist::vDFLT);
    
    static void close(hid_t file);

    hsize_t get_filesize() const;

    static ssize_t get_filename(hid_t obj, char *name, size_t size);
    static string get_filename(hid_t obj);

    static void flush(hid_t obj, scope_t scope);
};

inline _File::_File(hid_t file, int state) noexcept 
: parent_t(file, state)
{ }

inline _File::_File(const char *name, const char *flag, hid_t cprop, 
    hid_t aprop ) 
: _File(-1, 0)
{
    namespace fs = std::filesystem;
    string flag_s(flag);
    hid_t obj;
    if( flag_s == "w" ){
        obj = create(name, accTRUNC, cprop, aprop);
    }else if( flag_s == "x" ){
        obj = create(name, accEXCL, cprop, aprop);
    }else if( flag_s == "r" ){
        obj = open(name, accRDONLY, aprop);
    }else if ( flag_s == "a" ){
        obj = open(name, accRDWR, aprop);
    }else if( flag_s == "ac" || flag_s == "ca" ) {
        if( fs::exists(name) ) 
            obj = open(name, accRDWR, aprop);
        else 
            obj = create(name, accEXCL, cprop, aprop);
    }else
        ErrLogic::throw_(ErrLogic::eINVALIDARG, emFLPFB, 
            "  ... invalid file open/access flag ", flag_s, '\n');
    ErrH5::check(obj, emFLPFB);
    
    _obj = obj; 
    state_flag(stFREE);
}

inline _File::~_File() noexcept { 
    if(is_auto_free()) {
        close(raw()); state_flag(0);
    }
}

inline hid_t _File::open(const char *name, acc_flag_t flag, hid_t aprop) {
    auto ret = H5Fopen(name, flag, aprop);
    ErrH5::check(ret, emFLPFB, "  ... open file ", name, 
        " (flag=", flag, ") failed\n");
    return ret;
}

inline hid_t _File::create(const char *name, acc_flag_t flag,
    hid_t cprop, hid_t aprop)
{
    auto ret = H5Fcreate(name, flag, cprop, aprop);
    ErrH5::check(ret, emFLPFB, "  ... create file ", name, 
        " (flag=", flag, ") failed\n");
    return ret;
}

inline void _File::close(hid_t file) {
    ErrH5::check(H5Fclose(file), emFLPFB); 
}

inline hsize_t _File::get_filesize() const {
    hsize_t sz;
    ErrH5::check(H5Fget_filesize(raw(), &sz), emFLPFB, 
        "  ... get file size failed\n");
    return sz;
}

inline ssize_t _File::get_filename(hid_t obj, char *name, size_t size) {
    auto ret = H5Fget_name(obj, name, size);
    ErrH5::check(ret, emFLPFB, "  ... get file name failed\n");
    return ret;
}

inline string _File::get_filename(hid_t obj) {
    auto need_sz = get_filename(obj, NULL, 0);
    string s(need_sz, '\0');
    auto actual_sz = get_filename(obj, s.data(), need_sz+1);
    if( actual_sz != need_sz ) 
        ErrRuntime::throw_(ErrRuntime::eUNDERFLOW, emFLPFB, 
            "  ... get file name failed (expected ", need_sz, " characters ", 
            "but got ", actual_sz, ")\n");
    return s;
}

inline void _File::flush(hid_t obj, scope_t scope) {
    ErrH5::check(H5Fflush(obj, scope), emFLPFB);
}

} // namespace HIPP::IO::H5

#endif	//_HIPPIO_H5_RAW_FILE_H_