#include <hippio.h>

namespace HIPP::IO::H5 {
/*
auto H5Group::n_links() const -> size_t {
    info_t info;
    _obj_ptr->get_info(info);
    return info.nlinks;
}
vector<string> H5Group::keys(index_t index_field, iter_order_t order) const
{
    size_t _n_links = n_links();
    vector<string> _keys; 
    _keys.reserve(_n_links);
    hid_t lapl_id = H5Proplist::defaultval.raw();
    for(size_t i=0; i<_n_links; ++i){
        _keys.emplace_back( _H5Link::get_name_by_idx(
                raw(), ".", index_field, order, i, lapl_id));
    }
    return _keys;
}*/

void Group::get_info(info_t &group_info) const {
    obj_raw().get_info(group_info);
}

void Group::get_info(const string &name, info_t &group_info, 
    const Proplist &laprop) const
{
    obj_raw().get_info_by_name(name.c_str(), group_info, laprop.raw());
}

void Group::get_info(const string &group_name, hsize_t idx, info_t &group_info, 
    index_t idx_type, iter_order_t order, 
    const Proplist &laprop) const
{
    obj_raw().get_info_by_idx(group_name.c_str(), idx_type, order, idx, 
        group_info, laprop.raw());
}

Group::info_t Group::get_info() const {
    info_t info;
    get_info(info);
    return info;
}

Group::info_t Group::get_info(const string &name, 
    const Proplist &laprop) const 
{
    info_t info;
    get_info(name, info, laprop);
    return info;
}

Group::info_t Group::get_info(const string &group_name, hsize_t idx,
    index_t idx_type, iter_order_t order, 
    const Proplist &laprop) const
{
    info_t info;
    get_info(group_name, idx, info, idx_type, order, laprop);
    return info;
}

void Group::get_object_info(obj_info_t &info, info_field_t fields) const
{
    this->NamedObj::get_info(info, fields);
}

void Group::get_object_info(const string &name, obj_info_t &info, 
    info_field_t fields, const Proplist &laprop) const
{
    obj_raw()._NamedObj::get_info_by_name(name.c_str(), info, 
        fields, laprop.raw());
}

void Group::get_object_info(const string &group_name, hsize_t idx, 
    obj_info_t &info, info_field_t fields,
    index_t idx_type, iter_order_t order, 
    const Proplist &laprop) const
{
    obj_raw()._NamedObj::get_info_by_idx(group_name.c_str(), 
        idx_type, order, idx, info, fields, laprop.raw());
}

Group::obj_info_t Group::get_object_info(info_field_t fields) const
{
    obj_info_t info;
    get_object_info(info, fields);
    return info;
}

Group::obj_info_t Group::get_object_info(const string &name,  
    info_field_t fields, const Proplist &laprop) const
{
    obj_info_t info;
    get_object_info(name, info, fields, laprop);
    return info;
}

Group::obj_info_t Group::get_object_info(const string &group_name, hsize_t idx,  
    info_field_t fields, index_t idx_type, iter_order_t order, 
    const Proplist &laprop) const
{
    obj_info_t info;
    get_object_info(group_name, idx, info, fields, idx_type, order, laprop);
    return info;
}

void Group::get_link_info(const string &name, link_info_t &info, 
    const Proplist &laprop) const
{
    _Link::get_info(raw(), name.c_str(), info, laprop.raw());
}

void Group::get_link_info(const string &group_name, hsize_t idx, 
    link_info_t &info, index_t idx_type, iter_order_t order,
    const Proplist &laprop) const
{
    _Link::get_info_by_idx(raw(), group_name.c_str(), idx_type, order, 
        idx, info, laprop.raw());
}

Group::link_info_t Group::get_link_info(const string &name, 
    const Proplist &laprop) const
{
    link_info_t info;
    get_link_info(name, info, laprop);
    return info;
}

Group::link_info_t Group::get_link_info(const string &group_name, hsize_t idx,
    index_t idx_type, iter_order_t order, const Proplist &laprop) const
{
    link_info_t info;
    get_link_info(group_name, idx, info, idx_type, order, laprop);
    return info;
}

void Group::get_link_val(const string &name, vector<char> &buff, 
    const Proplist &laprop) const
{
    size_t n = get_link_info(name, laprop).u.val_size;
    buff.resize(n);
    _Link::get_val(raw(), name.c_str(), buff.data(), n, laprop.raw());
}

void Group::get_link_val(const string &group_name, hsize_t idx, 
    vector<char> &buff, index_t idx_type, iter_order_t order,
    const Proplist &laprop) const
{
    size_t n = get_link_info(group_name, idx, idx_type, order, 
        laprop).u.val_size;
    buff.resize(n);
    _Link::get_val_by_idx(raw(), group_name.c_str(), idx_type, order,  idx, 
        buff.data(), n, laprop.raw());
}

string Group::get_link_name(const string &group_name, hsize_t idx, 
    index_t idx_type, iter_order_t order, const Proplist &laprop) const
{
    return _Link::get_name_by_idx(raw(), group_name.c_str(), idx_type, order, 
        idx, laprop.raw());
}

bool Group::link_exists(const string &name, const Proplist &laprop) const {
    SubpathIterator subp {name};
    auto obj_id = raw();
    auto laprop_val = laprop.raw();

    bool ret = false;
    while( subp.next() ) {
        const char *subp_name = subp.c_str();
        if( !_Link::exists(obj_id, subp_name, laprop_val) ) break;
        
        if( !subp.has_next() ) {
            ret = true; break;
        }

        if( ! obj_raw().exists_by_name(subp_name, laprop_val) ) break;
    }

    return ret;
}

bool Group::object_exists(const string &name, const Proplist &laprop) const {
    if( ! link_exists(name) ) return false;
    return (bool) obj_raw().exists_by_name(name.c_str(), laprop.raw());
}

bool Group::object_exists_by_type(const string &name, 
    obj_type_t obj_type, const Proplist &laprop) const
{
    if( ! object_exists(name, laprop) ) return false;
    
    obj_info_t info;
    get_object_info(name, info, infoBASIC, laprop);
    return info.type == obj_type;
}

bool Group::group_exists(const string &name) const {
    return object_exists_by_type(name, tGROUP);
}

bool Group::dataset_exists(const string &name)const {
    return object_exists_by_type(name, tDATASET);
}

void Group::create_hard_link(const string &name, 
    const Group &src, const string &src_name, 
    const Proplist &lcprop, const Proplist &laprop)
{
    _Link::create_hard(src.raw(), src_name.c_str(), 
        raw(), name.c_str(), lcprop.raw(), laprop.raw());
}

void Group::create_soft_link(const string &name, 
    const string &src_name,
    const Proplist &lcprop, const Proplist &laprop)
{
    _Link::create_soft(src_name.c_str(), raw(), name.c_str(), 
        lcprop.raw(), laprop.raw());
}

void Group::create_external_link(const string &name, 
    const string &src_file_name, const string &src_obj_name,
    const Proplist &lcprop, const Proplist &laprop)
{
    _Link::create_external(src_file_name.c_str(), src_obj_name.c_str(), 
        raw(), name.c_str(), lcprop.raw(), laprop.raw());
}

void Group::move_link(const string &name, Group &dst, const string &dst_name,
    const Proplist &lcprop, const Proplist &laprop)
{
    _Link::move(raw(), name.c_str(), dst.raw(), dst_name.c_str(), lcprop.raw(),
        laprop.raw());
}

void Group::copy_link(const string &name, Group &dst, const string &dst_name,
    const Proplist &lcprop, const Proplist &laprop)
{
    _Link::copy(raw(), name.c_str(), dst.raw(), dst_name.c_str(), lcprop.raw(),
        laprop.raw());
}

void Group::delete_link(const string &name, const Proplist &laprop) {
    _Link::delete_(raw(), name.c_str(), laprop.raw());
}

void Group::delete_link(const string &group_name, hsize_t idx, 
    index_t idx_type, iter_order_t order,
    const Proplist &laprop)
{
    _Link::delete_by_idx(raw(), group_name.c_str(), idx_type, 
        order, idx, laprop.raw());
}

herr_t Group::link_iterate(hsize_t &idx, link_iter_op_t op, void *op_data,
    index_t idx_type, iter_order_t order) const 
{
    using namespace _group_link_helper;
    iter_data_t data { std::move(op), op_data };
    return _Link::iterate(raw(), idx_type, order, idx, raw_op, &data);
}

herr_t Group::link_iterate(const string &group_name,
    hsize_t &idx, link_iter_op_t op, void *op_data,
    index_t idx_type, iter_order_t order, const Proplist &laprop) const
{
    using namespace _group_link_helper;
    iter_data_t data {std::move(op), op_data};
    return _Link::iterate_by_name(raw(), group_name.c_str(), 
        idx_type, order, idx, raw_op, &data, laprop.raw());
}

herr_t Group::link_visit(link_iter_op_t op, void *op_data,
    index_t idx_type, iter_order_t order) const
{
    using namespace _group_link_helper;
    iter_data_t data {std::move(op), op_data};
    return _Link::visit(raw(), idx_type, order, raw_op, &data);
}

herr_t Group::link_visit(const string &group_name, link_iter_op_t op, 
    void *op_data, index_t idx_type, iter_order_t order,
    const Proplist &laprop) const
{
    using namespace _group_link_helper;
    iter_data_t data {std::move(op), op_data};
    return _Link::visit_by_name(raw(), group_name.c_str(), idx_type, order, 
        raw_op, &data, laprop.raw());
}

herr_t Group::object_visit(obj_iter_op_t op, void *op_data,
    info_field_t fields, index_t idx_type, iter_order_t order) const
{
    using namespace _group_obj_helper;
    iter_data_t data {std::move(op), op_data};
    return obj_raw()._NamedObj::visit(idx_type, order, raw_op, 
        &data, fields);
}

herr_t Group::object_visit(const string &group_name,
    obj_iter_op_t op, void *op_data, info_field_t fields,
    index_t idx_type, iter_order_t order,const Proplist &laprop) const
{
    using namespace _group_obj_helper;
    iter_data_t data {std::move(op), op_data};
    return obj_raw()._NamedObj::visit_by_name(group_name.c_str(), 
        idx_type, order, raw_op, &data, fields, laprop.raw());
}

Group Group::create_group(const string &name, const string &flag,
    const Proplist &lcprop, const Proplist &gcprop,
    const Proplist &gaprop)
{
    bool is_ac_mode {};
    if ( flag == "ac" || flag == "ca" || flag == "w" || flag == "trunc" ) 
        is_ac_mode = true;
    else if ( flag == "x" || flag == "excl" ) 
        is_ac_mode = false;
    else
        ErrLogic::throw_(ErrLogic::eDOMAIN, emFLPFB, 
            "  ... invalid group creation flag ", flag, '\n');    

    if( group_exists(name) ) {
        if( is_ac_mode ) 
            return open_group(name, gaprop);
        else {
            ErrH5::throw_(-1, emFLPFB, "  ... group ", name, " exists\n");
            return Group {nullptr};
        }
    } else {
        auto ret = _obj_raw_t::create(raw(), name.c_str(), 
            lcprop.raw(), gcprop.raw(), gaprop.raw());
        return _from_raw(ret);
    }   
}

Group Group::open_group(const string &name, 
    const Proplist &aprop) const
{
    auto ret = _obj_raw_t::open(raw(), name.c_str(), aprop.raw());
    return _from_raw(ret);
}

DatasetManager Group::datasets() noexcept {
    return DatasetManager {*this};
}

Dataset Group::create_dataset(const string &name, const Datatype &dtype, 
    const Dataspace &dspace, const string &flag, const Proplist &lcprop, 
    const Proplist &dcprop, const Proplist &daprop)
{   
    if( dataset_exists(name) ) {
        if( flag == "ac" || flag == "ca" || flag == "w" || flag == "trunc" )
            return open_dataset(name, daprop);
        else if ( flag == "x" || flag == "excl" )
            ErrH5::throw_(-1, emFLPFB, "  ... dataset ", name, " exists\n");
        else 
            ErrLogic::throw_(ErrLogic::eDOMAIN, emFLPFB, 
            "  ... invalid dataset creation flag ", flag, '\n');
    }
    auto ret = _Dataset::create(raw(), name.c_str(), dtype.raw(), dspace.raw(),
        lcprop.raw(), dcprop.raw(), daprop.raw());
    return Dataset::_from_raw(ret);
}

Dataset Group::create_dataset_str(const string &name, size_t n,
const string &flag) {
    return create_dataset(name, C_S1_T.resized(n), Dataspace::vSCALAR, flag);
}

Dataset Group::create_dataset_str(const string &name, 
    size_t n_str, size_t n,
    const string &flag)
{
    return create_dataset(name, C_S1_T.resized(n), {(hsize_t)n_str}, flag);
}

Dataset Group::create_dataset_for_str(const string &name, const string &s,
    const string &flag)
{
    auto [buff, dsp, dt] = ConstDatapacketStr {s};
    return create_dataset(name, dt, dsp, flag);
}

Dataset Group::create_dataset_for_str(const string &name, const char *s,
    const string &flag)
{
    auto [buff, dsp, dt] = ConstDatapacketStr {s};
    return create_dataset(name, dt, dsp, flag);
}

Dataset Group::create_dataset_for_str(const string &name, 
    const vector<string> &ss, const string &flag)
{
    auto [buff, dsp, dt] = ConstDatapacketStr {ss};
    return create_dataset(name, dt, dsp, flag);
}

Dataset Group::open_dataset(const string &name, const Proplist &aprop) const
{
    auto dset_id = _Dataset::open(raw(), name.c_str(), aprop.raw());
    return Dataset::_from_raw(dset_id);
}

DatasetManager::DatasetManager() noexcept : _obj() {}

DatasetManager::DatasetManager(refobj_t obj) noexcept
: _obj ( std::move(obj) )
{}

auto DatasetManager::refobj() const -> refobj_t { return _obj; }

void DatasetManager::reset(refobj_t obj) noexcept { _obj = obj; }

void DatasetManager::reset() noexcept { _obj.free(); }

bool DatasetManager::exists(const string &name) {
    return _obj.dataset_exists(name);
}

Dataset DatasetManager::open(const string &name) {
    return _obj.open_dataset(name);
}

} // namespace HIPP::IO::H5
