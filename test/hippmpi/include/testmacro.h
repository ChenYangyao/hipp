#ifndef HIPPMPI_TEST_TESTMACRO_H_
#define HIPPMPI_TEST_TESTMACRO_H_

#define HIPPMPI_TEST_ENTERF  pout << __FILE__, ".", __func__, endl;
#define HIPPMPI_TEST_PROGRESS pout <<  " - "
#define HIPPMPI_TEST_F_BEGIN(test_case) \
    void test_case() { \
        if( is_root ){ \
            HIPPMPI_TEST_ENTERF \
        }

#define HIPPMPI_TEST_F_END \
        comm.barrier(); \
        if( is_root ) HIPPMPI_TEST_PROGRESS, " exit call", endl; \
    }

#endif	//HIPPMPI_TEST_TESTMACRO_H_