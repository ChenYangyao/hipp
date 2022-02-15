#include <mpi_test_incl.h>
#include <mpi_test_data_buffer.h>

namespace HIPP::MPI {

template<typename ScalarT>
struct CollectiveTest : MPITestFixture {
    using data_buffer_t = MPITestDataBuffer<ScalarT>;
    using scalar_t = typename data_buffer_t::scalar_t;

    CollectiveTest(Comm _comm, string scalar_name)
    : MPITestFixture("CollectiveTest_" + scalar_name, _comm) {
        HIPPMPI_TEST_F_ADD_CASE(barrier);
        HIPPMPI_TEST_F_ADD_CASE(bcast);
        HIPPMPI_TEST_F_ADD_CASE(gather);
        HIPPMPI_TEST_F_ADD_CASE(gatherv);
        HIPPMPI_TEST_F_ADD_CASE(scatter);
        HIPPMPI_TEST_F_ADD_CASE(scatterv);
        HIPPMPI_TEST_F_ADD_CASE(allgather);
        HIPPMPI_TEST_F_ADD_CASE(allgatherv);
    }
    void barrier() {
        for(int i=0; i<10; ++i){
            comm.barrier();
        }

        auto sub_comm = comm.split(rank == 0);
        if( rank == 0 ) {
            for(int i=1; i<n_procs; ++i) {
                int recv_buf;
                comm.recv(ANY_SOURCE, 0, recv_buf);
                expect_eq(recv_buf, i);
            }
        } else {
            for(int i=1; i<n_procs; ++i){
                if( rank == i )
                    comm.ssend(0, 0, i);
                sub_comm.barrier();
            }
        }
    }
    void bcast() {
        data_buffer_t b_in(*this, false), b(*this);
        
        if( is_root ) b_in.init_values();
        else b_in.zero_values();

        comm.bcast(b_in.x_s, root_rank);
        comm.bcast(b_in.x_rawarr3, root_rank);
        comm.bcast(b_in.x_rawarr34, root_rank);
        comm.bcast(b_in.x_arr3, root_rank);
        comm.bcast(b_in.x_arr34, root_rank);
        comm.bcast(b_in.x_v8, root_rank);
        comm.bcast(b_in.x_v83, root_rank);
        comm.bcast(b_in.x_v834, root_rank);
        comm.bcast({b_in.x_p8, 8}, root_rank);
        comm.bcast({b_in.x_p8, 8, Datatype::from_type<scalar_t>()}, root_rank);
        
        b.expect_eq(b_in);
    }
    
