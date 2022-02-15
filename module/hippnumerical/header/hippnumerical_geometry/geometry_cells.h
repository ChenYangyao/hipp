/**
create: Yangyao CHEN, 2022/02/14
    [write   ] Cells, VIndex - regular cells and index on it.
*/

#ifndef _HIPPNUMERICAL_GEOMETRY_CELLS_H_
#define _HIPPNUMERICAL_GEOMETRY_CELLS_H_

#include "geometry_rect.h"

namespace HIPP::NUMERICAL::GEOMETRY {

/**
N-dimensional vectorized index, i.e. indices along all dimensions. This is 
usually used as a label on a N-dimensional cell in mesh.
*/
template<typename _IndexT, int _DIM> 
class VIndex {
public:
    static_assert( std::is_integral_v<_IndexT> &&
        std::is_signed_v<_IndexT>, 
        "argument _IndexT is not a signed integral type" );

    inline static constexpr int DIM = _DIM;

    /**
    index_t: the underlying integral scalar type used to represent the indices.
    value_t: a ``Svec`` instantiation holding the indices.
    */
    using index_t = _IndexT;
    using value_t = SVec<index_t, DIM>;

    /**
    Constructors.
    (1): uninitialized.
    (2): set all the indices by a ``SVec`` instance.
    (3): set all the indices by a initializer list.
    */
    VIndex() noexcept;
    VIndex(const value_t &value) noexcept;
    VIndex(std::initializer_list<index_t> value) noexcept;

    /**
    Get the underlying ``SVec`` that holds the indices.
    */
    value_t & value() noexcept;
    const value_t & value() const noexcept;

    /**
    Get the index at dimension ``i``.
    */
    index_t & operator[](size_t i) noexcept;
    const index_t & operator[](size_t i) const noexcept;
protected:
    value_t _value;
};

/**
Rectangular cells in a mesh.
*/
template<typename _IndexT, int _DIM> 
class Cells {
public:
    inline static constexpr int DIM = _DIM;

    /**
    v_index_t: N-dimensional index labeling a cell.
    index_t: the flattened, one-dimensional index of a cell.
    n_cell_t: a ``SVec`` instantiation representing the number of cells 
    at each dimension.
    */
    using v_index_t = VIndex<_IndexT, _DIM>;
    using index_t   = typename v_index_t::index_t;
    using n_cell_t  = typename v_index_t::value_t;

    /**
    Constructors.
    (1): uninitialized.

    The following methods set the number of cells of the mesh
    (2): to be ``n_cell`` at each dimension.
    (3): by a ``SVec`` instance.
    (4): by a initializer list.
    */
    Cells() noexcept;
    Cells(index_t n_cell) noexcept;
    Cells(const n_cell_t &n_cell) noexcept;
    Cells(std::initializer_list<index_t> n_cell) noexcept;

    ostream & info(ostream &os = cout, int fmt_cntl = 0, int level = 0) const;
    friend ostream & operator<<(ostream &os, const Cells &c) {
        return c.info(os);
    }

    /**
    Retrieve the number of cells at each dimension as a ``SVec``.
    */
    const n_cell_t & n_cell() const noexcept;
    n_cell_t & n_cell() noexcept;

    /**
    Get the total number of cells at all dimensions, i.e., ``n_cell().prod()``.
    */
    index_t total_n_cell() const noexcept;
    
    /**
    Conversions between N-dimensional and one-dimensional flattened indices.
    */
    v_index_t idx2v_idx(index_t idx) const noexcept;
    index_t v_idx2idx(const v_index_t &v_idx) const noexcept;

