/**
create: Yangyao CHEN, 2022/02/14
    [write   ] _KDMesh - implementation class of KDMesh.
*/

#ifndef _HIPPNUMERICAL_KDSEARCH_KDMESH_RAW_H_
#define _HIPPNUMERICAL_KDSEARCH_KDMESH_RAW_H_

#include "kdsearch_base.h"

namespace HIPP::NUMERICAL::_KDSEARCH {

template<typename KDPointT = KDPoint<>, typename IndexT = int>
class _KDMesh {
public:
    using kd_point_t = KDPointT;
    using node_t     = kd_point_t;
    using point_t    = typename kd_point_t::point_t;
    
    using float_t = typename kd_point_t::float_t;
    using index_t = IndexT;
    static constexpr int DIM = point_t::DIM;
    static constexpr size_t PADDING = point_t::PADDING;
    
    using mesh_t    = GEOMETRY::Mesh<float_t, index_t, DIM>;
    using rect_t    = typename mesh_t::rect_t;
    using cells_t   = typename mesh_t::cells_t;
    using n_cell_t  = typename mesh_t::n_cell_t;
    using v_index_t = typename mesh_t::v_index_t;
    using pos_t     = typename point_t::pos_t;
    using sphere_t  = GEOMETRY::Sphere<float_t, DIM>;

    struct construct_policy_t;
    struct rect_query_policy_t;
    struct sphere_query_policy_t;
    struct idx_pair_t;

    _KDMesh() noexcept;

    _KDMesh(const _KDMesh &kdm);
    _KDMesh(_KDMesh &&kdm) noexcept;
    _KDMesh & operator=(const _KDMesh &kdm);
    _KDMesh & operator=(_KDMesh &&kdm) noexcept;
    ~_KDMesh() noexcept;

    ostream & info(ostream &os = cout, int fmt_cntl = 0, int level = 0) const;
    friend ostream & operator<<(ostream &os, const _KDMesh &kdm) {
        return kdm.info(os);
    }

    /**
    Construct the mesh that contains the points ``pts``. ``pts.size() == 0`` is
    valid. The previous mesh structure is destroyed.
    
    @policy: control the mesh construction algorithm. See the API-ref of 
    ``construct_policy_t`` for details. 
    */
    void construct(ContiguousBuffer<const kd_point_t> pts, 
        const construct_policy_t &policy = construct_policy_t());

    /**
    Find the indices into points ``pts`` so that they are sorted according 
    to the cell rank. The order of points within a cell is defined by the policy 
    when constructing the mesh.
    
    ``PointT`` must be a derived type of ``point_t``.
    
    On exit ``idx_pairs`` is resized to the number of points in ``pts`` that 
    is within the mesh. For each item ``I`` of ``idx_pairs``, ``I.idx_in`` 
    is the index into ``pts`` and ``I.idx_cell`` is the cell rank that contains
    this point.
    */
    template<typename PointT>
    void argsort(ContiguousBuffer<const PointT> pts, 
        vector<idx_pair_t> &idx_pairs) const;

    /**
    Memory management methods.
    
    shrink_buffer(): shrink the internal buffer to fit the used storage. 
    The mesh structure is not affected.

    clear(): Destroy the mesh structure. 
    The internal buffer becomes unused, but it may not be recycled - use 
    ``shrink_buffer()`` to free the storage. 
    The mesh may be reconstructed again by ``construct()`` after clearance.
    */
    void shrink_buffer();
    void clear();

    /**
    Find the contiguous buffer containing all nodes within the cell indexed 
    ``cell_idx``. Return the starting pointer of that buffer and the number 
    of nodes in it.
    */
    std::pair<const node_t *, index_t> 
    get_nodes_in_cell(index_t cell_idx) const noexcept;

    /**
    Getters: return references to construct_policy, mesh, and nodes of the 
    instance, respectively.
    These internal states shall not be modified to avoid breaking consistency.
    */
    const construct_policy_t & construct_policy() const noexcept;
    construct_policy_t & construct_policy() noexcept;
    const mesh_t & mesh() const noexcept;
    mesh_t & mesh() noexcept;
    const vector<node_t> & nodes() const noexcept;
    vector<node_t> & nodes() noexcept;
    
    /**
    visit_rect(): visit all cells that intersect with ``rect``. 
    count_rect(): count the exact number of nodes within ``rect``.

    @rect: a rectangle instance within which the cells or nodes are visited.
    @op: callback on each cell visited. The calling signature depends on
    ``Policy::intersect_judge``:
        - ROUGH: op(i);
        - STRICT: op(i, bound);
    where ``index_t i`` is the cell index and ``int bound`` indicates whether
    or not the cell is fully bounded in ``rect``.

    See the API-ref of ``rect_query_policy_t`` for the details on query policy.

    Thread safety: these two calls do not modify the mesh.
    */
    template<typename Op, typename Policy = rect_query_policy_t>
    void visit_rect(const rect_t &rect, Op op, 
        Policy &&policy = Policy()) const;
    index_t count_rect(const rect_t &rect) const;

