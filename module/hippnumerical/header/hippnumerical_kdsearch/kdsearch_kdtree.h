/**
create: Yangyao CHEN, 2022/02/14
    [write   ] KDTree - K-dimensional tree algorithm for neighbor-based search.
*/

#ifndef _HIPPNUMERICAL_KDSEARCH_KDTREE_H_
#define _HIPPNUMERICAL_KDSEARCH_KDTREE_H_

#include "kdsearch_kdtree_raw impl.h"

namespace HIPP::NUMERICAL {

/**
K-dimensional tree algorithm for neighbor-based search.
*/
template<typename KDPointT = KDPoint<>, typename IndexT = int>
class KDTree {
public:
    /**
    Implementation detail.
    */
    using impl_t = _KDSEARCH::_KDTree<KDPointT, IndexT>;

    /**
    DIM: dimension of the space. 
    PADDING: size of the user-defined extra information attached to each 
    ``kd_point_t`` instance and tree node.
    */
    static constexpr int DIM = impl_t::DIM;
    static constexpr size_t PADDING = impl_t::PADDING;

    /**
    Point types. 
    ``point_t`` contains only the position of a point. 
    `kd_point_t`` is derived from ``point_t``, with padding to store 
    user-defined extra information. 
    ``node_t`` is derived from ``point_t``, with padding to store user-defined
    extra information and fields for tree implementation.
    */
    using point_t    = typename impl_t::point_t;
    using kd_point_t = typename impl_t::kd_point_t;
    using node_t     = typename impl_t::node_t;

    /**
    Numerical types.
    ``float_t`` and ``index_t`` are floating-point and signed integral scalar 
    numerical types, respectively.
    ``pos_t`` are Svec<> type position (floating-point).
    */
    using float_t    = typename impl_t::float_t;
    using index_t    = typename impl_t::index_t;
    using pos_t      = typename impl_t::pos_t;
    
    /**
    Geometry types for N-dimensional rectangle and sphere, respectively.
    */
    using rect_t     = typename impl_t::rect_t;
    using sphere_t   = typename impl_t::sphere_t;

    using construct_policy_t       = typename impl_t::construct_policy_t;
    using query_buff_policy_t      = typename impl_t::query_buff_policy_t;
    using nearest_query_policy_t   = typename impl_t::nearest_query_policy_t;
    using nearest_k_query_policy_t = typename impl_t::nearest_k_query_policy_t;
    using rect_query_policy_t      = typename impl_t::rect_query_policy_t;
    using sphere_query_policy_t    = typename impl_t::sphere_query_policy_t;

    using tree_info_t = typename impl_t::tree_info_t;
    using idx_pair_t  = typename impl_t::idx_pair_t;
    using ngb_t       = typename impl_t::ngb_t;

    /**
    Constructors.

    (1): Default construnction - no actual tree structure is constructed. 
    No query shall be made on the instance. Use ``construct()`` to construct 
    the tree.

    (2): Construct the tree by a series of points ``pts`` and the algorithm 
    ``policy``. For details, see :func:`construct`.
    */
    KDTree();
    KDTree(ContiguousBuffer<const kd_point_t> pts, 
        const construct_policy_t &policy = construct_policy_t());

    /**
    ``KDTree`` is copable and movable. The moved-from object is left in a 
    valid state but no other method, except copy, move, and destructor, shall
    be called on it.

    The copied-to object shares the same internal state with the source object.
    Queries can be made on them independently and thread-safely. Methods that 
    modify the tree may result in race condition.
    */
    KDTree(const KDTree &o) = default;
    KDTree(KDTree &&o) = default;
    KDTree & operator=(const KDTree &o) noexcept = default;
    KDTree & operator=(KDTree &&o) noexcept = default;
    ~KDTree() noexcept = default;

    ostream & info(ostream &os = cout, int  fmt_cntl = 0, int level = 0) const;
    friend ostream & operator<<(ostream &os, const KDTree &kdt) {
        return kdt.info(os);
    }

