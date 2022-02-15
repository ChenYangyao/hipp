/**
create: Yangyao CHEN, 2022/02/14
    [write   ] _KDTreeNode, _KDTree - implementation classes of KDTree.
*/


#ifndef _HIPPNUMERICAL_KDSEARCH_KDTREE_RAW_H_
#define _HIPPNUMERICAL_KDSEARCH_KDTREE_RAW_H_

#include "kdsearch_base.h"

namespace HIPP::NUMERICAL::_KDSEARCH {

template<typename _FloatT, int _DIM, size_t _PADDING, typename _IndexT> 
class _KDTreeNode : public GEOMETRY::Point<_FloatT, _DIM> {
public:
    inline static constexpr int DIM = _DIM;    
    inline static constexpr size_t PADDING = _PADDING;    

    using point_t = typename GEOMETRY::Point<_FloatT, _DIM>;
    using kd_point_t = KDPoint<_FloatT, _DIM, _PADDING>;

    using float_t = typename kd_point_t::float_t;
    using pos_t = typename kd_point_t::pos_t;
    using offset_t = typename kd_point_t::offset_t;
    using index_t = _IndexT;

    _KDTreeNode() noexcept;

    _KDTreeNode(const pos_t &pos) noexcept;

    template<typename PadT>
    _KDTreeNode(const pos_t &pos, const PadT &pad) noexcept;

    template<typename PadT>
    _KDTreeNode(const pos_t &pos, index_t size, int axis, 
        const PadT &pad) noexcept;

    ostream & info(ostream &os = cout, int fmt_cntl = 0, int level = 0) const;
    friend ostream & operator<<(ostream &os, const _KDTreeNode &n) {
        return n.info(os);
    }

    using point_t::pos;

    const index_t & size() const noexcept;
    index_t & size() noexcept;
    
    const int & axis() const noexcept;
    int & axis() noexcept;
    
    template<typename PadT>
    void fill_pad(const PadT &pad) noexcept;
    void fill_pad(const char *p, size_t n) noexcept;

    template<typename T = char[PADDING]>
    T & pad() noexcept;
    
    template<typename T = char[PADDING]>
    const T & pad() const noexcept;
protected:
    using point_t::_pos;

    index_t _size;
    int _axis;
    char _pad[PADDING];
};

template<typename KDPointT = KDPoint<>, typename IndexT = int>
class _KDTree {
public:
    using kd_point_t = KDPointT;

    using float_t = typename kd_point_t::float_t;
    using index_t = IndexT;
    static constexpr int DIM = kd_point_t::DIM;
    static constexpr size_t PADDING = kd_point_t::PADDING;

    using node_t = _KDTreeNode<float_t, DIM, PADDING, index_t>;
    using point_t = typename node_t::point_t;
    
    using pos_t = typename node_t::pos_t;
    using rect_t    = GEOMETRY::Rect<float_t, DIM>;
    using sphere_t  = GEOMETRY::Sphere<float_t, DIM>;

    struct tree_info_t;
    struct idx_pair_t;
    struct ngb_t;
    
    struct construct_policy_t;
    struct query_buff_policy_t;
    struct nearest_query_policy_t;
    struct nearest_k_query_policy_t;
    struct rect_query_policy_t;
    struct sphere_query_policy_t;

    _KDTree() noexcept;

    _KDTree(const _KDTree &o);
    _KDTree(_KDTree &&o);
    _KDTree & operator=(const _KDTree &o) noexcept;
    _KDTree & operator=(_KDTree &&o) noexcept;
    ~_KDTree() noexcept;

