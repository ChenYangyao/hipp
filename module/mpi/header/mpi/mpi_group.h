/**
 * creat: Yangyao CHEN, 2020/01/21
 *      [write   ] Group - high-level MPI process group class interface.
 */ 

#ifndef _HIPPMPI_MPI_GROUP_H_
#define _HIPPMPI_MPI_GROUP_H_
#include "mpi_obj_base.h"
#include "mpi_raw_group.h"
namespace HIPP{
namespace MPI{
    
/**
 * high-level MPI process group class interface.
 * 
 * A group of processes define the process name-ranking, which is the basis of
 * point-to-point communication. Process group also define the involved 
 * processes of the collective communication.
 * 
 * The group instance should not be constructed directly, but should returned
 * by a communicator. After that you may apply the group transformation 
 * functions (union, intersection, difference ...) to create new groups based
 * on existing groups.
 * 
 * The life time of the group is manipulated by the instance, and you are not
 * necessary to manually control it. However, you may call free() to free the 
 * group instance in advance.
 */
class Group: public MPIObj<_Group> {
public:
    typedef MPIObj<_Group> _obj_base_t;
    using _obj_base_t::_obj_base_t;

    /**
     * print the information of the current instance to `os`.
     * @fmt_cntl:   control the amount of information to be printed, 0 for a 
     *              short and inline priting, 1 for a verbose version.
     * 
     * The operator<< is equivalent to info() with default `fmt_cntl`.
     */
    ostream &info( ostream &os = cout, int fmt_cntl = 1 ) const;
    friend ostream & operator<<( ostream &os, const Group &group );

    /**
     * free the group instance and set it to null value as returned by 
     * nullval().
     * 
     * The free() is not necessary, since the life time is controlled 
     * automatically, but you may want to release the resources in advance.
     * 
     * free() can be called at any time, or even multiple times, even when
     * the instance is a null value.
     */
    void free() noexcept;

    /**
     * query the information of the group.
     * size() - number of processes in this group.
     * rank() - rank of the current process in this group. If not in the group,
     *          return UNDEFINED.
     * is_null() -  
     *          test whether the group is a null instance. Note that an empty 
     *          group is different from a null instance, the former is  a valid 
     *          group instance, the later is a 'invalid' one that cannot be used 
     *          as an argument of many functions.
     * translate_ranks() - 
     *          provide the ranks of processes in the current group, query their
     *          ranks in `othergroup`.
     * compare() - 
     *          compare two groups. May return IDENT, SIMILAR or UNEQUAL. See
     *          the MPI official documentation for detail.
     */
    int size() const;
    int rank() const;
    int is_null() const;
    vector<int> translate_ranks( 
        const vector<int> &ranks, const Group &othergroup )const;
    int compare( const Group &othergroup )const;

    /**
     * group transformation and creation functions
     */
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