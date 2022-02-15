/**
create: Yangyao CHEN, 2022/02/14
Implementation of kdsearch_kdmesh_raw.h
*/

#ifndef _HIPPNUMERICAL_KDSEARCH_KDMESH_RAW_IMPL_H_
#define _HIPPNUMERICAL_KDSEARCH_KDMESH_RAW_IMPL_H_

#include "kdsearch_kdmesh_raw.h"
#include <limits>

namespace HIPP::NUMERICAL::_KDSEARCH {

#define _HIPP_TEMPHD template<typename KDPointT, typename IndexT>
#define _HIPP_TEMPARG <KDPointT, IndexT>
#define _HIPP_TEMPCLS _KDMesh _HIPP_TEMPARG
#define _HIPP_TEMPRET _HIPP_TEMPHD inline auto _HIPP_TEMPCLS::
#define _HIPP_TEMPNORET _HIPP_TEMPHD inline _HIPP_TEMPCLS::

_HIPP_TEMPNORET
_KDMesh() noexcept {}

_HIPP_TEMPNORET
_KDMesh(const _KDMesh &kdm) 
: _construct_policy(kdm._construct_policy), 
_mesh(kdm._mesh), _nodes(kdm._nodes), _displs(kdm._displs)
{}

_HIPP_TEMPNORET
_KDMesh(_KDMesh &&kdm) noexcept 
: _construct_policy(kdm._construct_policy), 
 _mesh(std::move(kdm._mesh)), _nodes(std::move(kdm._nodes)),
 _displs(std::move(kdm._displs))
{}

_HIPP_TEMPRET
operator=(const _KDMesh &kdm) -> _KDMesh & {
    if( &kdm != this ) {
        _construct_policy = kdm._construct_policy;
        _mesh = kdm._mesh;
        _nodes = kdm._nodes;
        _displs = kdm._displs;
    }
    return *this;
}

_HIPP_TEMPRET
operator=(_KDMesh &&kdm) noexcept -> _KDMesh & {
    if( &kdm != this ) {
        _construct_policy = kdm._construct_policy;
        _mesh = std::move(kdm._mesh);
        _nodes = std::move(kdm._nodes);
        _displs = std::move(kdm._displs);
    }
    return *this;
}

_HIPP_TEMPNORET
~_KDMesh() noexcept = default;

_HIPP_TEMPRET
info(ostream &os, int fmt_cntl, int level) const -> ostream &  {
    PStream ps(os);
    size_t n_nodes = _nodes.size();
    size_t buf_sz = _nodes.capacity() * sizeof(kd_point_t)
        + _displs.capacity() * sizeof(index_t);
    if( fmt_cntl < 1 ) {
        ps << HIPPCNTL_CLASS_INFO_INLINE(_KDMesh), "{", 
            _mesh, ", no. points=", n_nodes, ", buf size=", 
            buf_sz, ", ", _construct_policy, "}";
    } else {
        auto ind = HIPPCNTL_CLASS_INFO_INDENT_STR(level);
        ps << HIPPCNTL_CLASS_INFO(_KDMesh),
              ind; _mesh.info(os, fmt_cntl, level+1);
        ps << ind, "No. points = ", n_nodes, '\n',
              ind, "Buf size = ", buf_sz, '\n',
              ind; _construct_policy.info(os, fmt_cntl, level+1);
    }
    return os;
}

_HIPP_TEMPHD
struct _HIPP_TEMPCLS::_Impl_construct {