    ostream & info(ostream &os = cout, int fmt_cntl = 0, int level = 0) const;
    friend ostream & operator<<(ostream &os, const _KDTree &kdt) {
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
    ``I.idx_in`` is the index into ``pts`` and ``I.idx_node`` is the index 
    of NN candidate of that point.
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

    template<typename Policy = nearest_query_policy_t>
    ngb_t nearest(const point_t &p,
        Policy &&policy = Policy()) const;
    template<typename Policy = nearest_k_query_policy_t>
    index_t nearest_k(const point_t &p, 
        ContiguousBuffer<ngb_t> ngbs,
        Policy &&policy = Policy()) const;

    /**
    ``op(i)`` is called on each visited node indexed ``i``.
    */
    template<typename Op, typename Policy = rect_query_policy_t>
    void visit_rect(const rect_t &rect, Op op, 
        Policy &&policy = Policy()) const;
    template<typename Policy = rect_query_policy_t>
    index_t count_rect(const rect_t &rect, 
        Policy &&policy = Policy()) const;

    template<typename Op, typename Policy = sphere_query_policy_t>
    void visit_sphere(const sphere_t &sphere, Op op, 
        Policy &&policy = Policy()) const;
    template<typename Policy = sphere_query_policy_t>
    index_t count_sphere(const sphere_t &sphere,
        Policy &&policy = Policy()) const;
private:
    construct_policy_t _construct_policy;
    tree_info_t _tree_info;
    vector<node_t> _nodes;

    struct _Impl_construct;

    /**
    Base classes for neighbor searching implementation. 
    
    Inheritance: _Impl_query_base <- _Impl_query_{pre|in}_order.
    
    The pre-order/in-order searching algorithms are used for fix-domain and
    adapt-domain queries, respectively.
    */
    struct _Impl_query_base;
    template<typename Policy> struct _Impl_query_pre_order;
    template<typename Policy> struct _Impl_query_in_order;

    /**
    Concrete classes for implementing nearest(), nearest_k(), visit_rect(), 
    and visit_sphere(), respectively.
    */
    template<typename Policy> struct _Impl_nearest;
    template<typename Policy> struct _Impl_nearest_k;
    template<typename Op, typename Policy> struct _Impl_visit_rect;
    template<typename Op, typename Policy> struct _Impl_visit_sphere;    
};

template<typename KDPointT, typename IndexT>
class _KDTree<KDPointT, IndexT>::construct_policy_t {
public:
    enum class split_axis_t { MAX_EXTREME, MAX_VARIANCE, ORDERED, RANDOM };

    static constexpr split_axis_t DFLT_SPLIT_AXIS = split_axis_t::MAX_EXTREME;

    using rng_t = UniformIntRandomNumber<int>;
    using random_seed_t = rng_t::seed_t;
    static constexpr random_seed_t DFLT_RANDOM_SEED = 0;

    construct_policy_t();
    construct_policy_t(const construct_policy_t &pl);
    const construct_policy_t & operator=(const construct_policy_t & pl);

    ostream & info(ostream &os = cout, int fmt_cntl = 0, int level = 0) const;
    friend ostream & operator<< (ostream &os, 
        const construct_policy_t &pl) { return pl.info(os); }
    
    split_axis_t split_axis() const noexcept;
    construct_policy_t & set_split_axis(split_axis_t split_axis) noexcept;

    random_seed_t random_seed() const noexcept;
    construct_policy_t & set_random_seed(random_seed_t seed);
private:
    friend class _KDTree;

    split_axis_t _split_axis;

    random_seed_t _random_seed;
    rng_t::engine_t _re;
    rng_t _rng;
};

template<typename KDPointT, typename IndexT>
class _KDTree<KDPointT, IndexT>::tree_info_t {
public:
    index_t max_depth() const noexcept { return _max_depth; }

    ostream & info(ostream &os = cout, int fmt_cntl = 0, int level = 0) const;
    friend ostream & operator<< (ostream &os, 
        const tree_info_t &tree_info) { return tree_info.info(os); }
private:
    friend class _KDTree;

    index_t _max_depth;
};

template<typename KDPointT, typename IndexT>
struct _KDTree<KDPointT, IndexT>::idx_pair_t {
    index_t idx_in, idx_node;

    bool operator<(const idx_pair_t &o) const noexcept { 
        return idx_node < o.idx_node; 
    }
};

template<typename KDPointT, typename IndexT>
struct _KDTree<KDPointT, IndexT>::ngb_t {
    index_t node_idx;
    float_t r_sq;
    
    bool operator<(const ngb_t &rhs) const { return r_sq < rhs.r_sq; }
};


template<typename KDPointT, typename IndexT>
class _KDTree<KDPointT, IndexT>::query_buff_policy_t {
public:
    using buff_t = vector<index_t>;
    
    index_t * get_buff(index_t n);
protected:
    buff_t _buff;
};

template<typename KDPointT, typename IndexT>
class _KDTree<KDPointT, IndexT>::nearest_query_policy_t : 
    public query_buff_policy_t 
{};

template<typename KDPointT, typename IndexT>
class _KDTree<KDPointT, IndexT>::nearest_k_query_policy_t : 
    public query_buff_policy_t 
{
public:
    nearest_k_query_policy_t() noexcept;
    bool sort_by_distance() const noexcept;
    nearest_k_query_policy_t & sort_by_distance_on() noexcept;
    nearest_k_query_policy_t & sort_by_distance_off() noexcept;
protected:
    bool _sort_by_distance;
};

template<typename KDPointT, typename IndexT>
class _KDTree<KDPointT, IndexT>::rect_query_policy_t : 
    public query_buff_policy_t 
{};

template<typename KDPointT, typename IndexT>
class _KDTree<KDPointT, IndexT>::sphere_query_policy_t : 
    public query_buff_policy_t 
{};



} // namespace HIPP::NUMERICAL::_KDSEARCH

#endif	//_HIPPNUMERICAL_KDSEARCH_KDTREE_RAW_H_