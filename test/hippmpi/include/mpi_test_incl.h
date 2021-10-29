#ifndef _HIPPMPI_MPI_TEST_INCL_H_
#define _HIPPMPI_MPI_TEST_INCL_H_

#include <hippmpi.h>

namespace HIPP::MPI {

#define HIPPMPI_TEST_F_ADD_CASE(name) \
    ( add_case(#name, [this]{ name(); }) )

class MPITestFixture {
public:
    typedef std::function<void()> test_case_t;

    string test_fixture_name;
    Comm comm;
    int rank, n_procs, root_rank;
    bool is_root;

    MPITestFixture(string name, Comm _comm, int _root_rank=0);
    ~MPITestFixture();
    void operator()();

    void add_case( string name, test_case_t test_case );

    template<typename ...Args>
    bool expect_true(bool x, Args &&...args);

    template<typename ...Args>
    void assert_true(bool x, Args &&...args);

    template<typename ...Args>
    bool expect_false(bool x, Args &&...args);
   
    template<typename ...Args>
    void assert_false(bool x, Args &&...args);

    template<typename T1, typename T2, typename ...Args>
    bool expect_eq(const T1 &x1, const T2 &x2, Args &&...args);

    template<typename T1, typename T2, typename ...Args>
    void assert_eq(const T1 &x1, const T2 &x2, Args &&...args);

    template<typename T1, typename T2, typename ...Args>
    bool expect_ne(const T1 &x1, const T2 &x2, Args &&...args);

    template<typename T1, typename T2, typename ...Args>
    void assert_ne(const T1 &x1, const T2 &x2, Args &&...args);

    template<typename T1, typename T2, typename ...Args>
    bool expect_ge(const T1 &x1, const T2 &x2, Args &&...args);

    template<typename T1, typename T2, typename ...Args>
    void assert_ge(const T1 &x1, const T2 &x2, Args &&...args);

    template<typename T1, typename T2, typename ...Args>
    bool expect_gt(const T1 &x1, const T2 &x2, Args &&...args);

    template<typename T1, typename T2, typename ...Args>
    void assert_gt(const T1 &x1, const T2 &x2, Args &&...args);

    template<typename T1, typename T2, typename ...Args>
    bool expect_le(const T1 &x1, const T2 &x2, Args &&...args);

    template<typename T1, typename T2, typename ...Args>
    void assert_le(const T1 &x1, const T2 &x2, Args &&...args);

    template<typename T1, typename T2, typename ...Args>
    bool expect_lt(const T1 &x1, const T2 &x2, Args &&...args);

    template<typename T1, typename T2, typename ...Args>
    void assert_lt(const T1 &x1, const T2 &x2, Args &&...args);

    template<typename I1, typename I2, typename ...Args>
    bool expect_index_bound(I1 i, I2 n, Args &&...args);

    template<typename I1, typename I2, typename ...Args>
    void assert_index_bound(I1 i, I2 n, Args &&...args);

    template<typename T1, typename T2, typename ...Args>
    bool expect_eq_range(const T1 &x1, const T2 &x2, Args &&...args);

    template<typename T1, typename T2, typename ...Args>
    void assert_eq_range(const T1 &x1, const T2 &x2, Args &&...args);

    template<typename T1, typename T2, typename ...Args>
    bool expect_eq_type(Args &&...args);

    template<typename T1, typename T2, typename ...Args>
    void assert_eq_type(Args &&...args);
private:
    struct test_case_info_t {
        string _name;
        test_case_t _case;
    };
    std::vector<test_case_info_t> _test_cases;
    
    Comm _priv_comm;
    std::size_t _n_error;
};

inline
MPITestFixture::MPITestFixture(string name, Comm _comm, int _root_rank) 
: test_fixture_name(name),
comm(std::move(_comm)), rank(comm.rank()), n_procs(comm.size()),
root_rank(_root_rank), is_root(rank == root_rank), _priv_comm(comm.dup()),
_n_error(0)
{        
    _priv_comm.barrier();
    if( is_root ){
        pout << "Fixture [", test_fixture_name, "] begins", endl;
    }
    _priv_comm.barrier();
}

inline
MPITestFixture::~MPITestFixture() {
    _priv_comm.barrier();
    long long n_error_each = _n_error, n_error_sum;
    _priv_comm.reduce(n_error_each, n_error_sum, SUM, root_rank);
    
    _priv_comm.barrier();
    if(is_root){
        if( n_error_sum != 0 ) {
            perr << "Fixture [", test_fixture_name, "] failed with ", 
                n_error_sum, " errors!", endl;  
            exit(EXIT_FAILURE);
        } 
        pout << "Fixture [", test_fixture_name, "] ends", endl;
    }
    _priv_comm.barrier();
}

inline
void MPITestFixture::operator()(){
    for(auto &info: _test_cases) {
        auto &[n, c] = info;
        _priv_comm.barrier();
        if( is_root ) {
            pout << "Fixture [", test_fixture_name, 
                "], Case [", n, "] begins", endl;
        }
        _priv_comm.barrier();
        
        try {
            c();
        } catch ( ErrMPI & e ) {
            perr << e.whats(), endl;
            perr << "Failed with a MPI exception at rank ",
                rank, endl;
            throw;
        } catch ( ErrApp & e ) {
            perr << e.whats(), endl;
            perr << 
                "Failed with an application specific exception at rank ",
                rank, endl;
            throw;
        } catch ( std::exception & e ) {
            perr << e.what(), endl;
            perr << "Failed with an exception at rank ",
                rank, endl;
            throw;
        } catch ( ... ) {
            perr << "Failed with an unknown exception at rank ",
                rank, endl;
            throw;
        }

        _priv_comm.barrier();
        if( is_root ) {
            pout << "Fixture [", test_fixture_name, 
                "], Case [", n, "] ends", endl;
        }
        _priv_comm.barrier();
    }
}

inline
void MPITestFixture::add_case( string name, test_case_t test_case ) {
    _test_cases.push_back({std::move(name), std::move(test_case)});
}

template<typename ...Args>
bool MPITestFixture::expect_true(bool x, Args &&...args) {
    bool suc = true;
    if( !x ) {
        ( (perr << "[ERROR]") , ... , std::forward<Args>(args) );
        perr << ": got false, expected true", endl;
        ++_n_error;
        suc = false;
    }
    return suc;
}

template<typename ...Args>
void MPITestFixture::assert_true(bool x, Args &&...args) {
    if( !expect_true(x, std::forward<Args>(args)...) )
        throw ErrLogic(ErrLogic::eDEFAULT);
}


template<typename ...Args>
bool MPITestFixture::expect_false(bool x, Args &&...args) {
    bool suc = true;
    if( x ) {
        ( (perr << "[ERROR]") , ... , std::forward<Args>(args) );
        perr << ": got true, expected false", endl;
        ++_n_error;
        suc = false;
    }
    return suc;
}

template<typename ...Args>
void MPITestFixture::assert_false(bool x, Args &&...args) {
    if( !expect_false(x, std::forward<Args>(args)...) )
        throw ErrLogic(ErrLogic::eDEFAULT);
}

template<typename T1, typename T2, typename ...Args>
bool MPITestFixture::expect_eq(const T1 &x1, const T2 &x2, Args &&...args) {
    bool suc = true;
    if( !(x1 == x2) ) {
        ( (perr << "[ERROR]") , ... , std::forward<Args>(args) );
        perr << ": ! x1 (", x1, ") == x2 (", x2, ") unexpected", endl; 
        ++_n_error;
        suc = false;
    }
    return suc;
}

template<typename T1, typename T2, typename ...Args>
void MPITestFixture::assert_eq(const T1 &x1, const T2 &x2, Args &&...args) {
    if( !expect_eq(x1,x2,std::forward<Args>(args)...) )
        throw ErrLogic(ErrLogic::eDEFAULT);
}

template<typename T1, typename T2, typename ...Args>
bool MPITestFixture::expect_ne(const T1 &x1, const T2 &x2, Args &&...args) {
    bool suc = true;
    if( !(x1 != x2) ) {
        ( (perr << "[ERROR]") , ... , std::forward<Args>(args) );
        perr << ": ! x1 (", x1, ") != x2 (", x2, ") unexpected", endl; 
        ++_n_error;
        suc = false;
    }
    return suc;
}

template<typename T1, typename T2, typename ...Args>
void MPITestFixture::assert_ne(const T1 &x1, const T2 &x2, Args &&...args) {
    if( !expect_ne(x1,x2,std::forward<Args>(args)...) )
        throw ErrLogic(ErrLogic::eDEFAULT);
}

template<typename T1, typename T2, typename ...Args>
bool MPITestFixture::expect_ge(const T1 &x1, const T2 &x2, Args &&...args) {
    bool suc = true;
    if( !(x1 >= x2) ) {
        ( (perr << "[ERROR]") , ... , std::forward<Args>(args) );
        perr << ": ! x1 (", x1, ") >= x2 (", x2, ") unexpected", endl; 
        ++_n_error;
        suc = false;
    }
    return suc;
}

template<typename T1, typename T2, typename ...Args>
void MPITestFixture::assert_ge(const T1 &x1, const T2 &x2, Args &&...args) {
    if( !expect_ge(x1,x2,std::forward<Args>(args)...) )
        throw ErrLogic(ErrLogic::eDEFAULT);
}

template<typename T1, typename T2, typename ...Args>
bool MPITestFixture::expect_gt(const T1 &x1, const T2 &x2, Args &&...args) {
    bool suc = true;
    if( !(x1 > x2) ) {
        ( (perr << "[ERROR]") , ... , std::forward<Args>(args) );
        perr << ": ! x1 (", x1, ") > x2 (", x2, ") unexpected", endl; 
        ++_n_error;
        suc = false;
    }
    return suc;
}

template<typename T1, typename T2, typename ...Args>
void MPITestFixture::assert_gt(const T1 &x1, const T2 &x2, Args &&...args) {
    if( !expect_gt(x1,x2,std::forward<Args>(args)...) )
        throw ErrLogic(ErrLogic::eDEFAULT);
}

template<typename T1, typename T2, typename ...Args>
bool MPITestFixture::expect_le(const T1 &x1, const T2 &x2, Args &&...args) {
    bool suc = true;
    if( !(x1 <= x2) ) {
        ( (perr << "[ERROR]") , ... , std::forward<Args>(args) );
        perr << ": ! x1 (", x1, ") <= x2 (", x2, ") unexpected", endl; 
        ++_n_error;
        suc = false;
    }
    return suc;
}

template<typename T1, typename T2, typename ...Args>
void MPITestFixture::assert_le(const T1 &x1, const T2 &x2, Args &&...args) {
    if( !expect_le(x1,x2,std::forward<Args>(args)...) )
        throw ErrLogic(ErrLogic::eDEFAULT);
}

template<typename T1, typename T2, typename ...Args>
bool MPITestFixture::expect_lt(const T1 &x1, const T2 &x2, Args &&...args) {
    bool suc = true;
    if( !(x1 < x2) ) {
        ( (perr << "[ERROR]") , ... , std::forward<Args>(args) );
        perr << ": ! x1 (", x1, ") < x2 (", x2, ") unexpected", endl; 
        ++_n_error;
        suc = false;
    }
    return suc;
}

template<typename T1, typename T2, typename ...Args>
void MPITestFixture::assert_lt(const T1 &x1, const T2 &x2, Args &&...args) {
    if( !expect_lt(x1,x2,std::forward<Args>(args)...) )
        throw ErrLogic(ErrLogic::eDEFAULT);
}

template<typename I1, typename I2, typename ...Args>
bool MPITestFixture::expect_index_bound(I1 i, I2 n, Args &&...args) {
    bool suc = true;
    if( i >= n || i < 0 ) {
        ( (perr << "[ERROR]") , ... , std::forward<Args>(args) );
        perr << ": index ", i, " beyond ", n, endl;
        ++ _n_error;
        suc = false;
    }
    return suc;
}

template<typename I1, typename I2, typename ...Args>
void MPITestFixture::assert_index_bound(I1 i, I2 n, Args &&...args) {
    if( !expect_index_bound(i, n, std::forward<Args>(args)...) )
        throw ErrLogic(ErrLogic::eDEFAULT);
}

template<typename T1, typename T2, typename ...Args>
bool MPITestFixture::expect_eq_range(const T1 &x1, const T2 &x2, 
    Args &&...args) 
{
    auto b1 = std::begin(x1), e1 = std::end(x1);
    auto b2 = std::begin(x2), e2 = std::end(x2);
    
    ptrdiff_t n1 = e1-b1, n2 = e2-b2;
    if( !expect_eq(n1, n2, 
        std::forward<Args>(args)..., ", range lengths check") ){
        ++ _n_error;
        return false;
    }

    bool suc = true;
    for(ptrdiff_t i=0; i<n1; ++i){
        suc = expect_eq(*b1, *b2, std::forward<Args>(args)..., 
            ", at index ", i);
        ++b1; ++b2;   
    }
    return suc;
}

template<typename T1, typename T2, typename ...Args>
void MPITestFixture::assert_eq_range(const T1 &x1, const T2 &x2, 
    Args &&...args) 
{
    auto b1 = std::begin(x1), e1 = std::end(x1);
    auto b2 = std::begin(x2), e2 = std::end(x2);
    
    ptrdiff_t n1 = e1-b1, n2 = e2-b2;
    assert_eq(n1, n2, std::forward<Args>(args)..., ", range lengths check");
    
    for(ptrdiff_t i=0; i<n1; ++i){
        assert_eq(*b1, *b2, std::forward<Args>(args)..., 
            ", at index ", i);
        ++b1; ++b2;   
    }
}

template<typename T1, typename T2, typename ...Args>
bool MPITestFixture::expect_eq_type(Args &&...args) {
    if( std::is_same_v<T1, T2> ) return true;
    ( (perr << "[ERROR]") , ... , std::forward<Args>(args) );
    perr << ": type T1 (", typeid(T1).name(), ") != type T2 (", 
        typeid(T2).name(), ")", endl;
    return false;
}

template<typename T1, typename T2, typename ...Args>
void MPITestFixture::assert_eq_type(Args &&...args) {
    if( !expect_eq_type<T1, T2>(std::forward<Args>(args)...) )
        throw ErrLogic(ErrLogic::eDEFAULT);
}





    
} // namespace HIPP::MPI


#endif	//_HIPPMPI_MPI_TEST_INCL_H_