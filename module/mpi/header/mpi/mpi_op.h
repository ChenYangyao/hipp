/**
 * creat: Yangyao CHEN, 2020/01/21
 *      [write   ] Op - the high-level interface class for MPI collective
 *                      computation operator.
 *          ...
 *      [modify  ]
 *      [debug   ]
 *      [comment ]
 */ 

#ifndef _HIPPMPI_MPI_OP_H_
#define _HIPPMPI_MPI_OP_H_
#include "mpi_raw_op.h"
#include "mpi_obj_base.h"
namespace HIPP{
namespace MPI{
    
/**
 * the high-level interface class for MPI collective computation operator.
 * 
 * Op is the counterpart of MPI_Op, which is used in collective computation,
 * e.g. MPI_Reduce(). This class provide the life-time anto-management of an
 * MPI_Op. 
 */
class Op: public MPIObj<_Op>{
public:
    typedef MPIObj<_Op> _obj_base_t;
    typedef _obj_raw_t::user_fn_t user_fn_t;

    using _obj_base_t::_obj_base_t;
    
    /**
     * constructor
     * Construct the Op instance by an user defined function.
     * @user_fn:    user-defined function.
     * @commute:    bool. Whether the operator is commutative.
     */
    Op( user_fn_t user_fn, int commute );

    /**
     * print some information about this instance into `os`. 
     * @fmt_cntl:   control the amount of information to be printed. 0 for a
     *              inline, short information, and 1 for a verbose version.
     */
    ostream & info( ostream &os = cout, int fmt_cntl=1 ) const;
    friend ostream & operator<<( ostream &os, const Op &op );

    /**
     * free the operator in advance, an set it to null value as returned by
     * nullval().
     */
    void free() noexcept;

    /**
     * test the properties of Op.
     * is_null() -  whether the instance of Op is a null value.
     * commutative() -
     *              whether the instance of Op is commutative.
     */
    bool is_null() const;
    bool commutative() const;

    /**
     * create a new operator
     * nullval() -  returns a null value.
     * create()  -  create a user-defined operator. The argument is the same as 
     *              the constructor of Op.
     */
    static Op nullval() noexcept;
    static Op create( user_fn_t user_fn, int commute );
protected:
    static Op _from_raw( mpi_t op, int state ) noexcept;
};

inline ostream & operator<<( ostream &os, const Op &op ){
    return op.info(os);
}
inline Op Op::_from_raw( mpi_t op, int state ) noexcept{
    auto ptr = std::make_shared<_obj_raw_t>( op, state );
    return Op( ptr );
}

#define _HIPPMPI_MPI_PREOP(name) \
    extern Op name;

_HIPPMPI_MPI_PREOP(MAX)
_HIPPMPI_MPI_PREOP(MIN)
_HIPPMPI_MPI_PREOP(SUM)
_HIPPMPI_MPI_PREOP(PROD)
_HIPPMPI_MPI_PREOP(LAND)
_HIPPMPI_MPI_PREOP(BAND)
_HIPPMPI_MPI_PREOP(LOR)
_HIPPMPI_MPI_PREOP(BOR)
_HIPPMPI_MPI_PREOP(LXOR)
_HIPPMPI_MPI_PREOP(BXOR)
_HIPPMPI_MPI_PREOP(MAXLOC)
_HIPPMPI_MPI_PREOP(MINLOC)
_HIPPMPI_MPI_PREOP(REPLACE)

#undef _HIPPMPI_MPI_PREOP

extern std::unordered_map<string, const Op *> _opcvt;

class Comm;
class Win;
class Oppacket{
public:
    Oppacket( const Op &op ): _op(op){ }
    Oppacket( const string &op );
    Oppacket( const char *op ): Oppacket( string(op) ){ }

    Oppacket(const Oppacket &opp) noexcept: Oppacket( opp._op ){ }
    Oppacket(Oppacket &&opp) noexcept: _op( std::move(opp._op) ){ }
    Oppacket & operator=(const Oppacket &opp) noexcept
        { if( this != &opp ) _op = opp._op; return *this; }
    Oppacket & operator=(Oppacket && opp) noexcept
        { if( this != &opp ) _op = std::move( opp._op ); return *this; }
    ~Oppacket() noexcept {}
protected:
    Op _op;
    friend class Comm;
    friend class Win;
};

} // namespace MPI
} // namespace HIPP

#endif	//_HIPPMPI_MPI_OP_H_