#include <hippnumerical.h>
#include <gmock/gmock.h>

namespace HIPP::NUMERICAL {

namespace {

namespace gt = ::testing;

class InsertableBallTreeRawEmptyPaddingTest : public gt::Test {
public:
    using kdp_t = KDPoint<float, 3>;
    using balltree_t = _KDSEARCH::_InsertableBallTree<kdp_t, int>;
    using node_t = balltree_t::node_t;
    using index_t = balltree_t::index_t;

    InsertableBallTreeRawEmptyPaddingTest() {}
    ~InsertableBallTreeRawEmptyPaddingTest() override {}
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(InsertableBallTreeRawEmptyPaddingTest, DefaultInitialization){
    kdp_t pt;
    balltree_t ballt;
    balltree_t::node_t n;
}

TEST_F(InsertableBallTreeRawEmptyPaddingTest, InfoPrinting){
    balltree_t ballt;
    cout << ballt << endl;
    ballt.info() << endl;
    ballt.info(cout, 1);

    balltree_t::node_t nd;
    cout << nd << endl;
    nd.info() << endl;
    nd.info(cout, 1);

    balltree_t::construct_policy_t c_pl;
    cout << c_pl;
    c_pl.info() << endl;
    c_pl.info(cout, 1);

    balltree_t::insert_policy_t ins_pl;
    cout << ins_pl;
    ins_pl.info() << endl;
    ins_pl.info(cout, 1);
}

TEST_F(InsertableBallTreeRawEmptyPaddingTest, Insert1Node){
    balltree_t ballt;
    ballt.insert({{0,0,0}});
    ASSERT_EQ(ballt.size(), 1);

    auto &nds = ballt.nodes();
    auto &r = ballt.root();
    EXPECT_EQ(r.right_child_idx(), node_t::idxNULL);
    auto i1 = r.left_child_idx();
    ASSERT_LT(i1, nds.size());
    auto &n1 = nds[i1];
    EXPECT_EQ(n1.left_child_idx(), node_t::idxNULL);
    EXPECT_EQ(n1.right_child_idx(), node_t::idxNULL);
}

TEST_F(InsertableBallTreeRawEmptyPaddingTest, Insert2Nodes){
    balltree_t ballt;
    ballt.insert({{0,0,0}});
    ballt.insert({{1,1,1}});
    ASSERT_EQ(ballt.size(), 2);
    
    auto &nds = ballt.nodes();
    auto &r = ballt.root();
    auto i1 = r.left_child_idx();
    EXPECT_EQ(r.right_child_idx(), node_t::idxNULL);

    auto &n1 = nds[i1];
    auto i2 = nds[i1].left_child_idx(),
         i3 = nds[i1].right_child_idx();
    ASSERT_LT(i2,nds.size());
    ASSERT_LT(i3,nds.size());
    auto & n2 = nds[i2], &n3 = nds[i3];

    EXPECT_TRUE(r.is_internal());
    EXPECT_TRUE(n1.is_internal());
    EXPECT_TRUE(n2.is_leaf());
    EXPECT_TRUE(n3.is_leaf());
    EXPECT_EQ(n2.left_child_idx(), node_t::idxNULL);
    EXPECT_EQ(n2.right_child_idx(), node_t::idxNULL);
    EXPECT_EQ(n3.left_child_idx(), node_t::idxNULL);
    EXPECT_EQ(n3.right_child_idx(), node_t::idxNULL);
    EXPECT_EQ(n1.parent_idx(), balltree_t::ROOT_IDX);
    EXPECT_EQ(n2.parent_idx(), i1);
    EXPECT_EQ(n3.parent_idx(), i1);
}

TEST_F(InsertableBallTreeRawEmptyPaddingTest, InsertionWithDefaultPolicy){
    int cnt = 0;
    
    balltree_t ballt;
    ASSERT_TRUE(ballt.empty());
    ASSERT_EQ(ballt.size(), cnt); ++cnt;

    for(int i=0; i<50; ++i){
        float_t x = i;
        ballt.insert({{x,x,x}});
        ASSERT_EQ(ballt.size(), cnt); ++cnt;
    }

    balltree_t::pos_t pt;
    for(int i=0; i<1000; ++i){    
        rand(pt.begin(), pt.end()); pt *= 50;
        ballt.insert({pt});
        ASSERT_EQ(ballt.size(), cnt); ++cnt;
    }
}

TEST_F(InsertableBallTreeRawEmptyPaddingTest, InsertionWithFinePolicy){
    using ins_pl_t = balltree_t::insert_policy_t;
    ins_pl_t pl;
    pl.set_insert_favor(ins_pl_t::insert_favor_t::FINE);

    int cnt = 0;
    
    balltree_t ballt;
    ASSERT_TRUE(ballt.empty());
    ASSERT_EQ(ballt.size(), cnt); ++cnt;

    for(int i=0; i<50; ++i){
        float_t x = i;
        ballt.insert({{x,x,x}}, pl);
        ASSERT_EQ(ballt.size(), cnt); ++cnt;
        ASSERT_TRUE(pl.fringe().empty());
    }

    balltree_t::pos_t pt;
    for(int i=0; i<1000; ++i){    
        rand(pt.begin(), pt.end()); pt *= 50;
        ballt.insert({pt}, pl);
        ASSERT_EQ(ballt.size(), cnt); ++cnt;
        ASSERT_TRUE(pl.fringe().empty());
    }

    pl.info(cout, 1);
}

TEST_F(InsertableBallTreeRawEmptyPaddingTest, InsertionManyNodes) {
    balltree_t ballt;
    balltree_t::pos_t pt;
    int n = 100000, m = 10;
    for(int i=0; i<n; ++i){
        rand(pt.begin(), pt.end());
        ballt.insert({pt});
    }
    ASSERT_EQ(ballt.size(), n);
    pout << "Insert finished with default policy", endl;

    using ins_pl_t = balltree_t::insert_policy_t;
    ins_pl_t pl;
    pl.set_insert_favor(ins_pl_t::insert_favor_t::FINE);
    for(int i=0; i<m; ++i){
        rand(pt.begin(), pt.end());
        ballt.insert({pt}, pl);
    }
    ASSERT_EQ(ballt.size(), n+m);
    pout << "Insert finished with fine policy", endl;
}

TEST_F(InsertableBallTreeRawEmptyPaddingTest, BottomUpRefineEmpty){
    balltree_t ballt;
    ballt.bottom_up_refine();
    ASSERT_TRUE(ballt.empty());
}

TEST_F(InsertableBallTreeRawEmptyPaddingTest, BottomUpRefine1Node){
    balltree_t ballt;
    ballt.insert({{0,0,0}});
    ballt.bottom_up_refine();
    ASSERT_EQ(ballt.size(), 1);

    auto &nds = ballt.nodes();
    auto &r = ballt.root();
    EXPECT_EQ(r.right_child_idx(), node_t::idxNULL);
    auto i1 = r.left_child_idx();
    ASSERT_LT(i1, nds.size());
    auto &n1 = nds[i1];
    EXPECT_EQ(n1.left_child_idx(), node_t::idxNULL);
    EXPECT_EQ(n1.right_child_idx(), node_t::idxNULL);
}

TEST_F(InsertableBallTreeRawEmptyPaddingTest, BottomUpRefine2Nodes){
    balltree_t ballt;
    ballt.insert({{0,0,0}});
    ballt.insert({{1,1,1}});
    ballt.bottom_up_refine();
    ASSERT_EQ(ballt.size(), 2);
    
    auto &nds = ballt.nodes();
    auto &r = ballt.root();
    auto i1 = r.left_child_idx();
    EXPECT_EQ(r.right_child_idx(), node_t::idxNULL);

    auto &n1 = nds[i1];
    auto i2 = nds[i1].left_child_idx(),
         i3 = nds[i1].right_child_idx();
    ASSERT_LT(i2,nds.size());
    ASSERT_LT(i3,nds.size());
    auto & n2 = nds[i2], &n3 = nds[i3];

    EXPECT_TRUE(r.is_internal());
    EXPECT_TRUE(n1.is_internal());
    EXPECT_TRUE(n2.is_leaf());
    EXPECT_TRUE(n3.is_leaf());
    EXPECT_EQ(n2.left_child_idx(), node_t::idxNULL);
    EXPECT_EQ(n2.right_child_idx(), node_t::idxNULL);
    EXPECT_EQ(n3.left_child_idx(), node_t::idxNULL);
    EXPECT_EQ(n3.right_child_idx(), node_t::idxNULL);
    EXPECT_EQ(n1.parent_idx(), balltree_t::ROOT_IDX);
    EXPECT_EQ(n2.parent_idx(), i1);
    EXPECT_EQ(n3.parent_idx(), i1);
}

TEST_F(InsertableBallTreeRawEmptyPaddingTest, BottomUpRefineManyNodes){
    balltree_t ballt;
    pout << "Volume = ", ballt.volume(), endl;
    
    int n = 1000;
    for(int i=0; i<n; ++i){
        balltree_t::pos_t pt;
        rand(pt.begin(), pt.end());
        ballt.insert({pt});
    }
    ASSERT_EQ(ballt.size(), n);
    pout << "Volume after insertion = ", ballt.volume(), endl;

    ballt.bottom_up_refine();
    ASSERT_EQ(ballt.size(), n);
    pout << "Volume after refinement = ", ballt.volume(), endl;
}

TEST_F(InsertableBallTreeRawEmptyPaddingTest, 
    BottomUpRefineManyNodesFineInsertionPolicy)
{
    balltree_t ballt;
    pout << "Volume = ", ballt.volume(), endl;
    
    using ins_pl_t = balltree_t::insert_policy_t;
    ins_pl_t pl;
    pl.set_insert_favor(ins_pl_t::insert_favor_t::FINE);

    int n = 1000;
    for(int i=0; i<n; ++i){
        balltree_t::pos_t pt;
        rand(pt.begin(), pt.end());
        ballt.insert({pt}, pl);
    }
    ASSERT_EQ(ballt.size(), n);
    pout << "Volume after insertion = ", ballt.volume(), endl;

    ballt.bottom_up_refine(pl);
    ASSERT_EQ(ballt.size(), n);
    pout << "Volume after refinement = ", ballt.volume(), endl;
}

class InsertableBallTreeRawIntPaddingTest : public gt::Test {
public:
    using kdp_t = KDPoint<float, 3, sizeof(int)>;
    using balltree_t = _KDSEARCH::_InsertableBallTree<kdp_t, int>;
    using node_t = balltree_t::node_t;
    using index_t = balltree_t::index_t;

