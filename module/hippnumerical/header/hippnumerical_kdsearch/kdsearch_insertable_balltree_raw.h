#ifndef _HIPPNUMERICAL_KDSEARCH_INSERTABLE_BALLTREE_RAW_H_
#define _HIPPNUMERICAL_KDSEARCH_INSERTABLE_BALLTREE_RAW_H_

#include "kdsearch_base.h"

namespace HIPP::NUMERICAL::_KDSEARCH {

template<typename KDPointT, typename IndexT> class _InsertableBallTree;

template<typename _FloatT, int _DIM, size_t _PADDING, typename _IndexT>
class _InsertableBallTreeNode : public GEOMETRY::Sphere<_FloatT, _DIM> {
public:
    static constexpr int DIM = _DIM;    
    static constexpr size_t PADDING = _PADDING;
    
    using point_t    = GEOMETRY::Point<_FloatT, _DIM>;
    using sphere_t   = GEOMETRY::Sphere<_FloatT, _DIM>;
    using kd_point_t = KDPoint<_FloatT, _DIM, _PADDING>;

    using float_t     = typename kd_point_t::float_t;
    using pos_t       = typename kd_point_t::pos_t;
    using offset_t    = typename kd_point_t::offset_t;
    using index_t     = _IndexT;
    using node_info_t = char;

    using sphere_t::center;
    using sphere_t::r;

    static constexpr node_info_t 
        tNULL     = 0x0,            // No node is here, i.e., a hole.
        tINTERNAL = 0x1, 
        tLEAF     = 0x2;
    static constexpr index_t 
        idxNULL   = -1;             // No such node.

    _InsertableBallTreeNode() noexcept;

    explicit _InsertableBallTreeNode(const point_t &center, 
        float_t r = float_t(0)) noexcept;

    template<typename PadT>
    _InsertableBallTreeNode(const point_t &center, float_t r, 
        const PadT &pad) noexcept;

    template<typename PadT>
    _InsertableBallTreeNode(const point_t &center, float_t r, 
        index_t parent_idx, index_t left_child_idx, index_t right_idx,
        node_info_t node_info, const PadT &pad) noexcept;

    ostream & info(ostream &os = cout, int fmt_cntl = 0, int level = 0) const;
    friend ostream & operator<<(ostream &os, const _InsertableBallTreeNode &n) {
        return n.info(os);
    }

    const index_t & parent_idx() const noexcept;
    index_t & parent_idx() noexcept;
    
    const index_t & left_child_idx() const noexcept;
    index_t & left_child_idx() noexcept;

    const index_t & right_child_idx() const noexcept;
    index_t & right_child_idx() noexcept;

    const node_info_t & node_info() const noexcept;
    node_info_t & node_info() noexcept;
    bool is_null() const noexcept;
    bool is_leaf() const noexcept;
    bool is_internal() const noexcept;

    template<typename PadT>
    void fill_pad(const PadT &pad) noexcept;
    void fill_pad(const char *p, size_t n) noexcept;

    template<typename T = char[PADDING]>
    T & pad() noexcept;
    
    template<typename T = char[PADDING]>
    const T & pad() const noexcept;

    /** Simply pow(d, DIM). */
    float_t volume() const noexcept;
    static float_t volume(float_t d) noexcept;

    /** Only pos and r is set. The point_t version assume r = 0. */
    _InsertableBallTreeNode bounding_ball(const point_t &pt) const noexcept;
    _InsertableBallTreeNode bounding_ball(const sphere_t &s) const noexcept;
    
    using sphere_t::bounding_d;
    using sphere_t::bounding_sphere;

    /**
    Simply distance between centers minus s.r. Could be negative.
    */
    float_t minimal_dist_to(const sphere_t &s) const noexcept;
protected:
    friend class _InsertableBallTree<kd_point_t, index_t>;

    using sphere_t::_center;
    using sphere_t::_r;

    index_t _parent_idx, _left_child_idx, _right_child_idx;
    node_info_t _node_info;
    char _pad[PADDING];

    static float_t _pow(float_t x) noexcept;
};


template<typename KDPointT = KDPoint<>, typename IndexT = int>
class _InsertableBallTree {
public:
    using kd_point_t = KDPointT;

    using float_t = typename kd_point_t::float_t;
    using index_t = IndexT;
    
    static constexpr int DIM        = kd_point_t::DIM;
    static constexpr size_t PADDING = kd_point_t::PADDING;
    static constexpr index_t ROOT_IDX = 0;

    using node_t   = _InsertableBallTreeNode<float_t, DIM, PADDING, index_t>;
    using sphere_t = typename node_t::sphere_t;
    using point_t  = typename node_t::point_t;
    using pos_t    = typename node_t::pos_t;
    using rect_t   = GEOMETRY::Rect<float_t, DIM>;
    
    class construct_policy_t;
    class insert_policy_t;
    class preorder_traverse_policy_t;
    class inorder_traverse_policy_t;

