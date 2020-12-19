#ifndef _HIPPALGORITHM_XTREE_RAW_IMPL_H_
#define _HIPPALGORITHM_XTREE_RAW_IMPL_H_
#include "xtree_raw.h"
namespace HIPP::ALGORITHM {

#if defined(HIPPDEV) && defined(HIPPDEBUG) && defined(HIPPVERBOSE)
#define HIPPPALL
#endif

namespace _xtree_raw_helper {

#define _HIPP_TEMPHD template<typename FLOAT_T, int DIM, int PACK, \
    int PADDING, int PADDING_V, int PADDING_N>
#define _HIPP_TEMPARG helper_t<FLOAT_T, DIM, PACK, \
    PADDING, PADDING_V, PADDING_N>::

_HIPP_TEMPHD
ostream & _HIPP_TEMPARG point_t::info(ostream &os, int fmt_cntl) const {
    HIPP::PStream ps(os);
    ps << "Points";
    for(int i=0; i<PACK; ++i){
        ps << " (";
        ps << coord(i, 0);
        for(int j=1; j<DIM; ++j)
            ps << ", ", coord(i, j);
        ps << ")";
    }
    return os;
}

_HIPP_TEMPHD
auto _HIPP_TEMPARG node_t::set_pos(const float *_pos)-> node_t & {
    std::copy(_pos, _pos+DIM, pos); 
    return *this;
}
_HIPP_TEMPHD
auto _HIPP_TEMPARG node_t::set_extent(const float *_extent)-> node_t & {
    std::copy(_extent, _extent+DIM, extent); 
    return *this;
}

_HIPP_TEMPHD
auto _HIPP_TEMPARG node_t::set_n_split(const int *_n_split)-> node_t & {
    std::copy(_n_split, _n_split+DIM, n_split);
    return *this;
}
_HIPP_TEMPHD
auto _HIPP_TEMPARG node_t::set_n_split(int _n_split)-> node_t & {
    std::fill_n(n_split, DIM, _n_split);
    return *this;
}

_HIPP_TEMPHD
ostream & _HIPP_TEMPARG node_t::info(ostream &os, int fmt_cntl) const {
    HIPP::PStream ps(os);
    ps << "Node -> ", first_child_id, 
        ", (", point_b, ", ", point_e, "), ",
        n_single_points, ", (";
    HIPP::prt_a(os, pos, pos+DIM) << "), (";
    HIPP::prt_a(os, extent, extent+DIM) << "), (";
    HIPP::prt_a(os, n_split, n_split+DIM) << ")";
    return os;
}

_HIPP_TEMPHD
_HIPP_TEMPARG param_t::param_t() {
    reset_default();
}

_HIPP_TEMPHD
void _HIPP_TEMPARG param_t::reset_default(){
    leaf_size = 64;
    for(int i=0; i<DIM; ++i){
        initial_n_split[i] = 8;
        refine_n_split[i] = 2;
        box_span[i][0] = -1.;
        box_span[i][1] = -2.;
        overbound_pos[i] = -3.;
    }
    shrink_cache_size = 1;
}

_HIPP_TEMPHD
auto _HIPP_TEMPARG param_t::get_default() -> param_t { 
    param_t p; p.reset_default(); return p; 
}

_HIPP_TEMPHD
ostream & _HIPP_TEMPARG param_t::info(ostream &os, int fmt_cntl) const {
    HIPP::PStream ps(os);
    if( fmt_cntl >= 1 ){
        ps << HIPPCNTL_CLASS_INFO(param_t), 
            "Leaf size =        ", leaf_size, 
            "\nInitial split =    (";
        HIPP::prt_a(os, initial_n_split, initial_n_split+DIM) 
            << ")\nRefine split=      (";
        HIPP::prt_a(os, refine_n_split, refine_n_split+DIM) 
            << ")\nBox span=          ";
        for(int i=0; i<DIM; ++i)
            ps << "(", box_span[i][0], box_span[i][1], ") ";
        ps << "\nOverbound position=(";
        HIPP::prt_a(os, overbound_pos, overbound_pos+DIM) << ")\n";
        ps << std::flush;
    }else{
        ps << HIPPCNTL_CLASS_INFO_INLINE(param_t),
            "Leaf size = ", leaf_size, " ...";
    }
    return os;
}

_HIPP_TEMPHD
_HIPP_TEMPARG coord_transf_t::coord_transf_t(const float_t *_origin, 
    const float_t *_extent, const int *_n_cells){
    std::copy(_origin, _origin+DIM, origin);
    std::copy(_extent, _extent+DIM, extent);
    std::copy(_n_cells, _n_cells+DIM, n_cells);
    for(int i=0; i < DIM; ++i)
        cell_size[i] = extent[i]/n_cells[i];
    tot_cells = std::accumulate(n_cells, n_cells+DIM, index_t(1), 
        std::multiplies<index_t>());
}

_HIPP_TEMPHD
int _HIPP_TEMPARG coord_transf_t::pos2id_dim(int idim, float_t pos) const {
    return std::floor((pos - origin[idim])/cell_size[idim]);
}

_HIPP_TEMPHD
void _HIPP_TEMPARG coord_transf_t::pos2id_dims(
    const float_t *pos, int *id_dims)  const
{
    for(int i=0; i<DIM; ++i){
        id_dims[i] = pos2id_dim(i, pos[i]);
    }
}

_HIPP_TEMPHD
auto _HIPP_TEMPARG coord_transf_t::pos2id(const float_t *pos) const -> index_t{
    index_t id = 0;
    for(int i=0; i<DIM; ++i){
        int id_dim = pos2id_dim(i, pos[i]);
        id_dim = std::min(n_cells[i]-1, id_dim);
        id_dim = std::max(0, id_dim);
        id = id * n_cells[i] + id_dim;
    }
    return id;
}

_HIPP_TEMPHD
void _HIPP_TEMPARG coord_transf_t::id_dims2pos(
        const int *id_dims, float_t *pos) const 
{
    for(int i=0; i<DIM; ++i){
        pos[i] = origin[i] + id_dims[i]*cell_size[i];
    }
}

_HIPP_TEMPHD
auto _HIPP_TEMPARG coord_transf_t::id_dims2id(const int *id_dims) const 
-> index_t
{
    index_t id = 0;
    for(int i=0; i<DIM; ++i){
        id = id * n_cells[i] + id_dims[i];
    }
    return id;
}

_HIPP_TEMPHD
void _HIPP_TEMPARG coord_transf_t::id2id_dims(index_t id, int *id_dims) const {
    for(int i=DIM-1; i>=0; --i){
        id_dims[i] = id % n_cells[i];
        id = id / n_cells[i];
    }
}

#undef _HIPP_TEMPHD 
#undef _HIPP_TEMPARG
} // namespace _xtree_raw_helper


#define _HIPP_TEMPHD template<typename FLOAT_T, int DIM, int PACK, \
    int PADDING, int PADDING_V, int PADDING_N>
#define _HIPP_TEMPARG _XTree<FLOAT_T, DIM, PACK, \
    PADDING, PADDING_V, PADDING_N>::

_HIPP_TEMPHD
void _HIPP_TEMPARG construct_tree(_p_single_point_t b, _p_single_point_t e){
    _points.clear();
    _nodes.clear();
    _chk_and_set_param(b, e);
    _construct_tree(b, e);

    if( _param.shrink_cache_size ){
        _points.shrink_to_fit();
        _nodes.shrink_to_fit();
    }
}

_HIPP_TEMPHD
void _HIPP_TEMPARG _chk_and_set_param(_p_single_point_t b, _p_single_point_t e){
    for(int i=0; i<DIM; ++i){
        if( _param.box_span[i][0] > _param.box_span[i][1] ){
            float_t max = std::numeric_limits<float_t>().lowest(),
                min = std::numeric_limits<float_t>().max();
            for(auto p=b; p!=e; ++p){
                max = std::max(max, p->pos[i]);
                min = std::min(min, p->pos[i]);
            }
            _param.box_span[i][0] = min;
            _param.box_span[i][1] = max;
        }
        float_t dx = _param.box_span[i][1] - _param.box_span[i][0];
        if( _param.overbound_pos[i] < _param.box_span[i][1] + dx*8. ){
            _param.overbound_pos[i] = _param.box_span[i][1] + dx*8.;
        }
    }
}

_HIPP_TEMPHD
ostream & _HIPP_TEMPARG info(ostream &os, int fmt_cntl) const {
    HIPP::PStream ps(os);
    if(fmt_cntl >= 1){
        ps << HIPPCNTL_CLASS_INFO(_XTree);
        _param.info(os);
        ps << "Points (count=",  _points.size(), 
            ", memory=", _points.size()*sizeof(point_t), " bytes), ",
            "Nodes (count=", _nodes.size(), 
            ", memory=", _nodes.size()*sizeof(node_t), " bytes)\n";
        ps << "  Node(id): first_child, point_range[2], n_single_points, "
             "pos[DIM], extent[DIM], n_split[DIM]\n",
              "  Point: pos[DIM]\n";
        if( _nodes.empty() ){
            ps << "  No nodes\n";
        }
        for(auto &node: _nodes){
            ps << "  ", index_t(&node-_nodes.data()), " - "; 
            node.info(os) << '\n';
            if( node.is_leaf() ){
                index_t pb = node.point_b, pe = node.point_e;
                for(index_t i=pb; i<pe; ++i){
                    ps << "    | - at ", i, ", "; 
                    _points.at(i).info(os) << '\n';
                }
                if( pb == pe )
                    ps << "    | - No points\n";
            }
        } 
        ps << std::flush;
    }
    return os;
}

_HIPP_TEMPHD
auto _HIPP_TEMPARG count_rect( const float_t *pos, 
    const float_t *dx ) const -> size_t 
{
    size_t cnt = 0;
    ivec_t cnt_v(0);

    fvec_t init_f, x_lo[DIM], x_hi[DIM];
    init_f.from_si( ivec_t(1) );
    for(int i=0; i<DIM; ++i){
        x_lo[i].set1(pos[i]-dx[i]);
        x_hi[i].set1(pos[i]+dx[i]);
    }

    auto op = [](const point_t &p)->void{};
    auto op_node = [&cnt](const node_t &n)->void { 
        cnt += n.n_single_points; };
    auto try_op = [&](const point_t &p)->void{
        fvec_t mask = init_f;
        for(int i=0; i<DIM; ++i)
            mask &= (p.pos[i] < x_hi[i]) & (p.pos[i] >= x_lo[i]);
        cnt_v += mask.to_si(); 
    };
    auto try_op_node = [](const node_t &n)->void {};

    std::stack<_walk_stackitem_t> stk;
    _nearest_rect(pos, dx, stk, op, try_op, op_node, try_op_node);

    union u_t { ivec_t iv; iscal_t iscal[PACK]; ~u_t(){} };
    u_t u = { cnt_v };
    for(int i=0; i<PACK; ++i) cnt += u.iscal[i];
    return cnt;
}

_HIPP_TEMPHD
void _HIPP_TEMPARG _construct_tree(_p_single_point_t b, _p_single_point_t e){
    index_t n_pts = e - b;
    vector<index_t> pids(n_pts);
    for(index_t i=0; i<n_pts; ++i) pids[i] = i;

    float_t orig[DIM], ext[DIM];
    for(int i=0; i<DIM; ++i){
        orig[i] = _param.box_span[i][0];
        ext[i] = _param.box_span[i][1] - _param.box_span[i][0];
    }
    _nodes.emplace_back(orig, ext);
    auto &root = _nodes[0];
    root.point_b = 0; root.point_e = n_pts; root.n_single_points = n_pts;

    index_t node_id = 0;
    while( node_id < index_t(_nodes.size()) ){
        const auto &node = _nodes[node_id];
        index_t pid_b = node.point_b, pid_e = node.point_e;
        if( pid_e - pid_b > _param.leaf_size ){
            _push_nodes(b, pids, pid_b, pid_e, node_id);
        }else{
            _sort_points_in_nodes(b, pids, pid_b, pid_e);
        }
        ++node_id;
    }
    _push_points(pids, b);
}


_HIPP_TEMPHD
void _HIPP_TEMPARG _push_nodes(
    _p_single_point_t b, vector<index_t> &pids, 
    index_t pid_b, index_t pid_e, index_t node_id)
{
    const int *n_cells = node_id?_param.refine_n_split:_param.initial_n_split;
    auto &root = _nodes[node_id];
    coord_transf_t ctrans(root.pos, root.extent, n_cells);
    root.set_n_split(n_cells);
    root.first_child_id = _nodes.size();

    index_t n_pts = pid_e - pid_b;
    vector<std::pair<index_t, index_t>> cell_ids(n_pts);
    for(index_t i=0; i<n_pts; ++i){
        index_t pid = pids[pid_b+i];
        const single_point_t &p = b[pid];
        cell_ids[i] = {pid, ctrans.pos2id(p.pos)};
    }
    std::sort(cell_ids.begin(), cell_ids.end(), 
        [](const auto &l, const auto &r)->bool{return l.second < r.second;});
    for(index_t i=0; i<n_pts; ++i)
        pids[pid_b+i] = cell_ids[i].first;

    _nodes.reserve(_nodes.size()+ctrans.tot_cells);
    index_t off = 0;
    int id_dims[DIM];
    float_t pos[DIM];
    for(index_t i=0; i<ctrans.tot_cells; ++i){
        index_t next_off = off;
        while( next_off < n_pts && cell_ids[next_off].second <= i ) 
            ++next_off;
        ctrans.id2id_dims(i, id_dims);
        ctrans.id_dims2pos(id_dims, pos);
        _nodes.emplace_back(pos, ctrans.cell_size);
        auto &node = _nodes.back();
        node.point_b = pid_b + off;
        node.point_e = pid_b + next_off;
        node.n_single_points = next_off - off;
        off = next_off;
    }
}

_HIPP_TEMPHD
void _HIPP_TEMPARG _sort_points_in_nodes(_p_single_point_t b, 
    vector<index_t> &pids, index_t pid_b, index_t pid_e)
{
    auto sort_f = [b, &pids](index_t i, index_t j)->bool{
        return b[i].pos[DIM-1] < b[j].pos[DIM-1];
    };
    std::sort(pids.data()+pid_b, pids.data()+pid_e, sort_f);
}


_HIPP_TEMPHD
void _HIPP_TEMPARG _push_points(vector<index_t> &pids, _p_single_point_t b) {
    _points.reserve(pids.size());
    for(auto &node: _nodes){
        if( !node.is_leaf() ) continue;          /* only push points on leafs */
        index_t cur_poff = _points.size();
        index_t pid_b = node.point_b, pid_e = node.point_e;
        index_t chunk_off = PACK-1;
        for(index_t i=pid_b; i<pid_e; ++i){
            chunk_off = (i - pid_b) % PACK;
            if( chunk_off == 0 )
                _points.emplace_back();
            const auto &p1 = b[pids[i]];
            auto &p2 = _points.back();
            p2.set_coord(chunk_off, p1.pos);
        }
        for(index_t i=chunk_off+1; i<PACK; ++i){
            auto &p2 = _points.back();
            p2.set_coord(i, _param.overbound_pos);
        }
        node.point_b = cur_poff;
        node.point_e = _points.size();
    }

    /* reset points range for non-leaf */
    for(auto p = _nodes.rbegin(); p != _nodes.rend(); ++p){
        if( p->is_leaf() ) continue;
        index_t fc = p->first_child_id, 
            nc = p->n_children();
        p->point_b = _nodes[fc].point_b;
        p->point_e = _nodes[fc+nc-1].point_e;
    }
}

_HIPP_TEMPHD
template<typename OpWithoutRsq,  typename TryOpWithoutRsq, 
    typename OpNode, typename TryOpNode>
void _HIPP_TEMPARG _nearest_rect(const float_t *pos, const float_t *dx, 
    std::stack<_walk_stackitem_t> & stk,  
    OpWithoutRsq op, TryOpWithoutRsq try_op, 
    OpNode op_node, TryOpNode try_op_node) const
{
    stk.emplace(0, 1);
    while(!stk.empty()){
        _walk_stackitem_t node_item = stk.top(); stk.pop();
        const auto &node = _nodes[node_item.node_id];
        if( !node_item.walk_flag ){
            op_node(node);
            _visit_allpoints_without_rsq(node, op);
        }else if(!node.n_split[0]){
            try_op_node(node);
            _tryvisit_without_rsq(node, try_op, pos, dx);
        }else{
            _rect_fill_stack(pos, dx, node, stk);
        }
    }
}

_HIPP_TEMPHD
template<typename OpWithoutRsq>
void _HIPP_TEMPARG _visit_allpoints_without_rsq(const node_t &node, 
    OpWithoutRsq op) const
{
    index_t b = node.point_b, e = node.point_e;
    for(index_t i=b; i<e; ++i){
        op(_points[i]);
    }
}

_HIPP_TEMPHD
template<typename TryOpWithoutRsq>
void _HIPP_TEMPARG _tryvisit_without_rsq(const node_t &node, 
    TryOpWithoutRsq try_op, const float_t *pos, const float_t *dx) const
{
    index_t b = node.point_b, e = node.point_e;
    if( e - b > 1 ){
        auto [new_b, new_e] = 
            _locate_point_range(b, e, pos[DIM-1], dx[DIM-1]);
        for(int i=new_b; i<new_e; ++i){
            try_op(_points[i]);
        }
    }else if( e - b == 1 ){
        try_op(_points[b]);
    }
}

_HIPP_TEMPHD
auto _HIPP_TEMPARG _locate_point_range(index_t b, index_t e, float_t last_coord, 
    float_t dx) const -> std::pair<index_t, index_t>
{
    float_t top = last_coord + dx;
    if( top <= _points[b].coord(0, DIM-1) )
        return {b,b};
    auto search_top = [top, this](index_t i)->bool
        { return _points[i].coord(0, DIM-1) < top; };
    index_t last = _bi_search(b, e, search_top);
    if( last == b )
        return {b, b+1};
    auto bot = last_coord - dx;
    
    auto search_bot = [bot, this](index_t i)->bool
        { return _points[i].coord(PACK-1, DIM-1) <= bot; };
    index_t first = _bi_search(b, last, search_bot);
    return {first, last+1};
}

_HIPP_TEMPHD
template<typename Comp>
auto _HIPP_TEMPARG _bi_search(index_t b, index_t e, Comp comp) -> index_t {
    while(e != b+1){
        index_t c = (b+e)/2;
        if( comp(c) )
            b = c;
        else 
            e = c;
    }
    return b;
}

_HIPP_TEMPHD
void _HIPP_TEMPARG _rect_fill_stack(const float_t *pos, const float_t *dx, 
    const node_t &node, std::stack<_walk_stackitem_t> &stk) const
{
    _rect_fill_stack_eachdim<0>(pos, dx, node, stk, 0, 0);
}

_HIPP_TEMPHD
template<int IDIM>
void _HIPP_TEMPARG _rect_fill_stack_eachdim(
    const float_t *pos, const float_t *dx, const node_t &node, 
    std::stack<_walk_stackitem_t> &stk, index_t id_prev, int walk_flag) const
{
    int n_split = node.n_split[IDIM];
    float_t cell_size = node.extent[IDIM]/n_split;
    index_t low = std::floor((pos[IDIM]-dx[IDIM]-node.pos[IDIM])/cell_size);
    if( low < 0 ) low = 0;
    index_t hi = std::floor((pos[IDIM]+dx[IDIM]-node.pos[IDIM])/cell_size);
    if( hi >= n_split ) hi = n_split - 1;
    
    id_prev *= n_split;
    if constexpr( IDIM == DIM-1 ){
        index_t fc = node.first_child_id;
        if( hi - low >= 1 ){
            for(auto i: {low, hi})
                stk.emplace(fc + id_prev + i, 1);
            for(index_t i=low+1; i<hi; ++i)
                stk.emplace(fc + id_prev + i, walk_flag);
        }else if( hi == low ){
            stk.emplace(fc + id_prev + low, 1);
        }
    }else{
        if( hi - low >= 1 ){
            for(auto i: {low, hi})
                _rect_fill_stack_eachdim<IDIM+1>(pos, dx, node, stk, 
                    id_prev + i, 1);
            for(index_t i=low+1; i<hi; ++i)
                _rect_fill_stack_eachdim<IDIM+1>(pos, dx, node, stk, 
                    id_prev + i, walk_flag);
        }else if( hi == low ){
            _rect_fill_stack_eachdim<IDIM+1>(pos, dx, node, stk,
                id_prev + low, 1);
        }
    }
}

#undef _HIPP_TEMPARG
#undef _HIPP_TEMPHD




} // namespace HIPP::ALGORITHM

#endif	//_HIPPALGORITHM_XTREE_RAW_IMPL_H_