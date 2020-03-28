#ifndef _HIPPALGORITHM_KD_TREE_RAW_IMPL_H_
#define _HIPPALGORITHM_KD_TREE_RAW_IMPL_H_
#include "kd_tree_raw.h"
namespace HIPP{
namespace ALGORITHM{
namespace _kd_tree_helper{

#define _HIPP_TEMPHD template<int DIM, typename FLOAT_T, std::size_t PADDING>
#define _HIPP_TEMPARG <DIM,FLOAT_T,PADDING>

_HIPP_TEMPHD
point_t _HIPP_TEMPARG::point_t(const FLOAT_T *_pos)noexcept{
    std::copy(_pos, _pos+DIM, pos); 
}

_HIPP_TEMPHD
point_t _HIPP_TEMPARG::point_t(const vector<FLOAT_T> &_pos)noexcept
:point_t(_pos.data()){ }

_HIPP_TEMPHD
template<typename T>
point_t _HIPP_TEMPARG::point_t(
    const FLOAT_T *_pos, const T &_pad )noexcept{ 
    std::copy(_pos, _pos+DIM, pos);
    std::memcpy( pad, &_pad, sizeof(T) );
}

_HIPP_TEMPHD
template<typename T>
point_t _HIPP_TEMPARG::point_t(
    const vector<FLOAT_T> &_pos, const T &_pad )noexcept
:point_t(_pos.data(), _pad){ }

#undef _HIPP_TEMPARG
#undef _HIPP_TEMPHD

#define _HIPP_TEMPHD template<int DIM, typename FLOAT_T, \
    typename INDEX_T, std::size_t PADDING>
#define _HIPP_TEMPARG <DIM,FLOAT_T,INDEX_T,PADDING>

_HIPP_TEMPHD
node_t _HIPP_TEMPARG::node_t( 
    INDEX_T _size, int _axis, const FLOAT_T *_pos ) noexcept
:size(_size), axis(_axis){ 
    std::copy(_pos, _pos+DIM, pos); 
}

_HIPP_TEMPHD
node_t _HIPP_TEMPARG::node_t( 
    INDEX_T _size, int _axis, const vector<FLOAT_T> &_pos ) noexcept
:node_t(_size, _axis, _pos){}

_HIPP_TEMPHD
node_t _HIPP_TEMPARG::node_t( 
    INDEX_T _size, int _axis, const point_t &p) noexcept
:size(_size), axis(_axis) { 
    std::memcpy(pos, p.pos, sizeof(FLOAT_T)*DIM+PADDING); 
}

_HIPP_TEMPHD
FLOAT_T node_t _HIPP_TEMPARG::r2( 
    const FLOAT_T p1[DIM], const FLOAT_T p2[DIM] ) noexcept{
    FLOAT_T r2 = 0.;
    for(int i=0; i<DIM; ++i){
        FLOAT_T dx = p1[i]-p2[i];
        r2 += dx*dx;
    }
    return r2;
}

_HIPP_TEMPHD
FLOAT_T node_t _HIPP_TEMPARG::r( 
    const FLOAT_T p1[DIM], const FLOAT_T p2[DIM] ) noexcept{
    return std::sqrt(r2(p1,p2));
}

_HIPP_TEMPHD
ostream & node_t _HIPP_TEMPARG::info(
    ostream &os, int fmt_cntl) const{
    os << size << ' ' << axis << " (";
    return prt_a(os, pos, pos+DIM) << ")";
}

_HIPP_TEMPHD
ostream & operator<<( 
    ostream &os, const node_t _HIPP_TEMPARG &node ){
    return node.info(os);
}

#undef _HIPP_TEMPARG
#undef _HIPP_TEMPHD
} // namespace _kd_tree_helper

#define _HIPP_TEMPHD template<int DIM, typename INDEX_T, \
    typename FLOAT_T, std::size_t PADDING>
#define _HIPP_TEMPARG <DIM,INDEX_T,FLOAT_T,PADDING>


_HIPP_TEMPHD
_KDTree _HIPP_TEMPARG::_KDTree() noexcept {}

_HIPP_TEMPHD
_KDTree _HIPP_TEMPARG::_KDTree( 
    const _p_point_t b, const _p_point_t e ){
    construct_tree( b, e );
}

_HIPP_TEMPHD
_KDTree _HIPP_TEMPARG::_KDTree(const vector<point_t> &points )
:_KDTree( points.data(), points.data()+points.size() )
{}

_HIPP_TEMPHD
_KDTree _HIPP_TEMPARG::_KDTree( const _KDTree &tr )
:_nodes(tr._nodes)
{}

_HIPP_TEMPHD
_KDTree _HIPP_TEMPARG::_KDTree( _KDTree &&tr ) noexcept
:_nodes( std::move(tr._nodes) )
{}

_HIPP_TEMPHD
_KDTree _HIPP_TEMPARG & 
_KDTree _HIPP_TEMPARG::operator=( const _KDTree &tr ){
    _nodes = tr._nodes;
    return *this;
}

_HIPP_TEMPHD
_KDTree _HIPP_TEMPARG & 
_KDTree _HIPP_TEMPARG::operator=( _KDTree &&tr ) noexcept{
    _nodes = std::move(tr._nodes);
    return *this;
}

_HIPP_TEMPHD
_KDTree _HIPP_TEMPARG::~_KDTree() noexcept{ }


_HIPP_TEMPHD 
void _KDTree _HIPP_TEMPARG::construct_tree( 
    const _p_point_t b, const _p_point_t e ){
    _construct_tree( b, e );
}
_HIPP_TEMPHD 
void _KDTree _HIPP_TEMPARG::construct_tree( 
    const vector<point_t> &points ){
    construct_tree( points.data(), points.data()+points.size() );
}

_HIPP_TEMPHD
ostream & _KDTree _HIPP_TEMPARG::info( 
    ostream &os, int fmt_cntl ) const {
    if( fmt_cntl == 0 ){
        prt(os, HIPPCNTL_CLASS_INFO_INLINE(_KDTree),
            "size: ", _nodes.size());
    }
    if( fmt_cntl >= 1 ){
        prt(os, HIPPCNTL_CLASS_INFO(_KDTree), 
            "  Size: ", _nodes.size(),
            "\n  Nodes: (depth, node size, partition axis, position)\n");

        struct record_t{ 
            _p_node_t pnode; index_t depth; 
            record_t( _p_node_t _pnode, index_t _depth )
                :pnode(_pnode), depth(_depth){ } };

        std::stack<record_t> stk;
        if( !_nodes.empty() ) stk.push( record_t(_nodes.data(), 0) );
        else prt(os, "    None\n");
        while (!stk.empty()){
            record_t t = stk.top(); stk.pop();
            for(index_t i=0, n=4+2*t.depth; i<n; ++i) os << ' ';
            prt(os, '[', t.depth, "] ") << (*t.pnode) << '\n';
            if( t.pnode->size > 1 ){
                _p_node_t left = t.pnode+1;
                if( t.pnode->size != left->size+1 )
                    stk.emplace( left+left->size, t.depth+1 );
                stk.emplace( left, t.depth+1 );
            }
        }
        os << std::flush;
    }
    return os;
}

_HIPP_TEMPHD
ostream & operator<<( 
    ostream &os, const _KDTree _HIPP_TEMPARG &tr ){
    return tr.info(os);
}

_HIPP_TEMPHD
typename _KDTree _HIPP_TEMPARG::ngb_t 
_KDTree _HIPP_TEMPARG::nearest( const float_t *pos ) const{
    std::stack<_p_2node_t> stk;
    _p_node_t pnode = nullptr;
    float_t rsq = std::numeric_limits<float_t>().max();
    if( !empty() )
        _nearest(pos, stk, pnode, rsq);
    return ngb_t( pnode, std::sqrt(rsq) );
}

_HIPP_TEMPHD
vector<typename _KDTree _HIPP_TEMPARG::ngb_t> 
_KDTree _HIPP_TEMPARG::nearest_k( const float_t *pos, index_t k ) const{
    std::stack<_p_2node_t> stk;
    std::priority_queue<ngb_t> pq;
    if( !empty() )
        _nearest_k(pos, k, stk, pq);
    
    vector<ngb_t> ans;
    while( !pq.empty() ){
        const auto &t = pq.top();
        if( t.node )
            ans.emplace_back( t.node, std::sqrt( t.r ) );
        pq.pop();
    }
    std::reverse( ans.begin(), ans.end() );
    return ans;
}

_HIPP_TEMPHD
vector<typename _KDTree _HIPP_TEMPARG::ngb_t> 
_KDTree _HIPP_TEMPARG::nearest_r( const float_t *pos, float_t r ) const{
    std::stack<_p_2node_t> stk;
    vector<ngb_t> ngbs;
    if( !empty() )
        _nearest_r( pos, r*r, stk, ngbs );
    for(auto &ngb: ngbs)
        ngb.r = std::sqrt( ngb.r );
    return ngbs;
}

_HIPP_TEMPHD 
vector<typename _KDTree _HIPP_TEMPARG::_p_node_t> 
_KDTree _HIPP_TEMPARG::nearest_rect( 
    const float_t *pos, const float_t *dx ) const{
    std::stack<_p_2node_t> stk;
    vector<_p_node_t> ngbs;
    if( !empty() )
        _nearest_rect(pos, dx, stk, ngbs);
    return ngbs;
}

_HIPP_TEMPHD 
bool _KDTree _HIPP_TEMPARG::empty() const noexcept{
    return _nodes.empty();
}

_HIPP_TEMPHD 
typename _KDTree _HIPP_TEMPARG::index_t 
_KDTree _HIPP_TEMPARG::size() const noexcept{
    return _nodes.size();
}

_HIPP_TEMPHD
void _KDTree _HIPP_TEMPARG::clear() noexcept{
    _nodes.clear();
}

_HIPP_TEMPHD
void _KDTree _HIPP_TEMPARG::_construct_tree( 
    _p_point_t  const b, _p_point_t const e ){
    index_t nps = e-b;
    _nodes.clear();
    _nodes.reserve( nps );
    vector<_p_point_t > ps( nps );
    for(std::size_t i=0, n=ps.size(); i!=n; ++i) ps[i] = b+i;

    std::default_random_engine re;
    std::stack<std::pair<index_t, index_t> > brhs;
    index_t lo = 0, hi = ps.size();
#if defined(HIPPDEV) && defined(HIPPDEBUG) && defined(VERBOSE)
    prt(cerr, emFLF, "\nPartite: \n");
#endif
    while( lo != hi ){
        int direc = _choose_direc( ps, lo, hi );
        index_t mid = _partite( ps, lo, hi, direc, re );
        _nodes.emplace_back( hi-lo, direc, *ps[mid] );
#if defined(HIPPDEV) && defined(HIPPDEBUG) && defined(VERBOSE)
        prt(cerr, "  axis=", direc, ", midpoint=", mid, " (");
        prt_a( cerr, ps[mid]->pos, ps[mid]->pos+DIM ) << ")" << endl;
#endif
        if( lo != mid ){
            if( mid + 1 != hi ) brhs.emplace( mid+1, hi );
            hi = mid;
        }else if( !brhs.empty() ){
            const auto &top = brhs.top();
            lo = top.first;
            hi = top.second;
            brhs.pop();
        }else hi = lo;
    }
}

_HIPP_TEMPHD
int _KDTree _HIPP_TEMPARG::_choose_direc( 
    const vector<_p_point_t > &ps, index_t b, index_t e ){
    int direc = -1;
    float_t maxsep = std::numeric_limits<float_t>().lowest();
    for(int dim=0; dim<DIM; ++dim){
        float_t min = std::numeric_limits<float_t>().max(),
            max = std::numeric_limits<float_t>().lowest();
        for(index_t j=b; j!=e; ++j){
            min = std::min( min, ps[j]->pos[dim] );
            max = std::max( max, ps[j]->pos[dim] );
        }
        float_t sep = max - min;
        if( sep > maxsep ){
            maxsep = sep;
            direc = dim;
        }
    }
    return direc;
}

_HIPP_TEMPHD
typename _KDTree _HIPP_TEMPARG::index_t 
_KDTree _HIPP_TEMPARG::_partite( 
    vector<_p_point_t > &ps, index_t b, index_t e, const int direc,
    std::default_random_engine &re ){
    index_t cent = b + (e-b)/2;
    while(true){
        if( e - b <= index_t(1) ) break;
        index_t pivot_idx = 
            std::uniform_int_distribution<index_t>(b, e-1)( re );
        swap( ps[e-1], ps[pivot_idx] );
        _p_point_t pivot = ps[e-1];
        index_t mid_idx = b;
        for(index_t i=b; i!=e-1; ++i){
            if( ps[i]->pos[direc] < pivot->pos[direc] ){
                swap( ps[mid_idx++], ps[i] );
            }
        }
        swap(ps[mid_idx], ps[e-1]);
        if( mid_idx < cent ) b = mid_idx + 1;
        else if( mid_idx > cent) e = mid_idx;
        else break;
    }
    return cent;
}

_HIPP_TEMPHD
void _KDTree _HIPP_TEMPARG::_nearest( 
    const float_t pos[DIM], std::stack<_p_2node_t> &stk, 
    _p_node_t &pnode, FLOAT_T &rsq ) const
{
    _p_node_t near = nullptr;
    float_t min_r2 = std::numeric_limits<FLOAT_T>().max();
    _p_node_t root = _nodes.data(), parent, sibl;
#if defined(HIPPDEV) && defined(HIPPDEBUG) && defined(VERBOSE)
    prt(cerr, emFLF, '\n');
    prt(cerr, "pos=("); prt_a(cerr, pos, pos+DIM) << ')' << endl;
#endif    
    while( root ){
#if defined(HIPPDEV) && defined(HIPPDEBUG) && defined(VERBOSE)
    prt(cerr, "  Best R-sq: ", min_r2);
    if( near ) prt(cerr, ", node: ") << *near;
    cerr << endl;
#endif
        _walk_down(root, pos, stk, parent, sibl);
        do{
            int axis = parent->axis;
            float_t dx = pos[axis] - parent->pos[axis];
            if( dx*dx < min_r2 ){
                float_t r2 = node_t::r2( pos, parent->pos );
                if( r2 < min_r2 ){
                    min_r2 = r2; near = parent;
                }
                if( sibl ){ root = sibl; break; }
            }
            if( !stk.empty() ){
                const auto &t = stk.top();
                parent = t.parent; sibl = t.sibl;
                stk.pop();
            }else parent = nullptr;
        }while( parent );
    }
    pnode = near; rsq = min_r2;
}

_HIPP_TEMPHD
void _KDTree _HIPP_TEMPARG::_nearest_k( const float_t pos[DIM], const index_t k,
    std::stack<_p_2node_t> &stk, std::priority_queue<ngb_t> &pq ) const
{   
    float_t min_r2 = std::numeric_limits<float_t>().max();
    for(index_t i=0; i<k; ++i) pq.emplace( nullptr, min_r2);
    
    _p_node_t root = _nodes.data(), parent, sibl;
#if defined(HIPPDEV) && defined(HIPPDEBUG) && defined(VERBOSE)
    prt(cerr, emFLF, '\n');
    prt(cerr, "pos=("); prt_a(cerr, pos, pos+DIM) << "), k=" << k << endl;
#endif
    while( root ){
#if defined(HIPPDEV) && defined(HIPPDEBUG) && defined(VERBOSE)
        prt(cerr, "  Currrent threshold R-sq: ", min_r2);
        if( pq.top().node ) prt(cerr, ", farest node: ") << *pq.top().node;
        cerr << endl;
#endif
        _walk_down(root, pos, stk, parent, sibl);
        do{
            int axis = parent->axis;
            float_t dx = pos[axis] - parent->pos[axis];
            if( dx*dx < min_r2 ){
                float_t r2 = node_t::r2( pos, parent->pos );
                if( r2 < min_r2 ){
                    pq.pop();
                    pq.emplace(parent, r2);
                    min_r2 = pq.top().r;
                }
                if( sibl ){ root = sibl; break; }
            }
            if( !stk.empty() ){
                const auto &t = stk.top();
                parent = t.parent; sibl = t.sibl;
                stk.pop();
            }else parent = nullptr;
        }while( parent );
    }
}

_HIPP_TEMPHD
void _KDTree _HIPP_TEMPARG::_nearest_r( const float_t pos[DIM], 
    const float_t rsq,
    std::stack<_p_2node_t> &stk, vector<ngb_t> &ngbs ) const
{
    _p_node_t root = _nodes.data(), parent, sibl;
#if defined(HIPPDEV) && defined(HIPPDEBUG) && defined(VERBOSE)
    prt(cerr, emFLF, '\n');
    prt(cerr, "pos=("); prt_a(cerr, pos, pos+DIM) 
        << "), R-sq limit=" << rsq << endl;
#endif
    while( root ){

#if defined(HIPPDEV) && defined(HIPPDEBUG) && defined(VERBOSE)
        prt(cerr, "  Currrent found: ", ngbs.size(), " nodes") << endl;
        if( !ngbs.empty() ){
            prt(cerr, "    r, size, axis, pos:");
            for(const auto &ngb: ngbs)
                cerr << ' ' << std::sqrt(ngb.r) << *ngb.node;
            cerr << endl;
        }
#endif
        _walk_down(root, pos, stk, parent, sibl);
        do{
            int axis = parent->axis;
            float_t dx = pos[axis] - parent->pos[axis];
            if( dx*dx < rsq ){
                float_t r2 = node_t::r2( pos, parent->pos );
                if( r2 < rsq ){
                    ngbs.emplace_back( parent, r2 );
                }
                if( sibl ){ root = sibl; break; }
            }
            if( !stk.empty() ){
                const auto &t = stk.top();
                parent = t.parent; sibl = t.sibl;
                stk.pop();
            }else parent = nullptr;
        }while( parent );
    }
}

_HIPP_TEMPHD
void _KDTree _HIPP_TEMPARG::_nearest_rect( 
    const float_t pos[DIM], const float_t dx[DIM],
    std::stack<_p_2node_t> &stk, vector<_p_node_t> &ngbs ) const{

    _p_node_t root = _nodes.data(), parent, sibl;
#if defined(HIPPDEV) && defined(HIPPDEBUG) && defined(VERBOSE)
    prt(cerr, emFLF, '\n');
    prt(cerr, "pos=("); prt_a(cerr, pos, pos+DIM) << "), dx limit=(";
    prt_a(cerr, dx, dx+DIM) << ')' << endl;
#endif
    while( root ){
#if defined(HIPPDEV) && defined(HIPPDEBUG) && defined(VERBOSE)
        prt(cerr, "  Currrent found: ", ngbs.size(), " nodes") << endl;
        if( !ngbs.empty() ){
            prt(cerr, "    size, axis, pos:");
            for(const auto &ngb: ngbs)
                cerr << ' ' << *ngb;
            cerr << endl;
        }
#endif
        _walk_down(root, pos, stk, parent, sibl);
        do{
            int axis = parent->axis;
            if( std::fabs(pos[axis] - parent->pos[axis]) < dx[axis] ){
                bool inters = true;
                for(int i=0; i<DIM; ++i)
                    inters = inters && 
                        ( std::fabs(pos[i]-parent->pos[i]) < dx[i] );
                if( inters ){
                    ngbs.emplace_back( parent );
                }
                if( sibl ){ root = sibl; break; }
            }
            if( !stk.empty() ){
                const auto &t = stk.top();
                parent = t.parent; sibl = t.sibl;
                stk.pop();
            }else parent = nullptr;
        }while( parent );
    }
}

_HIPP_TEMPHD
void _KDTree _HIPP_TEMPARG::_walk_down( 
    _p_node_t &root, 
    const float_t pos[DIM], std::stack<_p_2node_t> &stk, 
    _p_node_t &parent, _p_node_t &sibl) const{

    parent = root; sibl = nullptr;

#if defined(HIPPDEV) && defined(HIPPDEBUG) && defined(VERBOSE)
    prt(cerr, "  Walk path:");
    cerr << ' ' << *parent;
#endif
    while ( parent->size > 1 ){
        _p_node_t left = parent+1,
            right = (parent->size > 2)?(left+left->size):NULL;
        int axis = parent->axis;
        _p_node_t child;
        if( pos[axis] < parent->pos[axis] ){
            child = left; sibl = right;
        }else{
            child = right; sibl = left;
        }
        if( child ){
            stk.emplace(parent, sibl);
            parent = child;
            sibl = nullptr;
        }else break;
#if defined(HIPPDEV) && defined(HIPPDEBUG) && defined(VERBOSE)
    cerr << ' ' << *parent;
#endif
    }
    root = nullptr;
#if defined(HIPPDEV) && defined(HIPPDEBUG) && defined(VERBOSE)
    cerr << endl;
#endif
}

#undef _HIPP_TEMPARG
#undef _HIPP_TEMPHD

} // namespace ALGORITHM
} // namespace HIPP
#endif	//_HIPPALGORITHM_KD_TREE_RAW_IMPL_H_