    /**
    Construct the tree that contains the points ``pts``. ``pts.size() == 0`` is
    valid. The previous tree structure is destroyed.
    
    @policy: control the tree construction algorithm. See the API-ref of 
    :type:`construct_policy_t` for details.
    */
    void construct(ContiguousBuffer<const kd_point_t> pts,
        const construct_policy_t &policy = construct_policy_t());

    /**
    Find the indices into points ``pts`` so that they are sorted according 
    to the rank of NN candidate node. The order of points belonging to the same 
    node is undefined.
    
    ``PointT`` must be a derived type of ``point_t``.
    
    On exit ``idx_pairs`` is resized to the ``pts.size()``, i.e., the number of
    points.
    For each item ``I`` of ``idx_pairs``, ``I.idx_in`` is the index into ``pts`` 
    and ``I.idx_node`` is the index of NN candidate of that point.
    */
    template<typename PointT>    
    void argsort(ContiguousBuffer<const PointT> pts, 
        vector<idx_pair_t> &idx_pairs) const;

    /**
    Memory management methods.
    
    shrink_buffer(): shrink the internal buffer to fit the used storage. 
    The tree structure is not affected.

    clear(): Destroy the tree structure. 
    The internal buffer becomes unused, but it may not be recycled - use 
    ``shrink_buffer()`` to free the storage. 
    The tree may be reconstructed again by ``construct()`` after clearance.
    */
    void shrink_buffer();
    void clear();

    std::shared_ptr<impl_t> impl() const noexcept;

    /**
    construct_policy() and nodes(): get the construct policy and nodes of the 
    instance, respectively.
    These internal states shall not be modified to avoid breaking consistency.

    left_child_idx(), right_child_idx(), right_sibling_idx(): return the indices
    of left child, right child, and right sibling, respectively, of a node 
    indexed ``node_idx``.
    */
    const construct_policy_t & construct_policy() const noexcept;
    construct_policy_t & construct_policy() noexcept;
    const vector<node_t> & nodes() const noexcept;
    vector<node_t> & nodes() noexcept;
    const tree_info_t & tree_info() const noexcept;
    tree_info_t & tree_info() noexcept;
    index_t left_child_idx(index_t node_idx) const noexcept;
    index_t right_child_idx(index_t node_idx) const noexcept;
    index_t right_sibling_idx(index_t node_idx) const noexcept;

    /**
    Walk down from a node indexed ``node_idx`` to a leaf node. 
    
    For a node ``n`` on the walk path, if 
    ``p.pos()[n.axis()] <= n.pos()[n.axis()]``, the left child of it is the 
    next node to visit, otherwise the right. If no such child exists, the walk
    is stopped.

    ``op(i)`` is called successively on each node on the path where ``i`` is the
    node index. On entry, ``node_idx`` specifies the index of the first node to
    visit. On exit, ``node_idx`` is set to the index of the last node along 
    the walk path, i.e. the NN candidate, in the subtree rooted in ``node_idx``, 
    of ``p``. 
    */
    template<typename Op>
    void walk_down(const point_t &p, Op op, index_t &node_idx) const;

    /**
    nearest(): find the tree node nearest to a given point ``p``. 
    A :type:`ngb_t` instance is returned, whose ``node_idx`` and ``r_sq`` fields
    are the result node and the squared distance to it.
    If tree is empty, returns {0, max_of_float_t}.

    nearest_k(): the same, but find the tree nodes that are the first k nearest 
    to ``p``. ``ngbs`` are an in-out argument. On entry, its size specifies 
    ``k``. On exit, the first ``k_used`` elements stores the information of 
    result neighbors. If the tree has less number of nodes than ``k``, 
    ``k_used`` is that number. Otherwise ``k_used = k``. ``k_used`` is returned.
    The order of the result nodes are undefined unless explicitly required by 
    the policy.

    See the API-ref of :type:`nearest_query_policy_t` and 
    :type:`nearest_k_query_policy_t` for the details on query policies.

    Thread safety: these three calls do not modify the tree. 
    */
    template<typename Policy = nearest_query_policy_t>
    ngb_t nearest(const point_t &p,
        Policy &&policy = Policy()) const;
    template<typename Policy = nearest_k_query_policy_t>
    index_t nearest_k(const point_t &p, 
        ContiguousBuffer<ngb_t> ngbs,
        Policy &&policy = Policy()) const;

