/**
create: Yangyao CHEN, 2022/01/28
    [write   ] Mesh: rectangular-cell space division algorithm.
*/

#ifndef _HIPPNUMERICAL_GEOMETRY_MESH_H_
#define _HIPPNUMERICAL_GEOMETRY_MESH_H_

#include "geometry_cells.h"

namespace HIPP::NUMERICAL::GEOMETRY {

/**
Rectangular-cell space-division algorithm. 

A ``Mesh`` instance = a bounding rectangle ``rect`` + the specification of 
cell division algorithm ``cells`` (i.e., no. of cells at all dimensions).

The bounding rectangle naturally provides a local frame, where each spatial
point has a local frame coordinate.

To work in a defined way, the bounding rectangle must have low < high at all 
dimensions.

Template parameters
------
@_FloatT: the type for spatial position/coordinate.
@_IndexT: the type for denoting the number of cells.
@_DIM: no. of dimensions.
*/
template<typename _FloatT, typename _IndexT, int _DIM> 
class Mesh {
public:
    inline static constexpr int DIM = _DIM;
    using float_t = _FloatT;
    using index_t = _IndexT;

    using rect_t      = Rect<float_t, DIM>;
    using point_t     = typename rect_t::point_t;
    using offset_t    = typename rect_t::offset_t;
    using coord_t     = typename rect_t::coord_t;
    using rect_size_t = typename rect_t::rect_size_t;

    using cells_t   = Cells<index_t, DIM>;
    using v_index_t = typename cells_t::v_index_t;
    using n_cell_t  = typename cells_t::n_cell_t;

    /**
    Constructors.
    (1): uninitialized.
    (2): set the bounding rectangle and space division by ``rect`` and 
    ``cells``, respectively.
    */
    Mesh() noexcept;
    Mesh(const rect_t &rect, const cells_t &cells) noexcept;

    ostream & info(ostream &os = cout, int  fmt_cntl = 0, int level = 0) const;
    friend ostream & operator<<(ostream &os, const Mesh &m) {
        return m.info(os);
    }

    /**
    Setters.
    Resets the bounding rectangle and/or space division.
    */
    void reset(const rect_t &rect, const cells_t &cells) noexcept;
    void reset(const rect_t &rect) noexcept;
    void reset(const cells_t &cells) noexcept;

    /**
    Getters.
    rect(): retrieve the bounding rectangle.
    low(), high(), size() returns the attributes of this rectangle.
    
    cells(): retrieve the space division instance.
    n_cell(), total_n_cell() returns the attributes of the cells.

    cell_size(): sizes of all dimensions of a cell.
    */
    const rect_t & rect() const noexcept;
    const point_t & low() const noexcept;
    const point_t & high() const noexcept;
    rect_size_t size() const noexcept;

    const cells_t & cells() const noexcept;
    const n_cell_t & n_cell() const noexcept;
    index_t total_n_cell() const noexcept;

    const rect_size_t & cell_size() const noexcept;

    /**
    Coordinate conversion methods. Here a point ``p`` is a position in global
    frame. ``coord`` is its relative coordinate in the frame spanned by the 
    bounding rectangle of the mesh - i.e., ``low() + coord == p``. ``v_idx``
    is the N-dimensional cell index of the point. ``idx`` is flattened cell 
    index.
    
    The following methods provide conversion among them.

    coord_of(): from ``p`` to ``coord``.
    v_idx_of(), idx_of(): from p to its cell index.
    point_at(): from ``coord`` to ``p``.
    coord2v_idx(): from ``coord`` to ``v_idx``.
    idx2v_idx(), v_idx2idx: between two types of index.

    bound_v_idx(): modify the ``v_idx`` to be in the valid range, i.e., 
    set negative components to 0 and set components >= ``n_cell()`` to 
    ``n_cell() - 1``.
    v_idx_is_bound(): check whether or not a ``v_idx`` is in the valid range.
    */
    coord_t coord_of(const point_t &p) const noexcept;
    v_index_t v_idx_of(const point_t &p) const noexcept;
    index_t idx_of(const point_t &p) const noexcept;

    point_t point_at(const coord_t &coord) const noexcept;

    v_index_t coord2v_idx(const coord_t &coord) const noexcept;
    v_index_t idx2v_idx(index_t idx) const noexcept;
    index_t v_idx2idx(const v_index_t &v_idx) const noexcept;

    void bound_v_idx(v_index_t &v_idx) const noexcept;
    bool v_idx_is_bound(const v_index_t &v_idx) const noexcept;
protected:
    rect_size_t _cell_size;
    rect_t _rect;
    cells_t _cells;

