/**
create: Yangyao CHEN, 2022/04/15
Implementation of kdsearch_insertable_balltree_raw.h
*/


#ifndef _HIPPNUMERICAL_KDSEARCH_INSERTABLE_BALLTREE_RAW_IMPL_H_
#define _HIPPNUMERICAL_KDSEARCH_INSERTABLE_BALLTREE_RAW_IMPL_H_

#include "kdsearch_insertable_balltree_raw.h"

namespace HIPP::NUMERICAL::_KDSEARCH {

#define _HIPP_TEMPHD template<typename _FloatT, int _DIM, \
    size_t _PADDING, typename _IndexT>
#define _HIPP_TEMPARG <_FloatT, _DIM, _PADDING, _IndexT>
#define _HIPP_TEMPCLS _InsertableBallTreeNode _HIPP_TEMPARG
#define _HIPP_TEMPRET _HIPP_TEMPHD inline auto _HIPP_TEMPCLS::
#define _HIPP_TEMPNORET _HIPP_TEMPHD inline _HIPP_TEMPCLS::

_HIPP_TEMPNORET
_InsertableBallTreeNode() noexcept {}

_HIPP_TEMPNORET
_InsertableBallTreeNode(const point_t &center, float_t r) noexcept
: sphere_t(center, r) {}

_HIPP_TEMPHD
template<typename PadT>
_HIPP_TEMPCLS::_InsertableBallTreeNode(const point_t &center, float_t r, 
    const PadT &pad) noexcept
: sphere_t(center, r)
{
    fill_pad(pad);
}

_HIPP_TEMPHD
template<typename PadT>
_HIPP_TEMPCLS::_InsertableBallTreeNode(const point_t &center, float_t r, 
    index_t parent_idx, index_t left_child_idx, index_t right_child_idx,
    node_info_t node_info, const PadT &pad) noexcept
: sphere_t(center, r), _parent_idx(parent_idx), 
_left_child_idx(left_child_idx), _right_child_idx(right_child_idx), 
_node_info(node_info) 
{
    fill_pad(pad);
}

_HIPP_TEMPRET
info(ostream &os, int  fmt_cntl, int level) const -> ostream & 
{
    PStream ps(os);

    auto v_info = _node_info;
    const char *s_info = "UNKNOWN";
    if( v_info == tNULL )
        s_info = "NULL";
    else if( v_info == tLEAF )
        s_info = "LEAF";
    else if( v_info == tINTERNAL )
        s_info = "INTERNAL";

    if( fmt_cntl < 1 ) {
        ps << HIPPCNTL_CLASS_INFO_INLINE(_InsertableBallTreeNode),
        "{center=", _center, ", r=", _r, ", left child=", _left_child_idx,
            "right child=", _right_child_idx, ", parent=", _parent_idx, 
            ", node info=", s_info, "}";
        return os;
    }
    auto ind = HIPPCNTL_CLASS_INFO_INDENT_STR(level);
    ps << HIPPCNTL_CLASS_INFO(_InsertableBallTreeNode),
    ind, "Center = ", _center, ", r = ", _r, '\n',
    ind, "Left child = ", _left_child_idx,
         ", right child = ", _right_child_idx, 
         ", parent = ", _parent_idx, '\n',
    ind, "Node info = ", s_info, '\n';
    return os;
}


_HIPP_TEMPRET
parent_idx() const noexcept -> const index_t & 
{
    return _parent_idx;
}

_HIPP_TEMPRET
parent_idx() noexcept -> index_t & 
{
    return _parent_idx;
}

_HIPP_TEMPRET
left_child_idx() const noexcept -> const index_t & 
{
    return _left_child_idx;
}

_HIPP_TEMPRET
left_child_idx() noexcept -> index_t & 
{
    return _left_child_idx;
}

_HIPP_TEMPRET
right_child_idx() const noexcept -> const index_t & 
{
    return _right_child_idx;
}

_HIPP_TEMPRET
right_child_idx() noexcept -> index_t & 
{
    return _right_child_idx;
}

_HIPP_TEMPRET
node_info() const noexcept -> const node_info_t & 
{
    return _node_info;
}

_HIPP_TEMPRET
node_info() noexcept -> node_info_t & 
{
    return _node_info;
}

_HIPP_TEMPRET
is_null() const noexcept -> bool 
{
    return _node_info == tNULL;
}

_HIPP_TEMPRET
is_leaf() const noexcept -> bool 
{
    return _node_info == tLEAF;
}

_HIPP_TEMPRET
is_internal() const noexcept -> bool 
{
    return _node_info == tINTERNAL;
}

_HIPP_TEMPHD
template<typename PadT>
void _HIPP_TEMPCLS::fill_pad(const PadT &pad) noexcept 
{
    fill_pad(reinterpret_cast<const char *>(&pad), sizeof(PadT));
}

_HIPP_TEMPRET
fill_pad(const char *p, size_t n) noexcept -> void 
{
    std::copy_n(p, n, _pad);
}

_HIPP_TEMPHD
template<typename T>
auto _HIPP_TEMPCLS::pad() noexcept -> T & 
{
    auto *p = reinterpret_cast<T *>(_pad);
    return *p;
}

_HIPP_TEMPHD
template<typename T>
auto _HIPP_TEMPCLS::pad() const noexcept -> const T & 
{
    const auto *p = reinterpret_cast<std::add_const_t<T> *>(_pad);
    return *p;
}

_HIPP_TEMPRET
volume() const noexcept -> float_t 
{
    return _pow( _r + _r );
}

_HIPP_TEMPRET
volume(float_t d) noexcept -> float_t 
{
    return _pow( d );
}

_HIPP_TEMPRET
bounding_ball(const point_t &p) const noexcept -> _InsertableBallTreeNode 
{
    sphere_t bs = bounding_sphere(p);
    return _InsertableBallTreeNode{bs.center(), bs.r()};    
}

_HIPP_TEMPRET
bounding_ball(const sphere_t &s) const noexcept -> _InsertableBallTreeNode 
{
    sphere_t bs = bounding_sphere(s);
    return _InsertableBallTreeNode{bs.center(), bs.r()};
}

_HIPP_TEMPRET
minimal_dist_to(const sphere_t &s) const noexcept -> float_t 
{
    return (s.center() - _center).r() - s.r();
}

_HIPP_TEMPRET
_pow(float_t x) noexcept -> float_t {
    if constexpr ( DIM == 1 ) { return x; }
    else if constexpr ( DIM == 2 ) { return x*x; }
    else if constexpr ( DIM == 3 ) { return x*x*x; }
    else if constexpr ( DIM == 4 ) { return (x*x)*(x*x); }
    else {
        return (float_t)std::pow(x, DIM);
    }
}

#undef _HIPP_TEMPHD
#undef _HIPP_TEMPARG
#undef _HIPP_TEMPCLS
#undef _HIPP_TEMPRET
#undef _HIPP_TEMPNORET

#define _HIPP_TEMPHD template<typename KDPointT, typename IndexT>
#define _HIPP_TEMPARG <KDPointT, IndexT>
#define _HIPP_TEMPCLS _InsertableBallTree _HIPP_TEMPARG
#define _HIPP_TEMPRET _HIPP_TEMPHD inline auto _HIPP_TEMPCLS::
#define _HIPP_TEMPNORET _HIPP_TEMPHD inline _HIPP_TEMPCLS::

_HIPP_TEMPNORET
_InsertableBallTree(const construct_policy_t &policy) noexcept 
: _construct_policy(policy) 
{
    _nodes.reserve( _construct_policy.init_alloc_size() );
    _nodes.emplace_back( pos_t{0,0,0}, float_t(0));

    node_t &r = root();
    r._parent_idx = r._left_child_idx = r._right_child_idx = node_t::idxNULL;
    r._node_info = node_t::tLEAF;
}

_HIPP_TEMPNORET
_InsertableBallTree(const _InsertableBallTree &o) = default;

_HIPP_TEMPNORET
_InsertableBallTree(_InsertableBallTree &&o) = default;

_HIPP_TEMPRET
operator=(const _InsertableBallTree &o) noexcept -> _InsertableBallTree & 
= default;

_HIPP_TEMPRET
operator=(_InsertableBallTree &&o) noexcept -> _InsertableBallTree & = default;

_HIPP_TEMPNORET
~_InsertableBallTree() noexcept = default;

_HIPP_TEMPRET
info(ostream &os, int  fmt_cntl, int level) const -> ostream & {
    PStream ps(os);
    if( fmt_cntl < 1 ) {
        ps << HIPPCNTL_CLASS_INFO_INLINE(_InsertableBallTree), 
        "{no. nodes allocated=", _nodes.size(), 
        ", ", _construct_policy, "}"; 
        return os;
    }
    auto ind = HIPPCNTL_CLASS_INFO_INDENT_STR(level);
    ps << HIPPCNTL_CLASS_INFO(_InsertableBallTree),
    ind, "No. nodes allocated = ", _nodes.size(), '\n',
    ind, ps.info_of(_construct_policy, fmt_cntl, level+1);
    return os;
}

_HIPP_TEMPRET
reserve(index_t n) -> void {
    _nodes.reserve(n);
}

_HIPP_TEMPRET
clear() -> void {
    if( empty() ) return;
    _unlink_subtree( root()._left_child_idx );
    _nodes.resize(1);
}

_HIPP_TEMPRET
construct_policy() noexcept -> construct_policy_t & {
    return _construct_policy; 
}

_HIPP_TEMPRET
construct_policy() const noexcept -> const construct_policy_t & {
    return _construct_policy; 
}

_HIPP_TEMPRET
nodes() noexcept -> vector<node_t> & {
    return _nodes; 
}

_HIPP_TEMPRET
nodes() const noexcept -> const vector<node_t> & {
    return _nodes; 
}

_HIPP_TEMPRET
empty() const noexcept -> bool {
    return root()._left_child_idx == node_t::idxNULL;
}

_HIPP_TEMPRET
size() const -> index_t 
{
    return size(root()._left_child_idx);
}

_HIPP_TEMPHD
template<typename PreorderTraversePolicy>
auto _HIPP_TEMPCLS::size(index_t node_idx, 
    PreorderTraversePolicy &&policy) const -> index_t 
{
    index_t cnt = 0;
    _preorder_traverse(node_idx, 
        [&cnt,this](index_t i){ 
            if( _nodes[i].is_leaf() ) ++cnt; 
        }, 
        policy);
    return cnt;
}

_HIPP_TEMPRET
volume() const -> float_t
{
    return volume(root()._left_child_idx);
}

_HIPP_TEMPHD
template<typename PreorderTraversePolicy>
auto _HIPP_TEMPCLS::volume(index_t node_idx, 
    PreorderTraversePolicy &&policy) const -> float_t
{
    double vol {};
    _preorder_traverse(node_idx,
        [&vol, this](index_t i){
            vol += static_cast<double>(_nodes[i].volume());
        }, policy);
    return static_cast<float_t>(vol);
}

_HIPP_TEMPRET
root() const noexcept -> const node_t & {
    return _nodes.front();
}

_HIPP_TEMPRET
root() noexcept -> node_t & {
    return _nodes.front();
}

_HIPP_TEMPHD
template<typename Policy>
void _HIPP_TEMPCLS::insert(const kd_point_t &pt, Policy &&policy) {
    node_t n_new (pt, float_t(0),
        node_t::idxNULL, node_t::idxNULL, node_t::idxNULL, node_t::tLEAF, 
        pt.pad());
    const index_t n_nodes = _nodes.size();

    if( empty() ) {
        _nodes.push_back(n_new);
        _insert_into_empty_tree(n_nodes);
        return;
    }

    const index_t i_sibl = _find_best_insert_loc(pt, policy);
    node_t n_par = _nodes[i_sibl].bounding_ball(pt);
    n_par._node_info = node_t::tINTERNAL;

    _nodes.push_back(n_par);
    _nodes.push_back(n_new);

    _insert_at(n_nodes+1, i_sibl, n_nodes);
}

_HIPP_TEMPHD
template<typename PreorderTraversePolicy>
void _HIPP_TEMPCLS::remove(index_t node_idx, PreorderTraversePolicy &&policy) {
    index_t i_par = _unlink_subtree(node_idx);
    if( i_par != node_t::idxNULL )
        _nodes[i_par]._node_info = node_t::tNULL;
    _preorder_traverse(node_idx, 
        [this](index_t i){ 
            _nodes[i]._node_info = node_t::tNULL; 
        }, policy);
}

_HIPP_TEMPHD
template<typename InsertPolicy>
auto _HIPP_TEMPCLS::bottom_up_refine(InsertPolicy &&policy) -> void {

    if( empty() ) return;

    struct pair_t {
        index_t i, i_ngb;
        float_t d;              // bounding d
        bool operator<(const pair_t &o) const noexcept { return d > o.d; }
    };
    
    // Push all leaves into the queue.
    std::priority_queue<pair_t> pairs;
    std::stack<std::pair<index_t, float_t> > workspace;

    const index_t n_nodes = _nodes.size();
    for(index_t i=1; i<n_nodes; ++i){
        const node_t &n = _nodes[i];
        if( n.is_leaf() ) {
            auto [i_ngb, d] = _find_best_leaf_pair_with(i, workspace);
            pairs.push(pair_t{i, i_ngb, d});
        }
    }
    
    // Pair up two leaves, insert back their parent as a new leaf.
    index_t n_leaves = pairs.size();
    while( n_leaves > 1 ) {
        auto [i, i_ngb, d] = pairs.top(); pairs.pop();
        node_t &n = _nodes[i];
        if( ! n.is_leaf() ) continue;

        if( ! _nodes[i_ngb].is_leaf() )
            std::tie(i_ngb, d) = _find_best_leaf_pair_with(i, workspace);

        auto i_par = _merge_pair(i, i_ngb, policy);
        auto [i_par_ngb, d_par] = _find_best_leaf_pair_with(i_par, workspace);
        pairs.push(pair_t{i_par, i_par_ngb, d_par});

        --n_leaves;
    }
    _nodes[ root()._left_child_idx ]._node_info = node_t::tINTERNAL;
    
    // Restore tLEAF flag for leaves.
    for(auto &n: _nodes)
        if( ! n.is_null() && n._left_child_idx == node_t::idxNULL ) 
            n._node_info = node_t::tLEAF;
}

_HIPP_TEMPHD
template<typename Op>
void _HIPP_TEMPCLS::preorder_traverse(Op op) const
{
    preorder_traverse(root()._left_child_idx, op);
}

_HIPP_TEMPHD
template<typename Op, typename Policy>
void _HIPP_TEMPCLS::preorder_traverse(index_t node_idx, Op op, 
    Policy &&policy) const 
{
    _preorder_traverse(node_idx, op, policy);
}

_HIPP_TEMPHD
template<typename Op1, typename Op2>
void _HIPP_TEMPCLS::inorder_traverse(Op1 op1, Op2 op2) const 
{
    inorder_traverse(root()._left_child_idx, op1, op2);
}

_HIPP_TEMPHD
template<typename Op1, typename Op2, typename Policy>
void _HIPP_TEMPCLS::inorder_traverse(index_t node_idx, Op1 op1, Op2 op2, 
    Policy &&policy) const
{
    _inorder_traverse(node_idx, op1, op2, policy);
}

_HIPP_TEMPHD
template<typename Policy, typename FBoundingVol>
struct _HIPP_TEMPCLS::_Impl_find_best_insert_loc {    
    Policy &pl;
    FBoundingVol &f_vol;

