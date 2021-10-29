#ifndef _H5_UTIL_GEOMETRY_IMPL_H_
#define _H5_UTIL_GEOMETRY_IMPL_H_

#include "../hippio_hdf5/h5_util_geometry.h"

namespace HIPP::IO::H5 {

inline Dimensions::Dimensions() noexcept {}

inline Dimensions::Dimensions(std::initializer_list<value_t> dims) 
: _dims(dims) {}

template<typename T>
Dimensions::Dimensions(const T *p, size_t ndims) : _dims(p, p+ndims) {}

inline Dimensions::Dimensions(size_t n, value_t value) : _dims(n, value) {}

template<typename Buff,
    std::enable_if_t<ContiguousBufferTraits<Buff>::is_buffer, int> >
Dimensions::Dimensions(const Buff &dims) {
    auto [p,n] = ContiguousBufferTraits<const Buff>(dims);
    _dims.assign(p, p+n);
}

inline ostream & Dimensions::info(ostream &os, int fmt_cntl) const {
    PStream ps{os};
    ps << "Dimensions{", _dims, "}";
    if( fmt_cntl != 0 ) 
        ps << '\n';
    return os;
}

inline ostream & operator<<(ostream &os, const Dimensions &dims) {
    return dims.info(os);
}

inline Dimensions & Dimensions::set() noexcept {
    _dims.clear();
    return *this;
}

inline Dimensions & Dimensions::set(std::initializer_list<value_t> dims) {
    _dims = dims;
    return *this;
}

template<typename T>
Dimensions & Dimensions::set(const T *p, size_t n) {
    _dims.assign(p, p+n);
    return *this;
}

template<typename Buff,
    std::enable_if_t<ContiguousBufferTraits<Buff>::is_buffer, int> >
Dimensions & Dimensions::set(const Buff &dims) {
    auto [p,n] = ContiguousBufferTraits<const Buff>(dims);
    _dims.assign(p,p+n);
    return *this;
}

inline Dimensions & Dimensions::push_back(hsize_t x) {
    _dims.push_back(x);
    return *this;
}

inline Dimensions & Dimensions::push_back(std::initializer_list<value_t> dims) {
    return push_back(dims.begin(), dims.size());
}

template<typename T>
Dimensions & Dimensions::push_back(const T *p, size_t n) {
    _dims.insert(_dims.end(), p, p+n);
    return *this;
}

template<typename Buff,
    std::enable_if_t<ContiguousBufferTraits<Buff>::is_buffer, int> >
Dimensions & Dimensions::push_back(const Buff &dims) {
    auto [p,n] = ContiguousBufferTraits<const Buff>(dims);
    return push_back(p, n);
}

inline void Dimensions::resize(size_t new_ndims) {
    _dims.resize(new_ndims);
}

inline const Dimensions::value_t & Dimensions::operator[](size_t i) const {
    return _dims[i];
}

inline Dimensions::value_t & Dimensions::operator[](size_t i) {
    return _dims[i];
}

inline const Dimensions::value_t * Dimensions::data() const noexcept {
    return _dims.data();
}

inline Dimensions::value_t * Dimensions::data() noexcept {
    return _dims.data();
}

inline auto Dimensions::begin() const noexcept -> const_iterator {
    return _dims.data();
}

inline auto Dimensions::end() const noexcept -> const_iterator {
    return _dims.data() + _dims.size();
}

inline auto Dimensions::begin() noexcept -> iterator {
    return _dims.data();
}

inline auto Dimensions::end() noexcept -> iterator {
    return _dims.data() + _dims.size();
}

inline auto Dimensions::cbegin() const noexcept -> const_iterator {
    return _dims.data();
}

inline auto Dimensions::cend() const noexcept -> const_iterator {
    return _dims.data() + _dims.size();
}

inline const vector<Dimensions::value_t> & 
Dimensions::as_vector() const noexcept 
{
    return _dims;
}

inline vector<Dimensions::value_t> & Dimensions::as_vector() noexcept {
    return _dims;
}

inline size_t Dimensions::ndims() const noexcept {
    return size();
}

inline size_t Dimensions::size() const noexcept {
    return _dims.size();
}

inline bool Dimensions::empty() const noexcept {
    return _dims.empty();
}

inline Dimensions::value_t Dimensions::n_elems() const noexcept {
    const value_t *b = data(), *e = b + ndims();
    return std::accumulate(b, e, value_t(1), std::multiplies<value_t>());
}

inline bool Dimensions::operator==(const Dimensions &o) const noexcept {
    size_t nd = ndims();
    if( nd != o.ndims() )
        return false;
    return std::equal(_dims.begin(), _dims.end(), o._dims.begin());
}

inline bool Dimensions::operator!=(const Dimensions &o) const noexcept {
    return ! this->operator==(o);
}

inline Points::Points() noexcept : Points(0, nullptr) {}

inline Points::Points(size_t n_points, size_t n_dims, const value_t *coords) 
noexcept : _n_points(n_points), _n_dims(n_dims), _coords(coords)
{}

inline Points::Points(size_t n_points, value_t *coords) noexcept
: Points(n_points, 0, coords)
{}

inline auto Points::coords() const noexcept -> const value_t * {
    return _coords;
}

inline size_t Points::n_points() const noexcept {
    return _n_points;
}

inline size_t Points::n_dims() const noexcept {
    return _n_dims;
}

inline Chunk::Chunk() noexcept {}

inline Chunk::Chunk(const Dimensions &start, const Dimensions &count) 
: _start(start), _count(count)
{}

template<typename T1, typename T2>
Chunk::Chunk(const T1 *start, const T2 *count, size_t ndims) 
: _start(start, ndims), _count(count, ndims)
{}

inline const Dimensions & Chunk::start() const {
    return _start;
}

inline const Dimensions & Chunk::count() const {
    return _count;
}

inline Dimensions & Chunk::start() {
    return _start;
}

inline Dimensions & Chunk::count() {
    return _count;
}

inline Hyperslab::Hyperslab() noexcept {}

inline Hyperslab::Hyperslab(const Chunk &chunk)
: Hyperslab(chunk.start(), chunk.count())
{ }
    
inline Hyperslab::Hyperslab(const Dimensions &start, const Dimensions &count)
: _start(start), _count(count)
{ }

inline Hyperslab::Hyperslab(const Dimensions &start, const Dimensions &count, 
    const Dimensions &stride, const Dimensions &block)
: _start(start), _count(count), _stride(stride), _block(block)
{ }

template<typename T1, typename T2>
inline Hyperslab::Hyperslab(const T1 *start, const T2 *count, size_t ndims) 
: _start(start, ndims), _count(count, ndims)
{ }

template<typename T1, typename T2, typename T3, typename T4>
inline Hyperslab::Hyperslab(const T1 *start, const T2 *count, 
    const T3 *stride, const T4 *block, size_t ndims)
: _start(start, ndims), _count(count, ndims), 
_stride(), _block()
{
    if( stride )
        _stride.set(stride, ndims);
    if( block )
        _block.set(block, ndims);
}

inline Hyperslab Hyperslab::subsetting_1d(Dimensions full_dims, 
    size_t dim, const std::array<hsize_t, 4> &select_1d) 
{
    const size_t ndims = full_dims.ndims();
    if( dim >= ndims ) 
        ErrLogic::throw_(ErrLogic::eLENGTH, emFLPFB, 
        "  ... dim ", dim, " >= ndims ", ndims, '\n');

    vector<value_t> sel(4*ndims, 0);
    std::copy_n(full_dims.data(), ndims, &sel[ndims]);
    std::fill_n(&sel[ndims*2], 2*ndims, 1);

    size_t off = dim;
    for(size_t i=0; i<4; ++i){
        sel[off] = select_1d[i];
        off += ndims;
    }
    
    return Hyperslab(&sel[0], &sel[ndims], &sel[2*ndims], 
        &sel[3*ndims], ndims);
}

inline const Dimensions & Hyperslab::start() const noexcept {
    return _start;
}

inline const Dimensions & Hyperslab::count() const noexcept {
    return _count;
}

inline const Dimensions & Hyperslab::stride() const noexcept {
    return _stride;
}

inline const Dimensions & Hyperslab::block() const noexcept {
    return _block;
}

inline Dimensions & Hyperslab::start() noexcept {
    return _start;
}

inline Dimensions & Hyperslab::count() noexcept {
    return _count;
}

inline Dimensions & Hyperslab::stride() noexcept {
    return _stride;
}

inline Dimensions & Hyperslab::block() noexcept {
    return _block;
}

inline auto Hyperslab::data() const 
-> std::tuple<const value_t*, const value_t*, const value_t*, const value_t*>
{
    return { _start.data(), _count.data(), 
        _stride.empty()?nullptr:_stride.data(), 
        _block.empty()?nullptr:_block.data() };
}

inline auto Hyperslab::data() 
-> std::tuple<value_t*, value_t*, value_t*, value_t*>
{
    return { _start.data(), _count.data(), 
        _stride.empty()?nullptr:_stride.data(), 
        _block.empty()?nullptr:_block.data() };
}


} // namespace HIPP::IO::H5

#endif	//_H5_UTIL_GEOMETRY_IMPL_H_