/**
create: Yangyao CHEN, 2022/02/14
Implementation of kdsearch_kdtree_raw.h
*/


#ifndef _HIPPNUMERICAL_KDSEARCH_KDTREE_RAW_IMPL_H_
#define _HIPPNUMERICAL_KDSEARCH_KDTREE_RAW_IMPL_H_

#include "kdsearch_kdtree_raw.h"
#include <limits>

namespace HIPP::NUMERICAL::_KDSEARCH {

#define _HIPP_TEMPHD template<typename _FloatT, int _DIM, size_t _PADDING, \
    typename _IndexT>
#define _HIPP_TEMPARG <_FloatT, _DIM, _PADDING, _IndexT>
#define _HIPP_TEMPCLS _KDTreeNode _HIPP_TEMPARG
#define _HIPP_TEMPRET _HIPP_TEMPHD inline auto _HIPP_TEMPCLS::
#define _HIPP_TEMPNORET _HIPP_TEMPHD inline _HIPP_TEMPCLS::

_HIPP_TEMPNORET 
_KDTreeNode() noexcept {}

_HIPP_TEMPNORET 
_KDTreeNode(const pos_t &pos) noexcept : point_t(pos) {}

_HIPP_TEMPHD 
template<typename PadT>
_HIPP_TEMPCLS::_KDTreeNode(const pos_t &pos, const PadT &pad) noexcept 
: point_t(pos)
{
    fill_pad(pad);
}

_HIPP_TEMPHD
template<typename PadT>
_HIPP_TEMPCLS::_KDTreeNode(const pos_t &pos, index_t size, int axis, 
    const PadT &pad) noexcept 
: point_t(pos), _size(size), _axis(axis)
{
    fill_pad(pad);
}

_HIPP_TEMPRET
info(ostream &os, int fmt_cntl, int level) const -> ostream & {
    PStream ps{os};
    if( fmt_cntl < 1 ) {
        ps << HIPPCNTL_CLASS_INFO_INLINE(_KDTreeNode),
        "{pos={", ps(_pos.begin(), _pos.end()), "}, size=", _size, 
            ", axis=", _axis, "}";
        return os;
    }
    auto ind = HIPPCNTL_CLASS_INFO_INDENT_STR(level);
    ps << HIPPCNTL_CLASS_INFO(_KDTreeNode),
    ind, "Pos = {", ps(_pos.begin(), _pos.end()), "}\n",
    ind, "Size = ", _size, ", axis = ", _axis, '\n';
    return os;
}

_HIPP_TEMPRET
size() const noexcept -> const index_t & {
    return _size;
}

_HIPP_TEMPRET
size() noexcept -> index_t & {
    return _size;
}

_HIPP_TEMPRET
axis() const noexcept -> const int & {
    return _axis;
}

_HIPP_TEMPRET
axis() noexcept -> int & {
    return _axis;
}

_HIPP_TEMPHD
template<typename PadT>
void _HIPP_TEMPCLS::fill_pad(const PadT &pad) noexcept {
    fill_pad(reinterpret_cast<const char *>(&pad), sizeof(PadT));
}

_HIPP_TEMPRET
fill_pad(const char *p, size_t n) noexcept -> void {
    std::copy_n(p, n, _pad);
}

_HIPP_TEMPHD
template<typename T>
auto _HIPP_TEMPCLS::pad() noexcept -> T & {
    auto *p = reinterpret_cast<T *>(_pad);
    return *p;
}

_HIPP_TEMPHD
template<typename T>
auto _HIPP_TEMPCLS::pad() const noexcept -> const T & {
    const auto *p = reinterpret_cast<std::add_const_t<T> *>(_pad);
    return *p;
}

#undef _HIPP_TEMPHD
#undef _HIPP_TEMPARG
#undef _HIPP_TEMPCLS
#undef _HIPP_TEMPRET
#undef _HIPP_TEMPNORET

#define _HIPP_TEMPHD template<typename KDPointT, typename IndexT>
#define _HIPP_TEMPARG <KDPointT, IndexT>
#define _HIPP_TEMPCLS _KDTree _HIPP_TEMPARG
#define _HIPP_TEMPRET _HIPP_TEMPHD inline auto _HIPP_TEMPCLS::
#define _HIPP_TEMPNORET _HIPP_TEMPHD inline _HIPP_TEMPCLS::


_HIPP_TEMPNORET _KDTree() noexcept {}

_HIPP_TEMPNORET _KDTree(const _KDTree &o) 
: _construct_policy(o._construct_policy), _tree_info(o._tree_info), 
_nodes(o._nodes)
{}

_HIPP_TEMPNORET _KDTree(_KDTree &&o) 
: _construct_policy(std::move(o._construct_policy)), 
_tree_info(std::move(o._tree_info)), 
_nodes(std::move(o._nodes))
{}

_HIPP_TEMPRET 
operator=(const _KDTree &o) noexcept -> _KDTree &  {
    if( this != &o ) {
        _construct_policy = o._construct_policy;
        _tree_info = o._tree_info;
        _nodes = o._nodes;
    }
    return *this;
}

_HIPP_TEMPRET 
operator=(_KDTree &&o) noexcept -> _KDTree &  {
    if( this != &o ) {
        _construct_policy = std::move(o._construct_policy);
        _tree_info = std::move(o._tree_info);
        _nodes = std::move(o._nodes);
    }
    return *this;
}

_HIPP_TEMPNORET 
~_KDTree() noexcept = default;

_HIPP_TEMPRET
info(ostream &os, int fmt_cntl, int level) const -> ostream &  {
    PStream ps(os);
    size_t n_nodes = _nodes.size();
    size_t buf_sz = _nodes.capacity() * sizeof(node_t);
    if( fmt_cntl < 1 ) {
        ps << HIPPCNTL_CLASS_INFO_INLINE(_KDTree), "{", 
            "no. nodes=", n_nodes, ", buf size=", 
            buf_sz, ", ", _construct_policy, "}";
    } else {
        auto ind = HIPPCNTL_CLASS_INFO_INDENT_STR(level);
        ps << HIPPCNTL_CLASS_INFO(_KDTree),
            ind, "No. nodes = ", n_nodes, ", buf size = ", buf_sz, '\n',
            ind; _tree_info.info(os, fmt_cntl, level+1);
        ps << ind; _construct_policy.info(os, fmt_cntl, level+1);
    }
    return os;
}

_HIPP_TEMPHD
struct _HIPP_TEMPCLS::_Impl_construct {

