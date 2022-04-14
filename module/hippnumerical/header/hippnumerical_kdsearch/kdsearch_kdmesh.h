/**
create: Yangyao CHEN, 2022/02/14
    [write   ] KDMesh - Regular mesh algorithm for neighbor-based search.
*/

#ifndef _HIPPNUMERICAL_KDSEARCH_KDMESH_H_
#define _HIPPNUMERICAL_KDSEARCH_KDMESH_H_

#include "kdsearch_kdmesh_raw_impl.h"

namespace HIPP::NUMERICAL {

/**
Regular mesh algorithm for neighbor-based search.
*/
template<typename KDPointT = KDPoint<>, typename IndexT = int> 
class KDMesh {
public:
    using impl_t = _KDSEARCH::_KDMesh<KDPointT, IndexT>;

    static constexpr int DIM = impl_t::DIM;
    static constexpr size_t PADDING = impl_t::PADDING;

    /**
    Point types. 
    ``point_t`` contains only the position of a point. 
    `kd_point_t`` is derived from ``point_t``, with padding to store 
    user-defined extra information. 
    ``node_t`` is aliased to ``kd_point_t``.
    */
    using point_t    = typename impl_t::point_t;
    using kd_point_t = typename impl_t::kd_point_t;
    using node_t     = typename impl_t::node_t;

    /**
    Numerical types.
    ``float_t`` and ``index_t`` are floating-point and signed integral scalar 
    numerical types, respectively.
    ``pos_t`` and ``n_cell_t`` are Svec<> types for position (floating-point) 
    and number of cells (signed integer), respectively.
    */
    using float_t    = typename impl_t::float_t;
    using index_t    = typename impl_t::index_t;
    using pos_t      = typename impl_t::pos_t;
    using n_cell_t   = typename impl_t::n_cell_t;
    
    /**
    Geometry types for N-dimensional index, mesh, rectangle, sphere and cells,
    respectively.
    */
    using v_index_t  = typename impl_t::v_index_t;
    using mesh_t     = typename impl_t::mesh_t;
    using rect_t     = typename impl_t::rect_t;
    using sphere_t   = typename impl_t::sphere_t;
    using cells_t    = typename impl_t::cells_t;

    using construct_policy_t    = typename impl_t::construct_policy_t;
    using rect_query_policy_t   = typename impl_t::rect_query_policy_t;
    using sphere_query_policy_t = typename impl_t::sphere_query_policy_t;
    using idx_pair_t            = typename impl_t::idx_pair_t;

    /**
    Constructors.

    (1): Default construnction - no actual mesh structure is constructed. 
    No query shall be made on the instance. Use ``construct()`` to construct 
    the mesh.

    (2): Construct the mesh by a series of points ``pts`` and the algorithm 
    ``policy``. For details, see :func:`construct`.
    */
    KDMesh();
    KDMesh(ContiguousBuffer<const kd_point_t> pts, 
        const construct_policy_t &policy = construct_policy_t());

    /**
    ``KDMesh`` is copable and movable. The moved-from object is left in a 
    valid state but no other method, except copy, move, and destructor, shall
    be called on it.

    The copied-to object shares the same internal state with the source object.
    Queries can be made on them independently and thread-safely. Methods that 
    modify the mesh may result in race condition.
    */
    KDMesh(const KDMesh &o) = default;
    KDMesh(KDMesh &&o) = default;
    KDMesh & operator=(const KDMesh &o) noexcept = default;
    KDMesh & operator=(KDMesh &&o) noexcept = default;
    ~KDMesh() noexcept = default;

    ostream & info(ostream &os = cout, int  fmt_cntl = 0, int level = 0) const;
    friend ostream & operator<<(ostream &os, const KDMesh &kdm) {
        return kdm.info(os);
    }

    /**
    Construct the mesh that contains the points ``pts``. ``pts.size() == 0`` is
    valid. The previous mesh structure is destroyed.
    
    @policy: control the mesh construction algorithm. See the API-ref of 
    :type:`construct_policy_t` for details.
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

    std::shared_ptr<impl_t> impl() const noexcept;

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
    visit_cells_rect(): visit all cells that intersect with ``rect``. 
    @rect: a rectangle instance within which the cells are visited.
    @op: callback on each cell visited. The calling signature depends on
    ``Policy::intersect_judge``:
        - ROUGH: op(i);
        - STRICT: op(i, bound);
    where ``index_t i`` is the cell index and ``int bound`` indicates whether
    or not the cell is fully bounded in ``rect``. See the API-ref of 
    :type:`rect_query_policy_t` for the details on query policy.

    visit_nodes_rect(): visit all nodes within ``rect``.
    ``op(const node_t &node)`` is called on each node visited.

    count_nodes_rect(): count the exact number of nodes within ``rect``.

    Thread safety: these three calls do not modify the mesh.
    */
    template<typename Op, typename Policy = rect_query_policy_t>
    void visit_cells_rect(const rect_t &rect, Op op, 
        Policy &&policy = Policy()) const;
    template<typename Op>
    void visit_nodes_rect(const rect_t &rect, Op op) const;
    index_t count_nodes_rect(const rect_t &rect) const;