    void gather() {
        vector<scalar_t> ibuf(96*n_procs);
        data_buffer_t b(*this);

        comm.gather(b.x_s, ibuf, root_rank);
        if(is_root) {
            int each_sz = 1;
            for(int i=0; i<n_procs; ++i){
                expect_eq_buff(&b.x_s, &ibuf[i*each_sz], each_sz, "chunk ", i);
            }
        }
        comm.gather(b.x_rawarr3, ibuf, root_rank);
        if(is_root) {
            int each_sz = 3;
            for(int i=0; i<n_procs; ++i){
                expect_eq_buff(&b.x_rawarr3[0], 
                    &ibuf[i*each_sz], each_sz, "chunk ", i);
            }
        }
        comm.gather(b.x_rawarr34, ibuf, root_rank);
        if(is_root) {
            int each_sz = 12;
            for(int i=0; i<n_procs; ++i){
                expect_eq_buff(&b.x_rawarr34[0][0], 
                    &ibuf[i*each_sz], each_sz, "chunk ", i);
            }
        }
        comm.gather(b.x_arr3, ibuf, root_rank);
        if(is_root) {
            int each_sz = 3;
            for(int i=0; i<n_procs; ++i){
                expect_eq_buff(&b.x_arr3[0], 
                    &ibuf[i*each_sz], each_sz, "chunk ", i);
            }
        }
        comm.gather(b.x_arr34, ibuf, root_rank);
        if(is_root) {
            int each_sz = 12;
            for(int i=0; i<n_procs; ++i){
                expect_eq_buff(&b.x_arr34[0][0], 
                    &ibuf[i*each_sz], each_sz, "chunk ", i);
            }
        }
        comm.gather(b.x_v8, ibuf, root_rank);
        if(is_root) {
            int each_sz = 8;
            for(int i=0; i<n_procs; ++i){
                expect_eq_buff(&b.x_v8[0], 
                    &ibuf[i*each_sz], each_sz, "chunk ", i);
            }
        }
        comm.gather(b.x_v83, ibuf, root_rank);
        if(is_root) {
            int each_sz = 24;
            for(int i=0; i<n_procs; ++i){
                expect_eq_buff(&b.x_v83[0][0], 
                    &ibuf[i*each_sz], each_sz, "chunk ", i);
            }
        }
        comm.gather(b.x_v834, ibuf, root_rank);
        if(is_root) {
            int each_sz = 96;
            for(int i=0; i<n_procs; ++i){
                expect_eq_buff(&b.x_v834[0][0][0], 
                    &ibuf[i*each_sz], each_sz, "chunk ", i);
            }
        }
        comm.gather({b.x_p8, 8}, ibuf, root_rank);
        if(is_root) {
            int each_sz = 8;
            for(int i=0; i<n_procs; ++i){
                expect_eq_buff(b.x_p8, 
                    &ibuf[i*each_sz], each_sz, "chunk ", i);
            }
        }
        comm.gather({b.x_p8, 8, Datatype::from_type<scalar_t>()}, 
            ibuf, root_rank);
        if(is_root) {
            int each_sz = 8;
            for(int i=0; i<n_procs; ++i){
                expect_eq_buff(b.x_p8, 
                    &ibuf[i*each_sz], each_sz, "chunk ", i);
            }
        }
    }
    void gatherv() {
        vector<scalar_t> ibuf(96*n_procs);
        data_buffer_t b(*this);

        vector<int> counts(n_procs), displs(n_procs);
        int each_sz;

        each_sz = 1;
        counts.assign(n_procs, each_sz);
        for(int i=0; i<n_procs; ++i) displs[i] = i * each_sz;
        comm.gatherv(b.x_s, ibuf, counts, displs, root_rank);
        if(is_root) {
            for(int i=0; i<n_procs; ++i){
                expect_eq_buff(&b.x_s, &ibuf[i*each_sz], each_sz, "chunk ", i);
            }
        }
        each_sz = 3;
        counts.assign(n_procs, each_sz);
        for(int i=0; i<n_procs; ++i) displs[i] = i * each_sz;
        comm.gatherv(b.x_rawarr3, ibuf, counts, displs, root_rank);
        if(is_root) {
            for(int i=0; i<n_procs; ++i){
                expect_eq_buff(&b.x_rawarr3[0], 
                    &ibuf[i*each_sz], each_sz, "chunk ", i);
            }
        }
        each_sz = 12;
        counts.assign(n_procs, each_sz);
        for(int i=0; i<n_procs; ++i) displs[i] = i * each_sz;
        comm.gatherv(b.x_rawarr34, ibuf, counts, displs, root_rank);
        if(is_root) {
            for(int i=0; i<n_procs; ++i){
                expect_eq_buff(&b.x_rawarr34[0][0], 
                    &ibuf[i*each_sz], each_sz, "chunk ", i);
            }
        }
        each_sz = 3;
        counts.assign(n_procs, each_sz);
        for(int i=0; i<n_procs; ++i) displs[i] = i * each_sz;
        comm.gatherv(b.x_arr3, ibuf, counts, displs, root_rank);
        if(is_root) {
            for(int i=0; i<n_procs; ++i){
                expect_eq_buff(&b.x_arr3[0], 
                    &ibuf[i*each_sz], each_sz, "chunk ", i);
            }
        }
        each_sz = 12;
        counts.assign(n_procs, each_sz);
        for(int i=0; i<n_procs; ++i) displs[i] = i * each_sz;
        comm.gatherv(b.x_arr34, ibuf, counts, displs, root_rank);
        if(is_root) {
            for(int i=0; i<n_procs; ++i){
                expect_eq_buff(&b.x_arr34[0][0], 
                    &ibuf[i*each_sz], each_sz, "chunk ", i);
            }
        }
        each_sz = 8;
        counts.assign(n_procs, each_sz);
        for(int i=0; i<n_procs; ++i) displs[i] = i * each_sz;
        comm.gatherv(b.x_v8, ibuf, counts, displs, root_rank);
        if(is_root) {
            for(int i=0; i<n_procs; ++i){
                expect_eq_buff(&b.x_v8[0], 
                    &ibuf[i*each_sz], each_sz, "chunk ", i);
            }
        }
        each_sz = 24;
        counts.assign(n_procs, each_sz);
        for(int i=0; i<n_procs; ++i) displs[i] = i * each_sz;
        comm.gatherv(b.x_v83, ibuf, counts, displs, root_rank);
        if(is_root) {
            for(int i=0; i<n_procs; ++i){
                expect_eq_buff(&b.x_v83[0][0], 
                    &ibuf[i*each_sz], each_sz, "chunk ", i);
            }
        }
        each_sz = 96;
        counts.assign(n_procs, each_sz);
        for(int i=0; i<n_procs; ++i) displs[i] = i * each_sz;
        comm.gatherv(b.x_v834, ibuf, counts, displs, root_rank);
        if(is_root) {
            for(int i=0; i<n_procs; ++i){
                expect_eq_buff(&b.x_v834[0][0][0], 
                    &ibuf[i*each_sz], each_sz, "chunk ", i);
            }
        }
        each_sz = 8;
        counts.assign(n_procs, each_sz);
        for(int i=0; i<n_procs; ++i) displs[i] = i * each_sz;
        comm.gatherv({b.x_p8, 8}, ibuf, counts, displs, root_rank);
        if(is_root) {
            for(int i=0; i<n_procs; ++i){
                expect_eq_buff(b.x_p8, 
                    &ibuf[i*each_sz], each_sz, "chunk ", i);
            }
        }
        each_sz = 8;
        counts.assign(n_procs, each_sz);
        for(int i=0; i<n_procs; ++i) displs[i] = i * each_sz;
        comm.gatherv({b.x_p8, 8, Datatype::from_type<scalar_t>()}, 
            ibuf, counts, displs, root_rank);
        if(is_root) {
            for(int i=0; i<n_procs; ++i){
                expect_eq_buff(b.x_p8, 
                    &ibuf[i*each_sz], each_sz, "chunk ", i);
            }
        }
    }