    InsertableBallTreeRawIntPaddingTest() {}
    ~InsertableBallTreeRawIntPaddingTest() override {}
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(InsertableBallTreeRawIntPaddingTest, DefaultInitialization){
    kdp_t pt;
    balltree_t ballt;
    balltree_t::node_t n;
}

TEST_F(InsertableBallTreeRawIntPaddingTest, InfoPrinting){
    balltree_t ballt;
    cout << ballt << endl;
    ballt.info() << endl;
    ballt.info(cout, 1);

    balltree_t::node_t nd;
    cout << nd << endl;
    nd.info() << endl;
    nd.info(cout, 1);

    balltree_t::construct_policy_t c_pl;
    cout << c_pl;
    c_pl.info() << endl;
    c_pl.info(cout, 1);

    balltree_t::insert_policy_t ins_pl;
    cout << ins_pl;
    ins_pl.info() << endl;
    ins_pl.info(cout, 1);
}

TEST_F(InsertableBallTreeRawIntPaddingTest, Insert1Node){
    balltree_t ballt;
    ballt.insert({{0,0,0}});
    ASSERT_EQ(ballt.size(), 1);

    auto &nds = ballt.nodes();
    auto &r = ballt.root();
    EXPECT_EQ(r.right_child_idx(), node_t::idxNULL);
    auto i1 = r.left_child_idx();
    ASSERT_LT(i1, nds.size());
    auto &n1 = nds[i1];
    EXPECT_EQ(n1.left_child_idx(), node_t::idxNULL);
    EXPECT_EQ(n1.right_child_idx(), node_t::idxNULL);
}

TEST_F(InsertableBallTreeRawIntPaddingTest, Insert2Nodes){
    balltree_t ballt;
    ballt.insert({{0,0,0}});
    ballt.insert({{1,1,1}});
    ASSERT_EQ(ballt.size(), 2);
    
    auto &nds = ballt.nodes();
    auto &r = ballt.root();
    auto i1 = r.left_child_idx();
    EXPECT_EQ(r.right_child_idx(), node_t::idxNULL);

    auto &n1 = nds[i1];
    auto i2 = nds[i1].left_child_idx(),
         i3 = nds[i1].right_child_idx();
    ASSERT_LT(i2,nds.size());
    ASSERT_LT(i3,nds.size());
    auto & n2 = nds[i2], &n3 = nds[i3];

    EXPECT_TRUE(r.is_internal());
    EXPECT_TRUE(n1.is_internal());
    EXPECT_TRUE(n2.is_leaf());
    EXPECT_TRUE(n3.is_leaf());
    EXPECT_EQ(n2.left_child_idx(), node_t::idxNULL);
    EXPECT_EQ(n2.right_child_idx(), node_t::idxNULL);
    EXPECT_EQ(n3.left_child_idx(), node_t::idxNULL);
    EXPECT_EQ(n3.right_child_idx(), node_t::idxNULL);
    EXPECT_EQ(n1.parent_idx(), balltree_t::ROOT_IDX);
    EXPECT_EQ(n2.parent_idx(), i1);
    EXPECT_EQ(n3.parent_idx(), i1);
}

TEST_F(InsertableBallTreeRawIntPaddingTest, InsertionWithDefaultPolicy){
    int cnt = 0;
    
    balltree_t ballt;
    ASSERT_TRUE(ballt.empty());
    ASSERT_EQ(ballt.size(), cnt); ++cnt;

    for(int i=0; i<50; ++i){
        float_t x = i;
        ballt.insert({{x,x,x}});
        ASSERT_EQ(ballt.size(), cnt); ++cnt;
    }

    balltree_t::pos_t pt;
    for(int i=0; i<1000; ++i){    
        rand(pt.begin(), pt.end()); pt *= 50;
        ballt.insert({pt});
        ASSERT_EQ(ballt.size(), cnt); ++cnt;
    }
}

TEST_F(InsertableBallTreeRawIntPaddingTest, InsertionWithFinePolicy){
    using ins_pl_t = balltree_t::insert_policy_t;
    ins_pl_t pl;
    pl.set_insert_favor(ins_pl_t::insert_favor_t::FINE);

    int cnt = 0;
    
    balltree_t ballt;
    ASSERT_TRUE(ballt.empty());
    ASSERT_EQ(ballt.size(), cnt); ++cnt;

    for(int i=0; i<50; ++i){
        float_t x = i;
        ballt.insert({{x,x,x}}, pl);
        ASSERT_EQ(ballt.size(), cnt); ++cnt;
        ASSERT_TRUE(pl.fringe().empty());
    }

    balltree_t::pos_t pt;
    for(int i=0; i<1000; ++i){    
        rand(pt.begin(), pt.end()); pt *= 50;
        ballt.insert({pt}, pl);
        ASSERT_EQ(ballt.size(), cnt); ++cnt;
        ASSERT_TRUE(pl.fringe().empty());
    }

    pl.info(cout, 1);
}

TEST_F(InsertableBallTreeRawIntPaddingTest, InsertionManyNodes) {
    balltree_t ballt;
    balltree_t::pos_t pt;
    int n = 100000, m = 10;
    for(int i=0; i<n; ++i){
        rand(pt.begin(), pt.end());
        ballt.insert({pt});
    }
    ASSERT_EQ(ballt.size(), n);
    pout << "Insert finished with default policy", endl;

    using ins_pl_t = balltree_t::insert_policy_t;
    ins_pl_t pl;
    pl.set_insert_favor(ins_pl_t::insert_favor_t::FINE);
    for(int i=0; i<m; ++i){
        rand(pt.begin(), pt.end());
        ballt.insert({pt}, pl);
    }
    ASSERT_EQ(ballt.size(), n+m);
    pout << "Insert finished with fine policy", endl;
}

TEST_F(InsertableBallTreeRawIntPaddingTest, BottomUpRefineEmpty){
    balltree_t ballt;
    ballt.bottom_up_refine();
    ASSERT_TRUE(ballt.empty());
}

TEST_F(InsertableBallTreeRawIntPaddingTest, BottomUpRefine1Node){
    balltree_t ballt;
    ballt.insert({{0,0,0}});
    ballt.bottom_up_refine();
    ASSERT_EQ(ballt.size(), 1);

    auto &nds = ballt.nodes();
    auto &r = ballt.root();
    EXPECT_EQ(r.right_child_idx(), node_t::idxNULL);
    auto i1 = r.left_child_idx();
    ASSERT_LT(i1, nds.size());
    auto &n1 = nds[i1];
    EXPECT_EQ(n1.left_child_idx(), node_t::idxNULL);
    EXPECT_EQ(n1.right_child_idx(), node_t::idxNULL);
}

TEST_F(InsertableBallTreeRawIntPaddingTest, BottomUpRefine2Nodes){
    balltree_t ballt;
    ballt.insert({{0,0,0}});
    ballt.insert({{1,1,1}});
    ballt.bottom_up_refine();
    ASSERT_EQ(ballt.size(), 2);
    
    auto &nds = ballt.nodes();
    auto &r = ballt.root();
    auto i1 = r.left_child_idx();
    EXPECT_EQ(r.right_child_idx(), node_t::idxNULL);

    auto &n1 = nds[i1];
    auto i2 = nds[i1].left_child_idx(),
         i3 = nds[i1].right_child_idx();
    ASSERT_LT(i2,nds.size());
    ASSERT_LT(i3,nds.size());
    auto & n2 = nds[i2], &n3 = nds[i3];

    EXPECT_TRUE(r.is_internal());
    EXPECT_TRUE(n1.is_internal());
    EXPECT_TRUE(n2.is_leaf());
    EXPECT_TRUE(n3.is_leaf());
    EXPECT_EQ(n2.left_child_idx(), node_t::idxNULL);
    EXPECT_EQ(n2.right_child_idx(), node_t::idxNULL);
    EXPECT_EQ(n3.left_child_idx(), node_t::idxNULL);
    EXPECT_EQ(n3.right_child_idx(), node_t::idxNULL);
    EXPECT_EQ(n1.parent_idx(), balltree_t::ROOT_IDX);
    EXPECT_EQ(n2.parent_idx(), i1);
    EXPECT_EQ(n3.parent_idx(), i1);
}

TEST_F(InsertableBallTreeRawIntPaddingTest, BottomUpRefineManyNodes){
    balltree_t ballt;
    pout << "Volume = ", ballt.volume(), endl;
    
    int n = 1000;
    for(int i=0; i<n; ++i){
        balltree_t::pos_t pt;
        rand(pt.begin(), pt.end());
        ballt.insert({pt});
    }
    ASSERT_EQ(ballt.size(), n);
    pout << "Volume after insertion = ", ballt.volume(), endl;

    ballt.bottom_up_refine();
    ASSERT_EQ(ballt.size(), n);
    pout << "Volume after refinement = ", ballt.volume(), endl;
}

TEST_F(InsertableBallTreeRawIntPaddingTest, 
    BottomUpRefineManyNodesFineInsertionPolicy)
{
    balltree_t ballt;
    pout << "Volume = ", ballt.volume(), endl;
    
    using ins_pl_t = balltree_t::insert_policy_t;
    ins_pl_t pl;
    pl.set_insert_favor(ins_pl_t::insert_favor_t::FINE);

    int n = 1000;
    for(int i=0; i<n; ++i){
        balltree_t::pos_t pt;
        rand(pt.begin(), pt.end());
        ballt.insert({pt}, pl);
    }
    ASSERT_EQ(ballt.size(), n);
    pout << "Volume after insertion = ", ballt.volume(), endl;

    ballt.bottom_up_refine(pl);
    ASSERT_EQ(ballt.size(), n);
    pout << "Volume after refinement = ", ballt.volume(), endl;
}

class InsertableBallTreeLargePaddingTest : public gt::Test {
public:
    using kdp_t = KDPoint<float, 3, 1024>;
    using balltree_t = _KDSEARCH::_InsertableBallTree<kdp_t, int>;
    using node_t = balltree_t::node_t;
    using index_t = balltree_t::index_t;

