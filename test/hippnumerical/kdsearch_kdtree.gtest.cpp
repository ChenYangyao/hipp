#include <hippnumerical.h>
#include <gmock/gmock.h>

namespace HIPP::NUMERICAL {

namespace {

namespace gt = ::testing;

class KDTreeTest: public ::testing::Test {
protected:
    using kdp_t = KDPoint<float, 3, sizeof(int)>;
    using kdtree_t = KDTree<kdp_t, int>;
    using index_t = kdtree_t::index_t;
    using float_t = kdtree_t::float_t;
    using pos_t = kdtree_t::pos_t;
    using kdtree_impl_t = kdtree_t::impl_t;
    
    using rng_t = UniformRealRandomNumber<>;

    KDTreeTest() : _eng(seed), _rng(0.0f, box_size, &_eng){}
    ~KDTreeTest() override {}
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


TEST_F(KDTreeTest, TreeConstructDefaultPolicy) {
    kdtree_t kdt(_kdpts1);
    EXPECT_EQ(kdt.nodes().size(), _kdpts1.size());
    
    kdt.construct(_kdpts2);
    EXPECT_EQ(kdt.nodes().size(), _kdpts2.size());

    // Check tree info.
    const auto &info = kdt.tree_info();
    EXPECT_GT(info.max_depth(), 1);

    // Check each node.
    const auto &nds = kdt.nodes();
    for(size_t i=0; i<nds.size(); ++i){
        const auto &nd = nds[i];
        EXPECT_GE(nd.axis(), 0);
        EXPECT_LT(nd.axis(), kdtree_t::DIM);

        index_t sz = nd.size(), children_sz = 0;
        if( sz > 1 ) {
            index_t l = kdt.left_child_idx(i);
            EXPECT_EQ(l, i+1);
            children_sz += nds[l].size();
        }
        if( sz > 2 ) {
            index_t r = kdt.right_child_idx(i);
            EXPECT_EQ(r, i+1 + nds[i+1].size());
            children_sz += nds[r].size();
        }
        EXPECT_EQ(sz, children_sz+1);
    }

    // Check paddings.
    std::unordered_set<int> pads;
    for(auto &p: _kdpts2) 
        pads.emplace(p.pad<int>());
    EXPECT_EQ(pads.size(), _kdpts2.size());

    for(auto &n: kdt.nodes())
        EXPECT_EQ(pads.erase(n.pad<int>()), 1);
    EXPECT_EQ(pads.size(), 0);

    // Check impl.
    auto impl = kdt.impl();
    EXPECT_THAT(impl, gt::NotNull());
    EXPECT_THAT(*impl, gt::A<kdtree_t::impl_t>());
}

TEST_F(KDTreeTest, TreeConstructPolicy) {
    using pl_t = kdtree_t::construct_policy_t;
    kdtree_t kdt;
    pl_t pl;

    // Check policy of the default.
    kdt.construct(_kdpts1, pl);
    {
        auto &used_pl = kdt.construct_policy();
        EXPECT_EQ(pl.split_axis(), used_pl.split_axis());
        EXPECT_EQ(pl.split_axis(), pl_t::DFLT_SPLIT_AXIS);
        EXPECT_EQ(pl.random_seed(), used_pl.random_seed());
        EXPECT_EQ(pl.random_seed(), pl_t::DFLT_RANDOM_SEED);
    }

    // Change split_axis.
    using spl_t = pl_t::split_axis_t;
    for(spl_t spl: {spl_t::MAX_EXTREME, 
        spl_t::MAX_VARIANCE, spl_t::ORDERED, spl_t::RANDOM})
    {
        pl.set_split_axis(spl);
        kdt.construct(_kdpts1, pl);
        EXPECT_EQ(kdt.nodes().size(), _kdpts1.size());

        auto &used_pl = kdt.construct_policy();
        EXPECT_EQ(pl.split_axis(), used_pl.split_axis());
        EXPECT_EQ(pl.split_axis(), spl);
    }
    
    // Try various random seeds.
    pl.set_split_axis(spl_t::RANDOM);
    for(pl_t::random_seed_t s : {1, 2, 3, 4, 5}) {
        pl.set_random_seed(s);
        kdt.construct(_kdpts1, pl);
        auto &used_pl = kdt.construct_policy();
        EXPECT_EQ(used_pl.random_seed(), s);
    }
}

TEST_F(KDTreeTest, TreeConstructPolicySplitAxisMaxExtreme) {
    vector<kdp_t> kd_pts {
        {{-3.03f, -2.01f, -1.0f}, 0}, {{-3.02f, -2.00f, 1.0f}, 1},
        {{-3.01f, 2.00f, -1.0f}, 2}, {{-3.00f, 2.01f, 1.0f}, 3},
        {{3.00f, -2.01f, -1.0f}, 4}, {{3.01f, -2.00f, 1.0f}, 5},
        {{3.02f, 2.01f, -1.0f}, 6}, {{3.03f, 2.01f, 1.0f}, 7}
    };
    kdtree_t::construct_policy_t pl;
    pl.set_split_axis(kdtree_t::construct_policy_t::split_axis_t::MAX_EXTREME);
    kdtree_t kdt(kd_pts, pl);
    
    auto &nds = kdt.nodes();
    index_t n_nds = nds.size();
    ASSERT_EQ(n_nds, kd_pts.size());

    auto *nd = &nds[0];
    EXPECT_EQ(nd->axis(), 0);
    EXPECT_EQ(nd->pad<int>(), 4);
    EXPECT_EQ(nd->size(), 8);
    EXPECT_TRUE( (nd->pos() == kd_pts[4].pos()).all() );

    nd = &nds[1];
    EXPECT_EQ(nd->axis(), 1);
    EXPECT_EQ(nd->pad<int>(), 2);
    EXPECT_EQ(nd->size(), 4);
    EXPECT_TRUE( (nd->pos() == kd_pts[2].pos()).all() );

    nd = &nds[2];
    EXPECT_EQ(nd->axis(), 2);
    EXPECT_EQ(nd->pad<int>(), 1);
    EXPECT_EQ(nd->size(), 2);
    EXPECT_TRUE( (nd->pos() == kd_pts[1].pos()).all() );

    nd = &nds[3];
    // EXPECT_EQ(nd->axis(), 1);
    EXPECT_EQ(nd->pad<int>(), 0);
    EXPECT_EQ(nd->size(), 1);
    EXPECT_TRUE( (nd->pos() == kd_pts[0].pos()).all() );

    nd = &nds[4];
    // EXPECT_EQ(nd->axis(), 1);
    EXPECT_EQ(nd->pad<int>(), 3);
    EXPECT_EQ(nd->size(), 1);
    EXPECT_TRUE( (nd->pos() == kd_pts[3].pos()).all() );

    nd = &nds[5];
    EXPECT_EQ(nd->axis(), 1);
    EXPECT_EQ(nd->pad<int>(), 6);
    EXPECT_EQ(nd->size(), 3);
    EXPECT_TRUE( (nd->pos() == kd_pts[6].pos()).all() );

    nd = &nds[6];
    // EXPECT_EQ(nd->axis(), 1);
    EXPECT_EQ(nd->pad<int>(), 5);
    EXPECT_EQ(nd->size(), 1);
    EXPECT_TRUE( (nd->pos() == kd_pts[5].pos()).all() );

    nd = &nds[7];
    // EXPECT_EQ(nd->axis(), 1);
    EXPECT_EQ(nd->pad<int>(), 7);
    EXPECT_EQ(nd->size(), 1);
    EXPECT_TRUE( (nd->pos() == kd_pts[7].pos()).all() );
}

TEST_F(KDTreeTest, TreeConstructPolicySplitAxisMaxVariance) {
    vector<kdp_t> kd_pts {
        {{-3.03f, -2.01f, -1.0f}, 0}, {{-3.02f, -2.00f, 1.0f}, 1},
        {{-3.01f, 2.00f, -1.0f}, 2}, {{-3.00f, 2.01f, 1.0f}, 3},
        {{3.00f, -2.01f, -1.0f}, 4}, {{3.01f, -2.00f, 1.0f}, 5},
        {{3.02f, 2.01f, -1.0f}, 6}, {{3.03f, 2.01f, 1.0f}, 7}
    };
    kdtree_t::construct_policy_t pl;
    pl.set_split_axis(kdtree_t::construct_policy_t::split_axis_t::MAX_VARIANCE);
    kdtree_t kdt(kd_pts, pl);
    
    auto &nds = kdt.nodes();
    index_t n_nds = nds.size();
    ASSERT_EQ(n_nds, kd_pts.size());

    auto *nd = &nds[0];
    EXPECT_EQ(nd->axis(), 0);
    EXPECT_EQ(nd->pad<int>(), 4);
    EXPECT_EQ(nd->size(), 8);
    EXPECT_TRUE( (nd->pos() == kd_pts[4].pos()).all() );

    nd = &nds[1];
    EXPECT_EQ(nd->axis(), 1);
    EXPECT_EQ(nd->pad<int>(), 2);
    EXPECT_EQ(nd->size(), 4);
    EXPECT_TRUE( (nd->pos() == kd_pts[2].pos()).all() );

    nd = &nds[2];
    EXPECT_EQ(nd->axis(), 2);
    EXPECT_EQ(nd->pad<int>(), 1);
    EXPECT_EQ(nd->size(), 2);
    EXPECT_TRUE( (nd->pos() == kd_pts[1].pos()).all() );

    nd = &nds[3];
    // EXPECT_EQ(nd->axis(), 1);
    EXPECT_EQ(nd->pad<int>(), 0);
    EXPECT_EQ(nd->size(), 1);
    EXPECT_TRUE( (nd->pos() == kd_pts[0].pos()).all() );

    nd = &nds[4];
    // EXPECT_EQ(nd->axis(), 1);
    EXPECT_EQ(nd->pad<int>(), 3);
    EXPECT_EQ(nd->size(), 1);
    EXPECT_TRUE( (nd->pos() == kd_pts[3].pos()).all() );

    nd = &nds[5];
    EXPECT_EQ(nd->axis(), 1);
    EXPECT_EQ(nd->pad<int>(), 6);
    EXPECT_EQ(nd->size(), 3);
    EXPECT_TRUE( (nd->pos() == kd_pts[6].pos()).all() );

    nd = &nds[6];
    // EXPECT_EQ(nd->axis(), 1);
    EXPECT_EQ(nd->pad<int>(), 5);
    EXPECT_EQ(nd->size(), 1);
    EXPECT_TRUE( (nd->pos() == kd_pts[5].pos()).all() );

    nd = &nds[7];
    // EXPECT_EQ(nd->axis(), 1);
    EXPECT_EQ(nd->pad<int>(), 7);
    EXPECT_EQ(nd->size(), 1);
    EXPECT_TRUE( (nd->pos() == kd_pts[7].pos()).all() );
}

TEST_F(KDTreeTest, TreeConstructPolicySplitAxisOrdered) {
    using pl_t = kdtree_t::construct_policy_t;
    kdtree_t kdt;
    pl_t pl;

    const auto split_axis = pl_t::split_axis_t::ORDERED;
    pl.set_split_axis(split_axis);
    kdt.construct(_kdpts1, pl);
    EXPECT_EQ(kdt.construct_policy().split_axis(), split_axis);

    // Check axis of each node.
    const auto &nds = kdt.nodes();
    ASSERT_EQ(nds.size(), _kdpts1.size());
    for(index_t i=0, n_nds = nds.size(); i<n_nds; ++i){
        auto &n = nds[i];
        EXPECT_EQ(n.axis(), i%kdtree_t::DIM);
    }
}


TEST_F(KDTreeTest, ArgSort) {
    kdtree_t kdt(_kdpts1);

    vector<kdtree_t::idx_pair_t> idx_prs;
    
    kdt.argsort<kdp_t>(_kdpts2, idx_prs);
    ASSERT_EQ(idx_prs.size(), _kdpts2.size());

    index_t n_nds = _kdpts1.size();
    kdt.argsort<kdp_t>(_kdpts1, idx_prs);
    ASSERT_EQ(idx_prs.size(), n_nds);

    for(index_t i=0; i<n_nds-1; ++i){
        EXPECT_LE(idx_prs[i].idx_node, idx_prs[i+1].idx_node);
    }

    std::set<index_t> s1;
    for(auto &[idx_in, idx_node]: idx_prs){
        s1.emplace(idx_in);
        EXPECT_GE(idx_in, 0);
        EXPECT_LT(idx_in, n_nds);
        EXPECT_GE(idx_node, 0);
        EXPECT_LT(idx_node, n_nds);

        auto &nd = kdt.nodes()[idx_node];
        auto &p = _kdpts1[idx_in];
        if( p.pos()[nd.axis()] <= nd.pos()[nd.axis()] ) {
            EXPECT_LT(nd.size(), 2);
        } else {
            EXPECT_LT(nd.size(), 3);
        }
    }
    EXPECT_EQ(s1.size(), n_nds);
}

TEST_F(KDTreeTest, WalkDown) {
    kdtree_t kdt(_kdpts1);

    for(size_t i=0; i<_kdpts1.size(); ++i){
        auto &p = _kdpts1[i];
        auto pad = p.pad<int>();

        index_t cnt = 0, root = 0;
        kdt.walk_down(p, [&pad, &cnt, &kdt](index_t nd_idx) {
            auto &nd = kdt.nodes()[nd_idx];
            if(nd.pad<int>() == pad) ++cnt;
        }, root);

        EXPECT_EQ(cnt, 1);
    }
}

TEST_F(KDTreeTest, Clear) {
    kdtree_t kdt(_kdpts1);
    EXPECT_EQ(kdt.nodes().size(), _kdpts1.size());
    EXPECT_GE(kdt.nodes().capacity(), _kdpts1.size());

    kdt.clear();
    EXPECT_EQ(kdt.nodes().size(), 0);

    kdt.shrink_buffer();
    EXPECT_EQ(kdt.nodes().size(), 0);
    EXPECT_EQ(kdt.nodes().capacity(), 0);
    
    kdt.construct(_kdpts2);
    EXPECT_EQ(kdt.nodes().size(), _kdpts2.size());
    EXPECT_GE(kdt.nodes().capacity(), _kdpts2.size());

    kdt.shrink_buffer();
    EXPECT_EQ(kdt.nodes().capacity(), _kdpts2.size());
}


TEST_F(KDTreeTest, VisitRectCompleteCheck) {
    kdtree_t kdt(_kdpts1);
    kdp_t::point_t p = {50.0f, 50.0f, 50.0f};
    
    /* Visit nodes. */
    for(auto off: {0.0f, 1.0f, 2.0f, 3.0f, 7.5f}){
        auto new_p = p + off;

        std::unordered_set<index_t> pads;
        kdt.visit_nodes_rect(
            {new_p-100.0f, new_p+100.0f}, 
            [&pads](const kdtree_t::node_t &n){ 
                pads.emplace(n.pad<int>()); 
            }
        );

        auto n = _kdpts1.size();
        EXPECT_EQ(pads.size(), n);
        while(n-- > 0)
            ASSERT_EQ(pads.erase(n), 1) << "Erase " << n;
    }

    /* Count nodes. */
    for(auto off: {0.0f, 1.0f, 2.0f, 3.0f, 7.5f}){
        auto new_p = p + off;
        EXPECT_EQ(kdt.count_nodes_rect({new_p-100.0f, new_p+100.0f}), 
            _kdpts1.size());
    }
}

TEST_F(KDTreeTest, VisitRectNodes) {
    kdtree_t kdt(_kdpts1);
    int n_dst = 1000;

    for(auto dx: {1.0f, 2.0f, 3.0f, 5.0f, 7.5f}) {
        /* Check node count. */
        for(int i=0; i<n_dst; ++i){
            auto &p_dst = _kdpts2[i];
            kdtree_t::rect_t r(p_dst-dx, p_dst+dx);
            index_t cnt1 = kdt.count_nodes_rect(r),
                cnt2 = std::count_if(_kdpts1.begin(), _kdpts1.end(), 
                    [&r](auto &p){ return r.contains(p); });
            EXPECT_EQ(cnt1, cnt2);
        }

        /* Check padding of each node. */
        for(int i=0; i<n_dst; ++i){
            auto &p_dst = _kdpts2[i];
            vector<index_t> pad_src, pad_dst;
            kdtree_t::rect_t r(p_dst-dx, p_dst+dx);
            kdt.visit_nodes_rect(r, 
                [&pad_src](const kdtree_t::node_t & n){ 
                    pad_src.push_back( n.pad<const int>() ); });
            for(auto &p_src: _kdpts1)
                if( r.contains(p_src) ) 
                    pad_dst.push_back( p_src.pad<const int>() );
            EXPECT_THAT(pad_src, 
                gt::UnorderedElementsAreArray(pad_dst));
        }
    }

}

TEST_F(KDTreeTest, VisitSphereCompleteCheck) {
    kdtree_t kdt(_kdpts1);
    kdp_t::point_t p = {50.0f, 50.0f, 50.0f};
    
    /* Visit nodes. */
    for(auto off: {0.0f, 1.0f, 2.0f, 3.0f, 7.5f}){
        auto new_p = p + off;

        std::unordered_set<index_t> pads;
        kdt.visit_nodes_sphere(
            {new_p, 500.0f}, 
            [&pads](const kdtree_t::node_t &n){ 
                pads.emplace(n.pad<int>()); 
            }
        );

        auto n = _kdpts1.size();
        EXPECT_EQ(pads.size(), n);
        while(n-- > 0)
            ASSERT_EQ(pads.erase(n), 1) << "Erase " << n;
    }

    /* Count nodes. */
    for(auto off: {0.0f, 1.0f, 2.0f, 3.0f, 7.5f}){
        auto new_p = p + off;
        EXPECT_EQ(kdt.count_nodes_sphere({new_p, 500.0f}), 
            _kdpts1.size());
    }
}

TEST_F(KDTreeTest, VisitSphereNodes) {
    kdtree_t kdt(_kdpts1);
    int n_dst = 1000;

    for(auto r: {1.0f, 2.0f, 3.0f, 5.0f, 7.5f}){
        /* Check node count. */
        for(int i=0; i<n_dst; ++i){
            auto &p_dst = _kdpts2[i];
            kdtree_t::sphere_t s(p_dst, r);
            index_t cnt1 = kdt.count_nodes_sphere(s),
                cnt2 = std::count_if(_kdpts1.begin(), _kdpts1.end(), 
                    [&s](auto &p){ return s.contains(p); });
            EXPECT_EQ(cnt1, cnt2);
        }

        /* Check padding of each node. */
        for(int i=0; i<n_dst; ++i){
            auto &p_dst = _kdpts2[i];
            vector<index_t> pad_src, pad_dst;
            kdtree_t::sphere_t s(p_dst, r);
            kdt.visit_nodes_sphere(s, 
                [&pad_src](const kdtree_t::node_t& n){ 
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