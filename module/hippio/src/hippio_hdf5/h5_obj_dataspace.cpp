#include <hippio.h>

namespace HIPP::IO::H5 {

const Dataspace 
    Dataspace::vALL     = _from_raw(_obj_raw_t::vALL, 0),
    Dataspace::vNULL    = create_null(),
    Dataspace::vSCALAR  = create_scalar();

const std::unordered_map<string, Dataspace::seloper_t> 
Dataspace::_seloper_map = {
    {"set", Dataspace::selSET}, 
    {"=", Dataspace::selSET},

    {"or", Dataspace::selOR}, 
    {"|", Dataspace::selOR},

    {"and", Dataspace::selAND}, 
    {"&", Dataspace::selAND},

    {"xor", Dataspace::selXOR}, 
    {"^", Dataspace::selXOR},

    {"notb", Dataspace::selNOTB}, 

    {"nota", Dataspace::selNOTA},

    {"+", Dataspace::selAPPEND},
    {"append", Dataspace::selAPPEND},

    {"prepend", Dataspace::selPREPEND}
};

Dataspace::Dataspace(std::initializer_list<hsize_t> dims) 
: Dataspace { _ptr_from_raw(dims.size(), std::data(dims)) }
{}

Dataspace::Dataspace(vector<hsize_t> & dims) 
: Dataspace { _ptr_from_raw(dims.size(), dims.data() ) }
{

}

Dataspace::Dataspace(const Dimensions &dims) 
: Dataspace { _ptr_from_raw(dims.size(), dims.data()) }
{

}

Dataspace::Dataspace(const Dimensions &dims, const Dimensions &maxdims) 
: Dataspace {nullptr}
{
    if( dims.size() != maxdims.size() ) 
        ErrLogic::throw_(ErrLogic::eLENGTH, emFLPFB, 
            "  ... inputs dims and maxdims have inconsistent size (", 
            dims.size(), " and ", maxdims.size(), ")\n");
    (*this) = _from_raw(dims.size(), dims.data(), maxdims.data());
}

ostream & Dataspace::info(ostream &os, int fmt_cntl) const {
    PStream ps {os};
    class_t c = get_type();
    if( fmt_cntl == 0 ) {
        ps << "Dataspace{type=";
        if( c == cNULL ) ps << "NULL";
        else if( c == cSCALAR ) ps << "SCALAR";
        else if( c == cSIMPLE ) {
            auto d = dims(), md = maxdims();
            ps << "SIMPLE, dims={", d.as_vector(), 
            "}, maxdims={", md.as_vector(), "}";
        } 
        else ps << "Unknown";
        ps << "}";
        return os;
    }

    ps << HIPPCNTL_CLASS_INFO(HIPP::IO::H5::Dataspace),
        "  Type{";
    
    if( c == cNULL ) ps << "NULL";
    else if( c == cSCALAR ) ps << "SCALAR";
    else if( c == cSIMPLE ) {
        auto d = dims(), md = maxdims();
        ps << "SIMPLE}\n",
            "Dims{", d.as_vector(), 
        "}, Maxdims{", md.as_vector();
    } 
    else ps << "Unknown";
    ps << "}\n";

    return os;
}

ostream & operator<< (ostream &os, const Dataspace &dspace) {
    return dspace.info(os);
}

Dataspace Dataspace::create(class_t type){
    return _from_raw( _obj_raw_t::create(type) );
}

Dataspace Dataspace::create_null(){
    return create(cNULL);
}

Dataspace Dataspace::create_scalar(){
    return create(cSCALAR);
}

Dataspace Dataspace::create_simple(){
    return create(cSIMPLE);
}

Dataspace Dataspace::copy() const {
    auto ret = obj_raw().copy();
    return _from_raw(ret);
}

void Dataspace::extent_copy(Dataspace &dst_dspace) const {
    obj_raw().extent_copy(dst_dspace.raw());
}

Dataspace::class_t Dataspace::get_type() const {
    return obj_raw().get_simple_extent_type();
}


int Dataspace::ndims() const {
    return obj_raw().get_simple_extent_ndims();
}

Dimensions Dataspace::dims() const {
    Dimensions d( ndims() );
    obj_raw().get_simple_extent_dims(d.data(), NULL);
    return d;
}

Dimensions Dataspace::maxdims() const {
    Dimensions d( ndims() );
    obj_raw().get_simple_extent_dims(NULL, d.data());
    return d;
}

hsize_t Dataspace::size() const {
    return dims().n_elems();
}

hsize_t Dataspace::maxsize() const {
    return maxdims().n_elems();
}

void Dataspace::select_none() {
    obj_raw().select_none();
}

void Dataspace::select_all() {
    obj_raw().select_all();
}

void Dataspace::select_hyperslab(const Hyperslab &hs) {
    select_hyperslab(selSET, hs);
}

void Dataspace::select_hyperslab(const string &op, const Hyperslab &hs) {
    
    select_hyperslab(_find_seloper(op), hs);
}

void Dataspace::select_hyperslab(seloper_t op, const Hyperslab &hs) {
    auto [start, count, stride, block] = hs.data();
    select_hyperslab(op, start, count, stride, block);
}

void Dataspace::select_hyperslab(const string &op,
    const hsize_t *start, const hsize_t *count, 
    const hsize_t *stride, const hsize_t *block) 
{
    select_hyperslab(_find_seloper(op), start, count, stride, block);
}

void Dataspace::select_hyperslab(seloper_t op,
    const hsize_t *start, const hsize_t *count, 
    const hsize_t *stride, const hsize_t *block) 
{
    obj_raw().select_hyperslab(op, start, stride, count, block);
}

Dataspace Dataspace::combine_hyperslab(const string &op, 
    const Hyperslab &hs) const 
{
    return combine_hyperslab(_find_seloper(op), hs);
}

Dataspace Dataspace::combine_hyperslab(seloper_t op, const Hyperslab &hs) const 
{
    auto [start, count, stride, block] = hs.data();
    auto ret = obj_raw().combine_hyperslab(op, start, stride, count, block);
    return _from_raw(ret);
}

Dataspace Dataspace::combine_select(const string &op, 
    const Dataspace &dspace2) const 
{
    return combine_select(_find_seloper(op), dspace2);
}

Dataspace Dataspace::combine_select(seloper_t op, 
    const Dataspace &dspace2) const 
{
    auto ret = obj_raw().combine_select(op, dspace2.raw());
    return _from_raw(ret);
}

Dataspace Dataspace::hyperslabed(const Hyperslab &hs) const {
    auto ret = create_simple();
    extent_copy(ret);
    ret.select_hyperslab(hs);
    return ret;
}

void Dataspace::select_elements(const Points &points) {
    select_elements(selSET, points);
}

void Dataspace::select_elements(const string &op, const Points &points) {
    select_elements(_find_seloper(op), points);
}

void Dataspace::select_elements(seloper_t op, const Points &points) {
    select_elements(op, points.n_points(), points.coords());
}

void Dataspace::select_elements(const string &op, size_t n_elems, 
    const hsize_t coords[])
{
    select_elements(_find_seloper(op), n_elems, coords);
}

void Dataspace::select_elements(seloper_t op, size_t n_elems, const hsize_t 
    coords[])
{
    obj_raw().select_elements(op, n_elems, coords);
}

Dataspace Dataspace::elemented(const Points &points) const {
    auto ret = create_simple();
    extent_copy(ret);
    ret.select_elements(points);
    return ret;
}

hssize_t Dataspace::get_select_npoints() const {
    return obj_raw().get_select_npoints();
}

auto Dataspace::get_select_type() const -> seltype_t {
    return obj_raw().get_select_type();
}
    
bool Dataspace::select_valid() const {
    return (bool)obj_raw().select_valid();
}

} // namespace HIPP::IO::H5