    const _InsertableBallTree &dst;
    const vector<node_t> &nodes;

_Impl_find_best_insert_loc(Policy &_pl, FBoundingVol &_f_vol, 
    const _InsertableBallTree &_dst) 
: pl(_pl), f_vol(_f_vol), dst(_dst), nodes(dst._nodes)
{}

index_t operator()() const noexcept {
    using ins_fav_t = typename Policy::insert_favor_t;

    index_t idx = node_t::idxNULL;
    if( pl.insert_favor() == ins_fav_t::FINE ) {
        idx = run_fine();
    } else if( pl.insert_favor() == ins_fav_t::ROUGH ) {
        idx = run_rough();
    } else
        ErrLogic::throw_(ErrLogic::eDOMAIN, 
            emFLPFB, "  ... invalid insertion favor ", 
            (int) pl.insert_favor() );
    return idx;
}

index_t run_rough() const noexcept {
    index_t idx = dst.root()._left_child_idx,
        idx_best = idx;
    float_t ances_exp = 0, 
        bounding_vol = f_vol( nodes[idx] ), 
        dvol = bounding_vol - nodes[idx].volume(),
        min_cost = bounding_vol;
    do {
        if( min_cost < ances_exp ) break;

        const float_t cost = ances_exp + bounding_vol;
        if( cost < min_cost ) {
            min_cost = cost;
            idx_best = idx;
        }

        const node_t &n = nodes[idx];
        if( n.is_leaf() ) break;

        ances_exp += dvol;
        const index_t i_lc = n._left_child_idx, 
            i_rc = n._right_child_idx;
        float_t bv_lc, bv_rc, dv_lc, dv_rc;
        child_info(i_lc, bv_lc, dv_lc);
        child_info(i_rc, bv_rc, dv_rc);
        if( dv_lc < dv_rc ) {
            idx = i_lc; 
            bounding_vol = bv_lc;
            dvol = dv_lc;
        } else {
            idx = i_rc;
            bounding_vol = bv_rc;
            dvol = dv_rc;
        }
    } while ( idx != node_t::idxNULL );

    return idx_best;
}

void child_info(index_t i_c, float_t &bv_c, float_t &dv_c) const noexcept 
{
    auto &n_c = nodes[i_c];
    bv_c = f_vol(n_c);
    dv_c = bv_c - n_c.volume();
}

index_t run_fine() const noexcept 
{
    using frg_item_t = typename Policy::fringe_item_t;
    auto &frg = pl.fringe();
    
    index_t idx = dst.root()._left_child_idx,
        idx_best = idx;
    float_t ances_exp = 0, 
        bounding_vol = f_vol( nodes[idx] ), 
        min_cost = bounding_vol;
    frg.push(frg_item_t{idx, ances_exp, bounding_vol});

    while( !frg.empty() ) {

        auto &t = frg.top();
        idx = t.node_idx;
        ances_exp = t.ances_exp;
        bounding_vol = t.bounding_vol;
        frg.pop();

        if( min_cost < ances_exp  ) break;

        const float_t cost = ances_exp + bounding_vol;
        if( cost < min_cost ) {
            min_cost = cost;
            idx_best = idx;
        }
        
        const node_t &n = nodes[idx];
        if( n.is_leaf() ) continue;
        
        ances_exp += bounding_vol - n.volume();
        
        idx = n._left_child_idx;
        frg.push(frg_item_t{idx, ances_exp, f_vol(nodes[idx])});

        idx = n._right_child_idx;
        frg.push(frg_item_t{idx, ances_exp, f_vol(nodes[idx])});
    }

    while( !frg.empty() ) frg.pop();

    return idx_best;
}

};

_HIPP_TEMPHD
template<typename Policy>
auto _HIPP_TEMPCLS::_find_best_insert_loc(const point_t &p, 
    Policy &&policy) const noexcept -> index_t
{
    auto f_vol = [&p](const node_t &n) {
        return n._pow(n.bounding_d(p));
    };
    return _Impl_find_best_insert_loc<
        std::remove_reference_t<Policy>,
        decltype(f_vol) > {policy, f_vol, *this}();
}

_HIPP_TEMPHD
template<typename Policy>
auto _HIPP_TEMPCLS::_find_best_insert_loc(const sphere_t &s, 
    Policy &&policy) const noexcept -> index_t
{
    auto f_vol = [&s](const node_t &n) {
        return n._pow(n.bounding_d(s));
    };
    return _Impl_find_best_insert_loc<
        std::remove_reference_t<Policy>,
        decltype(f_vol) > {policy, f_vol, *this}();
}


_HIPP_TEMPRET
_insert_at(index_t i, index_t i_sibl, index_t i_par) noexcept -> void 
{
    node_t &n = _nodes[i],
        &n_sibl = _nodes[i_sibl],
        &n_par = _nodes[i_par];
    const index_t i_old_par = n_sibl._parent_idx;
    node_t &n_old_par = _nodes[i_old_par];

    n_par._parent_idx = i_old_par;
    if( n_old_par._left_child_idx == i_sibl ) {
        n_old_par._left_child_idx = i_par;
        n_par._left_child_idx = i_sibl;
        n_par._right_child_idx = i;
    } else {
        n_old_par._right_child_idx = i_par;
        n_par._right_child_idx = i_sibl;
        n_par._left_child_idx = i;
    }
    n_sibl._parent_idx = n._parent_idx = i_par;

    _update_upward(i_old_par);
}

_HIPP_TEMPRET
_insert_into_empty_tree(index_t i) noexcept -> void 
{
    node_t &n = _nodes[i], &n_root = root();
    
    n_root._left_child_idx = i;
    n_root._node_info = node_t::tINTERNAL;
    
    n._parent_idx = ROOT_IDX;
}

_HIPP_TEMPRET
_update_upward(index_t i) noexcept -> void 
{
    while( i != ROOT_IDX ) {
        node_t &n = _nodes[i];
        sphere_t bs = _nodes[n._left_child_idx].bounding_sphere(
                _nodes[n._right_child_idx]);
        n._r = bs.r();
        n._center = bs.center();
        i = n._parent_idx;
    }
}

_HIPP_TEMPRET
_find_best_leaf_pair_with(index_t i,
    std::stack<std::pair<index_t, float_t> > &workspace) const noexcept 
-> std::pair<index_t, float_t>
{
    const node_t &n = _nodes[i];

    float_t d_best = std::numeric_limits<float_t>::max();
    index_t i_best = node_t::idxNULL;

    index_t i_cur = root()._left_child_idx;
    float_t min_dist = n.minimal_dist_to(_nodes[i_cur]);
    while( i_cur != node_t::idxNULL || ! workspace.empty() ) {
        if( i_cur == node_t::idxNULL ) {
            std::tie(i_cur, min_dist) = workspace.top(); 
            workspace.pop();
        }
        if( i_cur == i 
            || std::max(min_dist, n.r()) + n.r() >= d_best ) 
        {
            i_cur = node_t::idxNULL; 
            continue;
        }
        const node_t &n_cur = _nodes[i_cur];
        if( n_cur.is_leaf() ) {
            float_t d = n.bounding_d(n_cur);
            if( d < d_best ) {
                i_best = i_cur; d_best = d;
            }
            i_cur = node_t::idxNULL; 
            continue;
        } 
        const index_t i_lc = n_cur._left_child_idx, 
            i_rc = n_cur._right_child_idx;
        const float_t d_lc = n.minimal_dist_to(_nodes[i_lc]),
            d_rc = n.minimal_dist_to(_nodes[i_rc]);
        if( d_lc < d_rc ) {
            i_cur = i_lc; min_dist = d_lc;
            workspace.emplace(i_rc, d_rc);
        } else {
            i_cur = i_rc; min_dist = d_rc;
            workspace.emplace(i_lc, d_lc);
        }
    }

    return {i_best, d_best};
}

_HIPP_TEMPHD
template<typename InsertPolicy>
auto _HIPP_TEMPCLS::_merge_pair(index_t i, index_t i_ngb, 
    InsertPolicy &&policy) noexcept -> index_t 
{
    const index_t i_par = _unlink_subtree(i), 
        i_ngb_par = _unlink_subtree(i_ngb);
    node_t &n = _nodes[i], 
        &n_ngb = _nodes[i_ngb],
        &n_par = _nodes[i_par];                 // Must be no-root.
    n_par = n.bounding_ball(n_ngb);

    n._node_info = node_t::tINTERNAL;
    n_ngb._node_info = node_t::tINTERNAL;
    n._parent_idx = n_ngb._parent_idx = i_par;
    
    n_par._left_child_idx = i;
    n_par._right_child_idx = i_ngb;
    n_par._node_info = node_t::tLEAF;

    if( i_ngb_par == node_t::idxNULL ) {
        _insert_into_empty_tree(i_par);
        return i_par;
    }
    
    const index_t i_par_sibl = _find_best_insert_loc(n_par, 
        policy);
    auto &n_par_par = _nodes[i_ngb_par];
    n_par_par = _nodes[i_par_sibl].bounding_ball(n_par);
    n_par_par._node_info = node_t::tINTERNAL;

    _insert_at(i_par, i_par_sibl, i_ngb_par);

    return i_par;
}

_HIPP_TEMPRET
_unlink_subtree(index_t i) noexcept -> index_t {
    node_t &n = _nodes[i];
    const index_t i_par = n._parent_idx;

    if( i_par == ROOT_IDX ) {               // The whole tree is unlinked.
        node_t &n_root = root();
        n_root._left_child_idx = node_t::idxNULL;
        n_root._node_info = node_t::tLEAF;

        return node_t::idxNULL;
    }

    node_t &n_par = _nodes[i_par];
    const index_t i_par_par = n_par._parent_idx,
        i_sibl = (n_par._left_child_idx == i) ? 
            n_par._right_child_idx : n_par._left_child_idx;

    _nodes[i_sibl]._parent_idx = i_par_par;

    node_t &n_par_par = _nodes[i_par_par];
    if( n_par_par._left_child_idx == i_par ) {
        n_par_par._left_child_idx = i_sibl;
    } else {
        n_par_par._right_child_idx = i_sibl;
    }

    _update_upward(i_par_par);

    return i_par;
}

_HIPP_TEMPHD
template<typename Op, typename Policy>
void _HIPP_TEMPCLS::_preorder_traverse(index_t node_idx, Op op, 
    Policy &&policy) const 
{
    auto &stk = policy.stack();
    index_t idx = node_idx;
    while( idx != node_t::idxNULL || ! stk.empty() ) {
        if( idx == node_t::idxNULL ) {
            idx = stk.top(); stk.pop();
        }
        do {
            op(idx);
            const node_t &n = _nodes[idx];
            idx = n._left_child_idx;
            if ( idx == node_t::idxNULL ) break;
            stk.push(n._right_child_idx);
        } while(true);
    }
}

_HIPP_TEMPHD
template <typename Op1, typename Op2, typename Policy>
void _HIPP_TEMPCLS::_inorder_traverse(index_t node_idx, Op1 op1, Op2 op2, 
    Policy &&policy) const 
{
    auto &stk = policy.stack();
    index_t idx = node_idx;
    while( idx != node_t::idxNULL || ! stk.empty() ) {
        if( idx == node_t::idxNULL ) {
            idx = stk.top(); stk.pop();
            op2(idx);
            idx = _nodes[idx]._right_child_idx;
        }
        do {
            op1(idx);
            const auto &n = _nodes[idx];
            const index_t idx_lc = n._left_child_idx;
            if( idx_lc == node_t::idxNULL ) {
                op2(idx); 
                idx = node_t::idxNULL;
                break;
            }
            stk.push(idx);
            idx = idx_lc;
        } while(true);
    }
}

#undef _HIPP_TEMPHD
#undef _HIPP_TEMPARG
#undef _HIPP_TEMPCLS
#undef _HIPP_TEMPRET
#undef _HIPP_TEMPNORET

#define _HIPP_TEMPHD template<typename KDPointT, typename IndexT>
#define _HIPP_TEMPARG <KDPointT, IndexT>
#define _HIPP_TEMPCLS _InsertableBallTree _HIPP_TEMPARG::construct_policy_t
#define _HIPP_TEMPRET _HIPP_TEMPHD inline auto _HIPP_TEMPCLS::
#define _HIPP_TEMPNORET _HIPP_TEMPHD inline _HIPP_TEMPCLS::

_HIPP_TEMPNORET
construct_policy_t() noexcept : _init_alloc_size (DFLT_INIT_ALLOC_SIZE) {}

_HIPP_TEMPRET
info(ostream &os, int  fmt_cntl, int level) const -> ostream & {
    PStream ps(os);
    if( fmt_cntl < 1 ) {
        ps << HIPPCNTL_CLASS_INFO_INLINE(construct_policy_t),
        "{init alloc size=", _init_alloc_size, "}";
        return os;
    }
    auto ind = HIPPCNTL_CLASS_INFO_INDENT_STR(level);
    ps << HIPPCNTL_CLASS_INFO(construct_policy_t),
    ind, "Init alloc size = ", _init_alloc_size, '\n';
    return os;
}

_HIPP_TEMPRET
init_alloc_size() const noexcept -> index_t {
    return _init_alloc_size;
}

_HIPP_TEMPRET
set_init_alloc_size(index_t value) noexcept -> construct_policy_t & {
    _init_alloc_size = value;
    return *this;
}

#undef _HIPP_TEMPHD
#undef _HIPP_TEMPARG
#undef _HIPP_TEMPCLS
#undef _HIPP_TEMPRET
#undef _HIPP_TEMPNORET

#define _HIPP_TEMPHD template<typename KDPointT, typename IndexT>
#define _HIPP_TEMPARG <KDPointT, IndexT>
#define _HIPP_TEMPCLS _InsertableBallTree _HIPP_TEMPARG::insert_policy_t
#define _HIPP_TEMPRET _HIPP_TEMPHD inline auto _HIPP_TEMPCLS::
#define _HIPP_TEMPNORET _HIPP_TEMPHD inline _HIPP_TEMPCLS::

_HIPP_TEMPNORET
insert_policy_t() noexcept : insert_policy_t(DFLT_INSERT_FAVOR) {}

_HIPP_TEMPRET
info(ostream &os, int  fmt_cntl, int level) const -> ostream & {
    PStream ps(os);
    
    const char *s_fav = "UNKNOWN";
    if( _insert_favor == insert_favor_t::FINE ) 
        s_fav = "FINE";
    else if( _insert_favor == insert_favor_t::ROUGH )
        s_fav = "ROUGH";

    const size_t frg_sz = _fringe.size();

    if( fmt_cntl < 1 ) {
        ps << HIPPCNTL_CLASS_INFO_INLINE(insert_policy_t),
        "{insert favor=", s_fav, ", fringe size=", frg_sz, "}";
        return os;
    }
    auto ind = HIPPCNTL_CLASS_INFO_INDENT_STR(level);
    ps << HIPPCNTL_CLASS_INFO(insert_policy_t),
    ind, "Insert favor = ", s_fav, ", fringe size = ", frg_sz, '\n';
    return os;
}

_HIPP_TEMPNORET
insert_policy_t(insert_favor_t insert_favor) noexcept 
: _insert_favor(insert_favor)
{}

_HIPP_TEMPRET
insert_favor() const noexcept -> insert_favor_t {
    return _insert_favor;
}

_HIPP_TEMPRET
set_insert_favor(insert_favor_t value) noexcept -> insert_policy_t & {
    _insert_favor = value;
    return *this;
}

_HIPP_TEMPRET
fringe() noexcept -> fringe_t & {
    return _fringe;
}

#undef _HIPP_TEMPHD
#undef _HIPP_TEMPARG
#undef _HIPP_TEMPCLS
#undef _HIPP_TEMPRET
#undef _HIPP_TEMPNORET

#define _HIPP_TEMPHD template<typename KDPointT, typename IndexT>
#define _HIPP_TEMPARG <KDPointT, IndexT>
#define _HIPP_TEMPCLS _InsertableBallTree \
    _HIPP_TEMPARG::preorder_traverse_policy_t
#define _HIPP_TEMPRET _HIPP_TEMPHD inline auto _HIPP_TEMPCLS::
#define _HIPP_TEMPNORET _HIPP_TEMPHD inline _HIPP_TEMPCLS::

_HIPP_TEMPNORET
preorder_traverse_policy_t() noexcept {}

_HIPP_TEMPRET
info(ostream &os, int  fmt_cntl, int level) const -> ostream & {
    PStream ps(os);
    if( fmt_cntl < 1 ) {
        ps << HIPPCNTL_CLASS_INFO_INLINE(preorder_traverse_policy_t),
        "{stack size=", _stack.size(), "}";
        return os;
    }
    auto ind = HIPPCNTL_CLASS_INFO_INDENT_STR(level);
    ps << HIPPCNTL_CLASS_INFO(preorder_traverse_policy_t),
    ind, "Stack size = ", _stack.size(), '\n';
    return os;
}

_HIPP_TEMPRET
stack() noexcept -> stack_t & { return _stack; }

#undef _HIPP_TEMPHD
#undef _HIPP_TEMPARG
#undef _HIPP_TEMPCLS
#undef _HIPP_TEMPRET
#undef _HIPP_TEMPNORET

#define _HIPP_TEMPHD template<typename KDPointT, typename IndexT>
#define _HIPP_TEMPARG <KDPointT, IndexT>
#define _HIPP_TEMPCLS _InsertableBallTree \
    _HIPP_TEMPARG::inorder_traverse_policy_t
#define _HIPP_TEMPRET _HIPP_TEMPHD inline auto _HIPP_TEMPCLS::
#define _HIPP_TEMPNORET _HIPP_TEMPHD inline _HIPP_TEMPCLS::

_HIPP_TEMPNORET
inorder_traverse_policy_t() noexcept {}

_HIPP_TEMPRET
info(ostream &os, int  fmt_cntl, int level) const -> ostream & {
    PStream ps(os);
    if( fmt_cntl < 1 ) {
        ps << HIPPCNTL_CLASS_INFO_INLINE(inorder_traverse_policy_t),
        "{stack size=", _stack.size(), "}";
        return os;
    }
    auto ind = HIPPCNTL_CLASS_INFO_INDENT_STR(level);
    ps << HIPPCNTL_CLASS_INFO(inorder_traverse_policy_t),
    ind, "Stack size = ", _stack.size(), '\n';
    return os;
}

_HIPP_TEMPRET
stack() noexcept -> stack_t & { return _stack; }

#undef _HIPP_TEMPHD
#undef _HIPP_TEMPARG
#undef _HIPP_TEMPCLS
#undef _HIPP_TEMPRET
#undef _HIPP_TEMPNORET


} // namespace HIPP::NUMERICAL::_KDSEARCH

#endif	//_HIPPNUMERICAL_KDSEARCH_INSERTABLE_BALLTREE_RAW_IMPL_H_