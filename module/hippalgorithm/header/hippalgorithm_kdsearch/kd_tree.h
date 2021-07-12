/**
 * creat: Yangyao CHEN, 2020/02/24
 *      [write   ] KDTree<> - k-dimensional bi-partited search tree
 */ 

#ifndef _HIPPALGORITHM_KD_TREE_H_
#define _HIPPALGORITHM_KD_TREE_H_
#include "kd_tree_raw.h"
#include "kd_tree_raw_impl.h"
#include "kd_obj_base.h"
namespace HIPP{
namespace ALGORITHM{

#define _HIPP_TEMPHD template<int DIM, typename INDEX_T, \
    typename FLOAT_T, std::size_t PADDING>
#define _HIPP_TEMPARG <DIM,INDEX_T,FLOAT_T,PADDING>
#define _HIPP_RAWT(type) typedef typename _obj_raw_t::type type;

_HIPP_TEMPHD class KDTree;
_HIPP_TEMPHD ostream & operator<<( ostream &os, const KDTree _HIPP_TEMPARG &tr);

/**
 * k-dimensional bi-partited search tree
 * 
 * Various neighbor searching types are supported, including nearest neighbor,
 * k nearest neighbors, radius searching, and rectangular searching.
 * 
 * With the following statements, the tree can be thought as a highest 
 * performance KD tree:
 *  - The class implements KDTree with a continuous, depth first storage for 
 *    tree nodes so that the memory locality is maximized when performing 
 *    neighbor searching.
 *  - Only EUCLID metric is supported.
 *  - Tree does not support dynamic change, i.e, insert, remove and update of 
 *    nodes.
 *  - Tree is totally balanced, satisfying 
 *      size(right subtree)+1 >= size(left subtree) >= size(right subtree)
 * 
 * Still, user may use the PADDING template argument to make the node flexible,
 * although a larger padding value may reduce the cache hit rate.
 * 
 * Template arguments:
 * @DIM:        dimension of space
 * @INDEX_T:    a signed integer type which should be large enough to hold the
 *              number of points passed to contruct the tree.
 * @FLOAT_T:    a floating-point type used to represent the coordinates and 
 *              distance.
 * @PADDING:    number of bytes, statically associated with each data point.
 */ 
_HIPP_TEMPHD
class KDTree : public KDObj< _KDTree _HIPP_TEMPARG >{
public:
    typedef KDObj<_KDTree _HIPP_TEMPARG > _obj_base_t;
    using _obj_base_t::_obj_base_t;
    using typename _obj_base_t::_obj_raw_t;
    using typename _obj_base_t::_obj_ptr_t;

    _HIPP_RAWT(index_t)
    _HIPP_RAWT(float_t)
    _HIPP_RAWT(size_t)
    _HIPP_RAWT(point_t)
    _HIPP_RAWT(node_t)
    _HIPP_RAWT(_p_point_t)
    _HIPP_RAWT(_p_node_t)
    _HIPP_RAWT(_p_2node_t)
    typedef _p_point_t p_point_t;
    typedef _p_node_t p_node_t;
    typedef _p_2node_t p_2node_t;
    _HIPP_RAWT(ngb_t)

    /**
     * initializers:
     * (1)  an empty tree, with zero nodes.
     * (2)  with a list of points specified by [b, e) which refers to a 
     *      contiguous memory segement.
     * (3)  with a list of points specified by the std::vector. 
     * 
     * After the tree is initialized, you may change its content by 
     * reconstructing it with construct_tree() whose arguments are the same 
     * as the initializers, or erasing all nodes with clear().
     */
    KDTree() noexcept;
    KDTree( const _p_point_t b, const _p_point_t e );
    KDTree( const vector<point_t> &points );
    void construct_tree( const _p_point_t b, const _p_point_t e );
    void construct_tree( const vector<point_t> &points );

