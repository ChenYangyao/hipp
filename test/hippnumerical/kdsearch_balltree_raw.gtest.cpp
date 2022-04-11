#include <hippnumerical.h>
#include <gmock/gmock.h>

namespace HIPP::NUMERICAL {

namespace {

namespace gt = ::testing;

class BallTreeRawEmptyPaddingTest : public gt::Test {
public:
    using kdp_t = KDPoint<float, 3>;
    using balltree_t = _KDSEARCH::_BallTree<kdp_t, int>;
    using node_t = balltree_t::node_t;
    using point_t = balltree_t::point_t;
    using pos_t = balltree_t::pos_t;
    using index_t = balltree_t::index_t;
    using float_t = balltree_t::float_t;

    static constexpr int n_repeat_min = 25, 
        n_repeat_max = 10000;

    BallTreeRawEmptyPaddingTest() {}
    ~BallTreeRawEmptyPaddingTest() override {}
    void SetUp() override {}
    void TearDown() override {}

    vector<balltree_t::construct_policy_t> get_cstr_policy_table() {
        using pl_t = balltree_t::construct_policy_t;
        using alg_t = pl_t::algorithm_t;
        using spl_t = pl_t::split_axis_t;
        using ins_fav_t = pl_t::insert_favor_t;
        
        vector<pl_t> out;
        
        for(auto spl: { spl_t::MAX_EXTREME, spl_t::MAX_VARIANCE, 
            spl_t::ORDERED, spl_t::RANDOM })
        {
            pl_t cstr_pl;
            cstr_pl.set_algorithm(alg_t::KD);
            cstr_pl.set_split_axis(spl);
            out.push_back(cstr_pl);
        }
        for( auto alg: { pl_t::algorithm_t::TOP_DOWN, 
            alg_t::ROUGH_INSERTION, alg_t::FINE_INSERTION } ) 
        {
            pl_t cstr_pl;
            cstr_pl.set_algorithm(alg);
            out.push_back(cstr_pl);
        }
        for( auto ins_fav: { ins_fav_t::ROUGH, ins_fav_t::FINE } ) {
            pl_t cstr_pl;
            cstr_pl.set_algorithm(alg_t::BOTTOM_UP);
            cstr_pl.set_bottom_up_insert_favor(ins_fav);
        }

        return out;
    }
};

TEST_F(BallTreeRawEmptyPaddingTest, DefaultInitialization){
    kdp_t pt;
    
    balltree_t ballt;

    node_t nd {};

    balltree_t::sphere_t s {};
    balltree_t::point_t p {};
    balltree_t::pos_t pos {};
    balltree_t::rect_t rect {};

    balltree_t::construct_policy_t cstr_pl {};
    balltree_t::query_buff_policy_t qb_pl {};
    balltree_t::nearest_query_policy_t nq_pl {};
    balltree_t::nearest_k_query_policy_t nkq_pl {};
    balltree_t::rect_query_policy_t rq_pl {};
    balltree_t::sphere_query_policy_t sq_pl {};
}


TEST_F(BallTreeRawEmptyPaddingTest, InfoPrinting){
    balltree_t ballt;
    cout << ballt << endl;
    ballt.info() << endl;
    ballt.info(cout, 1);

    balltree_t::node_t nd;
    cout << nd << endl;
    nd.info() << endl;
    nd.info(cout, 1);

    balltree_t::construct_policy_t c_pl;
    cout << c_pl << endl;
    c_pl.info() << endl;
    c_pl.info(cout, 1);
}

TEST_F(BallTreeRawEmptyPaddingTest, NodeTypeConstruction) {
for(int i=0; i<n_repeat_max; ++i){
    node_t n1;

    node_t n2 {{1,2,3}};
    ASSERT_EQ(n2.r(), float_t(0));
    ASSERT_TRUE( (n2.center().pos() == pos_t{1,2,3}).all() );

    node_t n3 {{1,2,3},4};
    ASSERT_EQ(n3.r(), float_t(4));
    ASSERT_TRUE( (n3.center().pos() == pos_t{1,2,3}).all() );
    
    char pad[0];
    node_t n4 {{1,2,3},4,pad};
    ASSERT_EQ(n4.r(), float_t(4));
    ASSERT_TRUE( (n4.center().pos() == pos_t{1,2,3}).all() );

    node_t n5 {{1,2,3},4,5,pad};
    ASSERT_EQ(n5.r(), float_t(4));
    ASSERT_EQ(n5.size(), 5);
    ASSERT_TRUE( (n5.center().pos() == pos_t{1,2,3}).all() );
}}

TEST_F(BallTreeRawEmptyPaddingTest, NodeTypeAttributesSetGet) {
for(int i=0; i<n_repeat_max; ++i){
    node_t n1;
    n1.center() = point_t{1,2,3};
    n1.r() = 4;
    n1.size() = 5;
    char pad[0];
    n1.fill_pad(pad);

    ASSERT_EQ(n1.r(), float_t(4));
    ASSERT_EQ(n1.size(), 5);
    ASSERT_TRUE( (n1.center().pos() == pos_t{1,2,3}).all() );

    ASSERT_EQ(sizeof(n1.pad()), 0);
}}


TEST_F(BallTreeRawEmptyPaddingTest, EmptyTree) {
for(int i=0; i<n_repeat_max; ++i){
    balltree_t ballt;
    ASSERT_EQ(ballt.nodes().size(), 0);
}}

TEST_F(BallTreeRawEmptyPaddingTest, Construct1Node){
for(int i=0; i<n_repeat_max; ++i){
    balltree_t ballt;
    float_t loc = i;
    vector<kdp_t> pts { { {loc, loc, loc} } };
    ballt.construct(pts);
    
    auto &nds = ballt.nodes();
    ASSERT_EQ(nds.size(), 1);
    auto &r = nds[0];
    EXPECT_EQ(r.size(), 1);

    EXPECT_LT(r.r(), 1.0e-4);
    float_t dist = (r.center() - point_t{loc, loc, loc}).r();
    EXPECT_LT(dist, 1.0e-4);
}}

TEST_F(BallTreeRawEmptyPaddingTest, Construct2Nodes){
for(int i=0; i<n_repeat_max; ++i){
    balltree_t ballt;
    float_t loc = i;
    vector<kdp_t> pts { { {0,0,0} }, { {loc, loc, loc} } };
    ballt.construct(pts);

    auto &nds = ballt.nodes();
    ASSERT_EQ(nds.size(), 3);
    
    auto &r = nds[0], &lc = nds[1], &rc = nds[2];
    EXPECT_EQ(ballt.left_child_idx(0), 1);
    EXPECT_EQ(ballt.right_child_idx(0), 2);

    auto [i_lc, i_rc] = ballt.children_ids(0);
    EXPECT_EQ(i_lc, 1);
    EXPECT_EQ(i_rc, 2);

    EXPECT_EQ(r.size(), 3);
    EXPECT_EQ(lc.size(), 1);
    EXPECT_EQ(rc.size(), 1);

    point_t rc_dst =  0.5*(lc.center() - rc.center()) + rc.center();
    float_t dist = (r.center() - rc_dst).r();
    EXPECT_LT(dist, 1.0e-4);
}}

TEST_F(BallTreeRawEmptyPaddingTest, ConstructionWithDefaultPolicy){
for(int i=0; i<n_repeat_min; ++i){
    balltree_t ballt;
    ASSERT_EQ(ballt.nodes().size(), 0);

    vector<kdp_t> pts;
    for(int i=0; i<50; ++i){
        pos_t pos; rand(pos.begin(), pos.end());
        pts.emplace_back(pos);
    }

    ballt.construct(pts);
    ASSERT_EQ(ballt.nodes().size(), pts.size()*2-1);

    for(int i=0; i<500; ++i){
        pos_t pos; rand(pos.begin(), pos.end());
        pts.emplace_back(pos);
    }

    ballt.construct(pts);
    ASSERT_EQ(ballt.nodes().size(), pts.size()*2-1);

    const auto &nds = ballt.nodes();
    index_t cnt_leaves = 0;
    index_t cnt_nds = nds.size();
    for(index_t i=0; i<cnt_nds; ++i){
        auto &n = nds[i];
        if( n.size() == 1 ) {
            ++cnt_leaves; continue;
        }

        auto [i_lc, i_rc] = ballt.children_ids(i);
        ASSERT_LT(i_lc, cnt_nds);
        ASSERT_LT(i_rc, cnt_nds);
        auto &n_lc = nds[i_lc], &n_rc = nds[i_rc];
        ASSERT_EQ(n.size(), n_lc.size()+n_rc.size()+1);

        auto bs1 = n_lc.bounding_sphere(n_rc),
            bs2 = n_rc.bounding_sphere(n_lc);
        ASSERT_LT(std::fabs(bs1.r()-bs2.r()), 1.0e-4);
        ASSERT_LT((bs1.center() - bs2.center()).r() , 1.0e-4); 
        ASSERT_LT(std::fabs(bs1.r()-n.r()), 1.0e-4);
        ASSERT_LT((bs1.center() - n.center()).r() , 1.0e-4);
    }
    EXPECT_EQ(cnt_leaves, index_t(pts.size()));
}}


TEST_F(BallTreeRawEmptyPaddingTest, ConstructionWithKDPolicy){
for(int i=0; i<n_repeat_min; ++i){
    using pl_t = balltree_t::construct_policy_t;
    using spl_t = pl_t::split_axis_t;
    
    pl_t cstr_pl;
    cstr_pl.set_algorithm(pl_t::algorithm_t::KD);

    for(auto spl: { spl_t::MAX_EXTREME, spl_t::MAX_VARIANCE, 
        spl_t::ORDERED, spl_t::RANDOM })
    {
        cstr_pl.set_split_axis(spl);
    
        balltree_t ballt;
        ASSERT_EQ(ballt.nodes().size(), 0);

        vector<kdp_t> pts;
        for(int i=0; i<50; ++i){
            pos_t pos; rand(pos.begin(), pos.end());
            pts.emplace_back(pos);
        }

        ballt.construct(pts, cstr_pl);
        ASSERT_EQ(ballt.nodes().size(), pts.size()*2-1);

        for(int i=0; i<500; ++i){
            pos_t pos; rand(pos.begin(), pos.end());
            pts.emplace_back(pos);
        }

        ballt.construct(pts, cstr_pl);
        ASSERT_EQ(ballt.nodes().size(), pts.size()*2-1);

        const auto &nds = ballt.nodes();
        index_t cnt_leaves = 0;
        index_t cnt_nds = nds.size();
        for(index_t i=0; i<cnt_nds; ++i){
            auto &n = nds[i];
            if( n.size() == 1 ) {
                ++cnt_leaves; continue;
            }

            auto [i_lc, i_rc] = ballt.children_ids(i);
            ASSERT_LT(i_lc, cnt_nds);
            ASSERT_LT(i_rc, cnt_nds);
            auto &n_lc = nds[i_lc], &n_rc = nds[i_rc];
            ASSERT_EQ(n.size(), n_lc.size()+n_rc.size()+1);

            auto bs1 = n_lc.bounding_sphere(n_rc),
                bs2 = n_rc.bounding_sphere(n_lc);
            ASSERT_LT(std::fabs(bs1.r()-bs2.r()), 1.0e-4);
            ASSERT_LT((bs1.center() - bs2.center()).r() , 1.0e-4); 
            ASSERT_LT(std::fabs(bs1.r()-n.r()), 1.0e-4);
            ASSERT_LT((bs1.center() - n.center()).r() , 1.0e-4);
        }
        EXPECT_EQ(cnt_leaves, index_t(pts.size()));
        
    }
}}

TEST_F(BallTreeRawEmptyPaddingTest, ConstructionWithTopDownPolicy){
for(int i=0; i<n_repeat_min; ++i){
    using pl_t = balltree_t::construct_policy_t;
    
    pl_t cstr_pl;
    cstr_pl.set_algorithm(pl_t::algorithm_t::TOP_DOWN);

    balltree_t ballt;
    ASSERT_EQ(ballt.nodes().size(), 0);

    vector<kdp_t> pts;
    for(int i=0; i<50; ++i){
        pos_t pos; rand(pos.begin(), pos.end());
        pts.emplace_back(pos);
    }

    ballt.construct(pts, cstr_pl);
    ASSERT_EQ(ballt.nodes().size(), pts.size()*2-1);

    for(int i=0; i<500; ++i){
        pos_t pos; rand(pos.begin(), pos.end());
        pts.emplace_back(pos);
    }

    ballt.construct(pts, cstr_pl);
    ASSERT_EQ(ballt.nodes().size(), pts.size()*2-1);

    const auto &nds = ballt.nodes();
    index_t cnt_leaves = 0;
    index_t cnt_nds = nds.size();
    for(index_t i=0; i<cnt_nds; ++i){
        auto &n = nds[i];
        if( n.size() == 1 ) {
            ++cnt_leaves; continue;
        }

        auto [i_lc, i_rc] = ballt.children_ids(i);
        ASSERT_LT(i_lc, cnt_nds);
        ASSERT_LT(i_rc, cnt_nds);
        auto &n_lc = nds[i_lc], &n_rc = nds[i_rc];
        ASSERT_EQ(n.size(), n_lc.size()+n_rc.size()+1);
        auto bs1 = n_lc.bounding_sphere(n_rc),
            bs2 = n_rc.bounding_sphere(n_lc);
        ASSERT_LT(std::fabs(bs1.r()-bs2.r()), 1.0e-4);
        ASSERT_LT((bs1.center() - bs2.center()).r() , 1.0e-4); 
        ASSERT_LT(std::fabs(bs1.r()-n.r()), 1.0e-4);
        ASSERT_LT((bs1.center() - n.center()).r() , 1.0e-4);
    }
    EXPECT_EQ(cnt_leaves, index_t(pts.size()));
}}

TEST_F(BallTreeRawEmptyPaddingTest, ConstructionWithInsertionPolicy){
for(int i=0; i<n_repeat_min; ++i){
    using pl_t = balltree_t::construct_policy_t;
    using alg_t = pl_t::algorithm_t;

    for( auto alg: { alg_t::ROUGH_INSERTION, alg_t::FINE_INSERTION } ) {
        pl_t cstr_pl;
        cstr_pl.set_algorithm(alg);

        balltree_t ballt;
        ASSERT_EQ(ballt.nodes().size(), 0);

        vector<kdp_t> pts;
        for(int i=0; i<50; ++i){
            pos_t pos; rand(pos.begin(), pos.end());
            pts.emplace_back(pos);
        }

        ballt.construct(pts, cstr_pl);
        ASSERT_EQ(ballt.nodes().size(), pts.size()*2-1);

        for(int i=0; i<500; ++i){
            pos_t pos; rand(pos.begin(), pos.end());
            pts.emplace_back(pos);
        }

        ballt.construct(pts, cstr_pl);
        ASSERT_EQ(ballt.nodes().size(), pts.size()*2-1);

        const auto &nds = ballt.nodes();
        index_t cnt_leaves = 0;
        index_t cnt_nds = nds.size();
        for(index_t i=0; i<cnt_nds; ++i){
            auto &n = nds[i];
            if( n.size() == 1 ) {
                ++cnt_leaves; continue;
            }

            auto [i_lc, i_rc] = ballt.children_ids(i);
            ASSERT_LT(i_lc, cnt_nds);
            ASSERT_LT(i_rc, cnt_nds);
            auto &n_lc = nds[i_lc], &n_rc = nds[i_rc];
            ASSERT_EQ(n.size(), n_lc.size()+n_rc.size()+1);
            auto bs1 = n_lc.bounding_sphere(n_rc),
                bs2 = n_rc.bounding_sphere(n_lc);
            ASSERT_LT(std::fabs(bs1.r()-bs2.r()), 1.0e-4);
            ASSERT_LT((bs1.center() - bs2.center()).r() , 1.0e-4); 
            ASSERT_LT(std::fabs(bs1.r()-n.r()), 1.0e-4);
            ASSERT_LT((bs1.center() - n.center()).r() , 1.0e-4);
        }
        EXPECT_EQ(cnt_leaves, index_t(pts.size()));
    }
}}

TEST_F(BallTreeRawEmptyPaddingTest, ConstructionWithBottomUpPolicy){
for(int i=0; i<n_repeat_min; ++i){
    using pl_t = balltree_t::construct_policy_t;
    using ins_fav_t = pl_t::insert_favor_t;

    pl_t cstr_pl;
    cstr_pl.set_algorithm(pl_t::algorithm_t::BOTTOM_UP);
    for( auto ins_fav: { ins_fav_t::ROUGH, ins_fav_t::FINE } ) {

        cstr_pl.set_bottom_up_insert_favor(ins_fav);

        balltree_t ballt;
        ASSERT_EQ(ballt.nodes().size(), 0);

        vector<kdp_t> pts;
        for(int i=0; i<50; ++i){
            pos_t pos; rand(pos.begin(), pos.end());
            pts.emplace_back(pos);
        }
        ballt.construct(pts, cstr_pl);
        ASSERT_EQ(ballt.nodes().size(), pts.size()*2-1);

        for(int i=0; i<500; ++i){
            pos_t pos; rand(pos.begin(), pos.end());
            pts.emplace_back(pos);
        }

        ballt.construct(pts, cstr_pl);
        ASSERT_EQ(ballt.nodes().size(), pts.size()*2-1);

        const auto &nds = ballt.nodes();
        index_t cnt_leaves = 0;
        index_t cnt_nds = nds.size();
        for(index_t i=0; i<cnt_nds; ++i){
            auto &n = nds[i];
            if( n.size() == 1 ) {
                ++cnt_leaves; continue;
            }

            auto [i_lc, i_rc] = ballt.children_ids(i);
            ASSERT_LT(i_lc, cnt_nds);
            ASSERT_LT(i_rc, cnt_nds);
            auto &n_lc = nds[i_lc], &n_rc = nds[i_rc];
            ASSERT_EQ(n.size(), n_lc.size()+n_rc.size()+1);
            auto bs1 = n_lc.bounding_sphere(n_rc),
                bs2 = n_rc.bounding_sphere(n_lc);
            ASSERT_LT(std::fabs(bs1.r()-bs2.r()), 1.0e-4);
            ASSERT_LT((bs1.center() - bs2.center()).r() , 1.0e-4); 
            ASSERT_LT(std::fabs(bs1.r()-n.r()), 1.0e-4);
            ASSERT_LT((bs1.center() - n.center()).r() , 1.0e-4);
        }
        EXPECT_EQ(cnt_leaves, index_t(pts.size()));
    }
}}

TEST_F(BallTreeRawEmptyPaddingTest, Argsort){
    balltree_t ballt;
    vector<kdp_t> pts;
    for(int i=0; i<500; ++i){
        pos_t pos; rand(pos.begin(), pos.end());
        pts.emplace_back(pos);
    }
    ballt.construct(pts);
    auto &nds = ballt.nodes();

    for(int i=0; i<n_repeat_min; ++i){
        pts.clear();
        for(int j=0; j<i+100; ++j){
            pos_t pos; rand(pos.begin(), pos.end());
            pts.emplace_back(pos);
        }   
        vector<balltree_t::idx_pair_t> iprs;
        ballt.argsort<kdp_t>(pts, iprs);

        ASSERT_EQ(iprs.size(), pts.size());
        for(auto &[idx_in, idx_node]: iprs){
            ASSERT_LT(idx_in, pts.size());
            ASSERT_LT(idx_node, nds.size());
            ASSERT_EQ(nds[idx_node].size(), 1);
        }
    }
}

TEST_F(BallTreeRawEmptyPaddingTest, QueryNearestDefaultPolicy){
for(int i_repeat=0; i_repeat<n_repeat_min; ++i_repeat){
    balltree_t ballt;
    auto cstr_pls = get_cstr_policy_table();
    vector<kdp_t> pts, pts_dst;
    for(int i=0; i<500; ++i){
        pos_t pos; rand(pos.begin(), pos.end());
        pts.emplace_back(pos);
        rand(pos.begin(), pos.end());
        pts_dst.emplace_back(pos);
    }
    
    for(auto &cstr_pl: cstr_pls){
        ballt.construct(pts, cstr_pl);
        auto &nds = ballt.nodes();
        index_t n_nds = nds.size();
        for(auto &p: pts){
            auto ngb = ballt.nearest(p);
            ASSERT_GE(ngb.node_idx, 0);
            ASSERT_LT(ngb.node_idx, n_nds);
            ASSERT_LT( (nds[ngb.node_idx].center() - p).r(), 1.0e-4 );
        }
        for(auto &p: pts_dst){
            auto ngb = ballt.nearest(p);
            ASSERT_GE(ngb.node_idx, 0);
            ASSERT_LT(ngb.node_idx, n_nds);
            ASSERT_LT( (nds[ngb.node_idx].center() - p).r_sq(), 
                ngb.r_sq+1.0e-4);
        }
    }
}
}

TEST_F(BallTreeRawEmptyPaddingTest, QueryNearestSharedPolicy){
for(int i_repeat=0; i_repeat<n_repeat_min; ++i_repeat){
    balltree_t ballt;
    auto cstr_pls = get_cstr_policy_table();
    vector<kdp_t> pts, pts_dst;
    for(int i=0; i<500; ++i){
        pos_t pos; rand(pos.begin(), pos.end());
        pts.emplace_back(pos);
        rand(pos.begin(), pos.end());
        pts_dst.emplace_back(pos);
    }
    
    balltree_t::nearest_query_policy_t q_pl;

    for(auto &cstr_pl: cstr_pls){
        ballt.construct(pts, cstr_pl);
        auto &nds = ballt.nodes();
        index_t n_nds = nds.size();
        for(auto &p: pts){
            auto ngb = ballt.nearest(p, q_pl);
            ASSERT_GE(ngb.node_idx, 0);
            ASSERT_LT(ngb.node_idx, n_nds);
            ASSERT_LT( (nds[ngb.node_idx].center() - p).r(), 1.0e-4 );
        }
        for(auto &p: pts_dst){
            auto ngb = ballt.nearest(p, q_pl);
            ASSERT_GE(ngb.node_idx, 0);
            ASSERT_LT(ngb.node_idx, n_nds);
            ASSERT_LT( (nds[ngb.node_idx].center() - p).r_sq(), 
                ngb.r_sq+1.0e-4);
        }
    }
}
}


} // namespace

} // namespace HIPP::NUMERICAL