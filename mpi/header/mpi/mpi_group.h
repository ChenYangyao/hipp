#ifndef _HIPPMPI_MPI_GROUP_H_
#define _HIPPMPI_MPI_GROUP_H_
#include "mpi_obj_base.h"
#include "mpi_raw_group.h"
namespace HIPP{
namespace MPI{
    
class Group: public MPIObj<_Group> {
public:
    typedef MPIObj<_Group> _obj_base_t;
    using _obj_base_t::_obj_base_t;
    ostream &info( ostream &os = cout, int fmt_cntl = 1 ) const;
    friend ostream & operator<<( ostream &os, const Group &group );
    void free() noexcept;
    int size() const;
    int rank() const;
    int is_null() const;
    vector<int> translate_ranks( 
        const vector<int> &ranks, const Group &othergroup )const;
    int compare( const Group &othergroup )const;
    Group union_( const Group &othergroup )const;
    Group intersection( const Group &othergroup )const;
    Group difference( const Group &othergroup )const;
    Group incl( const vector<int> &ranks )const;
    Group excl( const vector<int> &ranks )const;
    Group range_incl( const vector<int> &ranks )const;
    Group range_excl( const vector<int> &ranks )const;
    static Group emptyval() noexcept;
    static Group nullval() noexcept;
protected:
    static Group _from_raw( mpi_t obj, int state );
};

inline ostream & operator<<( ostream &os, const Group &group ){
    return group.info(os);
}
inline Group Group::_from_raw( mpi_t obj, int state ){
    return Group( std::make_shared<_obj_raw_t>(obj, state) );
}


} // namespace MPI
} // namespace HIPP

#endif	//_HIPPMPI_MPI_GROUP_H_