    _KDMesh &dst;
    const kd_point_t *p_pts;
    const size_t n_pts;
    const construct_policy_t &pl;

_Impl_construct(_KDMesh &_dst, const kd_point_t *_p_pts, size_t _n_pts)
: dst(_dst), p_pts(_p_pts), n_pts(_n_pts), pl(dst._construct_policy)
{ }

void operator()() const {
    _verify_args();
    _set_mesh();
    _assign_points();
}

void _verify_args() const {
    auto &bd = pl.bound();
    auto [m_rel, m_abs] = pl.margin();

    if( !bd && (m_rel < 0 || m_abs < 0) )
        ErrLogic::throw_(ErrLogic::eDOMAIN, emFLPFB, 
            "Invalid margin (rel=", m_rel, ", abs=", m_abs, ")");

    if( bd && (bd->low().pos() >= bd->high().pos()).any() )
        ErrLogic::throw_(ErrLogic::eDOMAIN, emFLPFB, 
            "Rectangle lower bound exceeds higher bound (low=", bd->low(), 
            ", high=", bd->high(), ")");

    auto &n_cell = pl.n_cell();
    if( (n_cell <= 0).any() )
        ErrLogic::throw_(ErrLogic::eDOMAIN, emFLPFB, 
        "Negative n_cell ", n_cell, " found");
    
    using sort_t = typename construct_policy_t::in_cell_sort_t;
    bool sort_ax = pl.in_cell_sort() == sort_t::ALONG_AXIS;
    int d = pl.dim_sorted();
    if( sort_ax && (d < 0 || d >= DIM) )
        ErrLogic::throw_(ErrLogic::eDOMAIN, emFLPFB, 
            "Cannot sort points along axis ", d, " (DIM=", DIM, ")");
}

void _set_mesh() const noexcept {
    rect_t rect;
    if( pl.bound() ) rect = *pl.bound();
    else{
        auto [low, high] = _find_bound();
        rect = rect_t(low, high);
    }
    
    cells_t cells = pl.n_cell();

    dst._mesh = mesh_t(rect, cells);
}

std::pair<point_t, point_t> _find_bound() const noexcept {
    using pos_t = typename point_t::pos_t;
    if( n_pts == 0 ) 
        return std::pair{pos_t(0), pos_t(1)};

    using lim = std::numeric_limits<float_t>; 
    pos_t low( lim::max() ), high( lim::lowest() );
    for(size_t i=0; i<n_pts; ++i){
        auto &pos = p_pts[i].pos();
        low[ pos < low ] = pos;
        high[ pos > high ] = pos;
    }
    auto [m_rel, m_abs] = pl.margin();
    auto rg = high - low, margin = m_rel * rg + m_abs;
    return std::pair{low-margin, high+margin};
}

void _assign_points() const {
    vector<idx_pair_t> i_prs;
    dst.argsort(ContiguousBuffer{p_pts, n_pts}, i_prs);
    
    auto &nodes = dst._nodes;
    const size_t n_pts_used = i_prs.size();
    nodes.resize(n_pts_used);
    for(size_t i=0; i<n_pts_used; ++i)
        nodes[i] = p_pts[ i_prs[i].idx_in ];

    _assign_displs(i_prs);
}

void _assign_displs(const vector<idx_pair_t> &i_prs) const {
    const auto &mesh = dst._mesh;
    auto &displs = dst._displs;
    
    const index_t total_n_cell = mesh.total_n_cell();
    displs.assign(total_n_cell+1, 0);

    for(const auto &i_pr: i_prs)
        ++ displs[ i_pr.idx_cell ];
    
    index_t cur_displ = 0;
    for(auto &d: displs){
        index_t next_displ = cur_displ + d;
        d = cur_displ;
        cur_displ = next_displ;
    }
}

};

_HIPP_TEMPRET
construct(ContiguousBuffer<const kd_point_t> pts, 
    const construct_policy_t &policy) -> void 
{
    _construct_policy = policy;
    auto [p_pts, n_pts] = pts;
    _Impl_construct{*this, p_pts, n_pts}();
}

_HIPP_TEMPHD
template<typename PointT>
void _HIPP_TEMPCLS::argsort(ContiguousBuffer<const PointT> pts, 
    vector<idx_pair_t> &idx_pairs) const
{
    auto [p_pts, n_pts] = pts;
    idx_pairs.clear();
    idx_pairs.reserve(n_pts);
    
    for(size_t i=0; i<n_pts; ++i){
        auto v_idx = _mesh.v_idx_of(p_pts[i]);
        if( _mesh.v_idx_is_bound(v_idx) )
            idx_pairs.push_back({index_t(i), _mesh.v_idx2idx(v_idx)});
    }

    const auto &pl = _construct_policy;
    using sort_t = typename construct_policy_t::in_cell_sort_t;
    if( pl._in_cell_sort == sort_t::NONE ) {
        std::sort(idx_pairs.begin(), idx_pairs.end());
    } else {
        const int d = pl._dim_sorted;
        auto sort_f = [p_pts, d](const idx_pair_t &l, 
            const idx_pair_t &r) -> bool 
        {
            if( l.idx_cell == r.idx_cell ) 
                return p_pts[l.idx_in].pos()[d] < p_pts[r.idx_in].pos()[d];
            return l < r;
        };
        std::sort(idx_pairs.begin(), idx_pairs.end(), sort_f);
    }
}

_HIPP_TEMPRET
shrink_buffer() -> void {
    _nodes.shrink_to_fit();
    _displs.shrink_to_fit();
}

_HIPP_TEMPRET
clear() -> void {
    _nodes.clear();
    _displs.clear();
}

_HIPP_TEMPRET
get_nodes_in_cell(index_t cell_idx) const noexcept 
-> std::pair<const node_t *, index_t> {
    auto b = _displs[cell_idx], e = _displs[cell_idx+1];
    return {_nodes.data() + b, e - b};
}

_HIPP_TEMPRET
construct_policy() const noexcept -> const construct_policy_t & {
    return _construct_policy; 
}

_HIPP_TEMPRET
construct_policy() noexcept -> construct_policy_t & {
    return _construct_policy; 
}

_HIPP_TEMPRET
mesh() const noexcept -> const mesh_t & {
    return _mesh; 
}

_HIPP_TEMPRET
mesh() noexcept -> mesh_t & {
    return _mesh; 
}

_HIPP_TEMPRET
nodes() const noexcept -> const vector<node_t> & {
    return _nodes; 
}

_HIPP_TEMPRET
nodes() noexcept -> vector<node_t> & {
    return _nodes; 
}

_HIPP_TEMPHD
template<typename Op, typename Policy> 
struct _HIPP_TEMPCLS::_Impl_visit_rect 
{
    static constexpr bool use_strict_judge = Policy::intersect_judge
        == rect_query_policy_t::intersect_judge_t::STRICT;