    _KDTree &dst;
    vector<node_t> &nodes;
    tree_info_t &tree_info;

    const kd_point_t * __restrict__ p_pts;
    const index_t n_pts;
    construct_policy_t &pl;
    
    int cur_split_axis;
    vector<index_t> sorted_ids;

_Impl_construct(_KDTree &_dst, ContiguousBuffer<const kd_point_t> _pts, 
    const construct_policy_t &_pl) 
: dst(_dst), nodes(dst._nodes), tree_info(dst._tree_info),
p_pts(_pts.get_cbuff()), n_pts(_pts.get_size()), pl(dst._construct_policy),
cur_split_axis(0), sorted_ids(n_pts)
{
    nodes.resize(n_pts);
    dst._construct_policy = _pl;
}

void operator()() {
    for(index_t i=0; i<n_pts; ++i) 
        sorted_ids[i] = i;

    std::stack<std::pair<index_t, index_t> > idx_ranges;
    index_t b = 0, e = n_pts, n_found = 0;
    while( b != e ) {
        // Update cur_split_axis.
        const int direc = find_best_direc(b, e);            
        // Update sorted_ids.
        const index_t pivot = sort_by_pivot(b, e, direc);
        auto &pt = get_pt(pivot);
        nodes[n_found++] = node_t(pt.pos(), e-b, direc, pt.pad());
        if( b != pivot ) {
            if( pivot+1 != e ) idx_ranges.emplace(pivot+1, e);
            e = pivot;
        } else if( !idx_ranges.empty() ) {
            std::tie(b,e) = idx_ranges.top();
            idx_ranges.pop();
        } else {
            e = b;
        }
    }
    assert(n_found == n_pts);

    // Update tree_info. Use sorted_ids as buffer.
    find_info();
}

const kd_point_t & get_pt(int i) const noexcept { 
    return p_pts[ sorted_ids[i] ]; 
}

int find_best_direc(index_t b, index_t e) {   
    int direc = -1;
    using ax_t = typename construct_policy_t::split_axis_t;
    switch (pl._split_axis) {
        case ax_t::MAX_EXTREME :
            direc = max_extreme_direc(b, e); break;
        case ax_t::MAX_VARIANCE : 
            direc = max_variance_direc(b, e); break;
        case ax_t::ORDERED : 
            direc = cur_split_axis++ % DIM; break;
        default : 
            direc = pl._rng(); break;
    }
    return direc;
}

int max_extreme_direc(index_t b, index_t e) {
    using lim = std::numeric_limits<float_t>;
    pos_t min_pos(lim::max()), max_pos(lim::lowest());
    for(auto i=b; i!=e; ++i){
        const auto &pos = get_pt(i).pos();
        min_pos[ pos < min_pos ] = pos;
        max_pos[ pos > max_pos ] = pos;
    }
    auto ret = (max_pos - min_pos).max_index();
    return static_cast<int>(ret);
}

int max_variance_direc(index_t b, index_t e) {
    using dvec_t = SVec<double, DIM>;
    
    dvec_t mean_pos(0.0);
    for(int i=b; i!=e; ++i){
        auto &pos = get_pt(i).pos();
        mean_pos += dvec_t(pos);
    }
    mean_pos /= static_cast<double>(e-b);

    dvec_t pos_var(0.0);
    for(int i=b; i!=e; ++i){
        auto &pos = get_pt(i).pos();
        auto dpos = dvec_t(pos) - mean_pos;
        pos_var += dpos*dpos;
    }
    
    return static_cast<int>(pos_var.max_index());
}

index_t sort_by_pivot(index_t b, index_t e, const int direc) {
    const index_t pivot = b + (e-b)/2;
    auto *data = sorted_ids.data();
    std::nth_element(data+b, data+pivot, data+e, 
        [direc, this] (index_t i, index_t j)-> bool {
            return p_pts[i].pos()[direc] < p_pts[j].pos()[direc];
        });
    return pivot;
}

void find_info() {
    if( n_pts == 0 ) {
        tree_info._max_depth = 0; return;
    }

    index_t max_depth = 0;
    auto &depths = sorted_ids;
    depths[0] = 0;
    for(index_t i=0; i<n_pts; ++i){
        auto &depth = depths[i];
        ++depth;
        if( depth > max_depth ) max_depth = depth;

        const node_t &n = nodes[i];
        if( n.size() == 1 ) continue;
        
        const index_t left = dst.left_child_idx(i);
        depths[left] = depth;
        if( n.size() == 2 ) continue;

        const index_t right = dst.right_sibling_idx(left);
        depths[right] = depth;
    }

    tree_info._max_depth = max_depth;
}

};

_HIPP_TEMPRET
construct(ContiguousBuffer<const kd_point_t> pts, 
    const construct_policy_t &policy) -> void 
{
    _Impl_construct{*this, pts, policy}();
}

_HIPP_TEMPHD
template<typename PointT>
void _HIPP_TEMPCLS::argsort(ContiguousBuffer<const PointT> pts, 
    vector<idx_pair_t> &idx_pairs) const
{
    auto [p_pts, n_pts] = pts;
    idx_pairs.resize(n_pts);

    if( n_pts != 0 && _nodes.empty() ) 
        ErrLogic::throw_(ErrLogic::eLENGTH, emFLPFB, 
            "  ... Cannot sort ", n_pts, " points in an empty tree");

    for(size_t i=0; i<n_pts; ++i){
        index_t root = 0;
        walk_down(p_pts[i], [](index_t){}, root);
        idx_pairs[i] = idx_pair_t{index_t(i), root};
    }

    std::sort(idx_pairs.begin(), idx_pairs.end());
}

_HIPP_TEMPRET
shrink_buffer() -> void {
    _nodes.shrink_to_fit();
}

_HIPP_TEMPRET
clear() -> void {
    _nodes.clear();
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
nodes() const noexcept -> const vector<node_t> & {
    return _nodes;
}

_HIPP_TEMPRET
nodes() noexcept -> vector<node_t> & {
    return _nodes;
}

_HIPP_TEMPRET
tree_info() const noexcept -> const tree_info_t & {
    return _tree_info;
}

_HIPP_TEMPRET
tree_info() noexcept -> tree_info_t & {
    return _tree_info;
}

_HIPP_TEMPRET
left_child_idx(index_t node_idx) const noexcept -> index_t {
    return node_idx+1;
}

_HIPP_TEMPRET
right_child_idx(index_t node_idx) const noexcept -> index_t {
    return right_sibling_idx(left_child_idx(node_idx));
}

_HIPP_TEMPRET
right_sibling_idx(index_t node_idx) const noexcept -> index_t {
    return node_idx + _nodes[node_idx].size();
}

_HIPP_TEMPHD
template<typename Op>
void _HIPP_TEMPCLS::walk_down(const point_t &p, Op op, 
    index_t &node_idx) const 
{
    const auto &pos = p.pos();
    index_t root = node_idx;
    do {
        op(root);
        const auto &n = _nodes[root];
        const int axis = n.axis();
        const index_t sz = n.size();
        if( pos[axis] <= n.pos()[axis] ) {
            if(sz == 1) break;
            root = left_child_idx(root);
        } else {
            if(sz <= 2) break;
            root = right_child_idx(root);
        }
    } while(true);
    node_idx = root;
}

_HIPP_TEMPHD
struct _HIPP_TEMPCLS::_Impl_query_base {

    const _KDTree &kdt;
    const vector<node_t> &nodes;
    const pos_t dst_pos;

_Impl_query_base(const _KDTree &_kdt, const pos_t &_dst_pos) 
: kdt(_kdt), nodes(kdt._nodes), dst_pos(_dst_pos)
{}

bool on_left_of(const node_t &n) const noexcept {
    auto axis = n.axis();
    return dst_pos[axis] <= n.pos()[axis];
}

float_t offset_from(const node_t &n) const noexcept {
    auto axis = n.axis();
    return dst_pos[axis] - n.pos()[axis];
}

};

_HIPP_TEMPHD
template<typename Policy>
struct _HIPP_TEMPCLS::_Impl_query_pre_order : _Impl_query_base {

    using stack_item_t = index_t;
    stack_item_t *const __restrict__ stk_b,
        * __restrict__ stk_e;
    
_Impl_query_pre_order(const _KDTree &_kdt, const pos_t &_dst_pos, Policy &_pl)
: _Impl_query_base(_kdt, _dst_pos), stk_b(
    _pl.get_buff( _kdt._tree_info.max_depth() )
), stk_e(stk_b) {}


void push_stack(stack_item_t item) noexcept {
    *stk_e++ = item;
}

const stack_item_t & pop_stack() noexcept {
    return *--stk_e;
}

bool stack_not_empty() const noexcept {
    return stk_b != stk_e;
}

template<typename CrossSplitPlane, typename ContainNode, typename Op>
void walk_down(index_t idx, CrossSplitPlane &&cross,  
    ContainNode &&contain, Op &&op) noexcept
{
    do {
        if( idx < 0 ) idx = this->pop_stack();

        const node_t &n = this->nodes[idx];
        const auto sz = n.size();
        const index_t l_child = sz > 1 ? this->kdt.left_child_idx(idx) : -1;

        if( cross(n) ) {
            if( contain(n) ) op(idx);
            idx = l_child;
            if( sz > 2 )
                this->push_stack(this->kdt.right_sibling_idx(l_child));
        } else {
            if( this->on_left_of(n) ) {
                idx = l_child;
            } else {
                idx = sz > 2 ? this->kdt.right_sibling_idx(l_child) : -1;
            }
        }
    } while( idx >= 0 || this->stack_not_empty() );
}

};

_HIPP_TEMPHD
template<typename Policy>
struct _HIPP_TEMPCLS::_Impl_query_in_order : _Impl_query_base {

    Policy &pl;
    using stack_item_t = std::pair<index_t, index_t>;
    stack_item_t *const __restrict__ stk_b,
        * __restrict__ stk_e;

_Impl_query_in_order(const _KDTree &_kdt, const pos_t &_dst_pos, Policy &_pl) 
: _Impl_query_base(_kdt, _dst_pos), pl(_pl),
stk_b(reinterpret_cast<stack_item_t *>(
    _pl.get_buff(_kdt._tree_info.max_depth()*2))
), stk_e(stk_b)
{}

void push_stack(const stack_item_t &item) noexcept {
    *stk_e++ = item;
}

const stack_item_t & pop_stack() noexcept {
    return *--stk_e;
}

bool stack_empty() const noexcept {
    return stk_b == stk_e;
}

stack_item_t walk_down(index_t root) noexcept {
    do {
        const auto &n = this->nodes[root];
        const index_t sz = n.size();
        
        if(sz == 1) return {root, -1};
        const index_t l_child = this->kdt.left_child_idx(root);
        
        if( this->on_left_of(n) ) {
            push_stack({ root, 
                (sz == 2)?(-1):this->kdt.right_sibling_idx(l_child) });
            root = l_child;
        } else {
            if(sz == 2) return {root, l_child};
            push_stack({root, l_child});
            root = this->kdt.right_sibling_idx(l_child);
        }
    } while(true);
}

template<typename CrossSplitPlane, typename Op>
void walk_down(index_t idx, CrossSplitPlane &&cross, Op &&op) noexcept
{
    index_t opp_idx;
    std::tie(idx, opp_idx) = walk_down(idx);
    do {
        const node_t &n = this->nodes[idx];
        if( cross(n) ) {
            op(idx, n);
            if( opp_idx != -1 ) {
                std::tie(idx, opp_idx) = walk_down(opp_idx);
                continue;
            }
        }
        if( stack_empty() ) break;
        std::tie(idx, opp_idx) = pop_stack();
    } while (true);
}

};

_HIPP_TEMPHD
template<typename Policy>
struct _HIPP_TEMPCLS::_Impl_nearest : _Impl_query_in_order<Policy> {

    using base_t = _Impl_query_in_order<Policy>;

    float_t dst_r_sq; 
    index_t dst_idx;
    
_Impl_nearest(const _KDTree &kdt, Policy &pl, const pos_t &dst_pos) 
: base_t(kdt, dst_pos, pl),
dst_r_sq(std::numeric_limits<float_t>::max()),
dst_idx(0) {}

void operator()() noexcept {
    if( this->nodes.size() == 0 ) return;

    this->walk_down(0, 
        [this](const node_t &n) {
            const auto dx = this->offset_from(n); 
            return dx * dx < dst_r_sq; }, 
        [this](index_t idx, const node_t &n) {
            const auto r_sq = (this->dst_pos - n.pos()).squared_norm();
            if( r_sq < dst_r_sq ) {
                dst_r_sq = r_sq; dst_idx = idx;
            }}
        );
}
};

_HIPP_TEMPHD
template<typename Policy>
auto _HIPP_TEMPCLS::nearest(const point_t &p, Policy &&policy) const
-> ngb_t 
{
    _Impl_nearest<Policy> impl{*this, policy, p.pos()};
    impl();
    return {impl.dst_idx, impl.dst_r_sq};
}

_HIPP_TEMPHD
template<typename Policy>
struct _HIPP_TEMPCLS::_Impl_nearest_k : _Impl_query_in_order<Policy> {

    using base_t = _Impl_query_in_order<Policy>;    
    
    ngb_t *const max_queue_b;
    const size_t dst_k;

    size_t used_k;
    float_t max_r_sq;
    
_Impl_nearest_k(const _KDTree &kdt, Policy &pl, const pos_t &dst_pos,
    ContiguousBuffer<ngb_t> ngbs) 
: base_t(kdt, dst_pos, pl), max_queue_b(ngbs.get_buff()), 
dst_k(ngbs.get_size()), used_k(0), 
max_r_sq(std::numeric_limits<float_t>::max())
{}

void operator()() noexcept {
    if( this->nodes.size() == 0 ) return;

    if( dst_k == 0 ) return;

    this->walk_down(0, 
        [this](const node_t &n) {
            const auto dx = this->offset_from(n); 
            return dx * dx < max_r_sq;
        }, 
        [this](index_t idx, const node_t &n) {
            const auto r_sq = (this->dst_pos - n.pos()).squared_norm();
            push_queue(idx, r_sq);
        }
    );

    if( this->pl.sort_by_distance() )
        std::sort_heap(max_queue_b, max_queue_b+used_k);
};

void push_queue(index_t idx, float_t r_sq) noexcept {
    if( r_sq >= max_r_sq ) return;
        
    if( used_k < dst_k ) {              // queue is not full
        max_queue_b[used_k++] = ngb_t{idx, r_sq};
    } else {                            // queue is full
        std::pop_heap(max_queue_b, max_queue_b+dst_k);
        max_queue_b[dst_k-1] = ngb_t{idx, r_sq};
    }
    std::push_heap(max_queue_b, max_queue_b+used_k);
    max_r_sq = max_queue_b->r_sq;
}

};

_HIPP_TEMPHD
template<typename Policy>
auto _HIPP_TEMPCLS::nearest_k(const point_t &p, ContiguousBuffer<ngb_t> ngbs, 
    Policy &&policy) const -> index_t
{
    _Impl_nearest_k<Policy> impl {*this, policy, p.pos(), ngbs};
    impl();
    return impl.used_k;
}

_HIPP_TEMPHD
template<typename Op, typename Policy>
struct _HIPP_TEMPCLS::_Impl_visit_rect : _Impl_query_pre_order<Policy> {

    using base_t = _Impl_query_pre_order<Policy>;

    const rect_t rect;
    Op &op;
    
_Impl_visit_rect(const _KDTree &kdt, Policy &pl, const rect_t &_rect, Op &_op) 
: base_t(kdt, _rect.center().pos(), pl), rect(_rect), op(_op)
{}

void operator()() noexcept {
    if( this->nodes.size() == 0 ) return;
    this->walk_down(0,
        [this](const node_t &n) { return this->contains_along_axis(n); },
        [this](const node_t &n) { return rect.contains(n); },
        op
    );
};

bool contains_along_axis(const node_t &n) const noexcept {
    const int axis = n.axis();
    return rect.contains_along_axis(axis, n.pos()[axis]);
}

};

_HIPP_TEMPHD
template<typename Op, typename Policy>
void _HIPP_TEMPCLS::visit_rect(const rect_t &rect, Op op, 
    Policy &&policy) const
{
    _Impl_visit_rect {*this, policy, rect, op} ();
}

_HIPP_TEMPHD
template<typename Policy>
auto _HIPP_TEMPCLS::count_rect(const rect_t &rect, 
    Policy &&policy) const -> index_t 
{
    index_t cnt = 0;
    visit_rect(rect, [&cnt](index_t node_idx) { ++cnt; }, policy);
    return cnt;
}

_HIPP_TEMPHD
template<typename Op, typename Policy>
struct _HIPP_TEMPCLS::_Impl_visit_sphere : _Impl_query_pre_order<Policy> {

    using base_t = _Impl_query_pre_order<Policy>;

    const sphere_t sphere;
    Op &op;
    
_Impl_visit_sphere(const _KDTree &kdt, Policy &pl, const sphere_t &_sphere, 
    Op &_op) 
: base_t(kdt, _sphere.center().pos(), pl), sphere(_sphere), op(_op)
{}

void operator()() noexcept {
    if( this->nodes.size() == 0 ) return;
    this->walk_down(0, 
        [this](const node_t &n) { 
            return std::fabs(this->offset_from(n)) < sphere.r(); }, 
        [this](const node_t &n) { return sphere.contains(n); },
        op);
};

};

_HIPP_TEMPHD
template<typename Op, typename Policy>
void _HIPP_TEMPCLS::visit_sphere(const sphere_t &sphere, Op op, 
    Policy &&policy) const
{
    _Impl_visit_sphere {*this, policy, sphere, op} ();
}

_HIPP_TEMPHD
template<typename Policy>
auto _HIPP_TEMPCLS::count_sphere(const sphere_t &sphere,
    Policy &&policy) const -> index_t
{
    index_t cnt = 0;
    visit_sphere(sphere, [&cnt](index_t node_idx) { ++cnt; }, policy);
    return cnt;
}

#undef _HIPP_TEMPHD
#undef _HIPP_TEMPARG
#undef _HIPP_TEMPCLS
#undef _HIPP_TEMPRET
#undef _HIPP_TEMPNORET

#define _HIPP_TEMPHD template<typename KDPointT, typename IndexT>
#define _HIPP_TEMPARG <KDPointT, IndexT>
#define _HIPP_TEMPCLS _KDTree _HIPP_TEMPARG::construct_policy_t
#define _HIPP_TEMPRET _HIPP_TEMPHD inline auto _HIPP_TEMPCLS::
#define _HIPP_TEMPNORET _HIPP_TEMPHD inline _HIPP_TEMPCLS::

_HIPP_TEMPNORET
construct_policy_t() : _rng(0, DIM-1, &_re) {
    set_split_axis(DFLT_SPLIT_AXIS);
    set_random_seed(DFLT_RANDOM_SEED);
}

_HIPP_TEMPNORET
construct_policy_t(const construct_policy_t &pl) : construct_policy_t() {
    set_split_axis(pl._split_axis);
    set_random_seed(pl._random_seed);
}

_HIPP_TEMPRET
operator=(const construct_policy_t & pl) -> const construct_policy_t & {
    if( this != &pl ) {
        set_split_axis(pl._split_axis);
        set_random_seed(pl._random_seed);
    }
    return *this;
}

_HIPP_TEMPRET
info(ostream &os, int fmt_cntl, int level) const -> ostream & {
    PStream ps{os};
    static const char *axis_split_strs[4] = 
        {"MAX_EXTREME", "MAX_VARIANCE", "ORDERRED", "RANDOM"};
    const int ax = static_cast<int>(_split_axis);
    if( fmt_cntl < 1 ) {
        ps << HIPPCNTL_CLASS_INFO_INLINE(_KDTree::construct_policy_t),
        "{split axis=", axis_split_strs[ax], 
        ", random seed=", _random_seed, "}";
        return os;
    }
    auto ind = HIPPCNTL_CLASS_INFO_INDENT_STR(level);
    ps << HIPPCNTL_CLASS_INFO(_KDTree::construct_policy_t),
    ind, "Split axis = ", axis_split_strs[ax], 
         ", random seed = ", _random_seed, '\n';
    return os;
}

_HIPP_TEMPRET
split_axis() const noexcept -> split_axis_t { 
    return _split_axis; 
}

_HIPP_TEMPRET
set_split_axis(split_axis_t split_axis) noexcept -> construct_policy_t & {
    _split_axis = split_axis; return *this; 
}

_HIPP_TEMPRET
random_seed() const noexcept -> random_seed_t { 
    return _random_seed; 
}

_HIPP_TEMPRET
set_random_seed(random_seed_t seed) -> construct_policy_t & {
    _random_seed = seed;
    _rng.seed(seed); 
    return *this;
}

#undef _HIPP_TEMPHD
#undef _HIPP_TEMPARG
#undef _HIPP_TEMPCLS
#undef _HIPP_TEMPRET
#undef _HIPP_TEMPNORET



#define _HIPP_TEMPHD template<typename KDPointT, typename IndexT>
#define _HIPP_TEMPARG <KDPointT, IndexT>
#define _HIPP_TEMPCLS _KDTree _HIPP_TEMPARG::tree_info_t
#define _HIPP_TEMPRET _HIPP_TEMPHD inline auto _HIPP_TEMPCLS::
#define _HIPP_TEMPNORET _HIPP_TEMPHD inline _HIPP_TEMPCLS::

_HIPP_TEMPRET
info(ostream &os, int fmt_cntl, int level) const -> ostream & {
    PStream ps{os};
    if( fmt_cntl < 1 ) {
        ps << HIPPCNTL_CLASS_INFO_INLINE(_KDTree::tree_info_t),
        "{max depth=", _max_depth, "}";
        return os;
    }
    auto ind = HIPPCNTL_CLASS_INFO_INDENT_STR(level);
    ps << HIPPCNTL_CLASS_INFO(_KDTree::tree_info_t),
    ind, "Max depth = ", _max_depth, '\n';
    return os;
}

#undef _HIPP_TEMPHD
#undef _HIPP_TEMPARG
#undef _HIPP_TEMPCLS
#undef _HIPP_TEMPRET
#undef _HIPP_TEMPNORET

#define _HIPP_TEMPHD template<typename KDPointT, typename IndexT>
#define _HIPP_TEMPARG <KDPointT, IndexT>
#define _HIPP_TEMPCLS _KDTree _HIPP_TEMPARG::query_buff_policy_t
#define _HIPP_TEMPRET _HIPP_TEMPHD inline auto _HIPP_TEMPCLS::
#define _HIPP_TEMPNORET _HIPP_TEMPHD inline _HIPP_TEMPCLS::

_HIPP_TEMPRET 
get_buff(index_t n) -> index_t * { 
    _buff.resize(n); return _buff.data(); 
}

#undef _HIPP_TEMPHD
#undef _HIPP_TEMPARG
#undef _HIPP_TEMPCLS
#undef _HIPP_TEMPRET
#undef _HIPP_TEMPNORET

#define _HIPP_TEMPHD template<typename KDPointT, typename IndexT>
#define _HIPP_TEMPARG <KDPointT, IndexT>
#define _HIPP_TEMPCLS _KDTree _HIPP_TEMPARG::nearest_k_query_policy_t
#define _HIPP_TEMPRET _HIPP_TEMPHD inline auto _HIPP_TEMPCLS::
#define _HIPP_TEMPNORET _HIPP_TEMPHD inline _HIPP_TEMPCLS::

_HIPP_TEMPNORET
nearest_k_query_policy_t() noexcept {
    sort_by_distance_off();
}

_HIPP_TEMPRET
sort_by_distance() const noexcept -> bool {
    return _sort_by_distance;
}

_HIPP_TEMPRET
sort_by_distance_on() noexcept -> nearest_k_query_policy_t & {
    _sort_by_distance = true;
    return *this;
}

_HIPP_TEMPRET
sort_by_distance_off() noexcept -> nearest_k_query_policy_t & {
    _sort_by_distance = false;
    return *this;
}

#undef _HIPP_TEMPHD
#undef _HIPP_TEMPARG
#undef _HIPP_TEMPCLS
#undef _HIPP_TEMPRET
#undef _HIPP_TEMPNORET


} // namespace HIPP::NUMERICAL::_KDSEARCH

#endif	//_HIPPNUMERICAL_KDSEARCH_KDTREE_RAW_IMPL_H_
