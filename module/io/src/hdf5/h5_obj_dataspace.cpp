#include <h5_obj_dataspace.h>
namespace HIPP{
namespace IO{
    
H5Dataspace::H5Dataspace( const vector<hsize_t> &dims )
    :_obj_base_t( std::make_shared<_obj_raw_t>( dims.size(), dims.data() ) ){}
H5Dataspace::H5Dataspace( const vector<hsize_t> &dims, 
    const vector<hsize_t> &maxdims )
    :_obj_base_t( std::make_shared<_obj_raw_t>( 
        dims.size(), dims.data(), maxdims.data() ) ){}

H5Dataspace H5Dataspace::create_null(){
    return _from_raw( _obj_raw_t::create( H5S_NULL ) );
}
H5Dataspace H5Dataspace::create_scalar(){
    return _from_raw( _obj_raw_t::create( H5S_SCALAR ) );
}
H5Dataspace H5Dataspace::create_simple(){
    return _from_raw( _obj_raw_t::create( H5S_SIMPLE ) );
}

int H5Dataspace::ndims( )const{
    return _obj_ptr->get_simple_extent_ndims();
}
vector<hsize_t> H5Dataspace::dims( )const{
    int _ndims = ndims();
    vector<hsize_t> _dims( _ndims, 0 );
    _obj_ptr->get_simple_extent_dims( _dims.data(), NULL );
    return _dims;
}
vector<hsize_t> H5Dataspace::maxdims( )const{
    int _ndims = ndims();
    vector<hsize_t> _maxdims( _ndims, 0 );
    _obj_ptr->get_simple_extent_dims( NULL, _maxdims.data() );
    return _maxdims;
}
hsize_t H5Dataspace::size() const{
    auto _dims = dims();
    hsize_t _size = std::accumulate(
        _dims.begin(), _dims.end(), hsize_t(1), std::multiplies<hsize_t>());
    return _size;
}
void H5Dataspace::select_hyperslab( 
    const vector<hsize_t> &start, const vector<hsize_t> &count ){
    _obj_ptr->select_hyperslab( H5S_SELECT_SET, start.data(), NULL, 
        count.data(), NULL );
}
void H5Dataspace::select_hyperslab( const string &op,
    const hsize_t *start, const hsize_t *count, 
    const hsize_t *stride, const hsize_t *block )
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
hssize_t H5Dataspace::get_select_npoint()const{
    return _obj_ptr->get_select_npoints();
}


const H5Dataspace H5Dataspace::allval 
    { std::make_shared<H5Dataspace::_obj_raw_t>(H5S_ALL, 0) },
    H5Dataspace::nullval = H5Dataspace::create_null(),
    H5Dataspace::scalarval = H5Dataspace::create_scalar();
    
} // namespace IO
} // namespace HIPP