    InsertableBallTreeLargePaddingTest() {}
    ~InsertableBallTreeLargePaddingTest() override {}
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(InsertableBallTreeLargePaddingTest, DefaultInitialization){
    kdp_t pt;
    balltree_t ballt;
    balltree_t::node_t n;
}

TEST_F(InsertableBallTreeLargePaddingTest, InfoPrinting){
    balltree_t ballt;
    cout << ballt << endl;
    ballt.info() << endl;
    ballt.info(cout, 1);

    balltree_t::node_t nd;
    cout << nd << endl;
    nd.info() << endl;
    nd.info(cout, 1);

    balltree_t::construct_policy_t c_pl;
    cout << c_pl;
    c_pl.info() << endl;
    c_pl.info(cout, 1);

    balltree_t::insert_policy_t ins_pl;
    cout << ins_pl;
    ins_pl.info() << endl;
    ins_pl.info(cout, 1);
}

TEST_F(InsertableBallTreeLargePaddingTest, Insert1Node){
    balltree_t ballt;
    ballt.insert({{0,0,0}});
    ASSERT_EQ(ballt.size(), 1);

    auto &nds = ballt.nodes();
    auto &r = ballt.root();
    EXPECT_EQ(r.right_child_idx(), node_t::idxNULL);
    auto i1 = r.left_child_idx();
    ASSERT_LT(i1, nds.size());
    auto &n1 = nds[i1];
    EXPECT_EQ(n1.left_child_idx(), node_t::idxNULL);
    EXPECT_EQ(n1.right_child_idx(), node_t::idxNULL);
}

TEST_F(InsertableBallTreeLargePaddingTest, Insert2Nodes){
    balltree_t ballt;
    ballt.insert({{0,0,0}});
    ballt.insert({{1,1,1}});
    ASSERT_EQ(ballt.size(), 2);
    
    auto &nds = ballt.nodes();
    auto &r = ballt.root();
    auto i1 = r.left_child_idx();
    EXPECT_EQ(r.right_child_idx(), node_t::idxNULL);

    auto &n1 = nds[i1];
    auto i2 = nds[i1].left_child_idx(),
         i3 = nds[i1].right_child_idx();
    ASSERT_LT(i2,nds.size());
    ASSERT_LT(i3,nds.size());
    auto & n2 = nds[i2], &n3 = nds[i3];

    EXPECT_TRUE(r.is_internal());
    EXPECT_TRUE(n1.is_internal());
    EXPECT_TRUE(n2.is_leaf());
    EXPECT_TRUE(n3.is_leaf());
    EXPECT_EQ(n2.left_child_idx(), node_t::idxNULL);
    EXPECT_EQ(n2.right_child_idx(), node_t::idxNULL);
    EXPECT_EQ(n3.left_child_idx(), node_t::idxNULL);
    EXPECT_EQ(n3.right_child_idx(), node_t::idxNULL);
    EXPECT_EQ(n1.parent_idx(), balltree_t::ROOT_IDX);
    EXPECT_EQ(n2.parent_idx(), i1);
    EXPECT_EQ(n3.parent_idx(), i1);
}

TEST_F(InsertableBallTreeLargePaddingTest, InsertionWithDefaultPolicy){
    int cnt = 0;
    
    balltree_t ballt;
    ASSERT_TRUE(ballt.empty());
    ASSERT_EQ(ballt.size(), cnt); ++cnt;

    for(int i=0; i<50; ++i){
        float_t x = i;
        ballt.insert({{x,x,x}});
        ASSERT_EQ(ballt.size(), cnt); ++cnt;
    }

    balltree_t::pos_t pt;
    for(int i=0; i<1000; ++i){    
        rand(pt.begin(), pt.end()); pt *= 50;
        ballt.insert({pt});
        ASSERT_EQ(ballt.size(), cnt); ++cnt;
    }
}

TEST_F(InsertableBallTreeLargePaddingTest, InsertionWithFinePolicy){
    using ins_pl_t = balltree_t::insert_policy_t;
    ins_pl_t pl;
    pl.set_insert_favor(ins_pl_t::insert_favor_t::FINE);

    int cnt = 0;
    
    balltree_t ballt;
    ASSERT_TRUE(ballt.empty());
    ASSERT_EQ(ballt.size(), cnt); ++cnt;

    for(int i=0; i<50; ++i){
        float_t x = i;
        ballt.insert({{x,x,x}}, pl);
        ASSERT_EQ(ballt.size(), cnt); ++cnt;
        ASSERT_TRUE(pl.fringe().empty());
    }

    balltree_t::pos_t pt;
    for(int i=0; i<1000; ++i){    
        rand(pt.begin(), pt.end()); pt *= 50;
        ballt.insert({pt}, pl);
        ASSERT_EQ(ballt.size(), cnt); ++cnt;
        ASSERT_TRUE(pl.fringe().empty());
    }

    pl.info(cout, 1);
}

TEST_F(InsertableBallTreeLargePaddingTest, InsertionManyNodes) {
    balltree_t ballt;
    balltree_t::pos_t pt;
    int n = 100000, m = 10;
    for(int i=0; i<n; ++i){
        rand(pt.begin(), pt.end());
        ballt.insert({pt});
    }
    ASSERT_EQ(ballt.size(), n);
    pout << "Insert finished with default policy", endl;

    using ins_pl_t = balltree_t::insert_policy_t;
    ins_pl_t pl;
    pl.set_insert_favor(ins_pl_t::insert_favor_t::FINE);
    for(int i=0; i<m; ++i){
        rand(pt.begin(), pt.end());
        ballt.insert({pt}, pl);
    }
    ASSERT_EQ(ballt.size(), n+m);
    pout << "Insert finished with fine policy", endl;
}

TEST_F(InsertableBallTreeLargePaddingTest, BottomUpRefineEmpty){
    balltree_t ballt;
    ballt.bottom_up_refine();
    ASSERT_TRUE(ballt.empty());
}

TEST_F(InsertableBallTreeLargePaddingTest, BottomUpRefine1Node){
    balltree_t ballt;
    ballt.insert({{0,0,0}});
    ballt.bottom_up_refine();
    ASSERT_EQ(ballt.size(), 1);

    auto &nds = ballt.nodes();
    auto &r = ballt.root();
    EXPECT_EQ(r.right_child_idx(), node_t::idxNULL);
    auto i1 = r.left_child_idx();
    ASSERT_LT(i1, nds.size());
    auto &n1 = nds[i1];
    EXPECT_EQ(n1.left_child_idx(), node_t::idxNULL);
    EXPECT_EQ(n1.right_child_idx(), node_t::idxNULL);
}

TEST_F(InsertableBallTreeLargePaddingTest, BottomUpRefine2Nodes){
    balltree_t ballt;
    ballt.insert({{0,0,0}});
    ballt.insert({{1,1,1}});
    ballt.bottom_up_refine();
    ASSERT_EQ(ballt.size(), 2);
    
    auto &nds = ballt.nodes();
    auto &r = ballt.root();
    auto i1 = r.left_child_idx();
    EXPECT_EQ(r.right_child_idx(), node_t::idxNULL);

    auto &n1 = nds[i1];
    auto i2 = nds[i1].left_child_idx(),
         i3 = nds[i1].right_child_idx();
    ASSERT_LT(i2,nds.size());
    ASSERT_LT(i3,nds.size());
    auto & n2 = nds[i2], &n3 = nds[i3];

    EXPECT_TRUE(r.is_internal());
    EXPECT_TRUE(n1.is_internal());
    EXPECT_TRUE(n2.is_leaf());
    EXPECT_TRUE(n3.is_leaf());
    EXPECT_EQ(n2.left_child_idx(), node_t::idxNULL);
    EXPECT_EQ(n2.right_child_idx(), node_t::idxNULL);
    EXPECT_EQ(n3.left_child_idx(), node_t::idxNULL);
    EXPECT_EQ(n3.right_child_idx(), node_t::idxNULL);
    EXPECT_EQ(n1.parent_idx(), balltree_t::ROOT_IDX);
    EXPECT_EQ(n2.parent_idx(), i1);
    EXPECT_EQ(n3.parent_idx(), i1);
}

TEST_F(InsertableBallTreeLargePaddingTest, BottomUpRefineManyNodes){
    balltree_t ballt;
    pout << "Volume = ", ballt.volume(), endl;
    
    int n = 1000;
    for(int i=0; i<n; ++i){
        balltree_t::pos_t pt;
        rand(pt.begin(), pt.end());
        ballt.insert({pt});
    }
    ASSERT_EQ(ballt.size(), n);
    pout << "Volume after insertion = ", ballt.volume(), endl;

    ballt.bottom_up_refine();
    ASSERT_EQ(ballt.size(), n);
    pout << "Volume after refinement = ", ballt.volume(), endl;
}

TEST_F(InsertableBallTreeLargePaddingTest, 
    BottomUpRefineManyNodesFineInsertionPolicy)
{
    balltree_t ballt;
    pout << "Volume = ", ballt.volume(), endl;
    
    using ins_pl_t = balltree_t::insert_policy_t;
    ins_pl_t pl;
    pl.set_insert_favor(ins_pl_t::insert_favor_t::FINE);

    int n = 1000;
    for(int i=0; i<n; ++i){
        balltree_t::pos_t pt;
        rand(pt.begin(), pt.end());
        ballt.insert({pt}, pl);
    }
    ASSERT_EQ(ballt.size(), n);
    pout << "Volume after insertion = ", ballt.volume(), endl;

    ballt.bottom_up_refine(pl);
    ASSERT_EQ(ballt.size(), n);
    pout << "Volume after refinement = ", ballt.volume(), endl;
}

class InsertableBallTreeNonAlignedPaddingTest : public gt::Test {
public:
    using kdp_t = KDPoint<float, 3, 11>;
    using balltree_t = _KDSEARCH::_InsertableBallTree<kdp_t, int>;
    using node_t = balltree_t::node_t;
    using index_t = balltree_t::index_t;

