#ifndef _HIPPALGORITHM_KD_TREE_RAW_H_
#define _HIPPALGORITHM_KD_TREE_RAW_H_
#include <hippcntl.h>
#include <numeric>
#include <cstring>
namespace HIPP{
namespace ALGORITHM{

namespace _kd_tree_helper{
template<int DIM, typename FLOAT_T, std::size_t PADDING>
struct point_t{
    FLOAT_T pos[DIM];
    char pad[PADDING];

    point_t() noexcept{ }
    explicit point_t(const FLOAT_T *_pos)noexcept;
    explicit point_t(const vector<FLOAT_T> &_pos)noexcept;
    template<typename T>
    point_t(const FLOAT_T *_pos, const T &_pad )noexcept;
    template<typename T>
    point_t(const vector<FLOAT_T> &_pos, const T &_pad )noexcept;
};

template<int DIM, typename FLOAT_T, typename INDEX_T, std::size_t PADDING>
struct node_t;
template<int DIM, typename FLOAT_T, typename INDEX_T, std::size_t PADDING>
ostream & operator<<( 
    ostream &os, const node_t<DIM,FLOAT_T,INDEX_T,PADDING> &node );

template<int DIM, typename FLOAT_T, typename INDEX_T, std::size_t PADDING>
struct node_t{
    typedef HIPP::ALGORITHM::_kd_tree_helper::point_t<DIM, FLOAT_T, PADDING> 
        point_t;

    INDEX_T size;
    int axis;
    FLOAT_T pos[DIM];
    char pad[PADDING];

    node_t() noexcept{ }
    node_t( INDEX_T _size, int _axis, const FLOAT_T *_pos ) noexcept;
    node_t( INDEX_T _size, int _axis, const vector<FLOAT_T> &_pos ) noexcept;
    node_t( INDEX_T _size, int _axis, const point_t &p) noexcept;

    static FLOAT_T r2( const FLOAT_T p1[DIM], const FLOAT_T p2[DIM] ) noexcept;
    static FLOAT_T r( const FLOAT_T p1[DIM], const FLOAT_T p2[DIM] ) noexcept;

    ostream & info(ostream &os = cout, int fmt_cntl = 0) const;
    friend ostream & operator<< <DIM,FLOAT_T,INDEX_T,PADDING>
        ( ostream &os, const node_t &node );
};
} // namespace _kd_tree_helper
template<int DIM, typename INDEX_T, typename FLOAT_T, std::size_t PADDING>
class _KDTree;

template<int DIM, typename INDEX_T, typename FLOAT_T, std::size_t PADDING>
ostream & operator<<( 
    ostream &os, const _KDTree<DIM,INDEX_T,FLOAT_T,PADDING> &tr );

template<int DIM, typename INDEX_T, typename FLOAT_T, std::size_t PADDING>
class _KDTree{
public:
    typedef INDEX_T index_t;
    typedef FLOAT_T float_t;
    typedef std::size_t size_t;
    typedef _kd_tree_helper::node_t<DIM, FLOAT_T, INDEX_T, PADDING> node_t;
    typedef typename node_t::point_t point_t;
    typedef const point_t * _p_point_t;
    typedef const node_t * _p_node_t;
    struct _p_2node_t { _p_node_t parent, sibl;
        _p_2node_t( _p_node_t _parent, _p_node_t _sibl )
            :parent(_parent), sibl(_sibl){ }
    };
    struct ngb_t{
        _p_node_t node; float_t r;
        ngb_t( _p_node_t _node, float_t _r ): node(_node), r(_r){}
        bool operator<( const ngb_t &n )const
        { return r < n.r; }
    };

    _KDTree() noexcept;
    _KDTree( const _p_point_t b, const _p_point_t e );
    _KDTree( const vector<point_t> &points );
    _KDTree( const _KDTree &tr );
    _KDTree( _KDTree &&tr ) noexcept;
    _KDTree & operator=( const _KDTree &tr );
    _KDTree & operator=( _KDTree &&tr ) noexcept;
    ~_KDTree() noexcept;

    void construct_tree( const _p_point_t b, const _p_point_t e );
    void construct_tree( const vector<point_t> &points );

    ostream & info( ostream &os = cout, int fmt_cntl = 1 ) const;
    friend ostream & operator<< <DIM,INDEX_T,FLOAT_T,PADDING>
        ( ostream &os, const _KDTree &tr );

    ngb_t nearest( const float_t *pos ) const;
    vector<ngb_t> nearest_k( const float_t *pos, index_t k ) const;
    void nearest_k( const float_t *pos, index_t k, vector<ngb_t> &ngbs) const;
    
    vector<ngb_t> nearest_r( const float_t *pos, float_t r ) const;
    void nearest_r( const float_t *pos, float_t r, vector<ngb_t> &ngbs) const;
    size_t count_r( const float_t *pos, float_t r) const;

    vector<_p_node_t> nearest_rect( 
        const float_t *pos, const float_t *dx ) const;
    void nearest_rect( 
        const float_t *pos, const float_t *dx, vector<_p_node_t> &ngbs) const;
    size_t count_rect( const float_t *pos, const float_t *dx ) const;

    bool empty() const noexcept;
    index_t size() const noexcept;
    void clear() noexcept;
protected:
    vector<node_t> _nodes;

    void _construct_tree( const _p_point_t b, const _p_point_t e );
    static int _choose_direc( 
        const vector<_p_point_t > &ps, index_t b, index_t e );
    static index_t _partite( 
        vector<_p_point_t > &ps, index_t b, index_t e, const int direc,
        std::default_random_engine &re );

    void _nearest( const float_t pos[DIM], 
        std::stack<_p_2node_t> &stk, _p_node_t &pnode, FLOAT_T &rsq ) const;
    void _nearest_k( const float_t pos[DIM], const index_t k,
        std::stack<_p_2node_t> &stk, std::priority_queue<ngb_t> &pq ) const;
    
    /* op: a callable that accepts (_pnode_t &pnode, float_t r_squred) */
    template<typename Op>
    void _nearest_r( const float_t pos[DIM], const float_t rsq,
        std::stack<_p_2node_t> &stk, Op op ) const;

    /* op: a callable that accepts (_pnode_t &pnode) */
    template<typename Op>
    void _nearest_rect( const float_t pos[DIM], const float_t dx[DIM],
        std::stack<_p_2node_t> &stk, Op op) const;

    void _walk_down( 
        _p_node_t &root, 
        const float_t pos[DIM], std::stack<_p_2node_t> &stk, 
        _p_node_t &parent, _p_node_t &sibl) const;
};

} // namespace ALGORITHM
} // namespace HIPP
#endif	//_HIPPALGORITHM_KD_TREE_RAW_H_