    /**
    Pseudo-root is used.
    - _nodes.size() is even, at least 1.
    - Pseudo-root can be tLEAF or tINTERNAL, r and pad is undefined.
    - For non-empty tree, pseudo-root has left child, while its right child and 
      parent is always -1.
    */
    _InsertableBallTree(
        const construct_policy_t &policy = construct_policy_t()) noexcept;

    _InsertableBallTree(const _InsertableBallTree &o);
    _InsertableBallTree(_InsertableBallTree &&o);
    _InsertableBallTree & operator=(const _InsertableBallTree &o) noexcept;
    _InsertableBallTree & operator=(_InsertableBallTree &&o) noexcept;
    ~_InsertableBallTree() noexcept;

    ostream & info(ostream &os = cout, int  fmt_cntl = 0, int level = 0) const;
    friend ostream & operator<<(ostream &os, const _InsertableBallTree &ballt) {
        return ballt.info(os);
    }

    /** Reserve space for n nodes (all, including none-leaf). */
    void reserve(index_t n);
    /** Empty the tree so that empty() returns true. */
    void clear();

    construct_policy_t & construct_policy() noexcept;
    const construct_policy_t & construct_policy() const noexcept;
    vector<node_t> & nodes() noexcept;
    const vector<node_t> & nodes() const noexcept;
    /** 
    Whether the tree has not data node (i.e., 
    pseudo-root has not left child). 
    */
    bool empty() const noexcept;
    
    /** 
    Get the number of leaves in the subtree rooted at ``idx``. 
    ``size()`` with no argument is the shortcut for the whole tree with default
    policy.
    */
    index_t size() const;
    template<typename PreorderTraversePolicy = preorder_traverse_policy_t>
    index_t size(index_t node_idx, PreorderTraversePolicy &&policy 
        = PreorderTraversePolicy()) const;
    
    /** Total volume for internal + leaf nodes, each using node.volume(). */
    float_t volume() const;
    template<typename PreorderTraversePolicy = preorder_traverse_policy_t>
    float_t volume(index_t node_idx, PreorderTraversePolicy &&policy 
        = PreorderTraversePolicy()) const;
    
    const node_t & root() const noexcept;
    node_t & root() noexcept;

    /** Need shrink_to_fit - move unused to holes. */

    /** 
    Insert a new node with pos and padding taken from ``pt``.
    */
    template<typename Policy = insert_policy_t>
    void insert(const kd_point_t &pt, Policy &&policy = Policy());
    
    /**
    The subtree is removed.
    Cheap, just mark the removed nodes as tNULL; modify the pointers and radii 
    and positions of the remaining.
    `node_idx` must be in the tree (leaf or internal), should not be the 
    pseudo-root.
    */
    template<typename PreorderTraversePolicy = preorder_traverse_policy_t>
    void remove(index_t node_idx, PreorderTraversePolicy &&policy
         = PreorderTraversePolicy());

    /** 
    Could be empty.
    */
    template<typename InsertPolicy = insert_policy_t>
    void bottom_up_refine(InsertPolicy &&policy = InsertPolicy());


    template<typename Op>
    void preorder_traverse(Op op) const;
    
    template<typename Op, typename Policy = preorder_traverse_policy_t>
    void preorder_traverse(index_t node_idx, Op op, 
        Policy &&policy = Policy()) const;
    
    template<typename Op1, typename Op2>
    void inorder_traverse(Op1 op1, Op2 op2) const;
    
    template<typename Op1, typename Op2, typename Policy = inorder_traverse_policy_t>
    void inorder_traverse(index_t node_idx, Op1 op1, Op2 op2,
        Policy &&policy = Policy()) const;
protected:
    construct_policy_t _construct_policy;
    vector<node_t> _nodes;


    template<typename Policy, typename FBoundingVol> 
    struct _Impl_find_best_insert_loc;
    /** 
    Return sibling idx of the best insertion location. 'best' criterion is
    minimal spanning volume whose grainule is controlled by construct policy.
    Must not be empty tree.
    */
    template<typename Policy = insert_policy_t>
    index_t _find_best_insert_loc(const point_t &p, 
        Policy &&policy = Policy()) const noexcept;
    
    template<typename Policy = insert_policy_t>
    index_t _find_best_insert_loc(const sphere_t &s,
        Policy &&policy = Policy()) const noexcept;

    /**
    Update
        i_par - 3 pointers of;
        i_old_par - one child;
        i and i_sibl - parent.
    If i_sibl is originally l|r child, then it is still l|r child of i_par.
    */
    void _insert_at(index_t i, index_t i_sibl, index_t i_par) noexcept;
    /** 
    root->lc, i->par are updated.
    root becomes tINTERNAL.
    */
    void _insert_into_empty_tree(index_t i) noexcept;
    /** 
    Starting at (and include) i, recompute r, pos of the ancestor chain.
    Takes no effect if i is root.
    i must not be a leaf.
    */
    void _update_upward(index_t i) noexcept;