    const _KDMesh &dst;
    const rect_t &rect;
    Op &op;  
    
    const mesh_t &mesh;
    const n_cell_t n_cell;
    v_index_t i_low, i_high;

    /* set only when use_strict_judge. */
    bool bounds[DIM][2];

_Impl_visit_rect(const _KDMesh &_dst, const rect_t &_rect, Op &_op)
: dst(_dst), rect(_rect), op(_op), mesh(dst._mesh), n_cell(mesh.n_cell()) 
{
    const point_t &f_low = rect.low(), &f_high = rect.high();    
    i_low = mesh.v_idx_of(f_low);
    i_high = mesh.v_idx_of(f_high);
    mesh.bound_v_idx(i_low);
    mesh.bound_v_idx(i_high);

    if constexpr ( use_strict_judge ) {
        auto &mf_low = mesh.low().pos(), &mf_high = mesh.high().pos();
        for(int d=0; d<DIM; ++d){
            bounds[d][0] = f_low.pos()[d] < mf_low[d];
            bounds[d][1] = f_high.pos()[d] >= mf_high[d];
        }
    }
}

void operator()() const {
    if constexpr (use_strict_judge) {
        strict_judge();
    } else {
        rough_judge();
    }
}

void rough_judge() const {
    rough_judge_at_dim<0>(0);
}
template<int D>
void rough_judge_at_dim(index_t i_prev) const {
    index_t ext = n_cell[D], 
        i_b = i_prev * ext + i_low[D],
        i_e = i_prev * ext + i_high[D];
    for(index_t i=i_b; i<=i_e; ++i){
        if constexpr (D == DIM - 1)
            op(i);
        else 
            rough_judge_at_dim<D+1>(i);
    }
}

void strict_judge() const {
    strict_judge_at_dim<0>(0, true);
}
template<int D>
void strict_judge_at_dim(index_t i_prev, bool bound) const {
    constexpr bool last_dim = D == DIM-1;
    index_t ext = n_cell[D], 
        i_b = i_prev * ext + i_low[D],
        i_e = i_prev * ext + i_high[D];
    if( !bound ) {
        for(index_t i=i_b; i<=i_e; ++i){
            if constexpr (last_dim)
                op(i, 0);
            else 
                strict_judge_at_dim<D+1>(i, false);
        }   
        return;
    }
    auto [bound_low, bound_high] = bounds[D];
    if( !bound_low ) {
        if constexpr (last_dim) 
            op(i_b, 0);
        else
            strict_judge_at_dim<D+1>(i_b, false);
        ++i_b;
    }
    if( !bound_high && i_b <= i_e ) {
        if constexpr (last_dim)
            op(i_e, 0);
        else
            strict_judge_at_dim<D+1>(i_e, false);
        --i_e;
    }
    for(index_t i=i_b; i<=i_e; ++i){
        if constexpr (last_dim)
            op(i, 1);
        else 
            strict_judge_at_dim<D+1>(i, true);
    }
}

};

_HIPP_TEMPHD
template<typename Op, typename Policy>
void _HIPP_TEMPCLS::visit_rect(const rect_t &rect, Op op, 
    Policy &&policy) const 
{
    _Impl_visit_rect<Op, Policy>{*this, rect, op}();
}

_HIPP_TEMPRET 
count_rect(const rect_t &rect) const -> index_t {
    index_t cnt = 0;
    auto op = [&rect, &cnt, this](index_t cell_idx, int bound){
        auto [b, n] = get_nodes_in_cell(cell_idx);
        if(bound) {
            cnt += n; return;
        } 
        while(n-- > 0)
            if( rect.contains(b[n]) ) ++cnt;
    };
    visit_rect<decltype(op)>(rect, op);
    return cnt;
}

_HIPP_TEMPHD
template<typename Op, typename Policy> 
struct _HIPP_TEMPCLS::_Impl_visit_sphere
{
    static constexpr bool use_strict_judge = Policy::intersect_judge
        == sphere_query_policy_t::intersect_judge_t::STRICT;
    static constexpr bool use_fine_judge = Policy::intersect_judge
        == sphere_query_policy_t::intersect_judge_t::FINE;

