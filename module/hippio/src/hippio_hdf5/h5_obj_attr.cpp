#include <hippio.h>

namespace HIPP::IO::H5 {

string Attr::get_name() const {
    return obj_raw().get_name();
}

hsize_t Attr::get_storage_size() const {
    return obj_raw().get_storage_size();
}

void Attr::get_info(info_t &link_info) const {
    obj_raw().get_info(link_info);
}

Attr::info_t Attr::get_info() const {
    info_t info;
    get_info(info);
    return info;
}

void Attr::write(const void *buff, const Datatype &memtype) {
    obj_raw().write(memtype.raw(), buff);
}

void Attr::write_str(const string &s) {
    ConstDatapacketStr dp {s};
    write(dp.buff, dp.dtype);
}

void Attr::write_str(const char *s) {
    ConstDatapacketStr dp {s};
    write(dp.buff, dp.dtype);
}

void Attr::write_str(const vector<string> &ss) {
    write_str(ss.data(), ss.size());
}

void Attr::write_str(const char * const *ss, size_t n_str) {
    ConstDatapacketStr dp {ss, n_str};
    vector<char> b;
    dp.buff_to(ss, n_str, b);
    write(b.data(), dp.dtype);
}

void Attr::write_str(const string ss[], size_t n_str) {
    ConstDatapacketStr dp {ss, n_str};
    vector<char> b;
    dp.buff_to(ss, n_str, b);
    write(b.data(), dp.dtype);
}

void Attr::read(void *buff, const Datatype &memtype) {
    obj_raw().read(memtype.raw(), buff);
}

void Attr::read_str(string &s) {
    auto dt = datatype();
    size_t len = dt.size();
    vector<char> buff(len);
    read(buff.data(), dt);
    s.assign(buff.data());
}

void Attr::read_str(char *s) {
    auto dt = datatype();
    read(s, dt);
}

void Attr::read_str(vector<string> &ss) {
    size_t n_str = dataspace().size();
    ss.resize(n_str);
    read_str(ss.data());
}

void Attr::read_str(string ss[]) {
    auto dt = datatype();
    auto dsp = dataspace();
    size_t n_str = dsp.size(), len = dt.size(), tot_len = n_str * len;
    vector<char> buff(tot_len);
    read(buff.data(), dt);
    for(size_t i=0; i<n_str; ++i)
        ss[i].assign(buff.data() + i * len);
}


Attr NamedObj::_attr_create_detect_exists(const string &name, 
    const string &flag, const Proplist &aprop)
{
    if( attr_exists(name) ) {
        if( flag == "x" || flag == "excl" ) 
            ErrH5::throw_(-1, emFLPFB, "  ... attribute ", name, " exits\n");
        else if( flag == "ac" || flag == "ca" 
            || flag == "w" || flag == "trunc" ) 
        {
            return open_attr(name, aprop);
        }else 
            ErrLogic::throw_(ErrLogic::eDOMAIN, emFLPFB, 
                "  ... invalid attribute creation flag ", flag, '\n');
    }
    return Attr {nullptr};
}

} // namespace HIPP::IO::H5