#ifndef _HIPPNUMERICAL_KDSEARCH_BALLTREE_RAW_IMPL_H_
#define _HIPPNUMERICAL_KDSEARCH_BALLTREE_RAW_IMPL_H_

#include "kdsearch_baltree_raw.h"

namespace HIPP::NUMERICAL::_KDSEARCH {

/** Implementation for _BallTreeNode. */

#define _HIPP_TEMPHD template<typename _FloatT, int _DIM, size_t _PADDING, \
    typename _IndexT>
#define _HIPP_TEMPARG <_FloatT, _DIM, _PADDING, _IndexT>
#define _HIPP_TEMPCLS _BallTreeNode _HIPP_TEMPARG
#define _HIPP_TEMPRET _HIPP_TEMPHD inline auto _HIPP_TEMPCLS::
#define _HIPP_TEMPNORET _HIPP_TEMPHD inline _HIPP_TEMPCLS::

_HIPP_TEMPNORET
_BallTreeNode() noexcept {}

_HIPP_TEMPNORET
_BallTreeNode(const point_t &center, float_t r) noexcept 
: sphere_t(center, r) {}

_HIPP_TEMPHD
template<typename PadT>
_HIPP_TEMPCLS::_BallTreeNode(const point_t &center, float_t r, 
    const PadT &pad) noexcept 
: sphere_t(center, r)
{
    fill_pad(pad);
}

_HIPP_TEMPHD
template<typename PadT>
_HIPP_TEMPCLS::_BallTreeNode(const point_t &center, float_t r, index_t size,
    const PadT &pad) noexcept
: sphere_t(center, r), _size(size)
{
    fill_pad(pad);
}

_HIPP_TEMPRET
info(ostream &os, int  fmt_cntl, int level) const -> ostream & {
    PStream ps(os);
    if( fmt_cntl < 1 ) {
        ps << HIPPCNTL_CLASS_INFO_INLINE(_BallTreeNode),
        "{center=", _center, ", r=", _r, ", size=", _size, "}";
        return os;
    }
    auto ind = HIPPCNTL_CLASS_INFO_INDENT_STR(level);
    ps << HIPPCNTL_CLASS_INFO(_BallTreeNode),
    ind, "Center = ", _center, ", r = ", _r, '\n',
    ind, "Size = ", _size, '\n';
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
T & _HIPP_TEMPCLS::pad() noexcept {
    auto *p = reinterpret_cast<T *>(_pad);
    return *p;
}

_HIPP_TEMPHD
template<typename T>
const T & _HIPP_TEMPCLS::pad() const noexcept {
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
#define _HIPP_TEMPCLS _BallTree _HIPP_TEMPARG
#define _HIPP_TEMPRET _HIPP_TEMPHD inline auto _HIPP_TEMPCLS::
#define _HIPP_TEMPNORET _HIPP_TEMPHD inline _HIPP_TEMPCLS::

_HIPP_TEMPNORET
_BallTree() noexcept = default;

_HIPP_TEMPNORET
_BallTree(const _BallTree &o) = default;

_HIPP_TEMPNORET
_BallTree(_BallTree &&o) = default;

_HIPP_TEMPRET
operator=(const _BallTree &o) noexcept -> _BallTree &  = default;

_HIPP_TEMPRET
operator=(_BallTree &&o) noexcept -> _BallTree & = default;

_HIPP_TEMPNORET
~_BallTree() noexcept = default;

_HIPP_TEMPRET
info(ostream &os, int  fmt_cntl, int level) const -> ostream & {
    PStream ps(os);
    size_t n_nodes = _nodes.size();
    size_t buf_sz = _nodes.capacity() * sizeof(node_t);
    if( fmt_cntl < 1 ) {
        ps << HIPPCNTL_CLASS_INFO_INLINE(_BallTree), "{", 
        "no. nodes=", n_nodes, ", buf size=", 
        buf_sz, ", ", _tree_info, ", ", _construct_policy, "}";
    } else {
        auto ind = HIPPCNTL_CLASS_INFO_INDENT_STR(level);
        ps << HIPPCNTL_CLASS_INFO(_BallTree),
        ind, "No. nodes = ", n_nodes, ", buf size = ", buf_sz, '\n',
        ind, ps.info_of(_tree_info, 0, level+1), '\n',
        ind, ps.info_of(_construct_policy, 0, level+1), '\n';
    }
    return os;
}

_HIPP_TEMPHD
struct _HIPP_TEMPCLS::_Impl_construct 
{

    using pl_t = construct_policy_t;
    using alg_t = typename pl_t::algorithm_t;
    using ins_fav_t = typename pl_t::insert_favor_t;
    using spl_ax_t = typename pl_t::split_axis_t;

    _BallTree &dst;
    vector<node_t> &nodes;
    tree_info_t &tree_info;

    const kd_point_t * __restrict__ p_pts;
    const index_t n_pts;
    construct_policy_t &pl;
    
    int cur_split_axis;
    vector<index_t> temp_ids;
    vector<float_t> temp_fs;

_Impl_construct(_BallTree &_dst, ContiguousBuffer<const kd_point_t> _pts, 
    const construct_policy_t &_pl) 
: dst(_dst), nodes(dst._nodes), tree_info(dst._tree_info),
p_pts(_pts.get_cbuff()), n_pts(_pts.get_size()), pl(dst._construct_policy),
cur_split_axis(0)
{
    pl = _pl;    
    nodes.resize(std::max(n_pts + n_pts - 1, 0));
}

void operator()() 
{
    const alg_t alg = pl.algorithm();
    switch (alg) {
        case alg_t::KD:
        case alg_t::TOP_DOWN:
            run_top_down();
            break;
        case alg_t::ROUGH_INSERTION:
        case alg_t::FINE_INSERTION:
        case alg_t::BOTTOM_UP:
            run_online();
            break;
        default:
            ErrLogic::throw_(ErrLogic::eDOMAIN, emFLPFB, 
                "  ... invalid algorithm ", alg);
            break;
    }

    // Update tree_info. Use sorted_ids as buffer.
    find_info();
}

void find_info() {
    if( n_pts == 0 ) {
        tree_info._max_depth = 0; return;
    }

    const index_t n_nodes = nodes.size();
    auto &depths = temp_ids; 
    depths.resize(n_nodes);
    depths[0] = 1;
    index_t max_depth = 1;
    for(index_t i=0; i<n_nodes; ++i){
        const index_t depth = depths[i];
        if( depth > max_depth ) max_depth = depth;

        if( nodes[i]._size == 1 ) continue;

        const auto [i_lc, i_rc] = dst.children_ids(i);
        depths[i_lc] = depth+1;
        depths[i_rc] = depth+1;
    }
    tree_info._max_depth = max_depth;
}

/**
The algorithm implementation for KD and TOP_DOWN.
*/
void run_top_down() 
{
    auto &ids_src = temp_ids;
    ids_src.resize(n_pts);
    for(index_t i=0; i<n_pts; ++i)
        ids_src[i] = i;

    if( pl.algorithm() == alg_t::TOP_DOWN ) {
        temp_fs.resize(n_pts);
    }

    std::stack<std::pair<index_t, index_t> > idx_ranges;
    index_t b = 0, e = n_pts, n_cur = 0;
    while( b != e || !idx_ranges.empty() ) {
        if( b == e ) {
            std::tie(b,e) = idx_ranges.top(); idx_ranges.pop();
        }
        if( e == b+1 ) {
            const kd_point_t &pt = get_point(b);
            nodes[n_cur++] = node_t{ pt, 0, 1, pt.pad() };
            e = b;
            continue;
        }

        const index_t pivot = top_down_split(b, e);
        node_t &n = nodes[n_cur++];
        n._size = 2*(e-b)-1;
        
        idx_ranges.emplace(pivot, e);
        e = pivot;
    }
    assert(n_cur == static_cast<index_t>(nodes.size()));

    while(n_cur-- > 0) {
        node_t &n = nodes[n_cur];
        if( n._size > 1 ) {
            const auto [i_lc, i_rc] = dst.children_ids(n_cur);
            sphere_t bs = nodes[i_lc].bounding_sphere(nodes[i_rc]);
            n._center = bs.center();
            n._r = bs.r();
        }
    }
}

const kd_point_t & get_point(index_t i) const noexcept {
    return p_pts[ temp_ids[i] ];
}

/** 
Must be called with e - b >= 2. 
Return the pivot index. Must give [b, pivot) and [pivot, e) both non-mepty, 
The entire [b, e) is partially sorted.
*/
index_t top_down_split(index_t b, index_t e) {
    return pl.algorithm() == alg_t::KD ? 
        kd_pivot(b, e) : 
        top_down_pivot(b, e);
}

index_t kd_pivot(index_t b, index_t e) 
{
    const int axis = find_best_axis(pl.split_axis(), b, e);
    const index_t pivot = b + (e-b)/2;
    pivot_at(b, pivot, e, axis);
    return pivot;
}

int find_best_axis(spl_ax_t spl_ax, index_t b, index_t e) 
{   
    int axis {};
    switch (spl_ax) {
        case spl_ax_t::MAX_EXTREME :
            axis = max_extreme_axis(b, e); 
            break;
        case spl_ax_t::MAX_VARIANCE : 
            axis = max_variance_axis(b, e); 
            break;
        case spl_ax_t::ORDERED : 
            axis = cur_split_axis++ % DIM; 
            break;
        default : 
            axis = pl._rng(); 
            break;
    }
    return axis;
}

int max_extreme_axis(index_t b, index_t e) const noexcept 
{
    using lim = std::numeric_limits<float_t>;
    pos_t min_pos(lim::max()), max_pos(lim::lowest());
    for(auto i=b; i<e; ++i){
        const auto &pos = get_point(i).pos();
        min_pos[ pos < min_pos ] = pos;
        max_pos[ pos > max_pos ] = pos;
    }
    auto ret = (max_pos - min_pos).max_index();
    return static_cast<int>(ret);
}

int max_variance_axis(index_t b, index_t e) const noexcept 
{
    using dvec_t = SVec<double, DIM>;
    
    dvec_t mean_pos(0.0);
    for(int i=b; i<e; ++i)
        mean_pos += dvec_t(get_point(i).pos());
    mean_pos /= static_cast<double>(e-b);

    dvec_t pos_var(0.0);
    for(int i=b; i<e; ++i){
        auto dpos = dvec_t(get_point(i).pos()) - mean_pos;
        pos_var += dpos*dpos;
    }
    
    return static_cast<int>(pos_var.max_index());
}

void pivot_at(index_t b, index_t pivot, index_t e, int axis) {
    index_t * const ids = temp_ids.data();
    std::nth_element(ids+b, ids+pivot, ids+e, 
        [axis, this] (index_t i, index_t j)-> bool {
            return p_pts[i].pos()[axis] < p_pts[j].pos()[axis];
        });
}

index_t top_down_pivot(index_t b, index_t e) {
    auto [axis, pivot] = find_minimal_split_volume(b, e);
    pivot_at(b, pivot, e, axis);
    return pivot;
}

/** Return best direction and pivot. */
std::pair<int, index_t> find_minimal_split_volume(index_t b, index_t e)
{
    vector<index_t> &ids = temp_ids;
    vector<float_t> &vols = temp_fs;
    
    int best_axis = 0;
    index_t pivot = b+1;
    float_t min_vol = std::numeric_limits<float_t>::max();

    for(int axis=0; axis<DIM; ++axis){
        
        // Sort points along axis. 
        std::sort(ids.data()+b, ids.data()+e, 
            [&](index_t i, index_t j) {
                return p_pts[i].pos()[axis] < p_pts[j].pos()[axis];
            });

        // Fill expanding volumes.
        sphere_t s { get_point(b), 0 }; vols[b] = 0;
        for(int i=b+1; i<e-1; ++i){
            s = s.bounding_sphere(get_point(i)); 
            vols[i] = insertable_tree_t::node_t::volume(s.r());
        }
        s = sphere_t{ get_point(e-1), 0 };
        for(int i=e-2; i>b; --i){
            s = s.bounding_sphere(get_point(i));
            vols[i-1] += insertable_tree_t::node_t::volume(s.r());
        }

        for(int i=b; i<e-1; ++i){
            if( vols[i] < min_vol ) {
                min_vol = vols[i]; 
                best_axis = axis;
                pivot = i+1;
            }
        }
    }

    return {best_axis, pivot};
}



/** Online-insertion based implementation. */

void run_online() 
{
    typename insertable_tree_t::construct_policy_t ins_cstr_pl {};
    ins_cstr_pl.set_init_alloc_size(n_pts + n_pts);
    insertable_tree_t ins_ballt {ins_cstr_pl};

    insert_points(ins_ballt);
    gather_nodes(ins_ballt);
}

void insert_points(insertable_tree_t &ins_ballt) 
{
    const alg_t alg = pl.algorithm();
    
    ins_fav_t ins_fav;
    switch (alg) {
        case alg_t::ROUGH_INSERTION:
            ins_fav = ins_fav_t::ROUGH;
            break;
        case alg_t::FINE_INSERTION:
            ins_fav = ins_fav_t::FINE;
            break;
        default:    
            ins_fav = pl.bottom_up_insert_favor();
            break;
    }

    // Make insertions (and refinement for BOTTOM_UP).
    typename insertable_tree_t::insert_policy_t ins_pl {};
    ins_pl.set_insert_favor(ins_fav);
    for(index_t i=0; i<n_pts; ++i)
        ins_ballt.insert(p_pts[i], ins_pl);

    if( alg == alg_t::BOTTOM_UP ) {
        ins_ballt.bottom_up_refine(ins_pl);
    }
}

void gather_nodes(insertable_tree_t &ins_ballt) 
{
    const auto &nodes_src = ins_ballt.nodes();
    vector<index_t> &ids_dst = temp_ids;
    ids_dst.resize(nodes_src.size());
    
    // Inorder visiting to gather nodes.
    index_t id_cur = 0;
    auto op1 = [&](index_t id_src){
        const auto &n_src = nodes_src[id_src];
        ids_dst[id_src] = id_cur;
        node_t &n_dst = nodes[id_cur++];
        n_dst._center = n_src.center();
        n_dst._r = n_src.r();
        n_dst.fill_pad( n_src.pad() );
    };
    auto op2 = [&](index_t id_src){
        const index_t id_dst = ids_dst[id_src];
        node_t &n_dst = nodes[id_dst];
        n_dst._size = id_cur - id_dst;          // 1 + left subtree size.
    };
    ins_ballt.inorder_traverse(op1, op2);
    assert( id_cur == static_cast<index_t>(nodes.size()) );

    // Update node size.
    while(id_cur-- > 0) {
        node_t &n = nodes[id_cur];
        index_t sz = n._size;
        if( sz > 1 )
            // 1 + left + right subtree size
            n._size += nodes[id_cur+sz]._size;
    }
}

};

_HIPP_TEMPHD
void _HIPP_TEMPCLS::construct(ContiguousBuffer<const kd_point_t> pts, 
    const construct_policy_t &policy)
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
shrink_buffer() -> void 
{
    _nodes.shrink_to_fit();
}

_HIPP_TEMPRET
clear() -> void 
{
    _nodes.clear();
}

_HIPP_TEMPRET
construct_policy() const noexcept -> const construct_policy_t &
{
    return _construct_policy;
}

_HIPP_TEMPRET
construct_policy() noexcept -> construct_policy_t &
{
    return _construct_policy;
}

_HIPP_TEMPRET
nodes() const noexcept -> const vector<node_t> &
{
    return _nodes;
}

_HIPP_TEMPRET
nodes() noexcept -> vector<node_t> &
{
    return _nodes;
}

_HIPP_TEMPRET
tree_info() const noexcept -> const tree_info_t &
{
    return _tree_info;
}

_HIPP_TEMPRET
tree_info() noexcept -> tree_info_t &
{
    return _tree_info;
}

_HIPP_TEMPRET
left_child_idx(index_t node_idx) const noexcept -> index_t
{
    return node_idx+1;
}

_HIPP_TEMPRET
right_child_idx(index_t node_idx) const noexcept -> index_t
{
    return right_sibling_idx(left_child_idx(node_idx));
}

_HIPP_TEMPRET
right_sibling_idx(index_t node_idx) const noexcept -> index_t
{
    return node_idx + _nodes[node_idx].size();
}

_HIPP_TEMPRET
children_ids(index_t node_idx) const noexcept 
-> std::pair<index_t, index_t> 
{
    auto l_idx = left_child_idx(node_idx),
        r_idx = right_sibling_idx(l_idx);
    return {l_idx, r_idx};
}

_HIPP_TEMPHD
template<typename Op>
void _HIPP_TEMPCLS::walk_down(const point_t &p, Op op, 
    index_t &node_idx) const 
{
    index_t root = node_idx;
    do {
        op(root);
        const auto &n = _nodes[root];
        const index_t sz = n.size();
        if( sz == 1 ) break;
        
        const auto [l_idx, r_idx] = children_ids(root);
        const auto &l_n = _nodes[l_idx], 
            &r_n = _nodes[r_idx];
        const float_t l_d = (p - l_n.center()).r() - l_n.r(),
            r_d = (p - r_n.center()).r() - r_n.r();
        root = l_d < r_d ? l_idx : r_idx;
    } while(true);

    node_idx = root;
}

_HIPP_TEMPHD
struct _HIPP_TEMPCLS::_Impl_query_base {

    const _BallTree &ballt;
    const vector<node_t> &nodes;
    const pos_t dst_pos;

_Impl_query_base(const _BallTree &_ballt, const pos_t &_dst_pos) 
: ballt(_ballt), nodes(ballt._nodes), dst_pos(_dst_pos)
{}

float_t dist_sq_to(const node_t &n) const noexcept {
    return (dst_pos - n.center().pos()).squared_norm();
}

float_t dist_to(const node_t &n)  const noexcept {
    return std::sqrt( dist_sq_to(n) );
}

};

_HIPP_TEMPHD
template<typename Policy>
struct _HIPP_TEMPCLS::_Impl_query_pre_order : _Impl_query_base 
{

    using stack_item_t = index_t;
    stack_item_t *const __restrict__ stk_b,
        * __restrict__ stk_e;
    
_Impl_query_pre_order(const _BallTree &_ballt, const pos_t &_dst_pos, 
    Policy &_pl)
: _Impl_query_base(_ballt, _dst_pos), stk_b(
    _pl.get_buff( _ballt._tree_info.max_depth() )
), stk_e(stk_b) {}


void push_stack(stack_item_t item) noexcept {
    *stk_e++ = item;
}

const stack_item_t &pop_stack() noexcept {
    return *--stk_e;
}

bool stack_not_empty() const noexcept {
    return stk_b != stk_e;
}

template<typename ContainNode, typename ContainLeaf,
    typename OpNode, typename OpLeaf>
void walk_down(index_t idx, ContainNode &&contain_n, 
    ContainLeaf &&contain_l, 
    OpNode &&op_n, OpLeaf &&op_l) noexcept
{
    do {
        if( idx < 0 ) idx = this->pop_stack();

        const node_t &n = this->nodes[idx];
        const auto sz = n.size();
        if( sz == 1 ) {
            if( contain_l(n) ) op_l(idx);
            idx = -1; continue;
        }
        auto [intersects, contains] = contain_n(n);
        if( !intersects ) {
            idx = -1; continue;  
        } 

        if( contains ) {
            op_n(idx); idx = -1;
        } else {
            idx = this->ballt.left_child_idx(idx);
            this->push_stack( this->ballt.right_sibling_idx(idx) );
        }
        
    } while( idx >= 0 || stack_not_empty() );
}

};

_HIPP_TEMPHD
template<typename Policy>
struct _HIPP_TEMPCLS::_Impl_query_in_order : _Impl_query_base 
{

    Policy &pl;
    using stack_item_t = std::pair<index_t, float_t>;
    stack_item_t *const __restrict__ stk_b,
        * __restrict__ stk_e;

_Impl_query_in_order(const _BallTree &_ballt, const pos_t &_dst_pos, 
    Policy &_pl) 
: _Impl_query_base(_ballt, _dst_pos), pl(_pl),
stk_b(reinterpret_cast<stack_item_t *>(
    _pl.get_buff(_ballt._tree_info.max_depth()*2) )
), stk_e(stk_b)
{}

void push_stack(const stack_item_t &item) noexcept {
    *stk_e++ = item;
}

const stack_item_t &pop_stack() noexcept {
    return *--stk_e;
}

bool stack_not_empty() const noexcept {
    return stk_b != stk_e;
}

/** Negative value means ``dst_pos`` is contained in ``n``. */
float_t minimal_dist_to(const node_t &n) {
    return this->dist_to(n) - n.r();
}

template<typename CoverDist, typename Op>
void walk_down(index_t idx, CoverDist &&cover, Op &&op) noexcept 
{
    // if a leaf
    const auto &n = this->nodes[idx];
    if( n.size() == 1 ) {
        op(idx, n); return;
    }

    // otherwise, an internal node
    float_t min_dist = minimal_dist_to(n);
    do {
        if( idx < 0 ) {
            std::tie(idx, min_dist) = pop_stack();
        } 

        if( ! cover(min_dist) ) {
            idx = -1; continue;
        }
        const auto [l_idx, r_idx] = this->ballt.children_ids(idx);
        const auto &l_n = this->nodes[l_idx],
            &r_n = this->nodes[r_idx];
        const auto l_sz = l_n.size(), r_sz = r_n.size();
        if( l_sz == 1 ) { 
            op(l_idx, l_n); 
            if( r_sz == 1 ) {
                op(r_idx, r_n); idx = -1;
            } else {
                idx = r_idx; min_dist = minimal_dist_to(r_n);
            }
            continue;
        } 
        const float_t l_d = minimal_dist_to(l_n);
        if( r_sz == 1 ) {
            op(r_idx, r_n); 
            idx = l_idx; min_dist = l_d;
        } else {
            const float_t r_d = minimal_dist_to(r_n);
            if( l_d < r_d ) {
                push_stack({r_idx, r_d});
                idx = l_idx; min_dist = l_d;
            } else {
                push_stack({l_idx, l_d});
                idx = r_idx; min_dist = r_d;
            }
        }
    } while(idx >= 0 || stack_not_empty());
}

};

_HIPP_TEMPHD
template<typename Policy>
struct _HIPP_TEMPCLS::_Impl_nearest : _Impl_query_in_order<Policy> 
{

    using base_t = _Impl_query_in_order<Policy>;

    float_t dst_r_sq, dst_r;
    index_t dst_idx;
    
_Impl_nearest(const _BallTree &ballt, Policy &pl, const pos_t &dst_pos) 
: base_t(ballt, dst_pos, pl),
dst_r_sq(std::numeric_limits<float_t>::max()), dst_r(dst_r_sq),
dst_idx(0) {}

void operator()() noexcept {
    if( this->nodes.size() == 0 ) return;

    this->walk_down(0, 
        [this](float_t min_dist) { return min_dist < dst_r; }, 
        [this](index_t idx, const node_t &n) {
            const auto r_sq = this->dist_sq_to(n);
            if( r_sq < dst_r_sq ) {
                dst_r_sq = r_sq; dst_r = std::sqrt(r_sq); dst_idx = idx;
            }
        });
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
struct _HIPP_TEMPCLS::_Impl_nearest_k : _Impl_query_in_order<Policy> 
{

    using base_t = _Impl_query_in_order<Policy>;    
    
    ngb_t *const max_queue_b;
    const size_t dst_k;

    size_t used_k;
    float_t max_r_sq, max_r;
    
_Impl_nearest_k(const _BallTree &ballt, Policy &pl, const pos_t &dst_pos,
    ContiguousBuffer<ngb_t> ngbs) 
: base_t(ballt, dst_pos, pl), max_queue_b(ngbs.get_buff()), 
dst_k(ngbs.get_size()), used_k(0), 
max_r_sq(std::numeric_limits<float_t>::max()), max_r(max_r_sq)
{}

void operator()() noexcept {
    if( this->nodes.size() == 0 ) return;

    if( dst_k == 0 ) return;

    this->walk_down(0, 
        [this](float_t min_dst) { return min_dst < max_r; }, 
        [this](index_t idx, const node_t &n) {
            push_queue(idx, this->dist_sq_to(n));
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
    max_r = std::sqrt(max_r_sq);
}

};

_HIPP_TEMPHD
template<typename Policy>
auto _HIPP_TEMPCLS::nearest_k(const point_t &p, 
    ContiguousBuffer<ngb_t> ngbs, 
    Policy &&policy) const -> index_t
{
    _Impl_nearest_k<Policy> impl {*this, policy, p.pos(), ngbs};
    impl();
    return impl.used_k;
}

_HIPP_TEMPHD
template<typename OpNode, typename OpLeaf, typename Policy>
struct _HIPP_TEMPCLS::_Impl_visit_rect : _Impl_query_pre_order<Policy> 
{

    using base_t = _Impl_query_pre_order<Policy>;

    const rect_t rect;
    const point_t &low, &high;
    OpNode &op_n;
    OpLeaf &op_l;
    
_Impl_visit_rect(const _BallTree &ballt, Policy &pl, const rect_t &_rect, 
    OpNode &_op_n, OpLeaf &_op_l) 
: base_t(ballt, _rect.center().pos(), pl), rect(_rect), 
low(rect.low()), high(rect.high()), op_n(_op_n), op_l(_op_l)
{}

void operator()() noexcept {
    if( this->nodes.size() == 0 ) return;
    this->walk_down(0,
        [this](const node_t &n) {
            const auto r_n = n.r(); const auto &c_n = n.center();
            bool inters = rect_t{low-r_n, high+r_n}.contains(c_n),
                cont = inters ? 
                    rect_t{low+r_n, high-r_n}.contains(c_n) : false;
            return std::pair{inters, cont};
        },
        [this](const node_t &n) {
            return rect.contains(n.center());
        },
        op_n, op_l
    );
};

};

_HIPP_TEMPHD
template<typename OpNode, typename OpLeaf, 
    typename Policy>
void _HIPP_TEMPCLS::visit_rect(const rect_t &rect, OpNode op_n, OpLeaf op_l,
    Policy &&policy) const
{
    _Impl_visit_rect {*this, policy, rect, op_n, op_l} ();

}

_HIPP_TEMPHD
template<typename Policy>
auto _HIPP_TEMPCLS::count_rect(const rect_t &rect,
    Policy &&policy) const -> index_t 
{
    index_t cnt = 0;
    visit_rect(rect, 
        [&cnt, this](index_t node_idx) { 
            cnt += this->nodes[node_idx].size(); 
        }, 
        [&cnt](index_t node_idx) { ++cnt; },
        policy);
    return cnt;
}

_HIPP_TEMPHD
template<typename OpNode, typename OpLeaf, typename Policy>
struct _HIPP_TEMPCLS::_Impl_visit_sphere : _Impl_query_pre_order<Policy> {

    using base_t = _Impl_query_pre_order<Policy>;

    const sphere_t sphere;
    OpNode &op_n;
    OpLeaf &op_l;
    
_Impl_visit_sphere(const _BallTree &ballt, Policy &pl, 
    const sphere_t &_sphere, 
    OpNode &_op_n, OpLeaf &_op_l)
: base_t(ballt, _sphere.center().pos(), pl), sphere(_sphere), 
op_n(_op_n), op_l(_op_l)
{}

void operator()() noexcept {
    if( this->nodes.size() == 0 ) return;

    const float_t r = sphere.r(), r_sq = r * r;
    this->walk_down(0, 
        [this, r](const node_t &n) {
            const float_t d = this->dist_to(n), r_n = n.r();
            return std::pair<bool, bool>(d < r + r_n, d < r - r_n);
        },
        [this, r_sq](const node_t &n) { return this->dist_sq_to(n) < r_sq; },
        op_n, op_l);
};

};

_HIPP_TEMPHD
template<typename OpNode, typename OpLeaf, typename Policy>
void _HIPP_TEMPCLS::visit_sphere(const sphere_t &sphere, OpNode op_n, 
    OpLeaf op_l, Policy &&policy) const
{
    _Impl_visit_sphere {*this, policy, sphere, op_n, op_l} ();
}

_HIPP_TEMPHD
template<typename Policy>
auto _HIPP_TEMPCLS::count_sphere(const sphere_t &sphere,
    Policy &&policy) const -> index_t
{
    index_t cnt = 0;
    visit_sphere(sphere, 
        [&cnt, this](index_t node_idx) { 
            cnt += this->nodes[node_idx].size(); 
        },
        [&cnt](index_t node_idx) { ++cnt; },
        policy);
    return cnt;
}

#undef _HIPP_TEMPHD
#undef _HIPP_TEMPARG
#undef _HIPP_TEMPCLS
#undef _HIPP_TEMPRET
#undef _HIPP_TEMPNORET

#define _HIPP_TEMPHD template<typename KDPointT, typename IndexT>
#define _HIPP_TEMPARG <KDPointT, IndexT>
#define _HIPP_TEMPCLS _BallTree _HIPP_TEMPARG::construct_policy_t
#define _HIPP_TEMPRET _HIPP_TEMPHD inline auto _HIPP_TEMPCLS::
#define _HIPP_TEMPNORET _HIPP_TEMPHD inline _HIPP_TEMPCLS::

_HIPP_TEMPNORET
construct_policy_t() : _rng(0, DIM-1, &_re) {
    set_algorithm(DFLT_ALGORITHM);
    set_bottom_up_insert_favor(DFLT_BOTTOM_UP_INSERT_FAVOR);
    set_split_axis(DFLT_SPLIT_AXIS);
    set_random_seed(DFLT_RANDOM_SEED);
}

_HIPP_TEMPNORET
construct_policy_t(const construct_policy_t &pl) : construct_policy_t() {
    set_algorithm(pl._algorithm);
    set_bottom_up_insert_favor(pl._bottom_up_insert_favor);
    set_split_axis(pl._split_axis);
    set_random_seed(pl._random_seed);
}

_HIPP_TEMPRET
operator=(const construct_policy_t & pl) -> const construct_policy_t & {
    if( this != &pl ) {
        set_algorithm(pl._algorithm);
        set_bottom_up_insert_favor(pl._bottom_up_insert_favor);
        set_split_axis(pl._split_axis);
        set_random_seed(pl._random_seed);
    }
    return *this;
}

_HIPP_TEMPRET
info(ostream &os, int fmt_cntl, int level) const -> ostream & {
    PStream ps{os};
    
    static const char * const alg_strs[5] = {
        "KD", "TOP_DOWN", "ROUGH_INSERTION", "FINE_INSERTION", "BOTTOM_UP"};
    const char * const alg 
        = alg_strs[static_cast<int>(_algorithm)];

    static const char * const ins_fav_strs[2] = {"ROUGH", "FINE"};
    const char * const ins_fav = 
        ins_fav_strs[static_cast<int>(_bottom_up_insert_favor)];

    static const char * const axis_split_strs[4] = 
        {"MAX_EXTREME", "MAX_VARIANCE", "ORDERRED", "RANDOM"};
    const char * const ax 
        = axis_split_strs[static_cast<int>(_split_axis)];

    if( fmt_cntl < 1 ) {
        ps << HIPPCNTL_CLASS_INFO_INLINE(_BallTree::construct_policy_t),
        "{algorithm=", alg, ", bottom up insert favor=", ins_fav, 
        ", split axis=", ax, ", random seed=", _random_seed, "}";
        return os;
    }
    auto ind = HIPPCNTL_CLASS_INFO_INDENT_STR(level);
    ps << HIPPCNTL_CLASS_INFO(_BallTree::construct_policy_t),
    ind, "Algorithm = ", alg, ", bottom up insert favor = ", ins_fav, '\n',
    ind, "Split axis = ", ax, ", random seed = ", _random_seed, '\n';
    return os;
}
_HIPP_TEMPRET
algorithm() const noexcept -> algorithm_t
{
    return _algorithm;
}

_HIPP_TEMPRET
set_algorithm(algorithm_t algorithm) noexcept -> construct_policy_t &
{
    _algorithm = algorithm;
    return *this;
}

_HIPP_TEMPRET
bottom_up_insert_favor() const noexcept -> insert_favor_t
{
    return _bottom_up_insert_favor;
}

_HIPP_TEMPRET
set_bottom_up_insert_favor(insert_favor_t favor) noexcept 
-> construct_policy_t & 
{
    _bottom_up_insert_favor = favor;
    return *this;
}

_HIPP_TEMPRET
split_axis() const noexcept -> split_axis_t { 
    return _split_axis; 
}

_HIPP_TEMPRET
set_split_axis(split_axis_t split_axis) noexcept -> construct_policy_t &
{
    _split_axis = split_axis; return *this; 
}

_HIPP_TEMPRET
random_seed() const noexcept -> random_seed_t
{ 
    return _random_seed; 
}

_HIPP_TEMPRET
set_random_seed(random_seed_t seed) -> construct_policy_t &
{
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
#define _HIPP_TEMPCLS _BallTree _HIPP_TEMPARG::tree_info_t
#define _HIPP_TEMPRET _HIPP_TEMPHD inline auto _HIPP_TEMPCLS::
#define _HIPP_TEMPNORET _HIPP_TEMPHD inline _HIPP_TEMPCLS::

_HIPP_TEMPNORET
tree_info_t() noexcept : _max_depth(0) {}

_HIPP_TEMPRET
info(ostream &os, int fmt_cntl, int level) const -> ostream & {
    PStream ps{os};
    if( fmt_cntl < 1 ) {
        ps << HIPPCNTL_CLASS_INFO_INLINE(_BallTree::tree_info_t),
        "{max depth=", _max_depth, "}";
        return os;
    }
    auto ind = HIPPCNTL_CLASS_INFO_INDENT_STR(level);
    ps << HIPPCNTL_CLASS_INFO(_BallTree::tree_info_t),
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
#define _HIPP_TEMPCLS _BallTree _HIPP_TEMPARG::query_buff_policy_t
#define _HIPP_TEMPRET _HIPP_TEMPHD inline auto _HIPP_TEMPCLS::
#define _HIPP_TEMPNORET _HIPP_TEMPHD inline _HIPP_TEMPCLS::

_HIPP_TEMPRET 
get_buff(index_t n) -> index_t * { 
    _container.resize(n); return _container.data(); 
}

_HIPP_TEMPRET 
container() noexcept -> container_t & 
{
    return _container;
}

_HIPP_TEMPRET 
container() const noexcept -> const container_t & 
{
    return _container;
}

#undef _HIPP_TEMPHD
#undef _HIPP_TEMPARG
#undef _HIPP_TEMPCLS
#undef _HIPP_TEMPRET
#undef _HIPP_TEMPNORET

#define _HIPP_TEMPHD template<typename KDPointT, typename IndexT>
#define _HIPP_TEMPARG <KDPointT, IndexT>
#define _HIPP_TEMPCLS _BallTree _HIPP_TEMPARG::nearest_k_query_policy_t
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

#endif	//_HIPPNUMERICAL_KDSEARCH_BALLTREE_RAW_IMPL_H_