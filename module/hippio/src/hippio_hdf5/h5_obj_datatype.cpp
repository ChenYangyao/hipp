#include <hippio.h>

namespace HIPP::IO::H5 {

Datatype Datatype::copy() const {
    auto ret = obj_raw().copy( raw() );
    return _from_raw(ret);
}

void Datatype::commit(Group &group, const string &name, const Proplist &lcprop,
    Proplist &tcprop, Proplist &taprop)
{
    obj_raw().commit(group.raw(), name.c_str(), lcprop.raw(), tcprop.raw(), 
        taprop.raw());
}

void Datatype::lock() {
    obj_raw().lock();
}

bool Datatype::committed() const {
    return static_cast<bool>(obj_raw().committed());
}

bool Datatype::equal( const Datatype &dtype ) const {
    return static_cast<bool>( obj_raw().equal( dtype.raw() ) );
}

size_t Datatype::size() const {
    return obj_raw().get_size();
}

auto Datatype::get_class() const -> class_t {
    return obj_raw().get_class();
}

void Datatype::resize( size_t size ){
    obj_raw().set_size( size );
}

Datatype Datatype::resized(size_t size) const {
    auto ret = copy();
    ret.resize(size);
    return ret;
}

void Datatype::set_precision(size_t precision) {
    obj_raw().set_precision(precision);
}

Datatype Datatype::create(class_t c, size_t size){
    return _from_raw(_obj_raw_t::create(c, size));
}

Datatype Datatype::create_compound(size_t size) {
    return create(cCOMPOUND, size);
}

Datatype & Datatype::insert(const string &mem_name, size_t mem_offset, 
    const Datatype &mem_dtype) 
{
    obj_raw().insert(mem_name.c_str(), mem_offset, mem_dtype.raw());
    return *this;
}

Datatype & Datatype::pack(){
    obj_raw().pack();
    return *this;
}

unsigned Datatype::nmembers() const {
    return obj_raw().get_nmembers();
}

unsigned Datatype::member_index(const string &name) const {
    return obj_raw().get_member_index(name.c_str());
}

string Datatype::member_name(unsigned idx) const {
    return obj_raw().get_member_name(idx);
}

Datatype::class_t Datatype::member_class(unsigned idx) const {
    return obj_raw().get_member_class(idx);
}

size_t Datatype::member_offset(unsigned idx) const {
    return obj_raw().get_member_offset(idx);
}

Datatype Datatype::member_type(unsigned idx) const {
    return _from_raw(obj_raw().get_member_type(idx));
}

Datatype Datatype::create_array(const Dimensions &dims) const {
    auto ret = _obj_raw_t::create_array(raw(), dims.ndims(), dims.data());
    return _from_raw(ret);
}

unsigned Datatype::array_ndims() const {
    return obj_raw().get_array_ndims();
}

Dimensions Datatype::array_dims() const {
    Dimensions d( array_ndims() );
    obj_raw().get_array_dims(d.data());
    return d;
}

} // namespace HIPP::IO::H5