    ostream & info( ostream &os = cout, int fmt_cntl = 1 )const;
    friend ostream & operator<< _HIPP_TEMPARG( 
        ostream &os, const KDTree &tr);

    ngb_t nearest( const float_t pos[DIM] ) const;
    ngb_t nearest( const vector<float_t> &pos ) const;

    vector<ngb_t> nearest_k( const float_t pos[DIM], index_t k ) const;
    vector<ngb_t> nearest_k( const vector<float_t> &pos, index_t k ) const;
    void nearest_k( const float_t pos[DIM], index_t k, 
        vector<ngb_t> &ngbs) const;
    
    vector<ngb_t> nearest_r( const float_t pos[DIM], float_t r ) const;
    vector<ngb_t> nearest_r( const vector<float_t> &pos, float_t r ) const;
    void nearest_r(const float_t pos[DIM], float_t r, 
        vector<ngb_t> &ngbs) const;
    size_t count_r( const float_t pos[DIM], float_t r ) const;
    
    /**
    Query all points in the neighbor rectangular region.
    @pos: center of the rectangle.
    @dx: criteria, i.e., ngb with position |pos_ngb[i] - pos[i]| < dx[i] is 
        selected.
    Return a vector of pointers, each pointing to a tree node selected.
    */
    vector<_p_node_t> nearest_rect( 
        const float_t pos[DIM], const float_t dx[DIM] ) const;
    vector<_p_node_t> nearest_rect( 
        const vector<float_t> &pos, const vector<float_t> &dx ) const;
    void nearest_rect( const float_t pos[DIM], const float_t dx[DIM], 
        vector<_p_node_t> &ngbs ) const;
    size_t count_rect( const float_t pos[DIM], const float_t dx[DIM] ) const;