    /**
    visit_sphere(): visit all cells within ``sphere``.
    count_sphere(): count the exact number of nodes within ``sphere``.
    
    @sphere: a sphere instance within which the cells or nodes are visited.
    @op: callback on each cell visited. The calling signature depends on
    ``Policy::intersect_judge``:
        - ROUGH or FINE: op(i);
        - STRICT: op(i, bound);
    where ``index_t i`` is the cell index and ``int bound`` indicates whether
    or not the cell is fully bounded in ``sphere``.

    See the API-ref of ``sphere_query_policy_t`` for the details on query 
    policy.

    Thread safety: these two calls do not modify the mesh.
    */
    template<typename Op, typename Policy = sphere_query_policy_t>
    void visit_sphere(const sphere_t &sphere, Op op, 
        Policy &&policy = Policy()) const;
    index_t count_sphere(const sphere_t &sphere) const;
protected:
    construct_policy_t _construct_policy;
    
    mesh_t _mesh;
    vector<node_t> _nodes;
    vector<index_t> _displs;

    /**
    Given a buffer of points each typed kdpoint_t, construct the meshed 
    structure.
    */
    struct _Impl_construct;

    /**
    Visit cells in row-major order within a rectangle or a sphere. 
    @Policy: see API-ref of ``rect_query_policy_t`` and 
    ``sphere_query_policy_t``, respectively.
    */
    template<typename Op, typename Policy> struct _Impl_visit_rect;
    template<typename Op, typename Policy> struct _Impl_visit_sphere;
};

template<typename KDPointT, typename IndexT>
ostream & operator<< (ostream &os, 
    const typename _KDMesh<KDPointT, IndexT>::construct_policy_t &pl);

template<typename KDPointT, typename IndexT>
struct _KDMesh<KDPointT, IndexT>::construct_policy_t {
    enum class in_cell_sort_t : int { NONE, ALONG_AXIS };
    
    static constexpr float_t 
        DFLT_MARGIN_REL     = 1.0e-3,
        DFLT_MARGIN_ABS     = 1.0e-3;
    static constexpr index_t 
        DFLT_N_CELL         = 10;
    static constexpr in_cell_sort_t 
        DFLT_IN_CELL_SORT   = in_cell_sort_t::NONE;
    static constexpr int 
        DFLT_DIM_SORTED     = DIM-1;

    construct_policy_t() noexcept;

    ostream & info(ostream &os = cout, int fmt_cntl = 0, int level = 0) const;
    friend ostream & operator<< (ostream &os, 
        const construct_policy_t &pl) { return pl.info(os); }

    std::pair<float_t, float_t> margin() const noexcept;
    construct_policy_t & set_margin(float_t margin_rel, 
        float_t margin_abs) noexcept;

    const std::optional<rect_t> & bound() const noexcept;
    construct_policy_t & set_bound(const rect_t &bound) noexcept;
    construct_policy_t & unset_bound() noexcept;

    const n_cell_t & n_cell() const noexcept;
    construct_policy_t & set_n_cell(index_t n_cell) noexcept;
    construct_policy_t & set_n_cell(const n_cell_t &n_cell) noexcept;

    in_cell_sort_t in_cell_sort() const noexcept;
    construct_policy_t & set_in_cell_sort(in_cell_sort_t in_cell_sort) noexcept;

    int dim_sorted() const noexcept;
    construct_policy_t & set_dim_sorted(int dim) noexcept;
private:
    friend class _KDMesh;

    float_t _margin_rel, _margin_abs;
    std::optional<rect_t> _bound;

    cells_t _cells;

    in_cell_sort_t _in_cell_sort;
    int _dim_sorted;
};
template<typename KDPointT, typename IndexT>
struct _KDMesh<KDPointT, IndexT>::idx_pair_t { 
    
    index_t idx_in, idx_cell;

    bool operator<(const idx_pair_t &o) const noexcept { 
        return idx_cell < o.idx_cell; 
    }
};

template<typename KDPointT, typename IndexT>
struct _KDMesh<KDPointT, IndexT>::rect_query_policy_t {

    enum class intersect_judge_t : int {ROUGH, STRICT};

    static constexpr intersect_judge_t
        intersect_judge = intersect_judge_t::STRICT;
};

template<typename KDPointT, typename IndexT>
struct _KDMesh<KDPointT, IndexT>::sphere_query_policy_t {

    enum class intersect_judge_t : int {ROUGH, FINE, STRICT};

    static constexpr intersect_judge_t
        intersect_judge = intersect_judge_t::STRICT;
};

} // namespace HIPP::NUMERICAL::_KDSEARCH

#endif	//_HIPPNUMERICAL_KDSEARCH_KDMESH_RAW_H_