    /**
    visit_cells_sphere(): visit all cells within ``sphere``.    

    @sphere: a sphere instance within which the cells are visited.
    @op: callback on each cell visited. The calling signature depends on
    ``Policy::intersect_judge``:
        - ROUGH or FINE: op(i);
        - STRICT: op(i, bound);
    where ``index_t i`` is the cell index and ``int bound`` indicates whether
    or not the cell is fully bounded in ``sphere``. See the API-ref of 
    :type:`sphere_query_policy_t` for the details on query policy.

    visit_nodes_sphere(): visit all nodes within ``sphere``. 
    ``op(const node_t &node)`` is called on each node visited.

    count_sphere(): count the exact number of nodes within ``sphere``.
    
    Thread safety: these three calls do not modify the mesh.
    */
    template<typename Op, typename Policy = sphere_query_policy_t>
    void visit_cells_sphere(const sphere_t &sphere, Op op, 
        Policy &&policy = Policy()) const;
    template<typename Op>
    void visit_nodes_sphere(const sphere_t &sphere, Op op) const;
    index_t count_nodes_sphere(const sphere_t &sphere) const;
protected:
    std::shared_ptr<impl_t> _impl;

};


#define _HIPP_TEMPHD template<typename KDPointT, typename IndexT>
#define _HIPP_TEMPARG <KDPointT, IndexT>
#define _HIPP_TEMPCLS KDMesh _HIPP_TEMPARG
#define _HIPP_TEMPRET _HIPP_TEMPHD inline auto _HIPP_TEMPCLS::
#define _HIPP_TEMPNORET _HIPP_TEMPHD inline _HIPP_TEMPCLS::

_HIPP_TEMPNORET
KDMesh() : _impl( std::make_shared<impl_t>() ) 
{}

_HIPP_TEMPNORET
KDMesh(ContiguousBuffer<const kd_point_t> pts, 
    const construct_policy_t &policy)
: KDMesh()
{
    construct(pts, policy);
}

_HIPP_TEMPRET
info(ostream &os, int  fmt_cntl, int level) const -> ostream & {
    PStream ps(os);
    if( fmt_cntl < 1 ) {
        ps << HIPPCNTL_CLASS_INFO_INLINE(KDMesh),
        "{impl=", *_impl, "}";
        return os;
    }
    auto ind = HIPPCNTL_CLASS_INFO_INDENT_STR(level);
    ps << HIPPCNTL_CLASS_INFO(KDMesh),
    ind, ps.info_of(*_impl, fmt_cntl, level+1);
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
get_nodes_in_cell(index_t cell_idx) const noexcept 
-> std::pair<const node_t *, index_t> 
{
    return _impl->get_nodes_in_cell(cell_idx);
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
mesh() const noexcept -> const mesh_t & {
    return _impl->mesh();
}

_HIPP_TEMPRET
mesh() noexcept -> mesh_t & {
    return _impl->mesh();
}

_HIPP_TEMPRET
nodes() const noexcept -> const vector<node_t> & {
    return _impl->nodes();
}

_HIPP_TEMPRET
nodes() noexcept -> vector<node_t> & {
    return _impl->nodes();
}

_HIPP_TEMPHD
template<typename Op, typename Policy>
void _HIPP_TEMPCLS::visit_cells_rect(const rect_t &rect, Op op, 
    Policy &&policy) const 
{
    _impl->template visit_rect<Op, Policy>(rect, op, 
        std::forward<Policy>(policy) );
}

_HIPP_TEMPHD
template<typename Op>
void _HIPP_TEMPCLS::visit_nodes_rect(const rect_t &rect, Op op) const {
    auto op_on_cell = [&op, &rect, this](index_t cell_idx, int bound) {
        auto [p, n] = get_nodes_in_cell(cell_idx);
        if( bound ) {
            for(index_t i=0; i<n; ++i) op(p[i]);
            return;
        }
        for(index_t i=0; i<n; ++i){
            auto &node = p[i];
            if( rect.contains(node) ) op(node);
        }
    };
    _impl->template visit_rect(rect, op_on_cell);
}

_HIPP_TEMPRET
count_nodes_rect(const rect_t &rect) const -> index_t {
    return _impl->count_rect(rect);
}

_HIPP_TEMPHD
template<typename Op, typename Policy>
void _HIPP_TEMPCLS::visit_cells_sphere(const sphere_t &sphere, Op op, 
    Policy &&policy) const 
{
    _impl->template visit_sphere<Op, Policy>(sphere, op, 
        std::forward<Policy>(policy));
}

_HIPP_TEMPHD
template<typename Op>
void _HIPP_TEMPCLS::visit_nodes_sphere(const sphere_t &sphere, Op op) const {
    auto op_on_cell = [&op, &sphere, this](index_t cell_idx, int bound) {
        auto [p, n] = get_nodes_in_cell(cell_idx);
        if( bound ) {
            for(index_t i=0; i<n; ++i) op(p[i]);
            return;
        }
        for(index_t i=0; i<n; ++i){
            auto &node = p[i];
            if( sphere.contains(node) ) op(node);
        }
    };
    _impl->template visit_sphere(sphere, op_on_cell);
}

_HIPP_TEMPRET
count_nodes_sphere(const sphere_t &sphere) const -> index_t {
    return _impl->count_sphere(sphere);

}


#undef _HIPP_TEMPHD
#undef _HIPP_TEMPARG
#undef _HIPP_TEMPCLS
#undef _HIPP_TEMPRET
#undef _HIPP_TEMPNORET


} // namespace HIPP::NUMERICAL

#endif	//_HIPPNUMERICAL_KDSEARCH_KDMESH_H_