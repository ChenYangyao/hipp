#include <hippnumerical.h>
#include <gmock/gmock.h>

namespace HIPP::NUMERICAL {

namespace {

namespace gt = ::testing;

class KDMeshTest: public ::testing::Test {
protected:
    using kdp_t = KDPoint<float, 3, sizeof(int)>;
    using kdm_t = KDMesh<kdp_t, int>;
    using index_t = kdm_t::index_t;
    using v_index_t = kdm_t::v_index_t;
    using kdm_impl_t = _KDSEARCH::_KDMesh<kdp_t, int>;
    
    using rng_t = UniformRealRandomNumber<>;
    struct cell_info_t {
        index_t idx;
        int bound;
        bool operator==(const cell_info_t &that) const {
            return idx == that.idx && bound == that.bound;
        }
        friend ostream & operator<< (ostream &os, const cell_info_t &c) {
            return os << c.idx << ": " << c.bound;
        }
    };

    KDMeshTest() : _eng(seed), _rng(0.0f, box_size, &_eng){}
    ~KDMeshTest() override {}
    void SetUp() override {
        int n1 = 100000, n2 = 200000;
        _kdpts1.resize(n1);
        _kdpts2.resize(n2);

        for(int i=0; i<n1; ++i){
            auto &p = _kdpts1[i];
            _rng(p.pos().begin(), p.pos().end());
            p.fill_pad(i);
        }
        for(int i=0; i<n2; ++i){
            auto &p = _kdpts2[i];
            _rng(p.pos().begin(), p.pos().end());
            p.fill_pad(i);
        }
    }
    void TearDown() override {}

    inline static const rng_t::seed_t seed = 0;
    inline static const float box_size = 100.0;