    bool empty() const noexcept;
    index_t size() const noexcept;
    void clear() noexcept;
protected:
    using _obj_base_t::_obj_ptr;
};

_HIPP_TEMPHD
KDTree _HIPP_TEMPARG::KDTree() noexcept
:_obj_base_t( std::make_shared<_obj_raw_t>() ){ }

_HIPP_TEMPHD
KDTree _HIPP_TEMPARG::KDTree( 
    const _p_point_t b, const _p_point_t e )
:_obj_base_t( std::make_shared<_obj_raw_t>(b, e) ){ }

_HIPP_TEMPHD
KDTree _HIPP_TEMPARG::KDTree( const vector<point_t> &points )
:_obj_base_t( std::make_shared<_obj_raw_t>(points) ){}

_HIPP_TEMPHD
void KDTree _HIPP_TEMPARG::construct_tree( 
    const _p_point_t b, const _p_point_t e ){
    _obj_ptr->construct_tree(b,e);
}
_HIPP_TEMPHD
void KDTree _HIPP_TEMPARG::construct_tree( 
    const vector<point_t> &points ){
    _obj_ptr->construct_tree(points);
}

_HIPP_TEMPHD
ostream & KDTree _HIPP_TEMPARG::info( ostream &os, int fmt_cntl )const{
    return _obj_ptr->info(os, fmt_cntl);
}

_HIPP_TEMPHD
ostream & operator<<( 
    ostream &os, const KDTree _HIPP_TEMPARG &tr){
    return os << *tr._obj_ptr;
}

_HIPP_TEMPHD
typename KDTree _HIPP_TEMPARG::ngb_t 
KDTree _HIPP_TEMPARG::nearest( const float_t pos[DIM] ) const{
    return _obj_ptr->nearest(pos);
}

_HIPP_TEMPHD
typename KDTree _HIPP_TEMPARG::ngb_t 
KDTree _HIPP_TEMPARG::nearest( const vector<float_t> &pos ) const{
    return nearest(pos.data());
}

_HIPP_TEMPHD
vector<typename KDTree _HIPP_TEMPARG::ngb_t> 
KDTree _HIPP_TEMPARG::nearest_k( const float_t pos[DIM], index_t k ) const{
    return _obj_ptr->nearest_k(pos, k);
}

_HIPP_TEMPHD
vector<typename KDTree _HIPP_TEMPARG::ngb_t> 
KDTree _HIPP_TEMPARG::nearest_k( const vector<float_t> &pos, index_t k ) const{
    return nearest_k(pos.data(), k);
}

_HIPP_TEMPHD
void KDTree _HIPP_TEMPARG::nearest_k( const float_t pos[DIM], index_t k, 
    vector<ngb_t> &ngbs) const {
    _obj_ptr->nearest_k(pos, k, ngbs);
}

_HIPP_TEMPHD
vector<typename KDTree _HIPP_TEMPARG::ngb_t> 
KDTree _HIPP_TEMPARG::nearest_r( const float_t pos[DIM], float_t r ) const{
    return _obj_ptr->nearest_r(pos, r);
}

_HIPP_TEMPHD
vector<typename KDTree _HIPP_TEMPARG::ngb_t> 
KDTree _HIPP_TEMPARG::nearest_r( const vector<float_t> &pos, float_t r ) const{
    return nearest_r(pos.data(), r);
}

_HIPP_TEMPHD
void KDTree _HIPP_TEMPARG::nearest_r(const float_t pos[DIM], float_t r, 
    vector<ngb_t> &ngbs) const 
{
    _obj_ptr->nearest_r(pos, r, ngbs);
}

_HIPP_TEMPHD
auto KDTree _HIPP_TEMPARG::count_r( const float_t pos[DIM], 
    float_t r ) const -> size_t 
{
    return _obj_ptr->count_r(pos, r);
}

_HIPP_TEMPHD
vector<typename KDTree _HIPP_TEMPARG::_p_node_t> 
KDTree _HIPP_TEMPARG::nearest_rect( 
    const float_t pos[DIM], const float_t dx[DIM] ) const{
    return _obj_ptr->nearest_rect(pos, dx);
}

_HIPP_TEMPHD
vector<typename KDTree _HIPP_TEMPARG::_p_node_t> 
KDTree _HIPP_TEMPARG::nearest_rect( 
    const vector<float_t> &pos, const vector<float_t> &dx ) const{
    return nearest_rect(pos.data(), dx.data());
}

_HIPP_TEMPHD
void KDTree _HIPP_TEMPARG::nearest_rect( const float_t pos[DIM], 
    const float_t dx[DIM], vector<_p_node_t> &ngbs ) const 
{
    _obj_ptr->nearest_rect(pos, dx, ngbs);    
}

_HIPP_TEMPHD
auto KDTree _HIPP_TEMPARG::count_rect( const float_t pos[DIM], 
    const float_t dx[DIM] ) const -> size_t 
{
    return _obj_ptr->count_rect(pos, dx);
}

_HIPP_TEMPHD
bool KDTree _HIPP_TEMPARG::empty() const noexcept{
    return _obj_ptr->empty();
}

_HIPP_TEMPHD
typename KDTree _HIPP_TEMPARG::index_t 
KDTree _HIPP_TEMPARG::size() const noexcept{
    return _obj_ptr->size();
}

_HIPP_TEMPHD
void KDTree _HIPP_TEMPARG::clear() noexcept{
    _obj_ptr->clear();
}

#undef _HIPP_RAWT
#undef _HIPP_TEMPARG
#undef _HIPP_TEMPHD

template<int DIM, std::size_t PADDING=0>
using KDTreeif = KDTree<DIM, int, float, PADDING>;
template<int DIM, std::size_t PADDING=0>
using KDTreeid = KDTree<DIM, int, double, PADDING>;
template<int DIM, std::size_t PADDING=0>
using KDTreelf = KDTree<DIM, long, float, PADDING>;
template<int DIM, std::size_t PADDING=0>
using KDTreeld = KDTree<DIM, long, double, PADDING>;


} // namespace ALGORITHM
} // namespace HIPP
#endif	//_HIPPALGORITHM_KD_TREE_H_