#ifndef _HIPPIO_H5_TEST_INCL_H_
#define _HIPPIO_H5_TEST_INCL_H_

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <hippio.h>
#include <filesystem>

namespace HIPP::IO::H5 {


namespace fs = std::filesystem;
namespace gt = ::testing;

class H5TestBase {
public:
    virtual string fixture_name() =0;
};

class H5TestFile : public H5TestBase {
public:
    File create_file(string filename, string mode = "ac", 
        bool auto_remove=true);
    void remove_file(string filename, bool err_if_not_existing=false);
    string get_full_filename(string filename);
    void add_clear_up(string fname);
    void clear_up();
private:
    vector<string> _fnames_added;
};

inline File H5TestFile::create_file(string filename, string mode, 
    bool auto_remove) 
{
    auto f = File( get_full_filename(filename), mode);
    if( auto_remove ) add_clear_up(std::move(filename));
    return f;
}
inline void H5TestFile::remove_file(string filename, bool err_if_not_existing) {
    fs::path p{ get_full_filename(filename) };
    if( fs::exists(p) ) {
        fs::remove(p);
    } else if( err_if_not_existing )
        FAIL() << "cannot remove file" << p.string();
}
inline string H5TestFile::get_full_filename(string filename) {
    return fixture_name() + "_" + filename + ".h5";
}

inline void H5TestFile::add_clear_up(string fname) {
    _fnames_added.emplace_back(std::move(fname));
}

inline void H5TestFile::clear_up() {
    pout << "remove files:";
    for(auto &name: _fnames_added){
        pout << ' ', name;
        remove_file(name);
    }
    pout << " done.", endl;
}

} // namespace HIPP::IO::H5


#endif	//_HIPPIO_H5_TEST_INCL_H_