#include <hippio.h>

namespace HIPP::IO::H5{

File::File( const string &name, const string &flag, 
    const Proplist &cprop, const Proplist &aprop) 
: File { _from_raw(name.c_str(), flag.c_str(), cprop.raw(), aprop.raw()) }
{}

File File::open(const string &name, const string &flag, const Proplist &aprop) {
    _obj_raw_t::acc_flag_t flag_val;
    if( flag == "r" ) {
        flag_val = _obj_raw_t::accRDONLY;
    } else if (flag == "a"){
        flag_val = _obj_raw_t::accRDWR;
    } else
        ErrLogic::throw_(ErrLogic::eDOMAIN, emFLPFB, 
            "  ... invalid flag ", flag, '\n');
    hid_t f = _obj_raw_t::open(name.c_str(), flag_val, aprop.raw());
    return File { _from_raw(f) };
}

File File::create(const string &name, const string &flag, const Proplist &cprop, 
    const Proplist &aprop) 
{
    _obj_raw_t::acc_flag_t flag_val;
    if( flag == "w" ) {
        flag_val = _obj_raw_t::accTRUNC;
    } else if (flag == "x"){
        flag_val = _obj_raw_t::accEXCL;
    } else
        ErrLogic::throw_(ErrLogic::eDOMAIN, emFLPFB, 
            "  ... invalid flag ", flag, '\n');
    hid_t f = _obj_raw_t::create(name.c_str(), flag_val, cprop.raw(), 
        aprop.raw());
    return File { _from_raw(f) };
}

Proplist File::create_proplist(const string &cls){
    hid_t cls_val {};
    if( cls == "c" || cls == "create" )
        cls_val = Proplist::fCREATE;
    else if( cls == "a" || cls == "access" )
        cls_val = Proplist::fACCESS;
    else if( cls == "m" || cls == "mount" )
        cls_val = Proplist::fMOUNT;
    else 
        ErrLogic::throw_( ErrLogic::eDOMAIN, emFLPFB, 
            "   ... file property list class ", cls, " invalid "  );
    return Proplist::create(cls_val);
}

} // namespace HIPP::IO::H5