    InsertableBallTreeNonAlignedPaddingTest() {}
    ~InsertableBallTreeNonAlignedPaddingTest() override {}
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(InsertableBallTreeNonAlignedPaddingTest, DefaultInitialization){
    kdp_t pt;
    balltree_t ballt;
    balltree_t::node_t n;
}

TEST_F(InsertableBallTreeNonAlignedPaddingTest, InfoPrinting){
    balltree_t ballt;
    cout << ballt << endl;
    ballt.info() << endl;
    ballt.info(cout, 1);

    balltree_t::node_t nd;
    cout << nd << endl;
    nd.info() << endl;
    nd.info(cout, 1);

    balltree_t::construct_policy_t c_pl;
    cout << c_pl;
    c_pl.info() << endl;
    c_pl.info(cout, 1);

    balltree_t::insert_policy_t ins_pl;
    cout << ins_pl;
    ins_pl.info() << endl;
    ins_pl.info(cout, 1);
}

TEST_F(InsertableBallTreeNonAlignedPaddingTest, Insert1Node){
    balltree_t ballt;
    ballt.insert({{0,0,0}});
    ASSERT_EQ(ballt.size(), 1);

    auto &nds = ballt.nodes();
    auto &r = ballt.root();
    EXPECT_EQ(r.right_child_idx(), node_t::idxNULL);
    auto i1 = r.left_child_idx();
    ASSERT_LT(i1, nds.size());
    auto &n1 = nds[i1];
    EXPECT_EQ(n1.left_child_idx(), node_t::idxNULL);
    EXPECT_EQ(n1.right_child_idx(), node_t::idxNULL);
}

TEST_F(InsertableBallTreeNonAlignedPaddingTest, Insert2Nodes){
    balltree_t ballt;
    ballt.insert({{0,0,0}});
    ballt.insert({{1,1,1}});
    ASSERT_EQ(ballt.size(), 2);
    
    auto &nds = ballt.nodes();
    auto &r = ballt.root();
    auto i1 = r.left_child_idx();
    EXPECT_EQ(r.right_child_idx(), node_t::idxNULL);

    auto &n1 = nds[i1];
    auto i2 = nds[i1].left_child_idx(),
         i3 = nds[i1].right_child_idx();
    ASSERT_LT(i2,nds.size());
    ASSERT_LT(i3,nds.size());
    auto & n2 = nds[i2], &n3 = nds[i3];

    EXPECT_TRUE(r.is_internal());
    EXPECT_TRUE(n1.is_internal());
    EXPECT_TRUE(n2.is_leaf());
    EXPECT_TRUE(n3.is_leaf());
    EXPECT_EQ(n2.left_child_idx(), node_t::idxNULL);
    EXPECT_EQ(n2.right_child_idx(), node_t::idxNULL);
    EXPECT_EQ(n3.left_child_idx(), node_t::idxNULL);
    EXPECT_EQ(n3.right_child_idx(), node_t::idxNULL);
    EXPECT_EQ(n1.parent_idx(), balltree_t::ROOT_IDX);
    EXPECT_EQ(n2.parent_idx(), i1);
    EXPECT_EQ(n3.parent_idx(), i1);
}

TEST_F(InsertableBallTreeNonAlignedPaddingTest, InsertionWithDefaultPolicy){
    int cnt = 0;
    
    balltree_t ballt;
    ASSERT_TRUE(ballt.empty());
    ASSERT_EQ(ballt.size(), cnt); ++cnt;

    for(int i=0; i<50; ++i){
        float_t x = i;
        ballt.insert({{x,x,x}});
        ASSERT_EQ(ballt.size(), cnt); ++cnt;
    }

    balltree_t::pos_t pt;
    for(int i=0; i<1000; ++i){    
        rand(pt.begin(), pt.end()); pt *= 50;
        ballt.insert({pt});
        ASSERT_EQ(ballt.size(), cnt); ++cnt;
    }
}

TEST_F(InsertableBallTreeNonAlignedPaddingTest, InsertionWithFinePolicy){
    using ins_pl_t = balltree_t::insert_policy_t;
    ins_pl_t pl;
    pl.set_insert_favor(ins_pl_t::insert_favor_t::FINE);

    int cnt = 0;
    
    balltree_t ballt;
    ASSERT_TRUE(ballt.empty());
    ASSERT_EQ(ballt.size(), cnt); ++cnt;

    for(int i=0; i<50; ++i){
        float_t x = i;
        ballt.insert({{x,x,x}}, pl);
        ASSERT_EQ(ballt.size(), cnt); ++cnt;
        ASSERT_TRUE(pl.fringe().empty());
    }

    balltree_t::pos_t pt;
    for(int i=0; i<1000; ++i){    
        rand(pt.begin(), pt.end()); pt *= 50;
        ballt.insert({pt}, pl);
        ASSERT_EQ(ballt.size(), cnt); ++cnt;
        ASSERT_TRUE(pl.fringe().empty());
    }

    pl.info(cout, 1);
}

TEST_F(InsertableBallTreeNonAlignedPaddingTest, InsertionManyNodes) {
    balltree_t ballt;
    balltree_t::pos_t pt;
    int n = 100000, m = 10;
    for(int i=0; i<n; ++i){
        rand(pt.begin(), pt.end());
        ballt.insert({pt});
    }
    ASSERT_EQ(ballt.size(), n);
    pout << "Insert finished with default policy", endl;

    using ins_pl_t = balltree_t::insert_policy_t;
    ins_pl_t pl;
    pl.set_insert_favor(ins_pl_t::insert_favor_t::FINE);
    for(int i=0; i<m; ++i){
        rand(pt.begin(), pt.end());
        ballt.insert({pt}, pl);
    }
    ASSERT_EQ(ballt.size(), n+m);
    pout << "Insert finished with fine policy", endl;
}

TEST_F(InsertableBallTreeNonAlignedPaddingTest, BottomUpRefineEmpty){
    balltree_t ballt;
    ballt.bottom_up_refine();
    ASSERT_TRUE(ballt.empty());
}

TEST_F(InsertableBallTreeNonAlignedPaddingTest, BottomUpRefine1Node){
    balltree_t ballt;
    ballt.insert({{0,0,0}});
    ballt.bottom_up_refine();
    ASSERT_EQ(ballt.size(), 1);

    auto &nds = ballt.nodes();
    auto &r = ballt.root();
    EXPECT_EQ(r.right_child_idx(), node_t::idxNULL);
    auto i1 = r.left_child_idx();
    ASSERT_LT(i1, nds.size());
    auto &n1 = nds[i1];
    EXPECT_EQ(n1.left_child_idx(), node_t::idxNULL);
    EXPECT_EQ(n1.right_child_idx(), node_t::idxNULL);
}

TEST_F(InsertableBallTreeNonAlignedPaddingTest, BottomUpRefine2Nodes){
    balltree_t ballt;
    ballt.insert({{0,0,0}});
    ballt.insert({{1,1,1}});
    ballt.bottom_up_refine();
    ASSERT_EQ(ballt.size(), 2);
    
    auto &nds = ballt.nodes();
    auto &r = ballt.root();
    auto i1 = r.left_child_idx();
    EXPECT_EQ(r.right_child_idx(), node_t::idxNULL);

    auto &n1 = nds[i1];
    auto i2 = nds[i1].left_child_idx(),
         i3 = nds[i1].right_child_idx();
    ASSERT_LT(i2,nds.size());
    ASSERT_LT(i3,nds.size());
    auto & n2 = nds[i2], &n3 = nds[i3];

    EXPECT_TRUE(r.is_internal());
    EXPECT_TRUE(n1.is_internal());
    EXPECT_TRUE(n2.is_leaf());
    EXPECT_TRUE(n3.is_leaf());
    EXPECT_EQ(n2.left_child_idx(), node_t::idxNULL);
    EXPECT_EQ(n2.right_child_idx(), node_t::idxNULL);
    EXPECT_EQ(n3.left_child_idx(), node_t::idxNULL);
    EXPECT_EQ(n3.right_child_idx(), node_t::idxNULL);
    EXPECT_EQ(n1.parent_idx(), balltree_t::ROOT_IDX);
    EXPECT_EQ(n2.parent_idx(), i1);
    EXPECT_EQ(n3.parent_idx(), i1);
}

TEST_F(InsertableBallTreeNonAlignedPaddingTest, BottomUpRefineManyNodes){
    balltree_t ballt;
    pout << "Volume = ", ballt.volume(), endl;
    
    int n = 1000;
    for(int i=0; i<n; ++i){
        balltree_t::pos_t pt;
        rand(pt.begin(), pt.end());
        ballt.insert({pt});
    }
    ASSERT_EQ(ballt.size(), n);
    pout << "Volume after insertion = ", ballt.volume(), endl;

    ballt.bottom_up_refine();
    ASSERT_EQ(ballt.size(), n);
    pout << "Volume after refinement = ", ballt.volume(), endl;
}

TEST_F(InsertableBallTreeNonAlignedPaddingTest, 
    BottomUpRefineManyNodesFineInsertionPolicy)
{
    balltree_t ballt;
    pout << "Volume = ", ballt.volume(), endl;
    
    using ins_pl_t = balltree_t::insert_policy_t;
    ins_pl_t pl;
    pl.set_insert_favor(ins_pl_t::insert_favor_t::FINE);

    int n = 1000;
    for(int i=0; i<n; ++i){
        balltree_t::pos_t pt;
        rand(pt.begin(), pt.end());
        ballt.insert({pt}, pl);
    }
    ASSERT_EQ(ballt.size(), n);
    pout << "Volume after insertion = ", ballt.volume(), endl;

    ballt.bottom_up_refine(pl);
    ASSERT_EQ(ballt.size(), n);
    pout << "Volume after refinement = ", ballt.volume(), endl;
}

} // namespace

} // namespace HIPP::NUMERICAL