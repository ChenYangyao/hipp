#include <hippnumerical.h>
#include <gmock/gmock.h>

namespace HIPP::NUMERICAL {

namespace {

namespace gt = ::testing;

class BallTreeRawEmptyPaddingTest : public gt::Test {
public:
    using kdp_t = KDPoint<float, 3>;
    using balltree_t = BallTree<kdp_t, int>;
    using node_t = balltree_t::node_t;
    using point_t = balltree_t::point_t;
    using pos_t = balltree_t::pos_t;
    using index_t = balltree_t::index_t;
    using float_t = balltree_t::float_t;

    using cstr_pl_t = balltree_t::construct_policy_t;

    static constexpr int n_repeat_min = 10, 
        n_repeat_max = 10000;

    BallTreeRawEmptyPaddingTest() {}
    ~BallTreeRawEmptyPaddingTest() override {}
    void SetUp() override {}
    void TearDown() override {}

    vector<balltree_t::construct_policy_t> get_cstr_policy_table() 
    {
        using pl_t = balltree_t::construct_policy_t;
        using alg_t = pl_t::algorithm_t;
        using spl_t = pl_t::split_axis_t;
        using ins_fav_t = pl_t::insert_favor_t;
        
        vector<pl_t> out;
        
        for(auto spl: { 
            spl_t::MAX_EXTREME,
            spl_t::MAX_VARIANCE, 
            spl_t::ORDERED, 
            spl_t::RANDOM })
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

    vector<kdp_t> get_random_points(index_t n_pts) 
    {
        vector<kdp_t> pts(n_pts);
        for(index_t i=0; i<n_pts; ++i){
            rand( pts[i].pos().begin(), pts[i].pos().end() );
        }
        return pts;
    }

    vector<kdp_t> get_cubic_layout_points()
    {
        vector<kdp_t> pts;
        for(auto x0: {0.0f, 1.0f})
        for(auto x1: {0.0f, 1.0f})
        for(auto x2: {0.0f, 1.0f})
            pts.push_back({x0,x1,x2});
        return pts;
    }

    balltree_t get_tree_with_random_points(index_t n_pts, 
        cstr_pl_t pl = cstr_pl_t()) 
    {
        balltree_t tr;
        auto pts = get_random_points(n_pts);
        tr.construct(pts, pl);
        return tr;
    }

    balltree_t::ngb_t brute_force_nearest(const vector<node_t> &nds, 
        point_t p)
    {
        balltree_t::ngb_t ngb { -1, std::numeric_limits<float_t>::max() };
        for(auto &n: nds){
            if(n.size() != 1) continue;
            auto r_sq = (n.center() - p).r_sq();
            if( r_sq < ngb.r_sq ) {
                ngb.r_sq = r_sq;
                ngb.node_idx = index_t( &n - nds.data() );
            }
        }
        return ngb;
    }

    vector<balltree_t::ngb_t> brute_force_nearest_k(const vector<node_t> &nds, 
        point_t p, index_t k) 
    {
        vector<balltree_t::ngb_t> ngbs;
        for(auto &n: nds){
            if( n.size() != 1 ) continue;
            auto r_sq = (n.center() - p).r_sq();
            ngbs.push_back({ index_t(&n - nds.data()), r_sq});
        }
        std::sort(ngbs.begin(), ngbs.end());
        if( ngbs.size() > k ) ngbs.resize(k);
        return ngbs;
    }

    index_t brute_force_count_nodes_rect(const vector<node_t> &nds, 
        balltree_t::rect_t r) 
    {
        index_t cnt {};
        for(auto &n: nds){
            if( n.size() != 1 ) continue;
            if( r.contains(n.center()) ) ++cnt;
        }
        return cnt;
    }
    index_t brute_force_count_nodes_sphere(const vector<node_t> &nds, 
        balltree_t::sphere_t s) 
    {
        index_t cnt {};
        for(auto &n: nds){
            if( n.size() != 1 ) continue;
            if( s.contains(n.center()) ) ++cnt;
        }
        return cnt;
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
    
    char pad[0] {};
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
    char pad[0] {};
    n1.fill_pad(pad);

    ASSERT_EQ(n1.r(), float_t(4));
    ASSERT_EQ(n1.size(), 5);
    ASSERT_TRUE( (n1.center().pos() == pos_t{1,2,3}).all() );

    ASSERT_EQ(sizeof(n1.pad()), balltree_t::PADDING);
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

TEST_F(BallTreeRawEmptyPaddingTest, CopyMoveOperations){
for(int i_repeat=0; i_repeat<n_repeat_min; ++i_repeat){
    {
        balltree_t tr0;
        auto cpy = tr0;
        ASSERT_NE(&cpy, &tr0);
        ASSERT_EQ(tr0.nodes().size(), 0);
        ASSERT_EQ(cpy.nodes().size(), tr0.nodes().size());
        ASSERT_EQ(cpy.tree_info().max_depth(), tr0.tree_info().max_depth());
    }
    {
        auto tr = get_tree_with_random_points(500);
        auto cpy = tr;
        ASSERT_NE(&cpy, &tr);
        ASSERT_EQ(cpy.nodes().size(), tr.nodes().size());
        ASSERT_EQ(cpy.tree_info().max_depth(), tr.tree_info().max_depth());
        index_t n_nds = (index_t)tr.nodes().size();
        for(index_t i=0; i<n_nds; ++i){
            auto &n1 = cpy.nodes()[i], &n2 = cpy.nodes()[i];
            ASSERT_EQ(n1.size(), n2.size());
            ASSERT_TRUE( (n1.center().pos() == n2.center().pos()).all() );
        }
    }
}
}

TEST_F(BallTreeRawEmptyPaddingTest, NodeIndexAccess){
for(int i_repeat=0; i_repeat<n_repeat_min; ++i_repeat){
    auto tr = get_tree_with_random_points(500);
    auto &nds = tr.nodes();
    index_t n_nds = (index_t)nds.size();
    for(index_t i=0; i<n_nds; ++i){
        auto &n = nds[i];
        if( n.size() == 1 ) continue;
        auto i_lc = tr.left_child_idx(i), i_rc = tr.right_child_idx(i), 
            i_rs = tr.right_sibling_idx(i_lc);
        ASSERT_EQ(i_lc, i+1);
        ASSERT_EQ(i_rc, i_lc+nds[i_lc].size());
        ASSERT_EQ(i_rc, i_rs);
    }
}
}

TEST_F(BallTreeRawEmptyPaddingTest, TreeInfoAccess){
for(int i_repeat=0; i_repeat<n_repeat_min; ++i_repeat){
    balltree_t tr0;
    EXPECT_EQ(tr0.tree_info().max_depth(), 0);

    auto tr1 = get_tree_with_random_points(1);
    EXPECT_EQ(tr1.tree_info().max_depth(), 1);

    auto tr2 = get_tree_with_random_points(2);
    EXPECT_EQ(tr2.tree_info().max_depth(), 2);

    auto tr3 = get_tree_with_random_points(3);
    EXPECT_EQ(tr3.tree_info().max_depth(), 3);

    auto tr4 = get_tree_with_random_points(8);
    EXPECT_GE(tr4.tree_info().max_depth(), 4);
    EXPECT_LT(tr4.tree_info().max_depth(), 8);
}
}

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

TEST_F(BallTreeRawEmptyPaddingTest, QueryNearestEmptyTree){
for(int i_repeat=0; i_repeat<n_repeat_min; ++i_repeat){
    balltree_t tr;
    
    auto pts1 = get_random_points(10), 
        pts2 = get_cubic_layout_points();
    pts1.insert(pts1.end(), pts2.begin(), pts2.end());

    ASSERT_EQ(pts1.size(), 18);
    
    for(auto &pt: pts1){
        auto ngb = tr.nearest(pt);
        EXPECT_EQ( ngb.r_sq, std::numeric_limits<balltree_t::float_t>::max() );
        EXPECT_EQ( ngb.node_idx, node_t::idxNULL );
    }
}}

TEST_F(BallTreeRawEmptyPaddingTest, QueryNearest1Point){
for(int i_repeat=0; i_repeat<n_repeat_min; ++i_repeat){
    balltree_t tr;
    float_t x = float_t(i_repeat+1.);
    vector<kdp_t> pts { {x,x,x} };

    tr.construct(pts);

    point_t p { 0.1, 0.0, 0.0 };
    auto ngb = tr.nearest(p);
    auto &nds = tr.nodes();
    ASSERT_LT(ngb.node_idx, nds.size());
    auto &n = nds[ngb.node_idx];
    EXPECT_EQ(n.size(), 1);
    EXPECT_LT((n.center() - point_t{x,x,x}).r(), 1.0e-4);
}}

TEST_F(BallTreeRawEmptyPaddingTest, QueryNearest2Point){
for(int i_repeat=0; i_repeat<n_repeat_min; ++i_repeat){
    balltree_t tr;
    float_t x = float_t(i_repeat+1.);
    vector<kdp_t> pts { {0,0,0}, {x,x,x} };

    tr.construct(pts);

    point_t p { 0.1, 0.0, 0.0 };
    auto ngb = tr.nearest(p);
    EXPECT_LT(ngb.r_sq, 0.1+1.0e-4);

    auto &nds = tr.nodes();
    ASSERT_LT(ngb.node_idx, nds.size());
    auto &n = nds[ngb.node_idx];
    EXPECT_EQ(n.size(), 1);
    EXPECT_LT((n.center() - point_t{0,0,0}).r(), 1.0e-4);
}}

TEST_F(BallTreeRawEmptyPaddingTest, QueryNearestWithManyPoints){
for(int i_repeat=0; i_repeat<n_repeat_min; ++i_repeat){
    auto tr = get_tree_with_random_points(50);
    auto pts = get_random_points(50);
    for(auto &pt: pts){
        auto ngb = tr.nearest(pt);
        auto ngb_dst = brute_force_nearest(tr.nodes(), pt);
        EXPECT_LT( std::fabs(ngb.r_sq-ngb_dst.r_sq), 1.0e-4 ) << 
            "Dest r^2 = " << ngb_dst.r_sq << ". Got " << ngb.r_sq << endl; 
    }
}}

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

TEST_F(BallTreeRawEmptyPaddingTest, QueryNearestK0){
for(int i_repeat=0; i_repeat<n_repeat_min; ++i_repeat){
    balltree_t tr;
    
    auto pts1 = get_random_points(10), 
        pts2 = get_cubic_layout_points();
    pts1.insert(pts1.end(), pts2.begin(), pts2.end());

    ASSERT_EQ(pts1.size(), 18);
    
    vector<balltree_t::ngb_t> ngb;
    for(auto &pt: pts1){
        auto k_used = tr.nearest_k(pt, ngb);
        ASSERT_EQ(k_used, 0);
    }
}
}

TEST_F(BallTreeRawEmptyPaddingTest, QueryNearestKWithEmptyTree){
for(int i_repeat=0; i_repeat<n_repeat_min; ++i_repeat){
    balltree_t tr;
    
    auto pts1 = get_random_points(10), 
        pts2 = get_cubic_layout_points();
    pts1.insert(pts1.end(), pts2.begin(), pts2.end());

    ASSERT_EQ(pts1.size(), 18);
    vector<balltree_t::ngb_t> ngb(3);
    for(auto &pt: pts1){
        auto k_used = tr.nearest_k(pt, ngb);
        EXPECT_EQ( k_used, 0 );
    }
}}

TEST_F(BallTreeRawEmptyPaddingTest, QueryNearestKWith1Point){
for(int i_repeat=0; i_repeat<n_repeat_min; ++i_repeat){
    auto tr = get_tree_with_random_points(1);
    
    auto pts1 = get_random_points(10), 
        pts2 = get_cubic_layout_points();
    pts1.insert(pts1.end(), pts2.begin(), pts2.end());
    ASSERT_EQ(pts1.size(), 18);

    vector<balltree_t::ngb_t> ngb(3);
    for(auto &pt: pts1){
        auto k_used = tr.nearest_k(pt, ngb);
        ASSERT_EQ( k_used, 1 );
        EXPECT_EQ( ngb[0].node_idx, 0 );
    }

    ngb.resize(1);
    for(auto &pt: pts1){
        auto k_used = tr.nearest_k(pt, ngb);
        ASSERT_EQ( k_used, 1 );
        EXPECT_EQ( ngb[0].node_idx, 0 );
    }
}}

TEST_F(BallTreeRawEmptyPaddingTest, QueryNearestKWith2Points){
for(int i_repeat=0; i_repeat<n_repeat_min; ++i_repeat){
    auto tr = get_tree_with_random_points(2);
    
    auto pts1 = get_random_points(10), 
        pts2 = get_cubic_layout_points();
    pts1.insert(pts1.end(), pts2.begin(), pts2.end());
    ASSERT_EQ(pts1.size(), 18);

    vector<balltree_t::ngb_t> ngb(3);
    for(auto &pt: pts1){
        auto k_used = tr.nearest_k(pt, ngb);
        ASSERT_EQ( k_used, 2 );
        index_t ids[2] { ngb[0].node_idx, ngb[1].node_idx };
        EXPECT_THAT(ids, gt::UnorderedElementsAre(1,2));
    }

    ngb.resize(2);
    for(auto &pt: pts1){
        auto k_used = tr.nearest_k(pt, ngb);
        ASSERT_EQ( k_used, 2 );
        index_t ids[2] { ngb[0].node_idx, ngb[1].node_idx };
        EXPECT_THAT(ids, gt::UnorderedElementsAre(1,2));
    }

    ngb.resize(1);
    for(auto &pt: pts1){
        auto k_used = tr.nearest_k(pt, ngb);
        ASSERT_EQ( k_used, 1 );
        EXPECT_THAT(ngb[0].node_idx, gt::AnyOf(1,2) );
    }
}}

TEST_F(BallTreeRawEmptyPaddingTest, QueryNearestKWithManyPoints){
for(int i_repeat=0; i_repeat<n_repeat_min; ++i_repeat){
    index_t n_pts = 50;
    auto tr = get_tree_with_random_points(n_pts);
    const auto & nds = tr.nodes();

    auto pts1 = get_random_points(10), 
        pts2 = get_cubic_layout_points();
    pts1.insert(pts1.end(), pts2.begin(), pts2.end());
    ASSERT_EQ(pts1.size(), 18);

    for(index_t k=0; k<100; ++k){
        vector<balltree_t::ngb_t> ngb(k);
        index_t n_dst = std::min(k, n_pts);
        
        for(auto &pt: pts1){
            auto k_used = tr.nearest_k(pt, ngb);
            ASSERT_EQ(k_used, n_dst);
            std::sort(ngb.data(), ngb.data()+k_used);

            auto ngb_dst = brute_force_nearest_k(nds, pt, k);
            for(index_t i=0; i<k_used; ++i){
                EXPECT_LT( ngb[i].node_idx, nds.size() );
                EXPECT_LT(std::fabs(ngb[i].r_sq-ngb_dst[i].r_sq), 1.0e-4)
                    << "k=" << k << ", i=" << i << endl
                    << "Dest point (" << ngb_dst[i].node_idx << "): " <<
                    nds[ ngb_dst[i].node_idx ] << endl << "Got (" <<
                    ngb[i].node_idx << "): " << nds[ ngb[i].node_idx ] 
                    << endl << "Distances^2 are " << ngb_dst[i].r_sq
                    << " and " << ngb[i].r_sq;
            }
        }
    }

}}

TEST_F(BallTreeRawEmptyPaddingTest, QueryCountRectWithEmptyTree){
for(int i_repeat=0; i_repeat<n_repeat_min; ++i_repeat){

    balltree_t tr;
    
    auto pts1 = get_random_points(10), 
        pts2 = get_cubic_layout_points();
    pts1.insert(pts1.end(), pts2.begin(), pts2.end());
    ASSERT_EQ(pts1.size(), 18);
    float szs[] = {0.1f, 0.2f, 0.5f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f};
    for(auto &pt: pts1){
        for(auto &sz: szs){
            auto cnt = tr.count_nodes_rect({pt-sz, pt+sz});
            EXPECT_EQ(cnt, 0) << "Point " << pt << ", query size " << sz;
        }
    }
}}

TEST_F(BallTreeRawEmptyPaddingTest, QueryCountRectWith1Point){
for(int i_repeat=0; i_repeat<n_repeat_min; ++i_repeat){

    auto tr = get_tree_with_random_points(1);
    
    auto pts1 = get_random_points(10), 
        pts2 = get_cubic_layout_points();
    pts1.insert(pts1.end(), pts2.begin(), pts2.end());
    ASSERT_EQ(pts1.size(), 18);
    float szs[] = {0.1f, 0.2f, 0.5f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f};

    for(auto &pt: pts1){
        for(auto &sz: szs){
            auto cnt = tr.count_nodes_rect({pt-sz, pt+sz});
            EXPECT_THAT(cnt, gt::AnyOf(0,1));
            if( sz > 1.5f ) 
                EXPECT_EQ(cnt, 1);
        }
    }
}}

TEST_F(BallTreeRawEmptyPaddingTest, QueryCountRectWith2Points){
for(int i_repeat=0; i_repeat<n_repeat_min; ++i_repeat){

    auto tr = get_tree_with_random_points(2);
    
    auto pts1 = get_random_points(10), 
        pts2 = get_cubic_layout_points();
    pts1.insert(pts1.end(), pts2.begin(), pts2.end());
    ASSERT_EQ(pts1.size(), 18);
    float szs[] = {0.1f, 0.2f, 0.5f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f};

    for(auto &pt: pts1){
        for(auto &sz: szs){
            auto cnt = tr.count_nodes_rect({pt-sz, pt+sz});
            EXPECT_THAT(cnt, gt::AnyOf(0,1,2));
            if( sz > 1.5f ) 
                EXPECT_EQ(cnt, 2);
        }
    }
}}

TEST_F(BallTreeRawEmptyPaddingTest, QueryCountRectWithRegularPoints){
for(int i_repeat=0; i_repeat<n_repeat_min; ++i_repeat){

    auto pts = get_cubic_layout_points();
    balltree_t tr;
    tr.construct(pts);

    for(auto &pt: pts){
        for(auto sz: {0.1f, 0.2f, 0.3f, 0.5f}){
            auto cnt = tr.count_nodes_rect({pt-sz, pt+sz});
            EXPECT_EQ(cnt, 1);
        }
        for(auto sz: {0.1f, 0.2f, 0.3f, 0.5f}){
            auto cnt = tr.count_nodes_rect({pt+0.05f, pt+sz});
            EXPECT_EQ(cnt, 0);
        }
        for(auto sz: {1.1f, 1.5f, 5.0f, 10.0f}){
            auto cnt = tr.count_nodes_rect({pt-sz, pt+sz});
            EXPECT_EQ(cnt, 8);
        }
    }
}}

TEST_F(BallTreeRawEmptyPaddingTest, QueryCountRectWithManyPoints){
for(int i_repeat=0; i_repeat<n_repeat_min; ++i_repeat){

    int n_pts = 50;
    auto tr = get_tree_with_random_points(n_pts);
    auto &nds = tr.nodes();

    for(auto &n: nds){
        if( n.size() != 1 ) continue;
        for(auto sz: {0.1f, 0.2f, 0.3f, 0.5f}){
            auto cnt = tr.count_nodes_rect({n.center()-sz, n.center()+sz});
            EXPECT_GE(cnt, 1);
            EXPECT_LE(cnt, n_pts);
        }
        for(auto sz: {1.1f, 2.0f, 5.0f, 10.0f}){
            auto cnt = tr.count_nodes_rect({n.center()-sz, n.center()+sz});
            EXPECT_EQ(cnt, n_pts);
        }
    }

    auto pts = get_random_points(n_pts);
    for(auto &pt: pts){
        for(auto sz: {0.1f, 0.2f, 0.3f, 0.5f, 1.0f, 2.0f}){
            balltree_t::rect_t r {pt.pos()-sz, pt.pos()+sz};
            auto cnt = tr.count_nodes_rect(r),
                cnt_dst = brute_force_count_nodes_rect(nds, r);
            EXPECT_EQ(cnt, cnt_dst) << "Query rect = " 
                << r << ", size = " << sz;
        }
    }
}}

TEST_F(BallTreeRawEmptyPaddingTest, QueryCountSphereWithEmptyTree){
for(int i_repeat=0; i_repeat<n_repeat_min; ++i_repeat){

    balltree_t tr;
    
    auto pts1 = get_random_points(10), 
        pts2 = get_cubic_layout_points();
    pts1.insert(pts1.end(), pts2.begin(), pts2.end());
    ASSERT_EQ(pts1.size(), 18);
    float szs[] = {0.1f, 0.2f, 0.5f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f};
    for(auto &pt: pts1){
        for(auto &sz: szs){
            auto cnt = tr.count_nodes_sphere({pt, sz});
            EXPECT_EQ(cnt, 0) << "Point " << pt << ", query size " << sz;
        }
    }
}}

TEST_F(BallTreeRawEmptyPaddingTest, QueryCountSphereWith1Point){
for(int i_repeat=0; i_repeat<n_repeat_min; ++i_repeat){

    auto tr = get_tree_with_random_points(1);
    
    auto pts1 = get_random_points(10), 
        pts2 = get_cubic_layout_points();
    pts1.insert(pts1.end(), pts2.begin(), pts2.end());
    ASSERT_EQ(pts1.size(), 18);
    float szs[] = {0.1f, 0.2f, 0.5f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f};

    for(auto &pt: pts1){
        for(auto &sz: szs){
            auto cnt = tr.count_nodes_sphere({pt, sz});
            EXPECT_THAT(cnt, gt::AnyOf(0,1));
            if( sz > 1.5f ) 
                EXPECT_EQ(cnt, 1);
        }
    }
}}

TEST_F(BallTreeRawEmptyPaddingTest, QueryCountSphereWith2Points){
for(int i_repeat=0; i_repeat<n_repeat_min; ++i_repeat){

    auto tr = get_tree_with_random_points(2);
    
    auto pts1 = get_random_points(10), 
        pts2 = get_cubic_layout_points();
    pts1.insert(pts1.end(), pts2.begin(), pts2.end());
    ASSERT_EQ(pts1.size(), 18);
    float szs[] = {0.1f, 0.2f, 0.5f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f};

    for(auto &pt: pts1){
        for(auto &sz: szs){
            auto cnt = tr.count_nodes_sphere({pt, sz});
            EXPECT_THAT(cnt, gt::AnyOf(0,1,2));
            if( sz > 2.0f ) 
                EXPECT_EQ(cnt, 2);
        }
    }
}}

TEST_F(BallTreeRawEmptyPaddingTest, QueryCountSphereWithRegularPoints){
for(int i_repeat=0; i_repeat<n_repeat_min; ++i_repeat){

    auto pts = get_cubic_layout_points();
    balltree_t tr;
    tr.construct(pts);

    for(auto &pt: pts){
        for(auto sz: {0.1f, 0.2f, 0.3f, 0.5f}){
            auto cnt = tr.count_nodes_sphere({pt, sz});
            EXPECT_EQ(cnt, 1);
        }
        for(auto sz: {0.1f, 0.2f, 0.3f, 0.5f}){
            auto cnt = tr.count_nodes_sphere({pt+0.5f, sz});
            EXPECT_EQ(cnt, 0);
        }
        for(auto sz: {2.5f, 5.0f, 10.0f}){
            auto cnt = tr.count_nodes_sphere({pt, sz});
            EXPECT_EQ(cnt, 8);
        }
    }
}}

TEST_F(BallTreeRawEmptyPaddingTest, QueryCountSphereWithManyPoints){
for(int i_repeat=0; i_repeat<n_repeat_min; ++i_repeat){

    int n_pts = 50;
    auto tr = get_tree_with_random_points(n_pts);
    auto &nds = tr.nodes();

    for(auto &n: nds){
        if( n.size() != 1 ) continue;

        for(auto sz: {0.1f, 0.2f, 0.3f, 0.5f}){
            auto cnt = tr.count_nodes_sphere({n.center(), sz});
            EXPECT_GE(cnt, 1);
            EXPECT_LE(cnt, n_pts);
        }
        for(auto sz: {2.1f, 2.0f, 5.0f, 10.0f}){
            auto cnt = tr.count_nodes_sphere({n.center(), sz});
            EXPECT_EQ(cnt, n_pts);
        }
    }

    auto pts = get_random_points(n_pts);
    for(auto &pt: pts){
        for(auto sz: {0.1f, 0.2f, 0.3f, 0.5f, 1.0f, 2.0f}){
            balltree_t::sphere_t s {pt.pos(), sz};
            auto cnt = tr.count_nodes_sphere(s),
                cnt_dst = brute_force_count_nodes_sphere(nds, s);
            EXPECT_EQ(cnt, cnt_dst) << "Query sphere = " << s;
        }
    }
}}

class BallTreeRawIntPaddingTest : public gt::Test {
public:
    using kdp_t = KDPoint<float, 3, sizeof(int)>;
    using balltree_t = BallTree<kdp_t, int>;
    using node_t = balltree_t::node_t;
    using point_t = balltree_t::point_t;
    using pos_t = balltree_t::pos_t;
    using index_t = balltree_t::index_t;
    using float_t = balltree_t::float_t;

    using cstr_pl_t = balltree_t::construct_policy_t;

    static constexpr int n_repeat_min = 10, 
        n_repeat_max = 10000;

    BallTreeRawIntPaddingTest() {}
    ~BallTreeRawIntPaddingTest() override {}
    void SetUp() override {}
    void TearDown() override {}

    vector<balltree_t::construct_policy_t> get_cstr_policy_table() 
    {
        using pl_t = balltree_t::construct_policy_t;
        using alg_t = pl_t::algorithm_t;
        using spl_t = pl_t::split_axis_t;
        using ins_fav_t = pl_t::insert_favor_t;
        
        vector<pl_t> out;
        
        for(auto spl: { 
            spl_t::MAX_EXTREME,
            spl_t::MAX_VARIANCE, 
            spl_t::ORDERED, 
            spl_t::RANDOM })
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

    vector<kdp_t> get_random_points(index_t n_pts) 
    {
        vector<kdp_t> pts(n_pts);
        for(index_t i=0; i<n_pts; ++i){
            rand( pts[i].pos().begin(), pts[i].pos().end() );
        }
        return pts;
    }

    vector<kdp_t> get_cubic_layout_points()
    {
        vector<kdp_t> pts;
        for(auto x0: {0.0f, 1.0f})
        for(auto x1: {0.0f, 1.0f})
        for(auto x2: {0.0f, 1.0f})
            pts.push_back({x0,x1,x2});
        return pts;
    }

    balltree_t get_tree_with_random_points(index_t n_pts, 
        cstr_pl_t pl = cstr_pl_t()) 
    {
        balltree_t tr;
        auto pts = get_random_points(n_pts);
        tr.construct(pts, pl);
        return tr;
    }

    balltree_t::ngb_t brute_force_nearest(const vector<node_t> &nds, 
        point_t p)
    {
        balltree_t::ngb_t ngb { -1, std::numeric_limits<float_t>::max() };
        for(auto &n: nds){
            if(n.size() != 1) continue;
            auto r_sq = (n.center() - p).r_sq();
            if( r_sq < ngb.r_sq ) {
                ngb.r_sq = r_sq;
                ngb.node_idx = index_t( &n - nds.data() );
            }
        }
        return ngb;
    }

    vector<balltree_t::ngb_t> brute_force_nearest_k(const vector<node_t> &nds, 
        point_t p, index_t k) 
    {
        vector<balltree_t::ngb_t> ngbs;
        for(auto &n: nds){
            if( n.size() != 1 ) continue;
            auto r_sq = (n.center() - p).r_sq();
            ngbs.push_back({ index_t(&n - nds.data()), r_sq});
        }
        std::sort(ngbs.begin(), ngbs.end());
        if( ngbs.size() > k ) ngbs.resize(k);
        return ngbs;
    }

    index_t brute_force_count_nodes_rect(const vector<node_t> &nds, 
        balltree_t::rect_t r) 
    {
        index_t cnt {};
        for(auto &n: nds){
            if( n.size() != 1 ) continue;
            if( r.contains(n.center()) ) ++cnt;
        }
        return cnt;
    }
    index_t brute_force_count_nodes_sphere(const vector<node_t> &nds, 
        balltree_t::sphere_t s) 
    {
        index_t cnt {};
        for(auto &n: nds){
            if( n.size() != 1 ) continue;
            if( s.contains(n.center()) ) ++cnt;
        }
        return cnt;
    }
};

TEST_F(BallTreeRawIntPaddingTest, DefaultInitialization){
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


TEST_F(BallTreeRawIntPaddingTest, InfoPrinting){
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

TEST_F(BallTreeRawIntPaddingTest, NodeTypeConstruction) {
for(int i=0; i<n_repeat_max; ++i){
    node_t n1;

    node_t n2 {{1,2,3}};
    ASSERT_EQ(n2.r(), float_t(0));
    ASSERT_TRUE( (n2.center().pos() == pos_t{1,2,3}).all() );

    node_t n3 {{1,2,3},4};
    ASSERT_EQ(n3.r(), float_t(4));
    ASSERT_TRUE( (n3.center().pos() == pos_t{1,2,3}).all() );
    
    char pad[0] {};
    node_t n4 {{1,2,3},4,pad};
    ASSERT_EQ(n4.r(), float_t(4));
    ASSERT_TRUE( (n4.center().pos() == pos_t{1,2,3}).all() );

    node_t n5 {{1,2,3},4,5,pad};
    ASSERT_EQ(n5.r(), float_t(4));
    ASSERT_EQ(n5.size(), 5);
    ASSERT_TRUE( (n5.center().pos() == pos_t{1,2,3}).all() );
}}

TEST_F(BallTreeRawIntPaddingTest, NodeTypeAttributesSetGet) {
for(int i=0; i<n_repeat_max; ++i){
    node_t n1;
    n1.center() = point_t{1,2,3};
    n1.r() = 4;
    n1.size() = 5;
    char pad[0] {};
    n1.fill_pad(pad);

    ASSERT_EQ(n1.r(), float_t(4));
    ASSERT_EQ(n1.size(), 5);
    ASSERT_TRUE( (n1.center().pos() == pos_t{1,2,3}).all() );

    ASSERT_EQ(sizeof(n1.pad()), balltree_t::PADDING);
}}

TEST_F(BallTreeRawIntPaddingTest, EmptyTree) {
for(int i=0; i<n_repeat_max; ++i){
    balltree_t ballt;
    ASSERT_EQ(ballt.nodes().size(), 0);
}}

TEST_F(BallTreeRawIntPaddingTest, Construct1Node){
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

TEST_F(BallTreeRawIntPaddingTest, Construct2Nodes){
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

TEST_F(BallTreeRawIntPaddingTest, ConstructionWithDefaultPolicy){
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

TEST_F(BallTreeRawIntPaddingTest, ConstructionWithKDPolicy){
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

TEST_F(BallTreeRawIntPaddingTest, ConstructionWithTopDownPolicy){
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

TEST_F(BallTreeRawIntPaddingTest, ConstructionWithInsertionPolicy){
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

TEST_F(BallTreeRawIntPaddingTest, ConstructionWithBottomUpPolicy){
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

TEST_F(BallTreeRawIntPaddingTest, CopyMoveOperations){
for(int i_repeat=0; i_repeat<n_repeat_min; ++i_repeat){
    {
        balltree_t tr0;
        auto cpy = tr0;
        ASSERT_NE(&cpy, &tr0);
        ASSERT_EQ(tr0.nodes().size(), 0);
        ASSERT_EQ(cpy.nodes().size(), tr0.nodes().size());
        ASSERT_EQ(cpy.tree_info().max_depth(), tr0.tree_info().max_depth());
    }
    {
        auto tr = get_tree_with_random_points(500);
        auto cpy = tr;
        ASSERT_NE(&cpy, &tr);
        ASSERT_EQ(cpy.nodes().size(), tr.nodes().size());
        ASSERT_EQ(cpy.tree_info().max_depth(), tr.tree_info().max_depth());
        index_t n_nds = (index_t)tr.nodes().size();
        for(index_t i=0; i<n_nds; ++i){
            auto &n1 = cpy.nodes()[i], &n2 = cpy.nodes()[i];
            ASSERT_EQ(n1.size(), n2.size());
            ASSERT_TRUE( (n1.center().pos() == n2.center().pos()).all() );
        }
    }
}
}

TEST_F(BallTreeRawIntPaddingTest, NodeIndexAccess){
for(int i_repeat=0; i_repeat<n_repeat_min; ++i_repeat){
    auto tr = get_tree_with_random_points(500);
    auto &nds = tr.nodes();
    index_t n_nds = (index_t)nds.size();
    for(index_t i=0; i<n_nds; ++i){
        auto &n = nds[i];
        if( n.size() == 1 ) continue;
        auto i_lc = tr.left_child_idx(i), i_rc = tr.right_child_idx(i), 
            i_rs = tr.right_sibling_idx(i_lc);
        ASSERT_EQ(i_lc, i+1);
        ASSERT_EQ(i_rc, i_lc+nds[i_lc].size());
        ASSERT_EQ(i_rc, i_rs);
    }
}
}

TEST_F(BallTreeRawIntPaddingTest, TreeInfoAccess){
for(int i_repeat=0; i_repeat<n_repeat_min; ++i_repeat){
    balltree_t tr0;
    EXPECT_EQ(tr0.tree_info().max_depth(), 0);

    auto tr1 = get_tree_with_random_points(1);
    EXPECT_EQ(tr1.tree_info().max_depth(), 1);

    auto tr2 = get_tree_with_random_points(2);
    EXPECT_EQ(tr2.tree_info().max_depth(), 2);

    auto tr3 = get_tree_with_random_points(3);
    EXPECT_EQ(tr3.tree_info().max_depth(), 3);

    auto tr4 = get_tree_with_random_points(8);
    EXPECT_GE(tr4.tree_info().max_depth(), 4);
    EXPECT_LT(tr4.tree_info().max_depth(), 8);
}
}

TEST_F(BallTreeRawIntPaddingTest, Argsort){
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

TEST_F(BallTreeRawIntPaddingTest, QueryNearestEmptyTree){
for(int i_repeat=0; i_repeat<n_repeat_min; ++i_repeat){
    balltree_t tr;
    
    auto pts1 = get_random_points(10), 
        pts2 = get_cubic_layout_points();
    pts1.insert(pts1.end(), pts2.begin(), pts2.end());

    ASSERT_EQ(pts1.size(), 18);
    
    for(auto &pt: pts1){
        auto ngb = tr.nearest(pt);
        EXPECT_EQ( ngb.r_sq, std::numeric_limits<balltree_t::float_t>::max() );
        EXPECT_EQ( ngb.node_idx, node_t::idxNULL );
    }
}}

TEST_F(BallTreeRawIntPaddingTest, QueryNearest1Point){
for(int i_repeat=0; i_repeat<n_repeat_min; ++i_repeat){
    balltree_t tr;
    float_t x = float_t(i_repeat+1.);
    vector<kdp_t> pts { {x,x,x} };

    tr.construct(pts);

    point_t p { 0.1, 0.0, 0.0 };
    auto ngb = tr.nearest(p);
    auto &nds = tr.nodes();
    ASSERT_LT(ngb.node_idx, nds.size());
    auto &n = nds[ngb.node_idx];
    EXPECT_EQ(n.size(), 1);
    EXPECT_LT((n.center() - point_t{x,x,x}).r(), 1.0e-4);
}}

TEST_F(BallTreeRawIntPaddingTest, QueryNearest2Point){
for(int i_repeat=0; i_repeat<n_repeat_min; ++i_repeat){
    balltree_t tr;
    float_t x = float_t(i_repeat+1.);
    vector<kdp_t> pts { {0,0,0}, {x,x,x} };

    tr.construct(pts);

    point_t p { 0.1, 0.0, 0.0 };
    auto ngb = tr.nearest(p);
    EXPECT_LT(ngb.r_sq, 0.1+1.0e-4);

    auto &nds = tr.nodes();
    ASSERT_LT(ngb.node_idx, nds.size());
    auto &n = nds[ngb.node_idx];
    EXPECT_EQ(n.size(), 1);
    EXPECT_LT((n.center() - point_t{0,0,0}).r(), 1.0e-4);
}}

TEST_F(BallTreeRawIntPaddingTest, QueryNearestWithManyPoints){
for(int i_repeat=0; i_repeat<n_repeat_min; ++i_repeat){
    auto tr = get_tree_with_random_points(50);
    auto pts = get_random_points(50);
    for(auto &pt: pts){
        auto ngb = tr.nearest(pt);
        auto ngb_dst = brute_force_nearest(tr.nodes(), pt);
        EXPECT_LT( std::fabs(ngb.r_sq-ngb_dst.r_sq), 1.0e-4 ) << 
            "Dest r^2 = " << ngb_dst.r_sq << ". Got " << ngb.r_sq << endl; 
    }
}}

TEST_F(BallTreeRawIntPaddingTest, QueryNearestDefaultPolicy){
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

TEST_F(BallTreeRawIntPaddingTest, QueryNearestSharedPolicy){
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

TEST_F(BallTreeRawIntPaddingTest, QueryNearestK0){
for(int i_repeat=0; i_repeat<n_repeat_min; ++i_repeat){
    balltree_t tr;
    
    auto pts1 = get_random_points(10), 
        pts2 = get_cubic_layout_points();
    pts1.insert(pts1.end(), pts2.begin(), pts2.end());

    ASSERT_EQ(pts1.size(), 18);
    
    vector<balltree_t::ngb_t> ngb;
    for(auto &pt: pts1){
        auto k_used = tr.nearest_k(pt, ngb);
        ASSERT_EQ(k_used, 0);
    }
}
}

TEST_F(BallTreeRawIntPaddingTest, QueryNearestKWithEmptyTree){
for(int i_repeat=0; i_repeat<n_repeat_min; ++i_repeat){
    balltree_t tr;
    
    auto pts1 = get_random_points(10), 
        pts2 = get_cubic_layout_points();
    pts1.insert(pts1.end(), pts2.begin(), pts2.end());

    ASSERT_EQ(pts1.size(), 18);
    vector<balltree_t::ngb_t> ngb(3);
    for(auto &pt: pts1){
        auto k_used = tr.nearest_k(pt, ngb);
        EXPECT_EQ( k_used, 0 );
    }
}}

TEST_F(BallTreeRawIntPaddingTest, QueryNearestKWith1Point){
for(int i_repeat=0; i_repeat<n_repeat_min; ++i_repeat){
    auto tr = get_tree_with_random_points(1);
    
    auto pts1 = get_random_points(10), 
        pts2 = get_cubic_layout_points();
    pts1.insert(pts1.end(), pts2.begin(), pts2.end());
    ASSERT_EQ(pts1.size(), 18);

    vector<balltree_t::ngb_t> ngb(3);
    for(auto &pt: pts1){
        auto k_used = tr.nearest_k(pt, ngb);
        ASSERT_EQ( k_used, 1 );
        EXPECT_EQ( ngb[0].node_idx, 0 );
    }

    ngb.resize(1);
    for(auto &pt: pts1){
        auto k_used = tr.nearest_k(pt, ngb);
        ASSERT_EQ( k_used, 1 );
        EXPECT_EQ( ngb[0].node_idx, 0 );
    }
}}

TEST_F(BallTreeRawIntPaddingTest, QueryNearestKWith2Points){
for(int i_repeat=0; i_repeat<n_repeat_min; ++i_repeat){
    auto tr = get_tree_with_random_points(2);
    
    auto pts1 = get_random_points(10), 
        pts2 = get_cubic_layout_points();
    pts1.insert(pts1.end(), pts2.begin(), pts2.end());
    ASSERT_EQ(pts1.size(), 18);

    vector<balltree_t::ngb_t> ngb(3);
    for(auto &pt: pts1){
        auto k_used = tr.nearest_k(pt, ngb);
        ASSERT_EQ( k_used, 2 );
        index_t ids[2] { ngb[0].node_idx, ngb[1].node_idx };
        EXPECT_THAT(ids, gt::UnorderedElementsAre(1,2));
    }

    ngb.resize(2);
    for(auto &pt: pts1){
        auto k_used = tr.nearest_k(pt, ngb);
        ASSERT_EQ( k_used, 2 );
        index_t ids[2] { ngb[0].node_idx, ngb[1].node_idx };
        EXPECT_THAT(ids, gt::UnorderedElementsAre(1,2));
    }

    ngb.resize(1);
    for(auto &pt: pts1){
        auto k_used = tr.nearest_k(pt, ngb);
        ASSERT_EQ( k_used, 1 );
        EXPECT_THAT(ngb[0].node_idx, gt::AnyOf(1,2) );
    }
}}

TEST_F(BallTreeRawIntPaddingTest, QueryNearestKWithManyPoints){
for(int i_repeat=0; i_repeat<n_repeat_min; ++i_repeat){
    index_t n_pts = 50;
    auto tr = get_tree_with_random_points(n_pts);
    const auto & nds = tr.nodes();

    auto pts1 = get_random_points(10), 
        pts2 = get_cubic_layout_points();
    pts1.insert(pts1.end(), pts2.begin(), pts2.end());
    ASSERT_EQ(pts1.size(), 18);

    for(index_t k=0; k<100; ++k){
        vector<balltree_t::ngb_t> ngb(k);
        index_t n_dst = std::min(k, n_pts);
        
        for(auto &pt: pts1){
            auto k_used = tr.nearest_k(pt, ngb);
            ASSERT_EQ(k_used, n_dst);
            std::sort(ngb.data(), ngb.data()+k_used);

            auto ngb_dst = brute_force_nearest_k(nds, pt, k);
            for(index_t i=0; i<k_used; ++i){
                EXPECT_LT( ngb[i].node_idx, nds.size() );
                EXPECT_LT(std::fabs(ngb[i].r_sq-ngb_dst[i].r_sq), 1.0e-4)
                    << "k=" << k << ", i=" << i << endl
                    << "Dest point (" << ngb_dst[i].node_idx << "): " <<
                    nds[ ngb_dst[i].node_idx ] << endl << "Got (" <<
                    ngb[i].node_idx << "): " << nds[ ngb[i].node_idx ] 
                    << endl << "Distances^2 are " << ngb_dst[i].r_sq
                    << " and " << ngb[i].r_sq;
            }
        }
    }

}}

TEST_F(BallTreeRawIntPaddingTest, QueryCountRectWithEmptyTree){
for(int i_repeat=0; i_repeat<n_repeat_min; ++i_repeat){

    balltree_t tr;
    
    auto pts1 = get_random_points(10), 
        pts2 = get_cubic_layout_points();
    pts1.insert(pts1.end(), pts2.begin(), pts2.end());
    ASSERT_EQ(pts1.size(), 18);
    float szs[] = {0.1f, 0.2f, 0.5f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f};
    for(auto &pt: pts1){
        for(auto &sz: szs){
            auto cnt = tr.count_nodes_rect({pt-sz, pt+sz});
            EXPECT_EQ(cnt, 0) << "Point " << pt << ", query size " << sz;
        }
    }
}}

TEST_F(BallTreeRawIntPaddingTest, QueryCountRectWith1Point){
for(int i_repeat=0; i_repeat<n_repeat_min; ++i_repeat){

    auto tr = get_tree_with_random_points(1);
    
    auto pts1 = get_random_points(10), 
        pts2 = get_cubic_layout_points();
    pts1.insert(pts1.end(), pts2.begin(), pts2.end());
    ASSERT_EQ(pts1.size(), 18);
    float szs[] = {0.1f, 0.2f, 0.5f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f};

    for(auto &pt: pts1){
        for(auto &sz: szs){
            auto cnt = tr.count_nodes_rect({pt-sz, pt+sz});
            EXPECT_THAT(cnt, gt::AnyOf(0,1));
            if( sz > 1.5f ) 
                EXPECT_EQ(cnt, 1);
        }
    }
}}

TEST_F(BallTreeRawIntPaddingTest, QueryCountRectWith2Points){
for(int i_repeat=0; i_repeat<n_repeat_min; ++i_repeat){

    auto tr = get_tree_with_random_points(2);
    
    auto pts1 = get_random_points(10), 
        pts2 = get_cubic_layout_points();
    pts1.insert(pts1.end(), pts2.begin(), pts2.end());
    ASSERT_EQ(pts1.size(), 18);
    float szs[] = {0.1f, 0.2f, 0.5f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f};

    for(auto &pt: pts1){
        for(auto &sz: szs){
            auto cnt = tr.count_nodes_rect({pt-sz, pt+sz});
            EXPECT_THAT(cnt, gt::AnyOf(0,1,2));
            if( sz > 1.5f ) 
                EXPECT_EQ(cnt, 2);
        }
    }
}}

TEST_F(BallTreeRawIntPaddingTest, QueryCountRectWithRegularPoints){
for(int i_repeat=0; i_repeat<n_repeat_min; ++i_repeat){

    auto pts = get_cubic_layout_points();
    balltree_t tr;
    tr.construct(pts);

    for(auto &pt: pts){
        for(auto sz: {0.1f, 0.2f, 0.3f, 0.5f}){
            auto cnt = tr.count_nodes_rect({pt-sz, pt+sz});
            EXPECT_EQ(cnt, 1);
        }
        for(auto sz: {0.1f, 0.2f, 0.3f, 0.5f}){
            auto cnt = tr.count_nodes_rect({pt+0.05f, pt+sz});
            EXPECT_EQ(cnt, 0);
        }
        for(auto sz: {1.1f, 1.5f, 5.0f, 10.0f}){
            auto cnt = tr.count_nodes_rect({pt-sz, pt+sz});
            EXPECT_EQ(cnt, 8);
        }
    }
}}

TEST_F(BallTreeRawIntPaddingTest, QueryCountRectWithManyPoints){
for(int i_repeat=0; i_repeat<n_repeat_min; ++i_repeat){

    int n_pts = 50;
    auto tr = get_tree_with_random_points(n_pts);
    auto &nds = tr.nodes();

    for(auto &n: nds){
        if( n.size() != 1 ) continue;
        for(auto sz: {0.1f, 0.2f, 0.3f, 0.5f}){
            auto cnt = tr.count_nodes_rect({n.center()-sz, n.center()+sz});
            EXPECT_GE(cnt, 1);
            EXPECT_LE(cnt, n_pts);
        }
        for(auto sz: {1.1f, 2.0f, 5.0f, 10.0f}){
            auto cnt = tr.count_nodes_rect({n.center()-sz, n.center()+sz});
            EXPECT_EQ(cnt, n_pts);
        }
    }

    auto pts = get_random_points(n_pts);
    for(auto &pt: pts){
        for(auto sz: {0.1f, 0.2f, 0.3f, 0.5f, 1.0f, 2.0f}){
            balltree_t::rect_t r {pt.pos()-sz, pt.pos()+sz};
            auto cnt = tr.count_nodes_rect(r),
                cnt_dst = brute_force_count_nodes_rect(nds, r);
            EXPECT_EQ(cnt, cnt_dst) << "Query rect = " 
                << r << ", size = " << sz;
        }
    }
}}

TEST_F(BallTreeRawIntPaddingTest, QueryCountSphereWithEmptyTree){
for(int i_repeat=0; i_repeat<n_repeat_min; ++i_repeat){

    balltree_t tr;
    
    auto pts1 = get_random_points(10), 
        pts2 = get_cubic_layout_points();
    pts1.insert(pts1.end(), pts2.begin(), pts2.end());
    ASSERT_EQ(pts1.size(), 18);
    float szs[] = {0.1f, 0.2f, 0.5f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f};
    for(auto &pt: pts1){
        for(auto &sz: szs){
            auto cnt = tr.count_nodes_sphere({pt, sz});
            EXPECT_EQ(cnt, 0) << "Point " << pt << ", query size " << sz;
        }
    }
}}

TEST_F(BallTreeRawIntPaddingTest, QueryCountSphereWith1Point){
for(int i_repeat=0; i_repeat<n_repeat_min; ++i_repeat){

    auto tr = get_tree_with_random_points(1);
    
    auto pts1 = get_random_points(10), 
        pts2 = get_cubic_layout_points();
    pts1.insert(pts1.end(), pts2.begin(), pts2.end());
    ASSERT_EQ(pts1.size(), 18);
    float szs[] = {0.1f, 0.2f, 0.5f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f};

    for(auto &pt: pts1){
        for(auto &sz: szs){
            auto cnt = tr.count_nodes_sphere({pt, sz});
            EXPECT_THAT(cnt, gt::AnyOf(0,1));
            if( sz > 1.5f ) 
                EXPECT_EQ(cnt, 1);
        }
    }
}}

TEST_F(BallTreeRawIntPaddingTest, QueryCountSphereWith2Points){
for(int i_repeat=0; i_repeat<n_repeat_min; ++i_repeat){

    auto tr = get_tree_with_random_points(2);
    
    auto pts1 = get_random_points(10), 
        pts2 = get_cubic_layout_points();
    pts1.insert(pts1.end(), pts2.begin(), pts2.end());
    ASSERT_EQ(pts1.size(), 18);
    float szs[] = {0.1f, 0.2f, 0.5f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f};

    for(auto &pt: pts1){
        for(auto &sz: szs){
            auto cnt = tr.count_nodes_sphere({pt, sz});
            EXPECT_THAT(cnt, gt::AnyOf(0,1,2));
            if( sz > 2.0f ) 
                EXPECT_EQ(cnt, 2);
        }
    }
}}

TEST_F(BallTreeRawIntPaddingTest, QueryCountSphereWithRegularPoints){
for(int i_repeat=0; i_repeat<n_repeat_min; ++i_repeat){

    auto pts = get_cubic_layout_points();
    balltree_t tr;
    tr.construct(pts);

    for(auto &pt: pts){
        for(auto sz: {0.1f, 0.2f, 0.3f, 0.5f}){
            auto cnt = tr.count_nodes_sphere({pt, sz});
            EXPECT_EQ(cnt, 1);
        }
        for(auto sz: {0.1f, 0.2f, 0.3f, 0.5f}){
            auto cnt = tr.count_nodes_sphere({pt+0.5f, sz});
            EXPECT_EQ(cnt, 0);
        }
        for(auto sz: {2.5f, 5.0f, 10.0f}){
            auto cnt = tr.count_nodes_sphere({pt, sz});
            EXPECT_EQ(cnt, 8);
        }
    }
}}

TEST_F(BallTreeRawIntPaddingTest, QueryCountSphereWithManyPoints){
for(int i_repeat=0; i_repeat<n_repeat_min; ++i_repeat){

    int n_pts = 50;
    auto tr = get_tree_with_random_points(n_pts);
    auto &nds = tr.nodes();

    for(auto &n: nds){
        if( n.size() != 1 ) continue;

        for(auto sz: {0.1f, 0.2f, 0.3f, 0.5f}){
            auto cnt = tr.count_nodes_sphere({n.center(), sz});
            EXPECT_GE(cnt, 1);
            EXPECT_LE(cnt, n_pts);
        }
        for(auto sz: {2.1f, 2.0f, 5.0f, 10.0f}){
            auto cnt = tr.count_nodes_sphere({n.center(), sz});
            EXPECT_EQ(cnt, n_pts);
        }
    }

    auto pts = get_random_points(n_pts);
    for(auto &pt: pts){
        for(auto sz: {0.1f, 0.2f, 0.3f, 0.5f, 1.0f, 2.0f}){
            balltree_t::sphere_t s {pt.pos(), sz};
            auto cnt = tr.count_nodes_sphere(s),
                cnt_dst = brute_force_count_nodes_sphere(nds, s);
            EXPECT_EQ(cnt, cnt_dst) << "Query sphere = " << s;
        }
    }
}}

class BallTreeRawLargePaddingTest : public gt::Test {
public:
    using kdp_t = KDPoint<float, 3, 1024>;
    using balltree_t = BallTree<kdp_t, int>;
    using node_t = balltree_t::node_t;
    using point_t = balltree_t::point_t;
    using pos_t = balltree_t::pos_t;
    using index_t = balltree_t::index_t;
    using float_t = balltree_t::float_t;

    using cstr_pl_t = balltree_t::construct_policy_t;

    static constexpr int n_repeat_min = 10, 
        n_repeat_max = 10000;

    BallTreeRawLargePaddingTest() {}
    ~BallTreeRawLargePaddingTest() override {}
    void SetUp() override {}
    void TearDown() override {}

    vector<balltree_t::construct_policy_t> get_cstr_policy_table() 
    {
        using pl_t = balltree_t::construct_policy_t;
        using alg_t = pl_t::algorithm_t;
        using spl_t = pl_t::split_axis_t;
        using ins_fav_t = pl_t::insert_favor_t;
        
        vector<pl_t> out;
        
        for(auto spl: { 
            spl_t::MAX_EXTREME,
            spl_t::MAX_VARIANCE, 
            spl_t::ORDERED, 
            spl_t::RANDOM })
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

    vector<kdp_t> get_random_points(index_t n_pts) 
    {
        vector<kdp_t> pts(n_pts);
        for(index_t i=0; i<n_pts; ++i){
            rand( pts[i].pos().begin(), pts[i].pos().end() );
        }
        return pts;
    }

    vector<kdp_t> get_cubic_layout_points()
    {
        vector<kdp_t> pts;
        for(auto x0: {0.0f, 1.0f})
        for(auto x1: {0.0f, 1.0f})
        for(auto x2: {0.0f, 1.0f})
            pts.push_back({x0,x1,x2});
        return pts;
    }

    balltree_t get_tree_with_random_points(index_t n_pts, 
        cstr_pl_t pl = cstr_pl_t()) 
    {
        balltree_t tr;
        auto pts = get_random_points(n_pts);
        tr.construct(pts, pl);
        return tr;
    }

    balltree_t::ngb_t brute_force_nearest(const vector<node_t> &nds, 
        point_t p)
    {
        balltree_t::ngb_t ngb { -1, std::numeric_limits<float_t>::max() };
        for(auto &n: nds){
            if(n.size() != 1) continue;
            auto r_sq = (n.center() - p).r_sq();
            if( r_sq < ngb.r_sq ) {
                ngb.r_sq = r_sq;
                ngb.node_idx = index_t( &n - nds.data() );
            }
        }
        return ngb;
    }

    vector<balltree_t::ngb_t> brute_force_nearest_k(const vector<node_t> &nds, 
        point_t p, index_t k) 
    {
        vector<balltree_t::ngb_t> ngbs;
        for(auto &n: nds){
            if( n.size() != 1 ) continue;
            auto r_sq = (n.center() - p).r_sq();
            ngbs.push_back({ index_t(&n - nds.data()), r_sq});
        }
        std::sort(ngbs.begin(), ngbs.end());
        if( ngbs.size() > k ) ngbs.resize(k);
        return ngbs;
    }

    index_t brute_force_count_nodes_rect(const vector<node_t> &nds, 
        balltree_t::rect_t r) 
    {
        index_t cnt {};
        for(auto &n: nds){
            if( n.size() != 1 ) continue;
            if( r.contains(n.center()) ) ++cnt;
        }
        return cnt;
    }
    index_t brute_force_count_nodes_sphere(const vector<node_t> &nds, 
        balltree_t::sphere_t s) 
    {
        index_t cnt {};
        for(auto &n: nds){
            if( n.size() != 1 ) continue;
            if( s.contains(n.center()) ) ++cnt;
        }
        return cnt;
    }
};

TEST_F(BallTreeRawLargePaddingTest, DefaultInitialization){
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

TEST_F(BallTreeRawLargePaddingTest, InfoPrinting){
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

TEST_F(BallTreeRawLargePaddingTest, NodeTypeConstruction) {
for(int i=0; i<n_repeat_max; ++i){
    node_t n1;

    node_t n2 {{1,2,3}};
    ASSERT_EQ(n2.r(), float_t(0));
    ASSERT_TRUE( (n2.center().pos() == pos_t{1,2,3}).all() );

    node_t n3 {{1,2,3},4};
    ASSERT_EQ(n3.r(), float_t(4));
    ASSERT_TRUE( (n3.center().pos() == pos_t{1,2,3}).all() );
    
    char pad[0] {};
    node_t n4 {{1,2,3},4,pad};
    ASSERT_EQ(n4.r(), float_t(4));
    ASSERT_TRUE( (n4.center().pos() == pos_t{1,2,3}).all() );

    node_t n5 {{1,2,3},4,5,pad};
    ASSERT_EQ(n5.r(), float_t(4));
    ASSERT_EQ(n5.size(), 5);
    ASSERT_TRUE( (n5.center().pos() == pos_t{1,2,3}).all() );
}}

TEST_F(BallTreeRawLargePaddingTest, NodeTypeAttributesSetGet) {
for(int i=0; i<n_repeat_max; ++i){
    node_t n1;
    n1.center() = point_t{1,2,3};
    n1.r() = 4;
    n1.size() = 5;
    char pad[0] {};
    n1.fill_pad(pad);

    ASSERT_EQ(n1.r(), float_t(4));
    ASSERT_EQ(n1.size(), 5);
    ASSERT_TRUE( (n1.center().pos() == pos_t{1,2,3}).all() );

    ASSERT_EQ(sizeof(n1.pad()), balltree_t::PADDING);
}}

TEST_F(BallTreeRawLargePaddingTest, EmptyTree) {
for(int i=0; i<n_repeat_max; ++i){
    balltree_t ballt;
    ASSERT_EQ(ballt.nodes().size(), 0);
}}

TEST_F(BallTreeRawLargePaddingTest, Construct1Node){
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

TEST_F(BallTreeRawLargePaddingTest, Construct2Nodes){
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

TEST_F(BallTreeRawLargePaddingTest, ConstructionWithDefaultPolicy){
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

TEST_F(BallTreeRawLargePaddingTest, ConstructionWithKDPolicy){
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

TEST_F(BallTreeRawLargePaddingTest, ConstructionWithTopDownPolicy){
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

TEST_F(BallTreeRawLargePaddingTest, ConstructionWithInsertionPolicy){
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

TEST_F(BallTreeRawLargePaddingTest, ConstructionWithBottomUpPolicy){
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

TEST_F(BallTreeRawLargePaddingTest, CopyMoveOperations){
for(int i_repeat=0; i_repeat<n_repeat_min; ++i_repeat){
    {
        balltree_t tr0;
        auto cpy = tr0;
        ASSERT_NE(&cpy, &tr0);
        ASSERT_EQ(tr0.nodes().size(), 0);
        ASSERT_EQ(cpy.nodes().size(), tr0.nodes().size());
        ASSERT_EQ(cpy.tree_info().max_depth(), tr0.tree_info().max_depth());
    }
    {
        auto tr = get_tree_with_random_points(500);
        auto cpy = tr;
        ASSERT_NE(&cpy, &tr);
        ASSERT_EQ(cpy.nodes().size(), tr.nodes().size());
        ASSERT_EQ(cpy.tree_info().max_depth(), tr.tree_info().max_depth());
        index_t n_nds = (index_t)tr.nodes().size();
        for(index_t i=0; i<n_nds; ++i){
            auto &n1 = cpy.nodes()[i], &n2 = cpy.nodes()[i];
            ASSERT_EQ(n1.size(), n2.size());
            ASSERT_TRUE( (n1.center().pos() == n2.center().pos()).all() );
        }
    }
}
}

TEST_F(BallTreeRawLargePaddingTest, NodeIndexAccess){
for(int i_repeat=0; i_repeat<n_repeat_min; ++i_repeat){
    auto tr = get_tree_with_random_points(500);
    auto &nds = tr.nodes();
    index_t n_nds = (index_t)nds.size();
    for(index_t i=0; i<n_nds; ++i){
        auto &n = nds[i];
        if( n.size() == 1 ) continue;
        auto i_lc = tr.left_child_idx(i), i_rc = tr.right_child_idx(i), 
            i_rs = tr.right_sibling_idx(i_lc);
        ASSERT_EQ(i_lc, i+1);
        ASSERT_EQ(i_rc, i_lc+nds[i_lc].size());
        ASSERT_EQ(i_rc, i_rs);
    }
}
}

TEST_F(BallTreeRawLargePaddingTest, TreeInfoAccess){
for(int i_repeat=0; i_repeat<n_repeat_min; ++i_repeat){
    balltree_t tr0;
    EXPECT_EQ(tr0.tree_info().max_depth(), 0);

    auto tr1 = get_tree_with_random_points(1);
    EXPECT_EQ(tr1.tree_info().max_depth(), 1);

    auto tr2 = get_tree_with_random_points(2);
    EXPECT_EQ(tr2.tree_info().max_depth(), 2);

    auto tr3 = get_tree_with_random_points(3);
    EXPECT_EQ(tr3.tree_info().max_depth(), 3);

    auto tr4 = get_tree_with_random_points(8);
    EXPECT_GE(tr4.tree_info().max_depth(), 4);
    EXPECT_LT(tr4.tree_info().max_depth(), 8);
}
}

TEST_F(BallTreeRawLargePaddingTest, Argsort){
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

TEST_F(BallTreeRawLargePaddingTest, QueryNearestEmptyTree){
for(int i_repeat=0; i_repeat<n_repeat_min; ++i_repeat){
    balltree_t tr;
    
    auto pts1 = get_random_points(10), 
        pts2 = get_cubic_layout_points();
    pts1.insert(pts1.end(), pts2.begin(), pts2.end());

    ASSERT_EQ(pts1.size(), 18);
    
    for(auto &pt: pts1){
        auto ngb = tr.nearest(pt);
        EXPECT_EQ( ngb.r_sq, std::numeric_limits<balltree_t::float_t>::max() );
        EXPECT_EQ( ngb.node_idx, node_t::idxNULL );
    }
}}

TEST_F(BallTreeRawLargePaddingTest, QueryNearest1Point){
for(int i_repeat=0; i_repeat<n_repeat_min; ++i_repeat){
    balltree_t tr;
    float_t x = float_t(i_repeat+1.);
    vector<kdp_t> pts { {x,x,x} };

    tr.construct(pts);

    point_t p { 0.1, 0.0, 0.0 };
    auto ngb = tr.nearest(p);
    auto &nds = tr.nodes();
    ASSERT_LT(ngb.node_idx, nds.size());
    auto &n = nds[ngb.node_idx];
    EXPECT_EQ(n.size(), 1);
    EXPECT_LT((n.center() - point_t{x,x,x}).r(), 1.0e-4);
}}

TEST_F(BallTreeRawLargePaddingTest, QueryNearest2Point){
for(int i_repeat=0; i_repeat<n_repeat_min; ++i_repeat){
    balltree_t tr;
    float_t x = float_t(i_repeat+1.);
    vector<kdp_t> pts { {0,0,0}, {x,x,x} };

    tr.construct(pts);

    point_t p { 0.1, 0.0, 0.0 };
    auto ngb = tr.nearest(p);
    EXPECT_LT(ngb.r_sq, 0.1+1.0e-4);

    auto &nds = tr.nodes();
    ASSERT_LT(ngb.node_idx, nds.size());
    auto &n = nds[ngb.node_idx];
    EXPECT_EQ(n.size(), 1);
    EXPECT_LT((n.center() - point_t{0,0,0}).r(), 1.0e-4);
}}

TEST_F(BallTreeRawLargePaddingTest, QueryNearestWithManyPoints){
for(int i_repeat=0; i_repeat<n_repeat_min; ++i_repeat){
    auto tr = get_tree_with_random_points(50);
    auto pts = get_random_points(50);
    for(auto &pt: pts){
        auto ngb = tr.nearest(pt);
        auto ngb_dst = brute_force_nearest(tr.nodes(), pt);
        EXPECT_LT( std::fabs(ngb.r_sq-ngb_dst.r_sq), 1.0e-4 ) << 
            "Dest r^2 = " << ngb_dst.r_sq << ". Got " << ngb.r_sq << endl; 
    }
}}

TEST_F(BallTreeRawLargePaddingTest, QueryNearestDefaultPolicy){
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

TEST_F(BallTreeRawLargePaddingTest, QueryNearestSharedPolicy){
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

TEST_F(BallTreeRawLargePaddingTest, QueryNearestK0){
for(int i_repeat=0; i_repeat<n_repeat_min; ++i_repeat){
    balltree_t tr;
    
    auto pts1 = get_random_points(10), 
        pts2 = get_cubic_layout_points();
    pts1.insert(pts1.end(), pts2.begin(), pts2.end());

    ASSERT_EQ(pts1.size(), 18);
    
    vector<balltree_t::ngb_t> ngb;
    for(auto &pt: pts1){
        auto k_used = tr.nearest_k(pt, ngb);
        ASSERT_EQ(k_used, 0);
    }
}
}

TEST_F(BallTreeRawLargePaddingTest, QueryNearestKWithEmptyTree){
for(int i_repeat=0; i_repeat<n_repeat_min; ++i_repeat){
    balltree_t tr;
    
    auto pts1 = get_random_points(10), 
        pts2 = get_cubic_layout_points();
    pts1.insert(pts1.end(), pts2.begin(), pts2.end());

    ASSERT_EQ(pts1.size(), 18);
    vector<balltree_t::ngb_t> ngb(3);
    for(auto &pt: pts1){
        auto k_used = tr.nearest_k(pt, ngb);
        EXPECT_EQ( k_used, 0 );
    }
}}

TEST_F(BallTreeRawLargePaddingTest, QueryNearestKWith1Point){
for(int i_repeat=0; i_repeat<n_repeat_min; ++i_repeat){
    auto tr = get_tree_with_random_points(1);
    
    auto pts1 = get_random_points(10), 
        pts2 = get_cubic_layout_points();
    pts1.insert(pts1.end(), pts2.begin(), pts2.end());
    ASSERT_EQ(pts1.size(), 18);

    vector<balltree_t::ngb_t> ngb(3);
    for(auto &pt: pts1){
        auto k_used = tr.nearest_k(pt, ngb);
        ASSERT_EQ( k_used, 1 );
        EXPECT_EQ( ngb[0].node_idx, 0 );
    }

    ngb.resize(1);
    for(auto &pt: pts1){
        auto k_used = tr.nearest_k(pt, ngb);
        ASSERT_EQ( k_used, 1 );
        EXPECT_EQ( ngb[0].node_idx, 0 );
    }
}}

TEST_F(BallTreeRawLargePaddingTest, QueryNearestKWith2Points){
for(int i_repeat=0; i_repeat<n_repeat_min; ++i_repeat){
    auto tr = get_tree_with_random_points(2);
    
    auto pts1 = get_random_points(10), 
        pts2 = get_cubic_layout_points();
    pts1.insert(pts1.end(), pts2.begin(), pts2.end());
    ASSERT_EQ(pts1.size(), 18);

    vector<balltree_t::ngb_t> ngb(3);
    for(auto &pt: pts1){
        auto k_used = tr.nearest_k(pt, ngb);
        ASSERT_EQ( k_used, 2 );
        index_t ids[2] { ngb[0].node_idx, ngb[1].node_idx };
        EXPECT_THAT(ids, gt::UnorderedElementsAre(1,2));
    }

    ngb.resize(2);
    for(auto &pt: pts1){
        auto k_used = tr.nearest_k(pt, ngb);
        ASSERT_EQ( k_used, 2 );
        index_t ids[2] { ngb[0].node_idx, ngb[1].node_idx };
        EXPECT_THAT(ids, gt::UnorderedElementsAre(1,2));
    }

    ngb.resize(1);
    for(auto &pt: pts1){
        auto k_used = tr.nearest_k(pt, ngb);
        ASSERT_EQ( k_used, 1 );
        EXPECT_THAT(ngb[0].node_idx, gt::AnyOf(1,2) );
    }
}}

TEST_F(BallTreeRawLargePaddingTest, QueryNearestKWithManyPoints){
for(int i_repeat=0; i_repeat<n_repeat_min; ++i_repeat){
    index_t n_pts = 50;
    auto tr = get_tree_with_random_points(n_pts);
    const auto & nds = tr.nodes();

    auto pts1 = get_random_points(10), 
        pts2 = get_cubic_layout_points();
    pts1.insert(pts1.end(), pts2.begin(), pts2.end());
    ASSERT_EQ(pts1.size(), 18);

    for(index_t k=0; k<100; ++k){
        vector<balltree_t::ngb_t> ngb(k);
        index_t n_dst = std::min(k, n_pts);
        
        for(auto &pt: pts1){
            auto k_used = tr.nearest_k(pt, ngb);
            ASSERT_EQ(k_used, n_dst);
            std::sort(ngb.data(), ngb.data()+k_used);

            auto ngb_dst = brute_force_nearest_k(nds, pt, k);
            for(index_t i=0; i<k_used; ++i){
                EXPECT_LT( ngb[i].node_idx, nds.size() );
                EXPECT_LT(std::fabs(ngb[i].r_sq-ngb_dst[i].r_sq), 1.0e-4)
                    << "k=" << k << ", i=" << i << endl
                    << "Dest point (" << ngb_dst[i].node_idx << "): " <<
                    nds[ ngb_dst[i].node_idx ] << endl << "Got (" <<
                    ngb[i].node_idx << "): " << nds[ ngb[i].node_idx ] 
                    << endl << "Distances^2 are " << ngb_dst[i].r_sq
                    << " and " << ngb[i].r_sq;
            }
        }
    }

}}

TEST_F(BallTreeRawLargePaddingTest, QueryCountRectWithEmptyTree){
for(int i_repeat=0; i_repeat<n_repeat_min; ++i_repeat){

    balltree_t tr;
    
    auto pts1 = get_random_points(10), 
        pts2 = get_cubic_layout_points();
    pts1.insert(pts1.end(), pts2.begin(), pts2.end());
    ASSERT_EQ(pts1.size(), 18);
    float szs[] = {0.1f, 0.2f, 0.5f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f};
    for(auto &pt: pts1){
        for(auto &sz: szs){
            auto cnt = tr.count_nodes_rect({pt-sz, pt+sz});
            EXPECT_EQ(cnt, 0) << "Point " << pt << ", query size " << sz;
        }
    }
}}

TEST_F(BallTreeRawLargePaddingTest, QueryCountRectWith1Point){
for(int i_repeat=0; i_repeat<n_repeat_min; ++i_repeat){

    auto tr = get_tree_with_random_points(1);
    
    auto pts1 = get_random_points(10), 
        pts2 = get_cubic_layout_points();
    pts1.insert(pts1.end(), pts2.begin(), pts2.end());
    ASSERT_EQ(pts1.size(), 18);
    float szs[] = {0.1f, 0.2f, 0.5f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f};

    for(auto &pt: pts1){
        for(auto &sz: szs){
            auto cnt = tr.count_nodes_rect({pt-sz, pt+sz});
            EXPECT_THAT(cnt, gt::AnyOf(0,1));
            if( sz > 1.5f ) 
                EXPECT_EQ(cnt, 1);
        }
    }
}}

TEST_F(BallTreeRawLargePaddingTest, QueryCountRectWith2Points){
for(int i_repeat=0; i_repeat<n_repeat_min; ++i_repeat){

    auto tr = get_tree_with_random_points(2);
    
    auto pts1 = get_random_points(10), 
        pts2 = get_cubic_layout_points();
    pts1.insert(pts1.end(), pts2.begin(), pts2.end());
    ASSERT_EQ(pts1.size(), 18);
    float szs[] = {0.1f, 0.2f, 0.5f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f};

    for(auto &pt: pts1){
        for(auto &sz: szs){
            auto cnt = tr.count_nodes_rect({pt-sz, pt+sz});
            EXPECT_THAT(cnt, gt::AnyOf(0,1,2));
            if( sz > 1.5f ) 
                EXPECT_EQ(cnt, 2);
        }
    }
}}

TEST_F(BallTreeRawLargePaddingTest, QueryCountRectWithRegularPoints){
for(int i_repeat=0; i_repeat<n_repeat_min; ++i_repeat){

    auto pts = get_cubic_layout_points();
    balltree_t tr;
    tr.construct(pts);

    for(auto &pt: pts){
        for(auto sz: {0.1f, 0.2f, 0.3f, 0.5f}){
            auto cnt = tr.count_nodes_rect({pt-sz, pt+sz});
            EXPECT_EQ(cnt, 1);
        }
        for(auto sz: {0.1f, 0.2f, 0.3f, 0.5f}){
            auto cnt = tr.count_nodes_rect({pt+0.05f, pt+sz});
            EXPECT_EQ(cnt, 0);
        }
        for(auto sz: {1.1f, 1.5f, 5.0f, 10.0f}){
            auto cnt = tr.count_nodes_rect({pt-sz, pt+sz});
            EXPECT_EQ(cnt, 8);
        }
    }
}}

TEST_F(BallTreeRawLargePaddingTest, QueryCountRectWithManyPoints){
for(int i_repeat=0; i_repeat<n_repeat_min; ++i_repeat){

    int n_pts = 50;
    auto tr = get_tree_with_random_points(n_pts);
    auto &nds = tr.nodes();

    for(auto &n: nds){
        if( n.size() != 1 ) continue;
        for(auto sz: {0.1f, 0.2f, 0.3f, 0.5f}){
            auto cnt = tr.count_nodes_rect({n.center()-sz, n.center()+sz});
            EXPECT_GE(cnt, 1);
            EXPECT_LE(cnt, n_pts);
        }
        for(auto sz: {1.1f, 2.0f, 5.0f, 10.0f}){
            auto cnt = tr.count_nodes_rect({n.center()-sz, n.center()+sz});
            EXPECT_EQ(cnt, n_pts);
        }
    }

    auto pts = get_random_points(n_pts);
    for(auto &pt: pts){
        for(auto sz: {0.1f, 0.2f, 0.3f, 0.5f, 1.0f, 2.0f}){
            balltree_t::rect_t r {pt.pos()-sz, pt.pos()+sz};
            auto cnt = tr.count_nodes_rect(r),
                cnt_dst = brute_force_count_nodes_rect(nds, r);
            EXPECT_EQ(cnt, cnt_dst) << "Query rect = " 
                << r << ", size = " << sz;
        }
    }
}}

TEST_F(BallTreeRawLargePaddingTest, QueryCountSphereWithEmptyTree){
for(int i_repeat=0; i_repeat<n_repeat_min; ++i_repeat){

    balltree_t tr;
    
    auto pts1 = get_random_points(10), 
        pts2 = get_cubic_layout_points();
    pts1.insert(pts1.end(), pts2.begin(), pts2.end());
    ASSERT_EQ(pts1.size(), 18);
    float szs[] = {0.1f, 0.2f, 0.5f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f};
    for(auto &pt: pts1){
        for(auto &sz: szs){
            auto cnt = tr.count_nodes_sphere({pt, sz});
            EXPECT_EQ(cnt, 0) << "Point " << pt << ", query size " << sz;
        }
    }
}}

TEST_F(BallTreeRawLargePaddingTest, QueryCountSphereWith1Point){
for(int i_repeat=0; i_repeat<n_repeat_min; ++i_repeat){

    auto tr = get_tree_with_random_points(1);
    
    auto pts1 = get_random_points(10), 
        pts2 = get_cubic_layout_points();
    pts1.insert(pts1.end(), pts2.begin(), pts2.end());
    ASSERT_EQ(pts1.size(), 18);
    float szs[] = {0.1f, 0.2f, 0.5f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f};

    for(auto &pt: pts1){
        for(auto &sz: szs){
            auto cnt = tr.count_nodes_sphere({pt, sz});
            EXPECT_THAT(cnt, gt::AnyOf(0,1));
            if( sz > 1.5f ) 
                EXPECT_EQ(cnt, 1);
        }
    }
}}

TEST_F(BallTreeRawLargePaddingTest, QueryCountSphereWith2Points){
for(int i_repeat=0; i_repeat<n_repeat_min; ++i_repeat){

    auto tr = get_tree_with_random_points(2);
    
    auto pts1 = get_random_points(10), 
        pts2 = get_cubic_layout_points();
    pts1.insert(pts1.end(), pts2.begin(), pts2.end());
    ASSERT_EQ(pts1.size(), 18);
    float szs[] = {0.1f, 0.2f, 0.5f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f};

    for(auto &pt: pts1){
        for(auto &sz: szs){
            auto cnt = tr.count_nodes_sphere({pt, sz});
            EXPECT_THAT(cnt, gt::AnyOf(0,1,2));
            if( sz > 2.0f ) 
                EXPECT_EQ(cnt, 2);
        }
    }
}}

TEST_F(BallTreeRawLargePaddingTest, QueryCountSphereWithRegularPoints){
for(int i_repeat=0; i_repeat<n_repeat_min; ++i_repeat){

    auto pts = get_cubic_layout_points();
    balltree_t tr;
    tr.construct(pts);

    for(auto &pt: pts){
        for(auto sz: {0.1f, 0.2f, 0.3f, 0.5f}){
            auto cnt = tr.count_nodes_sphere({pt, sz});
            EXPECT_EQ(cnt, 1);
        }
        for(auto sz: {0.1f, 0.2f, 0.3f, 0.5f}){
            auto cnt = tr.count_nodes_sphere({pt+0.5f, sz});
            EXPECT_EQ(cnt, 0);
        }
        for(auto sz: {2.5f, 5.0f, 10.0f}){
            auto cnt = tr.count_nodes_sphere({pt, sz});
            EXPECT_EQ(cnt, 8);
        }
    }
}}

TEST_F(BallTreeRawLargePaddingTest, QueryCountSphereWithManyPoints){
for(int i_repeat=0; i_repeat<n_repeat_min; ++i_repeat){

    int n_pts = 50;
    auto tr = get_tree_with_random_points(n_pts);
    auto &nds = tr.nodes();

    for(auto &n: nds){
        if( n.size() != 1 ) continue;

        for(auto sz: {0.1f, 0.2f, 0.3f, 0.5f}){
            auto cnt = tr.count_nodes_sphere({n.center(), sz});
            EXPECT_GE(cnt, 1);
            EXPECT_LE(cnt, n_pts);
        }
        for(auto sz: {2.1f, 2.0f, 5.0f, 10.0f}){
            auto cnt = tr.count_nodes_sphere({n.center(), sz});
            EXPECT_EQ(cnt, n_pts);
        }
    }

    auto pts = get_random_points(n_pts);
    for(auto &pt: pts){
        for(auto sz: {0.1f, 0.2f, 0.3f, 0.5f, 1.0f, 2.0f}){
            balltree_t::sphere_t s {pt.pos(), sz};
            auto cnt = tr.count_nodes_sphere(s),
                cnt_dst = brute_force_count_nodes_sphere(nds, s);
            EXPECT_EQ(cnt, cnt_dst) << "Query sphere = " << s;
        }
    }
}}

class BallTreeRawNonAlignedPaddingTest : public gt::Test {
public:
    using kdp_t = KDPoint<float, 3, 11>;
    using balltree_t = BallTree<kdp_t, int>;
    using node_t = balltree_t::node_t;
    using point_t = balltree_t::point_t;
    using pos_t = balltree_t::pos_t;
    using index_t = balltree_t::index_t;
    using float_t = balltree_t::float_t;

    using cstr_pl_t = balltree_t::construct_policy_t;

    static constexpr int n_repeat_min = 10, 
        n_repeat_max = 10000;

    BallTreeRawNonAlignedPaddingTest() {}
    ~BallTreeRawNonAlignedPaddingTest() override {}
    void SetUp() override {}
    void TearDown() override {}

    vector<balltree_t::construct_policy_t> get_cstr_policy_table() 
    {
        using pl_t = balltree_t::construct_policy_t;
        using alg_t = pl_t::algorithm_t;
        using spl_t = pl_t::split_axis_t;
        using ins_fav_t = pl_t::insert_favor_t;
        
        vector<pl_t> out;
        
        for(auto spl: { 
            spl_t::MAX_EXTREME,
            spl_t::MAX_VARIANCE, 
            spl_t::ORDERED, 
            spl_t::RANDOM })
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

    vector<kdp_t> get_random_points(index_t n_pts) 
    {
        vector<kdp_t> pts(n_pts);
        for(index_t i=0; i<n_pts; ++i){
            rand( pts[i].pos().begin(), pts[i].pos().end() );
        }
        return pts;
    }

    vector<kdp_t> get_cubic_layout_points()
    {
        vector<kdp_t> pts;
        for(auto x0: {0.0f, 1.0f})
        for(auto x1: {0.0f, 1.0f})
        for(auto x2: {0.0f, 1.0f})
            pts.push_back({x0,x1,x2});
        return pts;
    }

    balltree_t get_tree_with_random_points(index_t n_pts, 
        cstr_pl_t pl = cstr_pl_t()) 
    {
        balltree_t tr;
        auto pts = get_random_points(n_pts);
        tr.construct(pts, pl);
        return tr;
    }

    balltree_t::ngb_t brute_force_nearest(const vector<node_t> &nds, 
        point_t p)
    {
        balltree_t::ngb_t ngb { -1, std::numeric_limits<float_t>::max() };
        for(auto &n: nds){
            if(n.size() != 1) continue;
            auto r_sq = (n.center() - p).r_sq();
            if( r_sq < ngb.r_sq ) {
                ngb.r_sq = r_sq;
                ngb.node_idx = index_t( &n - nds.data() );
            }
        }
        return ngb;
    }

    vector<balltree_t::ngb_t> brute_force_nearest_k(const vector<node_t> &nds, 
        point_t p, index_t k) 
    {
        vector<balltree_t::ngb_t> ngbs;
        for(auto &n: nds){
            if( n.size() != 1 ) continue;
            auto r_sq = (n.center() - p).r_sq();
            ngbs.push_back({ index_t(&n - nds.data()), r_sq});
        }
        std::sort(ngbs.begin(), ngbs.end());
        if( ngbs.size() > k ) ngbs.resize(k);
        return ngbs;
    }

    index_t brute_force_count_nodes_rect(const vector<node_t> &nds, 
        balltree_t::rect_t r) 
    {
        index_t cnt {};
        for(auto &n: nds){
            if( n.size() != 1 ) continue;
            if( r.contains(n.center()) ) ++cnt;
        }
        return cnt;
    }
    index_t brute_force_count_nodes_sphere(const vector<node_t> &nds, 
        balltree_t::sphere_t s) 
    {
        index_t cnt {};
        for(auto &n: nds){
            if( n.size() != 1 ) continue;
            if( s.contains(n.center()) ) ++cnt;
        }
        return cnt;
    }
};

TEST_F(BallTreeRawNonAlignedPaddingTest, DefaultInitialization){
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

TEST_F(BallTreeRawNonAlignedPaddingTest, InfoPrinting){
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

TEST_F(BallTreeRawNonAlignedPaddingTest, NodeTypeConstruction) {
for(int i=0; i<n_repeat_max; ++i){
    node_t n1;

    node_t n2 {{1,2,3}};
    ASSERT_EQ(n2.r(), float_t(0));
    ASSERT_TRUE( (n2.center().pos() == pos_t{1,2,3}).all() );

    node_t n3 {{1,2,3},4};
    ASSERT_EQ(n3.r(), float_t(4));
    ASSERT_TRUE( (n3.center().pos() == pos_t{1,2,3}).all() );
    
    char pad[0] {};
    node_t n4 {{1,2,3},4,pad};
    ASSERT_EQ(n4.r(), float_t(4));
    ASSERT_TRUE( (n4.center().pos() == pos_t{1,2,3}).all() );

    node_t n5 {{1,2,3},4,5,pad};
    ASSERT_EQ(n5.r(), float_t(4));
    ASSERT_EQ(n5.size(), 5);
    ASSERT_TRUE( (n5.center().pos() == pos_t{1,2,3}).all() );
}}

TEST_F(BallTreeRawNonAlignedPaddingTest, NodeTypeAttributesSetGet) {
for(int i=0; i<n_repeat_max; ++i){
    node_t n1;
    n1.center() = point_t{1,2,3};
    n1.r() = 4;
    n1.size() = 5;
    char pad[0] {};
    n1.fill_pad(pad);

    ASSERT_EQ(n1.r(), float_t(4));
    ASSERT_EQ(n1.size(), 5);
    ASSERT_TRUE( (n1.center().pos() == pos_t{1,2,3}).all() );

    ASSERT_EQ(sizeof(n1.pad()), balltree_t::PADDING);
}}

TEST_F(BallTreeRawNonAlignedPaddingTest, EmptyTree) {
for(int i=0; i<n_repeat_max; ++i){
    balltree_t ballt;
    ASSERT_EQ(ballt.nodes().size(), 0);
}}

TEST_F(BallTreeRawNonAlignedPaddingTest, Construct1Node){
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

TEST_F(BallTreeRawNonAlignedPaddingTest, Construct2Nodes){
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

TEST_F(BallTreeRawNonAlignedPaddingTest, ConstructionWithDefaultPolicy){
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

TEST_F(BallTreeRawNonAlignedPaddingTest, ConstructionWithKDPolicy){
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

TEST_F(BallTreeRawNonAlignedPaddingTest, ConstructionWithTopDownPolicy){
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

TEST_F(BallTreeRawNonAlignedPaddingTest, ConstructionWithInsertionPolicy){
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

TEST_F(BallTreeRawNonAlignedPaddingTest, ConstructionWithBottomUpPolicy){
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

TEST_F(BallTreeRawNonAlignedPaddingTest, CopyMoveOperations){
for(int i_repeat=0; i_repeat<n_repeat_min; ++i_repeat){
    {
        balltree_t tr0;
        auto cpy = tr0;
        ASSERT_NE(&cpy, &tr0);
        ASSERT_EQ(tr0.nodes().size(), 0);
        ASSERT_EQ(cpy.nodes().size(), tr0.nodes().size());
        ASSERT_EQ(cpy.tree_info().max_depth(), tr0.tree_info().max_depth());
    }
    {
        auto tr = get_tree_with_random_points(500);
        auto cpy = tr;
        ASSERT_NE(&cpy, &tr);
        ASSERT_EQ(cpy.nodes().size(), tr.nodes().size());
        ASSERT_EQ(cpy.tree_info().max_depth(), tr.tree_info().max_depth());
        index_t n_nds = (index_t)tr.nodes().size();
        for(index_t i=0; i<n_nds; ++i){
            auto &n1 = cpy.nodes()[i], &n2 = cpy.nodes()[i];
            ASSERT_EQ(n1.size(), n2.size());
            ASSERT_TRUE( (n1.center().pos() == n2.center().pos()).all() );
        }
    }
}
}

TEST_F(BallTreeRawNonAlignedPaddingTest, NodeIndexAccess){
for(int i_repeat=0; i_repeat<n_repeat_min; ++i_repeat){
    auto tr = get_tree_with_random_points(500);
    auto &nds = tr.nodes();
    index_t n_nds = (index_t)nds.size();
    for(index_t i=0; i<n_nds; ++i){
        auto &n = nds[i];
        if( n.size() == 1 ) continue;
        auto i_lc = tr.left_child_idx(i), i_rc = tr.right_child_idx(i), 
            i_rs = tr.right_sibling_idx(i_lc);
        ASSERT_EQ(i_lc, i+1);
        ASSERT_EQ(i_rc, i_lc+nds[i_lc].size());
        ASSERT_EQ(i_rc, i_rs);
    }
}
}

TEST_F(BallTreeRawNonAlignedPaddingTest, TreeInfoAccess){
for(int i_repeat=0; i_repeat<n_repeat_min; ++i_repeat){
    balltree_t tr0;
    EXPECT_EQ(tr0.tree_info().max_depth(), 0);

    auto tr1 = get_tree_with_random_points(1);
    EXPECT_EQ(tr1.tree_info().max_depth(), 1);

    auto tr2 = get_tree_with_random_points(2);
    EXPECT_EQ(tr2.tree_info().max_depth(), 2);

    auto tr3 = get_tree_with_random_points(3);
    EXPECT_EQ(tr3.tree_info().max_depth(), 3);

    auto tr4 = get_tree_with_random_points(8);
    EXPECT_GE(tr4.tree_info().max_depth(), 4);
    EXPECT_LT(tr4.tree_info().max_depth(), 8);
}
}

TEST_F(BallTreeRawNonAlignedPaddingTest, Argsort){
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

TEST_F(BallTreeRawNonAlignedPaddingTest, QueryNearestEmptyTree){
for(int i_repeat=0; i_repeat<n_repeat_min; ++i_repeat){
    balltree_t tr;
    
    auto pts1 = get_random_points(10), 
        pts2 = get_cubic_layout_points();
    pts1.insert(pts1.end(), pts2.begin(), pts2.end());

    ASSERT_EQ(pts1.size(), 18);
    
    for(auto &pt: pts1){
        auto ngb = tr.nearest(pt);
        EXPECT_EQ( ngb.r_sq, std::numeric_limits<balltree_t::float_t>::max() );
        EXPECT_EQ( ngb.node_idx, node_t::idxNULL );
    }
}}

TEST_F(BallTreeRawNonAlignedPaddingTest, QueryNearest1Point){
for(int i_repeat=0; i_repeat<n_repeat_min; ++i_repeat){
    balltree_t tr;
    float_t x = float_t(i_repeat+1.);
    vector<kdp_t> pts { {x,x,x} };

    tr.construct(pts);

    point_t p { 0.1, 0.0, 0.0 };
    auto ngb = tr.nearest(p);
    auto &nds = tr.nodes();
    ASSERT_LT(ngb.node_idx, nds.size());
    auto &n = nds[ngb.node_idx];
    EXPECT_EQ(n.size(), 1);
    EXPECT_LT((n.center() - point_t{x,x,x}).r(), 1.0e-4);
}}

TEST_F(BallTreeRawNonAlignedPaddingTest, QueryNearest2Point){
for(int i_repeat=0; i_repeat<n_repeat_min; ++i_repeat){
    balltree_t tr;
    float_t x = float_t(i_repeat+1.);
    vector<kdp_t> pts { {0,0,0}, {x,x,x} };

    tr.construct(pts);

    point_t p { 0.1, 0.0, 0.0 };
    auto ngb = tr.nearest(p);
    EXPECT_LT(ngb.r_sq, 0.1+1.0e-4);

    auto &nds = tr.nodes();
    ASSERT_LT(ngb.node_idx, nds.size());
    auto &n = nds[ngb.node_idx];
    EXPECT_EQ(n.size(), 1);
    EXPECT_LT((n.center() - point_t{0,0,0}).r(), 1.0e-4);
}}

TEST_F(BallTreeRawNonAlignedPaddingTest, QueryNearestWithManyPoints){
for(int i_repeat=0; i_repeat<n_repeat_min; ++i_repeat){
    auto tr = get_tree_with_random_points(50);
    auto pts = get_random_points(50);
    for(auto &pt: pts){
        auto ngb = tr.nearest(pt);
        auto ngb_dst = brute_force_nearest(tr.nodes(), pt);
        EXPECT_LT( std::fabs(ngb.r_sq-ngb_dst.r_sq), 1.0e-4 ) << 
            "Dest r^2 = " << ngb_dst.r_sq << ". Got " << ngb.r_sq << endl; 
    }
}}

TEST_F(BallTreeRawNonAlignedPaddingTest, QueryNearestDefaultPolicy){
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

TEST_F(BallTreeRawNonAlignedPaddingTest, QueryNearestSharedPolicy){
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

TEST_F(BallTreeRawNonAlignedPaddingTest, QueryNearestK0){
for(int i_repeat=0; i_repeat<n_repeat_min; ++i_repeat){
    balltree_t tr;
    
    auto pts1 = get_random_points(10), 
        pts2 = get_cubic_layout_points();
    pts1.insert(pts1.end(), pts2.begin(), pts2.end());

    ASSERT_EQ(pts1.size(), 18);
    
    vector<balltree_t::ngb_t> ngb;
    for(auto &pt: pts1){
        auto k_used = tr.nearest_k(pt, ngb);
        ASSERT_EQ(k_used, 0);
    }
}
}

TEST_F(BallTreeRawNonAlignedPaddingTest, QueryNearestKWithEmptyTree){
for(int i_repeat=0; i_repeat<n_repeat_min; ++i_repeat){
    balltree_t tr;
    
    auto pts1 = get_random_points(10), 
        pts2 = get_cubic_layout_points();
    pts1.insert(pts1.end(), pts2.begin(), pts2.end());

    ASSERT_EQ(pts1.size(), 18);
    vector<balltree_t::ngb_t> ngb(3);
    for(auto &pt: pts1){
        auto k_used = tr.nearest_k(pt, ngb);
        EXPECT_EQ( k_used, 0 );
    }
}}

TEST_F(BallTreeRawNonAlignedPaddingTest, QueryNearestKWith1Point){
for(int i_repeat=0; i_repeat<n_repeat_min; ++i_repeat){
    auto tr = get_tree_with_random_points(1);
    
    auto pts1 = get_random_points(10), 
        pts2 = get_cubic_layout_points();
    pts1.insert(pts1.end(), pts2.begin(), pts2.end());
    ASSERT_EQ(pts1.size(), 18);

    vector<balltree_t::ngb_t> ngb(3);
    for(auto &pt: pts1){
        auto k_used = tr.nearest_k(pt, ngb);
        ASSERT_EQ( k_used, 1 );
        EXPECT_EQ( ngb[0].node_idx, 0 );
    }

    ngb.resize(1);
    for(auto &pt: pts1){
        auto k_used = tr.nearest_k(pt, ngb);
        ASSERT_EQ( k_used, 1 );
        EXPECT_EQ( ngb[0].node_idx, 0 );
    }
}}

TEST_F(BallTreeRawNonAlignedPaddingTest, QueryNearestKWith2Points){
for(int i_repeat=0; i_repeat<n_repeat_min; ++i_repeat){
    auto tr = get_tree_with_random_points(2);
    
    auto pts1 = get_random_points(10), 
        pts2 = get_cubic_layout_points();
    pts1.insert(pts1.end(), pts2.begin(), pts2.end());
    ASSERT_EQ(pts1.size(), 18);

    vector<balltree_t::ngb_t> ngb(3);
    for(auto &pt: pts1){
        auto k_used = tr.nearest_k(pt, ngb);
        ASSERT_EQ( k_used, 2 );
        index_t ids[2] { ngb[0].node_idx, ngb[1].node_idx };
        EXPECT_THAT(ids, gt::UnorderedElementsAre(1,2));
    }

    ngb.resize(2);
    for(auto &pt: pts1){
        auto k_used = tr.nearest_k(pt, ngb);
        ASSERT_EQ( k_used, 2 );
        index_t ids[2] { ngb[0].node_idx, ngb[1].node_idx };
        EXPECT_THAT(ids, gt::UnorderedElementsAre(1,2));
    }

    ngb.resize(1);
    for(auto &pt: pts1){
        auto k_used = tr.nearest_k(pt, ngb);
        ASSERT_EQ( k_used, 1 );
        EXPECT_THAT(ngb[0].node_idx, gt::AnyOf(1,2) );
    }
}}

TEST_F(BallTreeRawNonAlignedPaddingTest, QueryNearestKWithManyPoints){
for(int i_repeat=0; i_repeat<n_repeat_min; ++i_repeat){
    index_t n_pts = 50;
    auto tr = get_tree_with_random_points(n_pts);
    const auto & nds = tr.nodes();

    auto pts1 = get_random_points(10), 
        pts2 = get_cubic_layout_points();
    pts1.insert(pts1.end(), pts2.begin(), pts2.end());
    ASSERT_EQ(pts1.size(), 18);

    for(index_t k=0; k<100; ++k){
        vector<balltree_t::ngb_t> ngb(k);
        index_t n_dst = std::min(k, n_pts);
        
        for(auto &pt: pts1){
            auto k_used = tr.nearest_k(pt, ngb);
            ASSERT_EQ(k_used, n_dst);
            std::sort(ngb.data(), ngb.data()+k_used);

            auto ngb_dst = brute_force_nearest_k(nds, pt, k);
            for(index_t i=0; i<k_used; ++i){
                EXPECT_LT( ngb[i].node_idx, nds.size() );
                EXPECT_LT(std::fabs(ngb[i].r_sq-ngb_dst[i].r_sq), 1.0e-4)
                    << "k=" << k << ", i=" << i << endl
                    << "Dest point (" << ngb_dst[i].node_idx << "): " <<
                    nds[ ngb_dst[i].node_idx ] << endl << "Got (" <<
                    ngb[i].node_idx << "): " << nds[ ngb[i].node_idx ] 
                    << endl << "Distances^2 are " << ngb_dst[i].r_sq
                    << " and " << ngb[i].r_sq;
            }
        }
    }

}}

TEST_F(BallTreeRawNonAlignedPaddingTest, QueryCountRectWithEmptyTree){
for(int i_repeat=0; i_repeat<n_repeat_min; ++i_repeat){

    balltree_t tr;
    
    auto pts1 = get_random_points(10), 
        pts2 = get_cubic_layout_points();
    pts1.insert(pts1.end(), pts2.begin(), pts2.end());
    ASSERT_EQ(pts1.size(), 18);
    float szs[] = {0.1f, 0.2f, 0.5f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f};
    for(auto &pt: pts1){
        for(auto &sz: szs){
            auto cnt = tr.count_nodes_rect({pt-sz, pt+sz});
            EXPECT_EQ(cnt, 0) << "Point " << pt << ", query size " << sz;
        }
    }
}}

TEST_F(BallTreeRawNonAlignedPaddingTest, QueryCountRectWith1Point){
for(int i_repeat=0; i_repeat<n_repeat_min; ++i_repeat){

    auto tr = get_tree_with_random_points(1);
    
    auto pts1 = get_random_points(10), 
        pts2 = get_cubic_layout_points();
    pts1.insert(pts1.end(), pts2.begin(), pts2.end());
    ASSERT_EQ(pts1.size(), 18);
    float szs[] = {0.1f, 0.2f, 0.5f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f};

    for(auto &pt: pts1){
        for(auto &sz: szs){
            auto cnt = tr.count_nodes_rect({pt-sz, pt+sz});
            EXPECT_THAT(cnt, gt::AnyOf(0,1));
            if( sz > 1.5f ) 
                EXPECT_EQ(cnt, 1);
        }
    }
}}

TEST_F(BallTreeRawNonAlignedPaddingTest, QueryCountRectWith2Points){
for(int i_repeat=0; i_repeat<n_repeat_min; ++i_repeat){

    auto tr = get_tree_with_random_points(2);
    
    auto pts1 = get_random_points(10), 
        pts2 = get_cubic_layout_points();
    pts1.insert(pts1.end(), pts2.begin(), pts2.end());
    ASSERT_EQ(pts1.size(), 18);
    float szs[] = {0.1f, 0.2f, 0.5f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f};

    for(auto &pt: pts1){
        for(auto &sz: szs){
            auto cnt = tr.count_nodes_rect({pt-sz, pt+sz});
            EXPECT_THAT(cnt, gt::AnyOf(0,1,2));
            if( sz > 1.5f ) 
                EXPECT_EQ(cnt, 2);
        }
    }
}}

TEST_F(BallTreeRawNonAlignedPaddingTest, QueryCountRectWithRegularPoints){
for(int i_repeat=0; i_repeat<n_repeat_min; ++i_repeat){

    auto pts = get_cubic_layout_points();
    balltree_t tr;
    tr.construct(pts);

    for(auto &pt: pts){
        for(auto sz: {0.1f, 0.2f, 0.3f, 0.5f}){
            auto cnt = tr.count_nodes_rect({pt-sz, pt+sz});
            EXPECT_EQ(cnt, 1);
        }
        for(auto sz: {0.1f, 0.2f, 0.3f, 0.5f}){
            auto cnt = tr.count_nodes_rect({pt+0.05f, pt+sz});
            EXPECT_EQ(cnt, 0);
        }
        for(auto sz: {1.1f, 1.5f, 5.0f, 10.0f}){
            auto cnt = tr.count_nodes_rect({pt-sz, pt+sz});
            EXPECT_EQ(cnt, 8);
        }
    }
}}

TEST_F(BallTreeRawNonAlignedPaddingTest, QueryCountRectWithManyPoints){
for(int i_repeat=0; i_repeat<n_repeat_min; ++i_repeat){

    int n_pts = 50;
    auto tr = get_tree_with_random_points(n_pts);
    auto &nds = tr.nodes();

    for(auto &n: nds){
        if( n.size() != 1 ) continue;
        for(auto sz: {0.1f, 0.2f, 0.3f, 0.5f}){
            auto cnt = tr.count_nodes_rect({n.center()-sz, n.center()+sz});
            EXPECT_GE(cnt, 1);
            EXPECT_LE(cnt, n_pts);
        }
        for(auto sz: {1.1f, 2.0f, 5.0f, 10.0f}){
            auto cnt = tr.count_nodes_rect({n.center()-sz, n.center()+sz});
            EXPECT_EQ(cnt, n_pts);
        }
    }

    auto pts = get_random_points(n_pts);
    for(auto &pt: pts){
        for(auto sz: {0.1f, 0.2f, 0.3f, 0.5f, 1.0f, 2.0f}){
            balltree_t::rect_t r {pt.pos()-sz, pt.pos()+sz};
            auto cnt = tr.count_nodes_rect(r),
                cnt_dst = brute_force_count_nodes_rect(nds, r);
            EXPECT_EQ(cnt, cnt_dst) << "Query rect = " 
                << r << ", size = " << sz;
        }
    }
}}

TEST_F(BallTreeRawNonAlignedPaddingTest, QueryCountSphereWithEmptyTree){
for(int i_repeat=0; i_repeat<n_repeat_min; ++i_repeat){

    balltree_t tr;
    
    auto pts1 = get_random_points(10), 
        pts2 = get_cubic_layout_points();
    pts1.insert(pts1.end(), pts2.begin(), pts2.end());
    ASSERT_EQ(pts1.size(), 18);
    float szs[] = {0.1f, 0.2f, 0.5f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f};
    for(auto &pt: pts1){
        for(auto &sz: szs){
            auto cnt = tr.count_nodes_sphere({pt, sz});
            EXPECT_EQ(cnt, 0) << "Point " << pt << ", query size " << sz;
        }
    }
}}

TEST_F(BallTreeRawNonAlignedPaddingTest, QueryCountSphereWith1Point){
for(int i_repeat=0; i_repeat<n_repeat_min; ++i_repeat){

    auto tr = get_tree_with_random_points(1);
    
    auto pts1 = get_random_points(10), 
        pts2 = get_cubic_layout_points();
    pts1.insert(pts1.end(), pts2.begin(), pts2.end());
    ASSERT_EQ(pts1.size(), 18);
    float szs[] = {0.1f, 0.2f, 0.5f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f};

    for(auto &pt: pts1){
        for(auto &sz: szs){
            auto cnt = tr.count_nodes_sphere({pt, sz});
            EXPECT_THAT(cnt, gt::AnyOf(0,1));
            if( sz > 1.5f ) 
                EXPECT_EQ(cnt, 1);
        }
    }
}}

TEST_F(BallTreeRawNonAlignedPaddingTest, QueryCountSphereWith2Points){
for(int i_repeat=0; i_repeat<n_repeat_min; ++i_repeat){

    auto tr = get_tree_with_random_points(2);
    
    auto pts1 = get_random_points(10), 
        pts2 = get_cubic_layout_points();
    pts1.insert(pts1.end(), pts2.begin(), pts2.end());
    ASSERT_EQ(pts1.size(), 18);
    float szs[] = {0.1f, 0.2f, 0.5f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f};

    for(auto &pt: pts1){
        for(auto &sz: szs){
            auto cnt = tr.count_nodes_sphere({pt, sz});
            EXPECT_THAT(cnt, gt::AnyOf(0,1,2));
            if( sz > 2.0f ) 
                EXPECT_EQ(cnt, 2);
        }
    }
}}

TEST_F(BallTreeRawNonAlignedPaddingTest, QueryCountSphereWithRegularPoints){
for(int i_repeat=0; i_repeat<n_repeat_min; ++i_repeat){

    auto pts = get_cubic_layout_points();
    balltree_t tr;
    tr.construct(pts);

    for(auto &pt: pts){
        for(auto sz: {0.1f, 0.2f, 0.3f, 0.5f}){
            auto cnt = tr.count_nodes_sphere({pt, sz});
            EXPECT_EQ(cnt, 1);
        }
        for(auto sz: {0.1f, 0.2f, 0.3f, 0.5f}){
            auto cnt = tr.count_nodes_sphere({pt+0.5f, sz});
            EXPECT_EQ(cnt, 0);
        }
        for(auto sz: {2.5f, 5.0f, 10.0f}){
            auto cnt = tr.count_nodes_sphere({pt, sz});
            EXPECT_EQ(cnt, 8);
        }
    }
}}

TEST_F(BallTreeRawNonAlignedPaddingTest, QueryCountSphereWithManyPoints){
for(int i_repeat=0; i_repeat<n_repeat_min; ++i_repeat){

    int n_pts = 50;
    auto tr = get_tree_with_random_points(n_pts);
    auto &nds = tr.nodes();

    for(auto &n: nds){
        if( n.size() != 1 ) continue;

        for(auto sz: {0.1f, 0.2f, 0.3f, 0.5f}){
            auto cnt = tr.count_nodes_sphere({n.center(), sz});
            EXPECT_GE(cnt, 1);
            EXPECT_LE(cnt, n_pts);
        }
        for(auto sz: {2.1f, 2.0f, 5.0f, 10.0f}){
            auto cnt = tr.count_nodes_sphere({n.center(), sz});
            EXPECT_EQ(cnt, n_pts);
        }
    }

    auto pts = get_random_points(n_pts);
    for(auto &pt: pts){
        for(auto sz: {0.1f, 0.2f, 0.3f, 0.5f, 1.0f, 2.0f}){
            balltree_t::sphere_t s {pt.pos(), sz};
            auto cnt = tr.count_nodes_sphere(s),
                cnt_dst = brute_force_count_nodes_sphere(nds, s);
            EXPECT_EQ(cnt, cnt_dst) << "Query sphere = " << s;
        }
    }
}}



} // namespace

} // namespace HIPP::NUMERICAL