    /**
    bound_v_idx(): modify the ``v_idx`` to be in the valid range, i.e., 
    set negative components to 0 and set components >= ``n_cell()`` to 
    ``n_cell() - 1``.
    v_idx_is_bound(): check whether or not a ``v_idx`` is in the valid range.
    */
    void bound_v_idx(v_index_t &v_idx) const noexcept;
    bool v_idx_is_bound(const v_index_t &v_idx) const noexcept;
protected:
    n_cell_t _n_cell;
};


#define _HIPP_TEMPHD template<typename _IndexT, int _DIM>
#define _HIPP_TEMPARG <_IndexT, _DIM>
#define _HIPP_TEMPCLS VIndex _HIPP_TEMPARG
#define _HIPP_TEMPRET _HIPP_TEMPHD inline auto _HIPP_TEMPCLS::
#define _HIPP_TEMPNORET _HIPP_TEMPHD inline _HIPP_TEMPCLS::

_HIPP_TEMPNORET
VIndex() noexcept {}

_HIPP_TEMPNORET
VIndex(const value_t & value) noexcept 
: _value(value) {}

_HIPP_TEMPNORET
VIndex(std::initializer_list<index_t> value) noexcept
: _value(value) {}

_HIPP_TEMPRET
value() noexcept -> value_t & {
    return _value;
}

_HIPP_TEMPRET
value() const noexcept -> const value_t & {
    return _value;
}

_HIPP_TEMPRET
operator[](size_t i) noexcept -> index_t & { 
    return _value[i]; 
}

_HIPP_TEMPRET
operator[](size_t i) const noexcept  -> const index_t & { 
    return _value[i]; 
}


#undef _HIPP_TEMPHD
#undef _HIPP_TEMPARG
#undef _HIPP_TEMPCLS
#undef _HIPP_TEMPRET
#undef _HIPP_TEMPNORET


#define _HIPP_TEMPHD template<typename _IndexT, int _DIM>
#define _HIPP_TEMPARG <_IndexT, _DIM>
#define _HIPP_TEMPCLS Cells _HIPP_TEMPARG
#define _HIPP_TEMPRET _HIPP_TEMPHD inline auto _HIPP_TEMPCLS::
#define _HIPP_TEMPNORET _HIPP_TEMPHD inline _HIPP_TEMPCLS::

_HIPP_TEMPNORET
Cells() noexcept {}

_HIPP_TEMPNORET
Cells(index_t n_cell) noexcept 
: _n_cell(n_cell) {}

_HIPP_TEMPNORET
Cells(const n_cell_t &n_cell) noexcept 
: _n_cell(n_cell) {}

_HIPP_TEMPNORET
Cells(std::initializer_list<index_t> n_cell) noexcept
: _n_cell(n_cell) {}

_HIPP_TEMPRET
info(ostream &os, int fmt_cntl, int level) const -> ostream & {
    PStream ps{os};
    auto &n = _n_cell;
    if( fmt_cntl < 1 ) {
        ps << HIPPCNTL_CLASS_INFO_INLINE(Cells),
        "{n_cell={", ps(n.begin(), n.end()), 
        "}, total_n_cell=", total_n_cell(), "}";
        return os;
    }
    auto ind = HIPPCNTL_CLASS_INFO_INDENT_STR(level);
    ps << HIPPCNTL_CLASS_INFO(Cells),
    ind, "No. cell = {", ps(n.begin(), n.end()), 
        "}, total no. cell = ", total_n_cell(), '\n';
    return os;
}

_HIPP_TEMPRET
n_cell() const noexcept -> const n_cell_t & {
    return _n_cell;
}

_HIPP_TEMPRET
n_cell() noexcept -> n_cell_t & {
    return _n_cell;
}

_HIPP_TEMPRET
total_n_cell() const noexcept -> index_t {
    return _n_cell.prod();
}

_HIPP_TEMPRET
idx2v_idx(index_t idx) const noexcept -> v_index_t {
    v_index_t v_idx;
    int d = DIM;
    while( d-- > 1 ) {
        v_idx[d] = idx % _n_cell[d];
        idx /= _n_cell[d];
    }
    v_idx[0] = idx;
}

_HIPP_TEMPRET
v_idx2idx(const v_index_t &v_idx) const noexcept -> index_t {
    index_t idx = v_idx[0];
    int d = 0;
    while( ++d < DIM ) {
        idx = idx * _n_cell[d] + v_idx[d];
    }
    return idx;
}

_HIPP_TEMPRET
bound_v_idx(v_index_t &v_idx) const noexcept -> void {
    auto &val = v_idx.value();
    val[ val < 0 ] = 0;
    val[ val >= _n_cell ] = _n_cell - 1;
}

_HIPP_TEMPRET
v_idx_is_bound(const v_index_t &v_idx) const noexcept -> bool {
    const auto &val = v_idx.value();
    return (val >= 0).all() && (val < _n_cell).all();
}

#undef _HIPP_TEMPHD
#undef _HIPP_TEMPARG
#undef _HIPP_TEMPCLS
#undef _HIPP_TEMPRET
#undef _HIPP_TEMPNORET


} // namespace HIPP::NUMERICAL::GEOMETRY

#endif	//_HIPPNUMERICAL_GEOMETRY_CELLS_H_