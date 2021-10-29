#include <hippio.h>

namespace HIPP::IO::H5 {

Proplist Dataset::create_proplist(const string &cls) {
    typedef Proplist pl_t;
    hid_t cls_val;
    if( cls == "c" || cls == "create" )
        cls_val = pl_t::dCREATE;
    else if( cls == "a" || cls == "access" )
        cls_val = pl_t::dACCESS;
    else if( cls == "x" || cls == "xfer" || cls == "transfer" )
        cls_val = pl_t::dXFER;
    else
        ErrLogic::throw_( ErrLogic::eDOMAIN, emFLPFB, 
            "   ... dataset property list class ", cls, " invalid\n" );
    return Proplist::create(cls_val);
}

Proplist Dataset::proplist(const string &cls) const{
    hid_t plist;
    if( cls == "c" || cls == "create" )
        plist = obj_raw().get_create_plist();
    else if( cls == "a" || cls == "access" )
        plist = obj_raw().get_access_plist();
    else 
        ErrLogic::throw_( ErrLogic::eDOMAIN, emFLPFB, 
            "   ... dataset property list class ", cls, " invalid\n");
    ErrH5::check(plist, emFLPFB);
    return Proplist::_from_raw(plist);
}

void Dataset::write(const void *buff, 
    const Datatype &memtype, const Dataspace &memspace,
    const Dataspace &filespace,
    const Proplist &xprop)
{
    obj_raw().write(memtype.raw(), memspace.raw(), 
        filespace.raw(), xprop.raw(), buff);
}

void Dataset::write_str(const string &s) {
    ConstDatapacketStr dp {s};
    write(dp.buff, dp.dtype);
}

void Dataset::write_str(const char *s) {
    ConstDatapacketStr dp {s};
    write(dp.buff, dp.dtype);
}

void Dataset::write_str(const vector<string> &ss) {
    write_str(ss.data(), ss.size());
}

void Dataset::write_str(const string ss[], size_t n_str) {
    ConstDatapacketStr dp {ss, n_str};
    vector<char> b;
    dp.buff_to(ss, n_str, b);
    write(b.data(), dp.dtype);
}

void Dataset::write_str(const char * const *ss, size_t n_str) {
    ConstDatapacketStr dp {ss, n_str};
    vector<char> b;
    dp.buff_to(ss, n_str, b);
    write(b.data(), dp.dtype);
}

void Dataset::read(void *buff, const Datatype &memtype,
    const Dataspace &memspace, const Dataspace &filespace,
    const Proplist &xprop) const
{
    obj_raw().read(memtype.raw(), memspace.raw(), filespace.raw(), 
        xprop.raw(), buff);
}

void Dataset::read_str(string &s) {
    auto dt = datatype();
    size_t len = dt.size();
    vector<char> buff(len);
    read(buff.data(), dt);
    s.assign(buff.data());
}

void Dataset::read_str(char *s) {
    auto dt = datatype();
    read(s, dt);
}

void Dataset::read_str(vector<string> &ss) {
    size_t n_str = dataspace().size();
    ss.resize(n_str);
    read_str(ss.data());
}

void Dataset::read_str(string ss[]) {
    auto dt = datatype();
    auto dsp = dataspace();
    size_t n_str = dsp.size(), len = dt.size(), tot_len = n_str * len;
    vector<char> buff(tot_len);
    read(buff.data(), dt);
    for(size_t i=0; i<n_str; ++i)
        ss[i].assign(buff.data() + i * len);
}

} // namespace HIPP::IO::H5