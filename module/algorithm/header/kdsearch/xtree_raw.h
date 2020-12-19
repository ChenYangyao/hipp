#ifndef _HIPPALGORITHM_XTREE_RAW_H_
#define _HIPPALGORITHM_XTREE_RAW_H_

#include <hippcntl.h>
#include <hippsimd.h>

namespace HIPP::ALGORITHM {


namespace _xtree_raw_helper {

template<
    typename FLOAT_T, int DIM, int PACK, 
    int PADDING, int PADDING_V, int PADDING_N>
struct helper_t {
    typedef std::size_t size_t;
    typedef ptrdiff_t index_t;
    typedef FLOAT_T float_t;
    typedef SIMD::Vec<float_t, PACK> fvec_t; 
    typedef typename fvec_t::IntVec ivec_t;
    typedef typename fvec_t::iscal_t iscal_t;

    struct single_point_t {
        float_t pos[DIM];
        char    pad[PACK][PADDING];
        single_point_t(const float_t *_pos){ std::copy(_pos, _pos+DIM, pos); }
    };
    struct point_t {
        fvec_t  pos[DIM];
        char    pad[PACK][PADDING];
        char    pad_v[PADDING_V];

        const float_t & coord(int ipoint, int idim)const { 
            return *((float_t *)(pos+idim) + ipoint);
        }
        float_t & coord(int ipoint, int idim){ 
            return *((float_t *)(pos+idim) + ipoint);
        }
        void set_coord(int ipoint, const float_t *pos){
            for(int i=0; i<DIM; ++i)
                coord(ipoint, i) = pos[i];
        }
        ostream & info(ostream &os=cout, int fmt_cntl=1) const;
    };
    struct node_t {
        index_t first_child_id;
        index_t point_b, point_e, n_single_points;
        float_t pos[DIM], 
                extent[DIM];
        int     n_split[DIM];
        char    pad[PADDING_N];

        node_t(){}
        node_t(const float *_pos, const float *_extent, const int *_n_split){
            set_pos(_pos); set_extent(_extent); set_n_split(_n_split);
            first_child_id = -1;
        }
        node_t(const float *_pos, const float *_extent){
            set_pos(_pos); set_extent(_extent); set_n_split(0);
            first_child_id = -1;
        }
        node_t & set_pos(const float *_pos);
        node_t & set_extent(const float *_extent);
        node_t & set_n_split(const int *_n_split);
        node_t & set_n_split(int _n_split);

        bool is_leaf() const { return n_split[0] == 0; }
        index_t n_children() const { 
            return std::accumulate(n_split, n_split+DIM, 
                index_t(1), std::multiplies<index_t>()); 
        }
        ostream & info(ostream &os=cout, int fmt_cntl=1) const;
    };
    struct param_t {
        int     leaf_size;
        int     initial_n_split[DIM];
        int     refine_n_split[DIM];
        float_t box_span[DIM][2];
        float_t overbound_pos[DIM];

        int shrink_cache_size;

        param_t();
        void reset_default();
        static param_t get_default();

        ostream & info(ostream &os=cout, int fmt_cntl=1) const;
    };

    struct coord_transf_t {
        float_t origin[DIM], extent[DIM], cell_size[DIM];
        int n_cells[DIM];
        index_t tot_cells;

        coord_transf_t(const float_t *_origin, 
            const float_t *_extent, const int *_n_cells);
        int pos2id_dim(int idim, float_t pos) const;
        void pos2id_dims(const float_t *pos, int *id_dims) const;
        index_t pos2id(const float_t *pos)  const;
        void id_dims2pos(const int *id_dims, float_t *pos) const;
        index_t id_dims2id(const int *id_dims) const;
        void id2id_dims(index_t id, int *id_dims) const;
    };

