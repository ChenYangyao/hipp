/**
 * creat: Yangyao CHEN, 2020/01/11
 *      [write   ] H5Dataspace - HDF5 high-level dataspace object.
 *      [modify  ]
 *      [debug   ]
 *      [comment ]
 */ 

#ifndef _HIPPIO_H5_OBJ_DATASPACE_H_
#define _HIPPIO_H5_OBJ_DATASPACE_H_
#include "h5_obj_base.h"
namespace HIPP{
namespace IO{

/**
 * HDF5 high-level dataspace object.
 */
class H5Dataspace: public H5Obj<_H5Dataspace>{
public:
    typedef H5Obj<_H5Dataspace> _obj_base_t;
    using _obj_base_t::_obj_base_t;
    H5Dataspace( const vector<size_t> &dims );
    H5Dataspace( const vector<size_t> &dims, const vector<size_t> &maxdims );

    int ndims( )const;
    vector<size_t> dims( )const;
    vector<size_t> maxdims( )const;
    size_t size() const;
    void select_hyperslab( 
        const vector<size_t> &start, const vector<size_t> &count );
    void select_hyperslab( const string &op,
        const size_t *start, const size_t *count, 
        const size_t *stride = NULL, const size_t *block = NULL );
    size_t get_select_npoint()const;

    static const H5Dataspace allval;
protected:
    static H5Dataspace _from_raw( id_t dspace ) noexcept;
};

inline H5Dataspace::H5Dataspace( const vector<size_t> &dims )
    :_obj_base_t( std::make_shared<_obj_raw_t>( dims.size(), dims.data() ) ){}
inline H5Dataspace::H5Dataspace( const vector<size_t> &dims, 
    const vector<size_t> &maxdims )
    :_obj_base_t( std::make_shared<_obj_raw_t>( 
        dims.size(), dims.data(), maxdims.data() ) ){}

inline int H5Dataspace::ndims( )const{
    return _obj_ptr->get_simple_extent_ndims();
}
inline vector<H5Dataspace::size_t> H5Dataspace::dims( )const{
    int _ndims = ndims();
    vector<size_t> _dims( _ndims, 0 );
    _obj_ptr->get_simple_extent_dims( _dims.data(), NULL );
    return _dims;
}
inline vector<H5Dataspace::size_t> H5Dataspace::maxdims( )const{
    int _ndims = ndims();
    vector<size_t> _maxdims( _ndims, 0 );
    _obj_ptr->get_simple_extent_dims( NULL, _maxdims.data() );
    return _maxdims;
}
inline H5Dataspace::size_t H5Dataspace::size() const{
    auto _dims = dims();
    size_t _size = std::accumulate(
        _dims.begin(), _dims.end(), size_t(1), std::multiplies<size_t>());
    return _size;
}
inline void H5Dataspace::select_hyperslab( 
    const vector<size_t> &start, const vector<size_t> &count ){
    _obj_ptr->select_hyperslab( H5S_SELECT_SET, start.data(), NULL, 
        count.data(), NULL );
}
inline void H5Dataspace::select_hyperslab( const string &op,
    const size_t *start, const size_t *count, 
    const size_t *stride, const size_t *block )
{
    _obj_raw_t::seloper_t _op = H5S_SELECT_SET;
    if( op == "set" )
        _op = H5S_SELECT_SET;
    else if( op == "or" || op == "|" )
        _op = H5S_SELECT_OR;
    else if( op == "and" || op == "&" )
        _op = H5S_SELECT_AND;
    else if( op == "xor" || op == "^" )
        _op = H5S_SELECT_XOR;
    else if( op == "notb" )
        _op = H5S_SELECT_NOTB;
    else if( op == "nota" )
        _op = H5S_SELECT_NOTA;
    else 
        ErrLogic::throw_( ErrLogic::eDOMAIN, "  ... oprerator ", 
            op, " not allowed" );
    _obj_ptr->select_hyperslab( _op, start, stride, count, block );    
}
inline H5Dataspace::size_t H5Dataspace::get_select_npoint()const{
    return _obj_ptr->get_select_npoints();
}

inline H5Dataspace H5Dataspace::_from_raw( id_t dspace ) noexcept{
    auto ptr = std::make_shared<_obj_raw_t>( dspace );
    return H5Dataspace( ptr );
}


} // namespace IO
} // namespace HIPP
#endif	//_HIPPIO_H5_OBJ_DATASPACE_H_