    /**

    visit_nodes_rect(): visit all nodes within ``rect``.
    ``op(const node_t &node)`` is called on each node visited.

    count_nodes_rect(): count the exact number of nodes within ``rect``.

    See the API-ref of :type:`rect_query_policy_t` for the details on query 
    policy.
    
    Thread safety: these three calls do not modify the mesh.
    */
    template<typename Op, typename Policy = rect_query_policy_t>
    void visit_nodes_rect(const rect_t &rect, Op op, 
        Policy &&policy = Policy()) const;
    template<typename Policy = rect_query_policy_t>
    index_t count_nodes_rect(const rect_t &rect, 
        Policy &&policy = Policy()) const;

    /**
    visit_nodes_sphere(): visit all nodes within ``sphere``. 
    ``op(const node_t &node)`` is called on each node visited.

    count_sphere(): count the exact number of nodes within ``sphere``.
    
    See the API-ref of :type:`sphere_query_policy_t` for the details on query 
    policy.
    
    Thread safety: these three calls do not modify the mesh.
    */
    template<typename Op, typename Policy = sphere_query_policy_t>
    void visit_nodes_sphere(const sphere_t &sphere, Op op, 
        Policy &&policy = Policy()) const;
    template<typename Policy = sphere_query_policy_t>
    index_t count_nodes_sphere(const sphere_t &sphere,
        Policy &&policy = Policy()) const;
protected:
    std::shared_ptr<impl_t> _impl;
};

#define _HIPP_TEMPHD template<typename KDPointT, typename IndexT>
#define _HIPP_TEMPARG <KDPointT, IndexT>
#define _HIPP_TEMPCLS KDTree _HIPP_TEMPARG
#define _HIPP_TEMPRET _HIPP_TEMPHD inline auto _HIPP_TEMPCLS::
#define _HIPP_TEMPNORET _HIPP_TEMPHD inline _HIPP_TEMPCLS::

_HIPP_TEMPNORET
KDTree() : _impl( std::make_shared<impl_t>() ) {}

_HIPP_TEMPNORET
KDTree(ContiguousBuffer<const kd_point_t> pts, 
    const construct_policy_t &policy)
: KDTree()
{
    construct(pts, policy);
}

_HIPP_TEMPRET
info(ostream &os, int  fmt_cntl, int level) const -> ostream & {
    PStream ps(os);
    if( fmt_cntl < 1 ) {
        ps << HIPPCNTL_CLASS_INFO_INLINE(KDTree),
        "{impl=", *_impl, "}";
        return os;
    }
    auto ind = HIPPCNTL_CLASS_INFO_INDENT_STR(level);
    ps << HIPPCNTL_CLASS_INFO(KDTree),
    ind, ps.info_of(*impl, fmt_cntl, level+1);
    return os;
}

_HIPP_TEMPRET
construct(ContiguousBuffer<const kd_point_t> pts,
    const construct_policy_t &policy) -> void 
{
    _impl->construct(pts, policy);
}

_HIPP_TEMPHD
template<typename PointT>    
void _HIPP_TEMPCLS::argsort(ContiguousBuffer<const PointT> pts, 
    vector<idx_pair_t> &idx_pairs) const
{
    _impl->argsort(pts, idx_pairs);
}

_HIPP_TEMPRET
shrink_buffer() -> void {
    _impl->shrink_buffer();
}

_HIPP_TEMPRET
clear() -> void {
    _impl->clear();
}

_HIPP_TEMPRET
impl() const noexcept -> std::shared_ptr<impl_t> {
    return _impl;
}

_HIPP_TEMPRET
construct_policy() const noexcept -> const construct_policy_t & {
    return _impl->construct_policy();
}

_HIPP_TEMPRET
construct_policy() noexcept -> construct_policy_t & {
    return _impl->construct_policy();
}

_HIPP_TEMPRET
nodes() const noexcept -> const vector<node_t> & {
    return _impl->nodes();    
}

_HIPP_TEMPRET
nodes() noexcept -> vector<node_t> & {
    return _impl->nodes();    
}

_HIPP_TEMPRET
tree_info() const noexcept -> const tree_info_t & {
    return _impl->tree_info();    
}

_HIPP_TEMPRET
tree_info() noexcept -> tree_info_t & {
    return _impl->tree_info();    
}

_HIPP_TEMPRET
left_child_idx(index_t node_idx) const noexcept -> index_t {
    return _impl->left_child_idx(node_idx);
}

_HIPP_TEMPRET
right_child_idx(index_t node_idx) const noexcept -> index_t {
    return _impl->right_child_idx(node_idx);
}

_HIPP_TEMPRET
right_sibling_idx(index_t node_idx) const noexcept -> index_t {
    return _impl->right_sibling_idx(node_idx);
}

_HIPP_TEMPHD
template<typename Op>
void _HIPP_TEMPCLS::walk_down(const point_t &p, Op op, index_t &node_idx) const
{
    _impl->walk_down(p, op, node_idx);
}

_HIPP_TEMPHD
template<typename Policy>
auto _HIPP_TEMPCLS::nearest(const point_t &p, Policy &&policy) const -> ngb_t {
    return _impl->nearest(p, std::forward<Policy>(policy));
}

_HIPP_TEMPHD
template<typename Policy>
auto _HIPP_TEMPCLS::nearest_k(const point_t &p, ContiguousBuffer<ngb_t> ngbs, 
    Policy &&policy) const -> index_t
{
    return _impl->nearest_k(p, ngbs, std::forward<Policy>(policy));
}

_HIPP_TEMPHD
template<typename Op, typename Policy>
void _HIPP_TEMPCLS::visit_nodes_rect(const rect_t &rect, Op op, 
    Policy &&policy) const
{
    const auto &nds = nodes();
    _impl->visit_rect(rect, 
        [&op, &nds](index_t i){ op( nds[i] ); },
        std::forward<Policy>(policy));
}

_HIPP_TEMPHD
template<typename Policy>
auto _HIPP_TEMPCLS::count_nodes_rect(const rect_t &rect, 
    Policy &&policy) const -> index_t 
{
    return _impl->count_rect(rect, std::forward<Policy>(policy));
}

_HIPP_TEMPHD
template<typename Op, typename Policy>
void _HIPP_TEMPCLS::visit_nodes_sphere(const sphere_t &sphere, Op op, 
    Policy &&policy) const
{
    const auto &nds = nodes();
    _impl->visit_sphere(sphere, 
        [&op, &nds](index_t i){ op( nds[i] ); },
        std::forward<Policy>(policy));
}

_HIPP_TEMPHD
template<typename Policy>
auto _HIPP_TEMPCLS::count_nodes_sphere(const sphere_t &sphere, 
    Policy &&policy) const -> index_t 
{
    return _impl->count_sphere(sphere, std::forward<Policy>(policy));
}

#undef _HIPP_TEMPHD
#undef _HIPP_TEMPARG
#undef _HIPP_TEMPCLS
#undef _HIPP_TEMPRET
#undef _HIPP_TEMPNORET

} // namespace HIPP::NUMERICAL


#endif	//_HIPPNUMERICAL_KDSEARCH_KDTREE_H_