    inline static param_t default_param;
};
} // namespace _xtree_raw_helper

template<typename FLOAT_T, int DIM, int PACK, 
    int PADDING = 0, int PADDING_V = 0, int PADDING_N = 0>
class _XTree: public _xtree_raw_helper::helper_t<
    FLOAT_T, DIM, PACK, 
    PADDING, PADDING_V, PADDING_N> 
{
public:
    typedef _xtree_raw_helper::helper_t<
        FLOAT_T, DIM, PACK, 
        PADDING, PADDING_V, PADDING_N> super_t;
    using typename super_t::size_t;
    using typename super_t::index_t;
    using typename super_t::float_t;
    using typename super_t::fvec_t;
    using typename super_t::ivec_t;
    using typename super_t::iscal_t;

    using typename super_t::point_t;
    using typename super_t::single_point_t;
    using typename super_t::node_t;
    using typename super_t::param_t;
    using typename super_t::coord_transf_t;
    typedef const point_t *_p_point_t;
    typedef const single_point_t *_p_single_point_t;
    using super_t::default_param;
    

    _XTree(const param_t &param = default_param):_param(param){}
    ~_XTree() noexcept {}

    void construct_tree(_p_single_point_t b, _p_single_point_t e);        
    // points must within box, otherwise they are piped up in the boundary cells

    ostream & info(ostream &os = cout, int fmt_cntl = 1) const;

    size_t count_rect( const float_t *pos, const float_t *dx ) const;
    
protected:
    vector<point_t> _points;
    vector<node_t> _nodes;
    param_t _param;

    struct _walk_stackitem_t {
        index_t node_id;
        int     walk_flag;              // 0 for full cover
        _walk_stackitem_t(index_t _node_id, int _walk_flag)
            : node_id(_node_id), walk_flag(_walk_flag){}
    };

    void _chk_and_set_param(_p_single_point_t b, _p_single_point_t e);
    void _construct_tree(_p_single_point_t b, _p_single_point_t e); // _points, _nodes must empty, _param set done.
    void _push_nodes(_p_single_point_t b, 
        vector<index_t> &pids,
        index_t pid_b, index_t pid_e, index_t node_id);
    void _sort_points_in_nodes(_p_single_point_t b, 
        vector<index_t> &pids, index_t pid_b, index_t pid_e);
    void _push_points(vector<index_t> &pids, _p_single_point_t b);


    template<typename OpWithRsq, typename OpWithoutRsq>
    void _nearest_r(const float_t *pos, float_t r, 
        std::stack<_walk_stackitem_t> & stk,
        OpWithRsq op_with_rsq, OpWithoutRsq op_without_rsq) const;

    // It is best that the rect is intersect with our box.
    template<typename OpWithoutRsq, typename TryOpWithoutRsq, 
        typename OpNode, typename TryOpNode>  // stk.empty() == True; tree could be empty, but root node must exists
    void _nearest_rect(const float_t *pos, const float_t *dx, 
        std::stack<_walk_stackitem_t> & stk,  
        OpWithoutRsq op, TryOpWithoutRsq try_op, 
        OpNode op_node, TryOpNode try_op_node) const;


    template<typename OpWithoutRsq>
    void _visit_allpoints_without_rsq(const node_t &node, 
        OpWithoutRsq op) const;

    template<typename TryOpWithoutRsq>
    void _tryvisit_without_rsq(const node_t &node, 
        TryOpWithoutRsq try_op, const float_t *pos, const float_t *dx) const;
    
    std::pair<index_t, index_t> _locate_point_range(index_t b, 
        index_t e, float_t last_coord, float_t dx) const;

    template<typename Comp>
    static index_t _bi_search(index_t b, index_t e, Comp comp);    // e - b >= 1 at entry

    void _rect_fill_stack(const float_t *pos, const float_t *dx, 
        const node_t &node, std::stack<_walk_stackitem_t> &stk) const;
    template<int IDIM>
    void _rect_fill_stack_eachdim(
        const float_t *pos, const float_t *dx, const node_t &node, 
        std::stack<_walk_stackitem_t> &stk, index_t id_prev, 
        int walk_flag) const;
};


} // namespace HIPP::ALGORITHM

#endif	//_HIPPALGORITHM_XTREE_RAW_H_