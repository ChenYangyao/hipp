#ifndef _HIPPALGORITHM_KD_OBJ_BASE_H_
#define _HIPPALGORITHM_KD_OBJ_BASE_H_
#include <hippcntl.h>
namespace HIPP{
namespace ALGORITHM{

template<typename ObjRaw>
class KDObj{
public:
    typedef ObjRaw _obj_raw_t;
    typedef std::shared_ptr<_obj_raw_t> _obj_ptr_t;

    explicit KDObj( _obj_ptr_t ptr ) noexcept: _obj_ptr(ptr){}
    explicit KDObj( _obj_raw_t *ptr ) noexcept: _obj_ptr(ptr){}
    ~KDObj()noexcept{}

    KDObj( const KDObj &obj ) noexcept
        :_obj_ptr( obj._obj_ptr ){ }
    KDObj( KDObj &&obj ) noexcept
        :_obj_ptr( obj._obj_ptr ){ obj._obj_ptr.reset(); }
    KDObj & operator=( const KDObj &obj ) noexcept{
        _obj_ptr = obj._obj_ptr; return *this;
    }
    KDObj & operator=( KDObj &&obj ) noexcept{
        _obj_ptr = obj._obj_ptr; obj._obj_ptr.reset();
        return *this;
    }
protected:
    _obj_ptr_t _obj_ptr;
};

} // namespace ALGORITHM
} // namespace HIPP
#endif	//_HIPPALGORITHM_KD_OBJ_BASE_H_