    rng_t::engine_t _eng;
    rng_t _rng;
    vector<kdp_t> _kdpts1, _kdpts2;
};


TEST_F(KDMeshTest, MeshConstructDefaultPolicy) {
    kdm_t kdm(_kdpts1);
    EXPECT_EQ(kdm.nodes().size(), _kdpts1.size());
    
    kdm.construct(_kdpts2);
    EXPECT_EQ(kdm.nodes().size(), _kdpts2.size());

    auto &mesh = kdm.mesh();
    auto &lo = mesh.low().pos(), &hi = mesh.high().pos();
    auto &pl = kdm.construct_policy();
    auto &n_cell = mesh.n_cell();

    for(int i=0; i<3; ++i){
        EXPECT_GT((hi-lo)[i], 0.);
        EXPECT_GT(n_cell[i], 0);
        EXPECT_EQ(n_cell[i], pl.n_cell()[i]);
    }

    for(auto &p: _kdpts2){
        bool is_bound = (p.pos() >= lo).all() && (p.pos() <= hi).all();
        EXPECT_TRUE(is_bound);

        auto v_idx = mesh.v_idx_of(p);
        EXPECT_TRUE(mesh.v_idx_is_bound(v_idx));
    }

    std::unordered_set<int> pads;
    for(auto &p: _kdpts2) 
        pads.emplace(p.pad<int>());
    EXPECT_EQ(pads.size(), _kdpts2.size());

    for(auto &n: kdm.nodes())
        EXPECT_EQ(pads.erase(n.pad<int>()), 1);
    EXPECT_EQ(pads.size(), 0);

    auto impl = kdm.impl();
    EXPECT_THAT(impl, gt::NotNull());
    EXPECT_THAT(*impl, gt::A<kdm_t::impl_t>());
    EXPECT_THAT(*impl, gt::A<kdm_impl_t>());
}

TEST_F(KDMeshTest, MeshConstructPolicy) {
    kdm_t::construct_policy_t pl;
    pl.set_bound(kdm_t::rect_t{{50.0f,50.0f,50.0f}, 
        {100.0f,100.0f,100.0f}});
    pl.set_n_cell(25);

    kdm_t kdm(_kdpts1, pl);
    EXPECT_LE(kdm.nodes().size(), _kdpts1.size());
    auto &mesh = kdm.mesh();
    for(int i=0; i<3; ++i){
        EXPECT_EQ(mesh.low().pos()[i], 
            pl.bound()->low().pos()[i]) << "Index " << i << endl;
        EXPECT_EQ(mesh.high().pos()[i], 
            pl.bound()->high().pos()[i]) << "Index " << i << endl;
    }
    EXPECT_TRUE((mesh.n_cell() == pl.n_cell()).all());
}

TEST_F(KDMeshTest, ArgSort) {
    kdm_t kdm(_kdpts1);

    vector<kdm_t::idx_pair_t> idx_prs;
    kdm.argsort<kdp_t>(_kdpts2, idx_prs);

    auto &r = kdm.mesh().rect();
    index_t cnt = 0;
    for(auto &p: _kdpts2){
        if( r.contains(p) ) ++cnt;
    }
    ASSERT_EQ(cnt, idx_prs.size());
    ASSERT_LE(cnt, _kdpts2.size());

    size_t n = idx_prs.size();
    for(size_t i=0; i<n-1; ++i){
        EXPECT_LE(idx_prs[i].idx_cell, idx_prs[i+1].idx_cell);
    }
    auto tot_n_cell = kdm.mesh().total_n_cell();
    for(size_t i=0; i<n; ++i){
        auto &idx_pr = idx_prs[i];
        EXPECT_GE(idx_pr.idx_cell, 0);
        EXPECT_LT(idx_pr.idx_cell, tot_n_cell);
        EXPECT_GE(idx_pr.idx_in, 0);
        EXPECT_LT(idx_pr.idx_in, _kdpts2.size());
    }
}

TEST_F(KDMeshTest, Clear) {
    kdm_t kdm(_kdpts1);
    kdm.clear();
    kdm.shrink_buffer();
    EXPECT_EQ(kdm.nodes().size(), 0);
    EXPECT_EQ(kdm.nodes().capacity(), 0);
    
    kdm.construct(_kdpts2);
    EXPECT_EQ(kdm.nodes().size(), _kdpts2.size());
    EXPECT_GE(kdm.nodes().capacity(), _kdpts2.size());
    kdm.shrink_buffer();
    EXPECT_EQ(kdm.nodes().capacity(), _kdpts2.size());
}

TEST_F(KDMeshTest, QueryNodesInCell) {
    kdm_t kdm(_kdpts1);
    auto &nodes = kdm.nodes();

    index_t tot_n_cell = kdm.mesh().total_n_cell(),
        cnt_nodes = 0;
    auto *pb = nodes.data(), *pe = pb + nodes.size();
    while(tot_n_cell-- > 0) {
        auto [p, n] = kdm.get_nodes_in_cell(tot_n_cell);
        EXPECT_GE(p, pb);
        EXPECT_LT(p, pe);
        cnt_nodes += n;
    }
    EXPECT_EQ(cnt_nodes, nodes.size());
}

TEST_F(KDMeshTest, VisitRectCompleteCheck) {
    kdm_t kdm(_kdpts1);
    kdp_t::point_t p = {50.0f, 50.0f, 50.0f};

    /* Visit cells. */
    for(auto off: {0.0f, 1.0f, 2.0f, 3.0f, 10.0f}){
        auto new_p = p + off;

        std::unordered_set<index_t> cell_ids;
        kdm.visit_cells_rect(
            {new_p-100.0f, new_p+100.0f}, 
            [&cell_ids](index_t i, int bound){ 
                cell_ids.emplace(i); 
                EXPECT_EQ(bound, 1);
            }
        );

        auto n = kdm.mesh().total_n_cell();
        EXPECT_EQ(cell_ids.size(), n);
        while(n-- > 0)
            ASSERT_EQ(cell_ids.erase(n), 1);
    }
    
    /* Visit nodes. */
    for(auto off: {0.0f, 1.0f, 2.0f, 3.0f, 10.0f}){
        auto new_p = p + off;

        std::unordered_set<index_t> pads;
        auto &mesh = kdm.mesh();
        kdm.visit_nodes_rect(
            {new_p-100.0f, new_p+100.0f}, 
            [&pads, &mesh](const kdm_t::node_t &n){ 
                pads.emplace(n.pad<int>()); 
            }
        );

        auto n = _kdpts1.size();
        EXPECT_EQ(pads.size(), n);
        while(n-- > 0)
            ASSERT_EQ(pads.erase(n), 1) << "Erase " << n;
    }

    /* Count nodes. */
    for(auto off: {0.0f, 1.0f, 2.0f, 3.0f, 10.0f}){
        auto new_p = p + off;
        EXPECT_EQ(kdm.count_nodes_rect({new_p-100.0f, new_p+100.0f}), 
            _kdpts1.size());
    }
}

TEST_F(KDMeshTest, VisitRectCells) {
    kdm_t::construct_policy_t pl;
    pl.set_n_cell(10);
    pl.set_bound({{0.0f, 0.0f, 0.0f}, {100.0f, 100.0f, 100.0f}});
    kdm_t kdm(_kdpts1, pl);
    auto &mesh = kdm.mesh();

    {
        kdp_t::point_t p {5., 15., 25.};        
        vector<cell_info_t> dst, src;
        dst.push_back({mesh.v_idx2idx({0, 1, 2}), 0});
        
        kdm.visit_cells_rect({p-2.5, p+2.5}, 
            [&src](index_t cell_idx, int bound){
                src.push_back({cell_idx, bound});
            });
        EXPECT_THAT(src, gt::UnorderedElementsAreArray(dst));
    }
    {
        kdp_t::point_t p {5., 15., 25.};        
        vector<cell_info_t> dst, src;
        for(int i=0; i<=1; ++i)
        for(int j=0; j<=2; ++j)
        for(int k=1; k<=3; ++k) {
            int bound = (i==0 && j==1 && k==2)?1:0;
            dst.push_back({mesh.v_idx2idx({i,j,k}), bound});
        }
        kdm.visit_cells_rect({p-5.5, p+5.5}, 
            [&src](index_t cell_idx, int bound){
                src.push_back({cell_idx, bound});
            });
        EXPECT_THAT(src, gt::UnorderedElementsAreArray(dst));
    }
    {
        kdp_t::point_t p {5., 15., 25.};
        vector<cell_info_t> dst, src;
        for(int i=0; i<=2; ++i)
        for(int j=0; j<=3; ++j)
        for(int k=0; k<=4; ++k) {
            int bound = (i<2 && j<3 && k>0 && k<4)?1:0;
            dst.push_back({mesh.v_idx2idx({i,j,k}), bound});
        }
        kdm.visit_cells_rect({p-15.5, p+15.5}, 
            [&src](index_t cell_idx, int bound){
                src.push_back({cell_idx, bound});
            });
        EXPECT_THAT(src, gt::UnorderedElementsAreArray(dst));
    }
    {
        kdp_t::point_t p {55., 65., 75.};
        vector<cell_info_t> dst, src;
        for(int i=3; i<=7; ++i)
        for(int j=4; j<=8; ++j)
        for(int k=5; k<=9; ++k) {
            v_index_t v_idx{i,j,k}, v_lo{3,4,5}, v_hi{7,8,9};
            int bound = (v_idx.value() > v_lo.value()).all()
                && (v_idx.value()<v_hi.value()).all();
            dst.push_back({mesh.v_idx2idx({i,j,k}), bound});
        }
        kdm.visit_cells_rect({p-15.5, p+15.5}, 
            [&src](index_t cell_idx, int bound){
                src.push_back({cell_idx, bound});
            });
        EXPECT_THAT(src, gt::UnorderedElementsAreArray(dst));
    }
    {
        kdp_t::point_t p {75., 85., 95.};
        vector<cell_info_t> dst, src;
        for(int i=6; i<=8; ++i)
        for(int j=7; j<=9; ++j)
        for(int k=8; k<=9; ++k) {
            int bound = (i==7 && j==8 && k==9)?1:0;
            dst.push_back({mesh.v_idx2idx({i,j,k}), bound});
        }
        kdm.visit_cells_rect({p-5.5, p+5.5}, 
            [&src](index_t cell_idx, int bound){
                src.push_back({cell_idx, bound});
            });
        EXPECT_THAT(src, gt::UnorderedElementsAreArray(dst));
    }
}

TEST_F(KDMeshTest, VisitRectNodes) {
    kdm_t kdm(_kdpts1);
    int n_dst = 1000;

    for(auto dx: {1.0f, 2.0f, 3.0f, 5.0f, 7.5f}){
        /* Check node count. */
        for(int i=0; i<n_dst; ++i){
            auto &p_dst = _kdpts2[i];
            kdm_t::rect_t r(p_dst-dx, p_dst+dx);
            index_t cnt1 = kdm.count_nodes_rect(r),
                cnt2 = std::count_if(_kdpts1.begin(), _kdpts1.end(), 
                    [&r](auto &p){ return r.contains(p); });
            EXPECT_EQ(cnt1, cnt2);
        }
        /* Check padding of each node. */
        for(int i=0; i<n_dst; ++i){
            auto &p_dst = _kdpts2[i];
            vector<index_t> pad_src, pad_dst;
            kdm_t::rect_t r(p_dst-dx, p_dst+dx);
            kdm.visit_nodes_rect(r, 
                [&pad_src](const kdm_t::node_t& n){ 
                    pad_src.push_back( n.pad<int>() ); });
            for(auto &p_src: _kdpts1)
                if( r.contains(p_src) ) 
                    pad_dst.push_back( p_src.pad<int>() );
            EXPECT_THAT(pad_src, 
                gt::UnorderedElementsAreArray(pad_dst));
        }
    }

}

TEST_F(KDMeshTest, VisitSphereCompleteCheck) {
    kdm_t kdm(_kdpts1);
    kdp_t::point_t p = {50.0f, 50.0f, 50.0f};

    /* Visit cells. */
    for(auto off: {0.0f, 1.0f, 2.0f, 3.0f, 10.0f}){
        auto new_p = p + off;

        std::unordered_set<index_t> cell_ids;
        kdm.visit_cells_sphere(
            {new_p, 500.0f}, 
            [&cell_ids](index_t i, int bound){ 
                cell_ids.emplace(i); 
                EXPECT_EQ(bound, 1);
            }
        );

        auto n = kdm.mesh().total_n_cell();
        EXPECT_EQ(cell_ids.size(), n);
        while(n-- > 0)
            ASSERT_EQ(cell_ids.erase(n), 1);
    }
    
    /* Visit nodes. */
    for(auto off: {0.0f, 1.0f, 2.0f, 3.0f, 10.0f}){
        auto new_p = p + off;

        std::unordered_set<index_t> pads;
        auto &mesh = kdm.mesh();
        kdm.visit_nodes_sphere(
            {new_p, 500.0f}, 
            [&pads, &mesh](const kdm_t::node_t &n){ 
                pads.emplace(n.pad<const int>()); 
            }
        );

        auto n = _kdpts1.size();
        EXPECT_EQ(pads.size(), n);
        while(n-- > 0)
            ASSERT_EQ(pads.erase(n), 1) << "Erase " << n;
    }

    /* Count nodes. */
    for(auto off: {0.0f, 1.0f, 2.0f, 3.0f, 10.0f}){
        auto new_p = p + off;
        EXPECT_EQ(kdm.count_nodes_sphere({new_p, 500.0f}), 
            _kdpts1.size());
    }
}

TEST_F(KDMeshTest, VisitSphereCells) {
    kdm_t::construct_policy_t pl;
    pl.set_n_cell(10)
        .set_bound({{0.0f, 0.0f, 0.0f}, {100.0f, 100.0f, 100.0f}});
    kdm_t kdm(_kdpts1, pl);
    auto &mesh = kdm.mesh();

    {
        kdp_t::point_t p {5., 15., 25.};        
        vector<cell_info_t> dst, src;
        dst.push_back({mesh.v_idx2idx({0, 1, 2}), 0});
        
        kdm.visit_cells_sphere({p, 2.5}, 
            [&src](index_t cell_idx, int bound){
                src.push_back({cell_idx, bound});
            });
        EXPECT_THAT(src, gt::UnorderedElementsAreArray(dst));
    }
    {
        kdp_t::point_t p {5., 15., 25.};        
        vector<cell_info_t> dst, src;
        dst.push_back({mesh.v_idx2idx({0,1,2}), 0});
        dst.push_back({mesh.v_idx2idx({0,1,1}), 0});
        dst.push_back({mesh.v_idx2idx({0,1,3}), 0});
        dst.push_back({mesh.v_idx2idx({0,0,2}), 0});
        dst.push_back({mesh.v_idx2idx({0,2,2}), 0});
        dst.push_back({mesh.v_idx2idx({1,1,2}), 0});
        
        kdm.visit_cells_sphere({p, 5.5}, 
            [&src](index_t cell_idx, int bound){
                src.push_back({cell_idx, bound});
            });
        EXPECT_THAT(src, gt::UnorderedElementsAreArray(dst));
    }
   
}

TEST_F(KDMeshTest, VisitSphereNodes) {
    kdm_t kdm(_kdpts1);
    int n_dst = 1000;

    for(auto r: {1.0f, 2.0f, 3.0f, 5.0f, 7.5f}){
        /* Check node count. */
        for(int i=0; i<n_dst; ++i){
            auto &p_dst = _kdpts2[i];
            kdm_t::sphere_t s(p_dst, r);
            index_t cnt1 = kdm.count_nodes_sphere(s),
                cnt2 = std::count_if(_kdpts1.begin(), _kdpts1.end(), 
                    [&s](auto &p){ return s.contains(p); });
            EXPECT_EQ(cnt1, cnt2);
        }

        /* Check padding of each node. */
        for(int i=0; i<n_dst; ++i){
            auto &p_dst = _kdpts2[i];
            vector<index_t> pad_src, pad_dst;
            kdm_t::sphere_t s(p_dst, r);
            kdm.visit_nodes_sphere(s, 
                [&pad_src](const kdm_t::node_t& n){ 
                    pad_src.push_back( n.pad<int>() ); });
            for(auto &p_src: _kdpts1)
                if( s.contains(p_src) ) 
                    pad_dst.push_back( p_src.pad<int>() );
            EXPECT_THAT(pad_src, 
                gt::UnorderedElementsAreArray(pad_dst));
        }
        
    }
}

} // namespace

} // namespace HIPP::NUMERICAL