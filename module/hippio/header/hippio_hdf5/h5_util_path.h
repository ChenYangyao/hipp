/**
create: Yangyao CHEN, 2021/10/19
    [write   ] SubpathIterator: path component resolver.
*/

#ifndef _HIPPIO_H5_UTIL_PATH_H_
#define _HIPPIO_H5_UTIL_PATH_H_

#include <hippcntl.h>

namespace HIPP::IO::H5 {

/**
SubpathIterator does not own the path string. Make sure that the source object
exists.
*/
class SubpathIterator {
public:
    SubpathIterator(const string &path);
    SubpathIterator(const char *path);
    SubpathIterator(const char *path, size_t length);

    bool has_next() const noexcept;
    bool next() noexcept;

    const char * c_str() const noexcept;
    string subpath() const noexcept;
    
    void reset();
protected:
    const char * const _path;
    const size_t _len;

    size_t _pos;
    vector<char> _subpath;
};

inline SubpathIterator::SubpathIterator(const string &path)
: SubpathIterator(path.c_str(), path.size())
{
    reset();
}

inline SubpathIterator::SubpathIterator(const char *path)
: SubpathIterator(path, strlen(path))
{
    reset();
}

inline SubpathIterator::SubpathIterator(const char *path, 
    size_t length)
: _path(path), _len(length)
{
    reset();
}

inline bool SubpathIterator::has_next() const noexcept {
    return _pos != _len;
}

inline bool SubpathIterator::next() noexcept {
    if( ! has_next() ) return false;

    _subpath[_pos] = _path[_pos];
    ++ _pos;
    
    while( _pos != _len && _path[_pos] != '/' ) {
        _subpath[_pos] = _path[_pos];
        ++ _pos;
    }

    return true;
}

inline const char * SubpathIterator::c_str() const noexcept {
    return _subpath.data();
}

inline string SubpathIterator::subpath() const noexcept {
    return string(c_str());
}

inline void SubpathIterator::reset() {
    _subpath.assign(_len + 1, '\0');
    _pos = 0;

    if( _len == 0 ) return;

    if( _path[0] == '.' ) {
        _subpath[0] = '.';
        _pos = 1;
    }
}


} // namespace HIPP::IO::H5

#endif	//_HIPPIO_H5_UTIL_PATH_H_