    const _KDMesh &dst;
    const sphere_t &sphere;
    Op &op;  

    const mesh_t &mesh;
    const pos_t mf_low;
    const n_cell_t n_cell;
    const typename mesh_t::rect_size_t cell_size;

    const pos_t f_cent;
    float_t r2;
    v_index_t i_low, i_high;

_Impl_visit_sphere(const _KDMesh &_dst, const sphere_t &_sphere, Op &_op)
: dst(_dst), sphere(_sphere), op(_op), mesh(dst._mesh), 
mf_low(mesh.low().pos()), n_cell(mesh.n_cell()), cell_size(mesh.cell_size()),
f_cent(sphere.center().pos()) {
    const float_t r = sphere.r();
    r2 = r * r;

    auto f_low = f_cent - r, f_high = f_cent + r;
    i_low = mesh.v_idx_of(f_low);
    i_high = mesh.v_idx_of(f_high);
    mesh.bound_v_idx(i_low);
    mesh.bound_v_idx(i_high);
}

void operator()() const {
    if constexpr (use_strict_judge) {
        strict_judge();
    } else if constexpr (use_fine_judge) {
        fine_judge();
    } else {
        rough_judge();
    }
}

void rough_judge() const {
    rough_judge_at_dim<0>(0);
}
template<int D>
void rough_judge_at_dim(index_t i_prev) const {
    const index_t ext = n_cell[D], 
        i_b = i_prev * ext + i_low[D],
        i_e = i_prev * ext + i_high[D];
    for(index_t i=i_b; i<=i_e; ++i){
        if constexpr (D == DIM - 1)
            op(i);
        else 
            rough_judge_at_dim<D+1>(i);
    }
}

void fine_judge() const {
    fine_judge_at_dim<0>(0, 0);
}
template<int D>
void fine_judge_at_dim(index_t i_prev, float_t r2_prev) const {
    index_t i_b = i_low[D], i_e = i_high[D];
    const float_t x_0 = mf_low[D], dx = cell_size[D], 
        x_c = f_cent[D];
    while(i_b <= i_e) {
        float_t x_off = x_0 + (i_b+1) * dx - x_c;
        if( x_off > 0 ) break;
        if( r2_prev + x_off*x_off <= r2 ) break;
        ++i_b;
    }
    while(i_b <= i_e) {
        float_t x_off = x_0 + i_e * dx - x_c;
        if( x_off < 0 ) break;
        if( r2_prev + x_off*x_off <= r2 ) break;
        --i_e;
    }

    i_prev *= n_cell[D];
    for(index_t i=i_b; i<=i_e; ++i){
        const index_t cell_idx = i + i_prev;
        if constexpr (D == DIM-1) 
            op(cell_idx);
        else {
            const float_t x_l_off = x_0 + i * dx - x_c,
                x_r_off = x_l_off + dx;
            float_t x_off;
            if (x_r_off < 0) {
                x_off = x_r_off;
            } else if (x_l_off > 0) {
                x_off = x_l_off;
            } else {
                x_off = 0;
            }
            fine_judge_at_dim<D+1>(cell_idx, r2_prev + x_off*x_off);
        }
    }
}

void strict_judge() const {
    strict_judge_at_dim<0>(0, 0, 0);
}
template<int D>
void strict_judge_at_dim(index_t i_prev, float_t r2_min_prev, 
    float_t r2_max_prev) const
{
    index_t i_b = i_low[D], i_e = i_high[D];
    const float_t x_0 = mf_low[D], dx = cell_size[D], 
        x_c = f_cent[D];
    while(i_b <= i_e) {
        float_t x_off = x_0 + (i_b+1) * dx - x_c;
        if( x_off > 0 ) break;
        if( r2_min_prev + x_off*x_off <= r2 ) break;
        ++i_b;
    }
    while(i_b <= i_e) {
        float_t x_off = x_0 + i_e * dx - x_c;
        if( x_off < 0 ) break;
        if( r2_min_prev + x_off*x_off <= r2 ) break;
        --i_e;
    }

    i_prev *= n_cell[D];
    for(index_t i=i_b; i<=i_e; ++i){
        const index_t cell_idx = i + i_prev;
        const float_t x_l_off = x_0 + i * dx - x_c,
            x_r_off = x_l_off + dx,
            r2_max = std::max(x_l_off*x_l_off, 
                x_r_off*x_r_off) + r2_max_prev;
        if constexpr (D == DIM-1) 
            op(cell_idx, r2_max <= r2);
        else {
            float_t x_off;
            if (x_r_off < 0) {
                x_off = x_r_off;
            } else if (x_l_off > 0) {
                x_off = x_l_off;
            } else {
                x_off = 0;
            }
            strict_judge_at_dim<D+1>(cell_idx, 
                r2_min_prev + x_off*x_off, r2_max);
        }
    }
}

};

_HIPP_TEMPHD
template<typename Op, typename Policy>
void _HIPP_TEMPCLS::visit_sphere(const sphere_t &sphere, Op op, 
    Policy &&policy) const 
{
    _Impl_visit_sphere<Op, Policy>{*this, sphere, op}();
}

_HIPP_TEMPRET
count_sphere(const sphere_t &sphere) const -> index_t {
    index_t cnt = 0;
    auto op = [&sphere, &cnt, this](index_t cell_idx, int bound){
        auto [b, n] = get_nodes_in_cell(cell_idx);
        if(bound) {
            cnt += n; return;
        }
        while(n-- > 0) 
            if( sphere.contains(b[n]) ) ++cnt;
    };
    visit_sphere<decltype(op)>(sphere, op);
    return cnt;
}


#undef _HIPP_TEMPHD
#undef _HIPP_TEMPARG
#undef _HIPP_TEMPCLS
#undef _HIPP_TEMPRET
#undef _HIPP_TEMPNORET

#define _HIPP_TEMPHD template<typename KDPointT, typename IndexT>
#define _HIPP_TEMPARG <KDPointT, IndexT>
#define _HIPP_TEMPCLS _KDMesh _HIPP_TEMPARG::construct_policy_t
#define _HIPP_TEMPRET _HIPP_TEMPHD inline auto _HIPP_TEMPCLS::
#define _HIPP_TEMPNORET _HIPP_TEMPHD inline _HIPP_TEMPCLS::

_HIPP_TEMPNORET
construct_policy_t() noexcept {
    set_margin(DFLT_MARGIN_REL, DFLT_MARGIN_ABS);
    set_n_cell(DFLT_N_CELL);
    set_in_cell_sort(DFLT_IN_CELL_SORT);
    set_dim_sorted(DFLT_DIM_SORTED);
}

_HIPP_TEMPRET
info(ostream &os, int fmt_cntl, int level) const -> ostream & {
    PStream ps{os};
    if( fmt_cntl < 1 ) {
        ps << HIPPCNTL_CLASS_INFO_INLINE(_KDMesh::construct_policy_t),
        "{margin {rel=", _margin_rel, ", abs=", _margin_abs, "}, ",
        "bounding=";
        if( _bound ) ps << *_bound;
        else ps << "none";
        ps << ", cells=", _cells, ", in-cell sort {";
        if( _in_cell_sort == in_cell_sort_t::NONE ) 
            ps << "NONE";
        else 
            ps << "ALONG_AXIS, dim_sorted=", _dim_sorted;
        ps << "}}";
        return os;
    }
    auto ind = HIPPCNTL_CLASS_INFO_INDENT_STR(level);
    ps << HIPPCNTL_CLASS_INFO(_KDMesh::construct_policy_t),
    ind, "Margin {rel=", _margin_rel, ", abs=", _margin_abs, "}\n",
    ind, "Bounding = ";
    if( _bound ) 
        ps << _bound;
    else 
        ps << "none";
    ps << "\n",
    ind, "Cells = ", _cells, '\n',
    ind, "In-cell sort {";
    if( _in_cell_sort == in_cell_sort_t::NONE ) 
        ps << "NONE";
    else 
        ps << "ALONG_AXIS, dim_sorted=", _dim_sorted;
    ps << "}\n";
    return os;
}

_HIPP_TEMPRET
margin() const noexcept -> std::pair<float_t, float_t> { 
    return {_margin_rel, _margin_abs}; 
}

_HIPP_TEMPRET
set_margin(float_t margin_rel, float_t margin_abs) noexcept 
-> construct_policy_t & 
{ 
    _margin_rel = margin_rel;
    _margin_abs = margin_abs;
    return *this;
}

_HIPP_TEMPRET
bound() const noexcept -> const std::optional<rect_t> &  { 
    return _bound; 
}

_HIPP_TEMPRET
set_bound(const rect_t &bound) noexcept ->construct_policy_t & { 
    _bound = bound; 
    return *this;
}

_HIPP_TEMPRET
unset_bound() noexcept -> construct_policy_t & { 
    _bound = std::nullopt; 
    return *this;
}

_HIPP_TEMPRET
n_cell() const noexcept -> const n_cell_t & { 
    return _cells.n_cell(); 
}

_HIPP_TEMPRET
set_n_cell(index_t n_cell) noexcept -> construct_policy_t & { 
    _cells = n_cell;
    return *this; 
}

_HIPP_TEMPRET
set_n_cell(const n_cell_t &n_cell) noexcept -> construct_policy_t & { 
    _cells = n_cell;
    return *this; 
}

_HIPP_TEMPRET
in_cell_sort() const noexcept -> in_cell_sort_t { 
    return _in_cell_sort; 
}

_HIPP_TEMPRET
set_in_cell_sort(in_cell_sort_t in_cell_sort) noexcept -> construct_policy_t & 
{
    _in_cell_sort = in_cell_sort;
    return *this;
}

_HIPP_TEMPRET
dim_sorted() const noexcept -> int { 
    return _dim_sorted; 
}

_HIPP_TEMPRET
set_dim_sorted(int dim) noexcept -> construct_policy_t & {
    _dim_sorted = dim;
    return *this;
}

#undef _HIPP_TEMPHD
#undef _HIPP_TEMPARG
#undef _HIPP_TEMPCLS
#undef _HIPP_TEMPRET
#undef _HIPP_TEMPNORET



} // namespace HIPP::NUMERICAL::_KDSEARCH

#endif	//_HIPPNUMERICAL_KDSEARCH_KDMESH_RAW_IMPL_H_