    void _update_cell_size() noexcept;
};

/* Implementation */

#define _HIPP_TEMPHD template<typename _FloatT, typename _IndexT, int _DIM>
#define _HIPP_TEMPARG <_FloatT, _IndexT, _DIM>
#define _HIPP_TEMPCLS Mesh _HIPP_TEMPARG
#define _HIPP_TEMPRET _HIPP_TEMPHD inline auto _HIPP_TEMPCLS::
#define _HIPP_TEMPNORET _HIPP_TEMPHD inline _HIPP_TEMPCLS::

_HIPP_TEMPNORET
Mesh() noexcept {}

_HIPP_TEMPNORET
Mesh(const rect_t &rect, const cells_t &cells) noexcept {
    reset(rect, cells);
}

_HIPP_TEMPRET
info(ostream &os, int  fmt_cntl, int level) const -> ostream & {
    PStream ps(os);
    auto *l = low().pos().data(), *h = high().pos().data();
    auto *ns = n_cell().data();
    auto _csz = cell_size();
    auto *csz = _csz.data();
    if( fmt_cntl < 1 ) {
        ps << HIPPCNTL_CLASS_INFO_INLINE(Mesh), 
            "{low=(", ps(l, l+DIM), "), high=(", ps(h, h+DIM), 
            "), n_cell=(", ps(ns, ns+DIM), 
            "), cell_size=(", ps(csz, csz+DIM), ")}";
        return os;
    }
    auto ind = HIPPCNTL_CLASS_INFO_INDENT_STR(level);
    ps << HIPPCNTL_CLASS_INFO(Mesh),
        ind, "low = (", ps(l, l+DIM), "), high = (", ps(h, h+DIM), ")\n",
        ind, "n_cell = (", ps(ns, ns+DIM), 
                "), cell_size = (", ps(csz, csz+DIM), ")\n";
    return os;
}

_HIPP_TEMPRET
reset(const rect_t &rect, const cells_t &cells) noexcept -> void {
    _rect = rect;
    _cells = cells;
    _update_cell_size();
}

_HIPP_TEMPRET
reset(const rect_t &rect) noexcept -> void {
    _rect = rect;
    _update_cell_size();
}

_HIPP_TEMPRET
reset(const cells_t &cells) noexcept -> void {
    _cells = cells;
    _update_cell_size();
}

_HIPP_TEMPRET
rect() const noexcept -> const rect_t & {
    return _rect;
}

_HIPP_TEMPRET
low() const noexcept -> const point_t & {
    return _rect.low();
}

_HIPP_TEMPRET
high() const noexcept -> const point_t & {
    return _rect.high();
}

_HIPP_TEMPRET
size() const noexcept -> rect_size_t {
    return _rect.size();
}

_HIPP_TEMPRET
cells() const noexcept -> const cells_t & {
    return _cells;
}

_HIPP_TEMPRET
n_cell() const noexcept -> const n_cell_t & {
    return _cells.n_cell();
}

_HIPP_TEMPRET
total_n_cell() const noexcept -> index_t {
    return _cells.total_n_cell();
}

_HIPP_TEMPRET
cell_size() const noexcept -> const rect_size_t & {
    return _cell_size;
}

_HIPP_TEMPRET
coord_of(const point_t &p) const noexcept -> coord_t {
    return _rect.coord_of(p);
}

_HIPP_TEMPRET
v_idx_of(const point_t &p) const noexcept -> v_index_t {
    return coord2v_idx(coord_of(p));
}

_HIPP_TEMPRET
idx_of(const point_t &p) const noexcept -> index_t {
    return v_idx2idx(v_idx_of(p));
}

_HIPP_TEMPRET
point_at(const coord_t &coord) const noexcept -> point_t {
    return _rect.point_at(coord);
}

_HIPP_TEMPRET
coord2v_idx(const coord_t &coord) const noexcept -> v_index_t {
    auto v_idx_f = coord.value() / _cell_size;
    auto v_idx = v_idx_f.template floor<index_t>();
    return v_index_t(v_idx);
}

_HIPP_TEMPRET
idx2v_idx(index_t idx) const noexcept -> v_index_t {
    return _cells.idx2v_idx(idx);
}

_HIPP_TEMPRET
v_idx2idx(const v_index_t &v_idx) const noexcept -> index_t {
    return _cells.v_idx2idx(v_idx);
}

_HIPP_TEMPRET
bound_v_idx(v_index_t &v_idx) const noexcept -> void {
    return _cells.bound_v_idx(v_idx);
}

_HIPP_TEMPRET
v_idx_is_bound(const v_index_t &v_idx) const noexcept -> bool {
    return _cells.v_idx_is_bound(v_idx);
}

_HIPP_TEMPRET
_update_cell_size() noexcept -> void {
    auto sz = _rect.size();
    auto &n_cell = _cells.n_cell();
    for(int i=0; i<DIM; ++i) 
        _cell_size[i] = sz[i] / n_cell[i];
}

#undef _HIPP_TEMPHD
#undef _HIPP_TEMPARG
#undef _HIPP_TEMPCLS
#undef _HIPP_TEMPRET
#undef _HIPP_TEMPNORET

} // namespace HIPP::NUMERICAL::GEOMETRY

#endif	//_HIPPNUMERICAL_GEOMETRY_MESH_H_