    /** 
    Must return a tLEAF node idx and the bounding diameter. 
    Tree must not be empty.

    i may be not in the tree. 
    Subtree rooted at i is not searched. Returned cound not be i itself.
    If not found (e.g. i is the only leaf), return {idxNULL, max_of_float_t}. 

    On entry, workspace must be empty. On exit, it is also empty. 
    */
    std::pair<index_t, float_t> _find_best_leaf_pair_with(index_t i,
        std::stack<std::pair<index_t, float_t> > &workspace) const noexcept;
    /** 
    Return their parent idx i_par. 
    i_par is inserted back to the tree as tLEAF. 
    i, i_ngb becomes tINTERNAL.
    May update 
        i, i_ngb, i_pars - all fields.
    */
    template<typename InsertPolicy>
    index_t _merge_pair(index_t i, index_t i_ngb, 
        InsertPolicy &&policy) noexcept;
    /** 
    Its removed parent idx is returned. If no such parent, do not remove any and 
    return idxNULL.
    Fields of unlinked subtree is not modified.
    Fields of the remaining tree is updated consistently.
    */
    index_t _unlink_subtree(index_t i) noexcept;

    /**
    Call op(i) on each node indexed i in the subtree rooted at `idx`.
    Use left_child_idx = idxNULL for branch termination.
    @idx: any valid node index or idxNULL, the later results in no op.

    Inorder: op1(i) and op2(i) called on the first (pre) and second (in) 
    visiting, respectively.
    */
    template<typename Op, typename Policy = preorder_traverse_policy_t>
    void _preorder_traverse(index_t node_idx, Op op, 
        Policy &&policy = Policy()) const;
    template<typename Op1, typename Op2, 
        typename Policy = inorder_traverse_policy_t>
    void _inorder_traverse(index_t node_idx, Op1 op1, Op2 op2, 
        Policy &&policy = Policy()) const;
};

template<typename KDPointT, typename IndexT>
class _InsertableBallTree<KDPointT, IndexT>::construct_policy_t {
public:
    static constexpr index_t DFLT_INIT_ALLOC_SIZE   = 64;

    construct_policy_t() noexcept;

    ostream & info(ostream &os = cout, int  fmt_cntl = 0, int level = 0) const;
    friend ostream & operator<<(ostream &os, const construct_policy_t &obj) {
        return obj.info(os);
    }

    index_t init_alloc_size() const noexcept;
    construct_policy_t & set_init_alloc_size(index_t value) noexcept;
private:
    index_t _init_alloc_size;
};

template<typename KDPointT, typename IndexT>
class _InsertableBallTree<KDPointT, IndexT>::insert_policy_t {
public:
    /**
    FINE:   time-consuming but better in terms of the tree volume.
    ROUGH:  cheaper.
    */
    enum class insert_favor_t {ROUGH, FINE};
    static constexpr insert_favor_t 
        DFLT_INSERT_FAVOR = insert_favor_t::ROUGH;

    struct fringe_item_t {
        index_t node_idx; 
        float_t ances_exp, bounding_vol;
        bool operator<(const fringe_item_t &o) const noexcept { 
            return ances_exp > o.ances_exp; }
    };
    using fringe_t = std::priority_queue<fringe_item_t>;

    insert_policy_t() noexcept;
    insert_policy_t(insert_favor_t insert_favor) noexcept;

    ostream & info(ostream &os = cout, int  fmt_cntl = 0, int level = 0) const;
    friend ostream & operator<<(ostream &os, const insert_policy_t &obj) {
        return obj.info(os);
    }

    insert_favor_t insert_favor() const noexcept;
    insert_policy_t & set_insert_favor(insert_favor_t value) noexcept;
    
    fringe_t & fringe() noexcept;
protected:
    insert_favor_t _insert_favor;
    fringe_t _fringe;
};

template<typename KDPointT, typename IndexT>
class _InsertableBallTree<KDPointT, IndexT>::preorder_traverse_policy_t {
public:  
    using stack_item_t = index_t;
    using stack_t = std::stack<stack_item_t>;

    preorder_traverse_policy_t() noexcept;

    ostream & info(ostream &os = cout, int  fmt_cntl = 0, int level = 0) const;
    friend ostream & operator<<(ostream &os, 
        const preorder_traverse_policy_t &obj) 
    {
        return obj.info(os);
    }

    stack_t & stack() noexcept;
protected:
    stack_t _stack;
};

template<typename KDPointT, typename IndexT>
class _InsertableBallTree<KDPointT, IndexT>::inorder_traverse_policy_t {
public:  
    using stack_item_t = index_t;
    using stack_t = std::stack<stack_item_t>;

    inorder_traverse_policy_t() noexcept;

    ostream & info(ostream &os = cout, int  fmt_cntl = 0, int level = 0) const;
    friend ostream & operator<<(ostream &os, 
        const inorder_traverse_policy_t &obj) 
    {
        return obj.info(os);
    }

    stack_t & stack() noexcept;
protected:
    stack_t _stack;
};

} // namespace HIPP::NUMERICAL::_KDSEARCH

#endif	//_HIPPNUMERICAL_KDSEARCH_INSERTABLE_BALLTREE_RAW_H_