    void scatter() {
        vector<scalar_t> ibuf(96*n_procs);
        data_buffer_t b(*this), b_in(*this, false);
        int each_sz;

        if( is_root ) {
            each_sz = 1;
            for(int i=0; i<n_procs; ++i)
                std::copy_n(&b.x_s, each_sz, &ibuf[i*each_sz]);
        }
        comm.scatter(ibuf, b_in.x_s, root_rank);

        if( is_root ) {
            each_sz = 3;
            for(int i=0; i<n_procs; ++i)
                std::copy_n(&b.x_rawarr3[0], each_sz, &ibuf[i*each_sz]);
        }
        comm.scatter(ibuf, b_in.x_rawarr3, root_rank);

        if( is_root ) {
            each_sz = 12;
            for(int i=0; i<n_procs; ++i)
                std::copy_n(&b.x_rawarr34[0][0], each_sz, &ibuf[i*each_sz]);
        }
        comm.scatter(ibuf, b_in.x_rawarr34, root_rank);

        if( is_root ) {
            each_sz = 3;
            for(int i=0; i<n_procs; ++i)
                std::copy_n(&b.x_arr3[0], each_sz, &ibuf[i*each_sz]);
        }
        comm.scatter(ibuf, b_in.x_arr3, root_rank);
        
        if( is_root ) {
            each_sz = 12;
            for(int i=0; i<n_procs; ++i)
                std::copy_n(&b.x_arr34[0][0], each_sz, &ibuf[i*each_sz]);
        }
        comm.scatter(ibuf, b_in.x_arr34, root_rank);
        
        if( is_root ) {
            each_sz = 8;
            for(int i=0; i<n_procs; ++i)
                std::copy_n(&b.x_v8[0], each_sz, &ibuf[i*each_sz]);
        }
        comm.scatter(ibuf, b_in.x_v8, root_rank);

        if( is_root ) {
            each_sz = 24;
            for(int i=0; i<n_procs; ++i)
                std::copy_n(&b.x_v83[0][0], each_sz, &ibuf[i*each_sz]);
        }
        comm.scatter(ibuf, b_in.x_v83, root_rank);

        if( is_root ) {
            each_sz = 96;
            for(int i=0; i<n_procs; ++i)
                std::copy_n(&b.x_v834[0][0][0], each_sz, &ibuf[i*each_sz]);
        }
        comm.scatter(ibuf, b_in.x_v834, root_rank);

        if( is_root ) {
            each_sz = 8;
            for(int i=0; i<n_procs; ++i)
                std::copy_n(b.x_p8, each_sz, &ibuf[i*each_sz]);
        }
        comm.scatter(ibuf, {b_in.x_p8, 8}, root_rank);

        if( is_root ) {
            each_sz = 8;
            for(int i=0; i<n_procs; ++i)
                std::copy_n(b.x_p8, each_sz, &ibuf[i*each_sz]);
        }
        comm.scatter(ibuf, {b_in.x_p8, 8, Datatype::from_type<scalar_t>()}, 
            root_rank);

        b.assert_eq(b_in);
    }
    void scatterv() {
        vector<scalar_t> ibuf(96*n_procs);
        data_buffer_t b(*this), b_in(*this, false);
        int each_sz;
        vector<int> counts(n_procs), displs(n_procs);

        if( is_root ) {
            each_sz = 1;
            for(int i=0; i<n_procs; ++i) {
                std::copy_n(&b.x_s, each_sz, &ibuf[i*each_sz]);
                displs[i] = i*each_sz;
            }
            counts.assign(n_procs, each_sz);
                
        }
        comm.scatterv(ibuf, counts, displs, b_in.x_s, root_rank);

        if( is_root ) {
            each_sz = 3;
            for(int i=0; i<n_procs; ++i) {
                std::copy_n(&b.x_rawarr3[0], each_sz, &ibuf[i*each_sz]);
                displs[i] = i*each_sz;
            }
            counts.assign(n_procs, each_sz);
        }
        comm.scatterv(ibuf, counts, displs, b_in.x_rawarr3, root_rank);

        if( is_root ) {
            each_sz = 12;
            for(int i=0; i<n_procs; ++i) {
                std::copy_n(&b.x_rawarr34[0][0], each_sz, &ibuf[i*each_sz]);
                displs[i] = i*each_sz;
            }
            counts.assign(n_procs, each_sz);
        }
        comm.scatterv(ibuf, counts, displs, b_in.x_rawarr34, root_rank);

        if( is_root ) {
            each_sz = 3;
            for(int i=0; i<n_procs; ++i) {
                std::copy_n(&b.x_arr3[0], each_sz, &ibuf[i*each_sz]);
                displs[i] = i*each_sz;
            }
            counts.assign(n_procs, each_sz);
        }
        comm.scatterv(ibuf, counts, displs, b_in.x_arr3, root_rank);
        
        if( is_root ) {
            each_sz = 12;
            for(int i=0; i<n_procs; ++i) {
                std::copy_n(&b.x_arr34[0][0], each_sz, &ibuf[i*each_sz]);
                displs[i] = i*each_sz;
            }
            counts.assign(n_procs, each_sz);
        }
        comm.scatterv(ibuf, counts, displs, b_in.x_arr34, root_rank);
        
        if( is_root ) {
            each_sz = 8;
            for(int i=0; i<n_procs; ++i) {
                std::copy_n(&b.x_v8[0], each_sz, &ibuf[i*each_sz]);
                displs[i] = i*each_sz;
            }
            counts.assign(n_procs, each_sz);
        }
        comm.scatterv(ibuf, counts, displs, b_in.x_v8, root_rank);

        if( is_root ) {
            each_sz = 24;
            for(int i=0; i<n_procs; ++i) {
                std::copy_n(&b.x_v83[0][0], each_sz, &ibuf[i*each_sz]);
                displs[i] = i*each_sz;
            }
            counts.assign(n_procs, each_sz);
        }
        comm.scatterv(ibuf, counts, displs, b_in.x_v83, root_rank);

        if( is_root ) {
            each_sz = 96;
            for(int i=0; i<n_procs; ++i) {
                std::copy_n(&b.x_v834[0][0][0], each_sz, &ibuf[i*each_sz]);
                displs[i] = i*each_sz;
            }
            counts.assign(n_procs, each_sz);
        }
        comm.scatterv(ibuf, counts, displs, b_in.x_v834, root_rank);

        if( is_root ) {
            each_sz = 8;
            for(int i=0; i<n_procs; ++i) {
                std::copy_n(b.x_p8, each_sz, &ibuf[i*each_sz]);
                displs[i] = i*each_sz;
            }
            counts.assign(n_procs, each_sz);
        }
        comm.scatterv(ibuf, counts, displs, {b_in.x_p8, 8}, root_rank);

        if( is_root ) {
            each_sz = 8;
            for(int i=0; i<n_procs; ++i) {
                std::copy_n(b.x_p8, each_sz, &ibuf[i*each_sz]);
                displs[i] = i*each_sz;
            }
            counts.assign(n_procs, each_sz);
        }
        comm.scatterv(ibuf, counts, displs, 
            {b_in.x_p8, 8, Datatype::from_type<scalar_t>()}, root_rank);

        b.assert_eq(b_in);
    }
    
    void allgather() {
        vector<scalar_t> ibuf(96*n_procs);
        data_buffer_t b(*this);
        int each_sz;

        comm.allgather(b.x_s, ibuf);
        each_sz = 1;
        for(int i=0; i<n_procs; ++i){
            expect_eq_buff(&b.x_s, 
                &ibuf[i*each_sz], each_sz, "chunk ", i);
        }
        
        comm.allgather(b.x_rawarr3, ibuf);
        each_sz = 3;
        for(int i=0; i<n_procs; ++i){
            expect_eq_buff(&b.x_rawarr3[0], 
                &ibuf[i*each_sz], each_sz, "chunk ", i);
        }
        
        comm.allgather(b.x_rawarr34, ibuf);
        each_sz = 12;
        for(int i=0; i<n_procs; ++i){
            expect_eq_buff(&b.x_rawarr34[0][0], 
                &ibuf[i*each_sz], each_sz, "chunk ", i);
        }
        
        comm.allgather(b.x_arr3, ibuf);
        each_sz = 3;
        for(int i=0; i<n_procs; ++i){
            expect_eq_buff(&b.x_arr3[0], 
                &ibuf[i*each_sz], each_sz, "chunk ", i);
        }
        
        comm.allgather(b.x_arr34, ibuf);
        each_sz = 12;
        for(int i=0; i<n_procs; ++i){
            expect_eq_buff(&b.x_arr34[0][0], 
                &ibuf[i*each_sz], each_sz, "chunk ", i);
        }
        
        comm.allgather(b.x_v8, ibuf);
        each_sz = 8;
        for(int i=0; i<n_procs; ++i){
            expect_eq_buff(&b.x_v8[0], 
                &ibuf[i*each_sz], each_sz, "chunk ", i);
        }
        
        comm.allgather(b.x_v83, ibuf);
        each_sz = 24;
        for(int i=0; i<n_procs; ++i){
            expect_eq_buff(&b.x_v83[0][0], 
                &ibuf[i*each_sz], each_sz, "chunk ", i);
        }
        
        comm.allgather(b.x_v834, ibuf);
        each_sz = 96;
        for(int i=0; i<n_procs; ++i){
            expect_eq_buff(&b.x_v834[0][0][0], 
                &ibuf[i*each_sz], each_sz, "chunk ", i);
        }
        
        comm.allgather({b.x_p8, 8}, ibuf);
        each_sz = 8;
        for(int i=0; i<n_procs; ++i){
            expect_eq_buff(b.x_p8, 
                &ibuf[i*each_sz], each_sz, "chunk ", i);
        }
        
        comm.allgather({b.x_p8, 8, Datatype::from_type<scalar_t>()}, ibuf);
        each_sz = 8;
        for(int i=0; i<n_procs; ++i){
            expect_eq_buff(b.x_p8, 
                &ibuf[i*each_sz], each_sz, "chunk ", i);
        }
        
    }
    void allgatherv() {
        vector<scalar_t> ibuf(96*n_procs);
        data_buffer_t b(*this);

        vector<int> counts(n_procs), displs(n_procs);
        int each_sz;

        each_sz = 1;
        counts.assign(n_procs, each_sz);
        for(int i=0; i<n_procs; ++i) displs[i] = i * each_sz;
        comm.allgatherv(b.x_s, ibuf, counts, displs);
        for(int i=0; i<n_procs; ++i){
            expect_eq_buff(&b.x_s, 
                &ibuf[i*each_sz], each_sz, "chunk ", i);
        }
    
        each_sz = 3;
        counts.assign(n_procs, each_sz);
        for(int i=0; i<n_procs; ++i) displs[i] = i * each_sz;
        comm.allgatherv(b.x_rawarr3, ibuf, counts, displs);
        for(int i=0; i<n_procs; ++i){
            expect_eq_buff(&b.x_rawarr3[0], 
                &ibuf[i*each_sz], each_sz, "chunk ", i);
        }
    
        each_sz = 12;
        counts.assign(n_procs, each_sz);
        for(int i=0; i<n_procs; ++i) displs[i] = i * each_sz;
        comm.allgatherv(b.x_rawarr34, ibuf, counts, displs);
        for(int i=0; i<n_procs; ++i){
            expect_eq_buff(&b.x_rawarr34[0][0], 
                &ibuf[i*each_sz], each_sz, "chunk ", i);
        }
    
        each_sz = 3;
        counts.assign(n_procs, each_sz);
        for(int i=0; i<n_procs; ++i) displs[i] = i * each_sz;
        comm.allgatherv(b.x_arr3, ibuf, counts, displs);
        for(int i=0; i<n_procs; ++i){
            expect_eq_buff(&b.x_arr3[0], 
                &ibuf[i*each_sz], each_sz, "chunk ", i);
        }
    
        each_sz = 12;
        counts.assign(n_procs, each_sz);
        for(int i=0; i<n_procs; ++i) displs[i] = i * each_sz;
        comm.allgatherv(b.x_arr34, ibuf, counts, displs);
        for(int i=0; i<n_procs; ++i){
            expect_eq_buff(&b.x_arr34[0][0], 
                &ibuf[i*each_sz], each_sz, "chunk ", i);
        }
    
        each_sz = 8;
        counts.assign(n_procs, each_sz);
        for(int i=0; i<n_procs; ++i) displs[i] = i * each_sz;
        comm.allgatherv(b.x_v8, ibuf, counts, displs);
        for(int i=0; i<n_procs; ++i){
            expect_eq_buff(&b.x_v8[0], 
                &ibuf[i*each_sz], each_sz, "chunk ", i);
        }
    
        each_sz = 24;
        counts.assign(n_procs, each_sz);
        for(int i=0; i<n_procs; ++i) displs[i] = i * each_sz;
        comm.allgatherv(b.x_v83, ibuf, counts, displs);
        for(int i=0; i<n_procs; ++i){
            expect_eq_buff(&b.x_v83[0][0], 
                &ibuf[i*each_sz], each_sz, "chunk ", i);
        }
    
        each_sz = 96;
        counts.assign(n_procs, each_sz);
        for(int i=0; i<n_procs; ++i) displs[i] = i * each_sz;
        comm.allgatherv(b.x_v834, ibuf, counts, displs);
        for(int i=0; i<n_procs; ++i){
            expect_eq_buff(&b.x_v834[0][0][0], 
                &ibuf[i*each_sz], each_sz, "chunk ", i);
        }
    
        each_sz = 8;
        counts.assign(n_procs, each_sz);
        for(int i=0; i<n_procs; ++i) displs[i] = i * each_sz;
        comm.allgatherv({b.x_p8, 8}, ibuf, counts, displs);
        for(int i=0; i<n_procs; ++i){
            expect_eq_buff(b.x_p8, 
                &ibuf[i*each_sz], each_sz, "chunk ", i);
        }
    
        each_sz = 8;
        counts.assign(n_procs, each_sz);
        for(int i=0; i<n_procs; ++i) displs[i] = i * each_sz;
        comm.allgatherv({b.x_p8, 8, Datatype::from_type<scalar_t>()}, ibuf, counts, displs);
        for(int i=0; i<n_procs; ++i){
            expect_eq_buff(b.x_p8, 
                &ibuf[i*each_sz], each_sz, "chunk ", i);
        }
    
    }

};

} // namespace HIPP::MPI

int main(int argc, char const *argv[]){
    using namespace HIPP::MPI;
    
    Env env;
    auto comm = env.world();

    for(int i=0; i<5; ++i){
        CollectiveTest<float>{comm, "float"}();
        CollectiveTest<double>{comm, "double"}();
        CollectiveTest<short>{comm, "short"}();
        CollectiveTest<int>{comm, "int"}();
        CollectiveTest<long>{comm, "long"}();
        CollectiveTest<long long>{comm, "long long"}();
    }
    
    return 0;
}