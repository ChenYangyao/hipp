#include <h5_test_incl.h>


namespace HIPP::IO::H5 {
namespace {

class XTableTest: public gt::Test, public H5TestFile { 
protected:
    string fixture_name() override { return "XTableTest"; }

    void SetUp() override {
        _file0 = create_file("file0", "w");
        _g0 = _file0.create_group("g0");
    }
    void TearDown() override {
        _g0.free();
        _file0.free();
        clear_up();
    }

    File _file0;
    Group _g0;

    /** Record type for testing. */
    struct record_t {
        int i;
        double d;
        int i5[5];
        double d34[3][4];
        std::array<int, 5> arr_i5;
        std::array<std::array<double, 4>, 3> arr_d34;
        record_t(int _i, double _d, int _i5, double _d34, 
            int _arr_i5, double _arr_d34)
        : i(_i), d(_d)
        {
            for(int i=0; i<5; ++i){
                i5[i] = _i5 + i;
                arr_i5[i] = _arr_i5 + i;
            }
            for(int i=0; i<3; ++i){
                for(int j=0; j<4; ++j){
                    d34[i][j] = _d34 + i*j;
                    arr_d34[i][j] = _arr_d34 + i*j;
                }
            }
        }
        record_t(){}
    };
    typedef XTable<record_t> xtable_t;
    typedef typename xtable_t::table_t table_t;
    table_t get_table(size_t n) {
        table_t ret(n);
        for(size_t i=0; i<n; ++i){
            ret[i] = record_t(i, i+1.0, (int)(i+2), 
                (i+3.0), (int)(i+4), i+5.0);
        }
        return ret;
    }
    static void chk_eq(string field, size_t beg, size_t cnt, size_t step, 
        size_t blk, const table_t &src,
        const table_t &dst, string err_info)
    {

        size_t i_dst = 0;
        for(size_t i_blk=0; i_blk<cnt; ++i_blk){
            size_t b = beg + step * i_blk;
            for(size_t i_src=b; i_src<b+blk; ++i_src){
                auto &srci = src[i_src];
                auto &dsti = dst[i_dst];
                if( field == "i" )
                    EXPECT_EQ(srci.i, dsti.i) 
                    << err_info << ", src block " << i_blk
                    << ", idx " << i_src << ", dst idx " << i_dst;
                else if(field == "d") 
                    EXPECT_DOUBLE_EQ(srci.d, dsti.d)
                    << err_info << ", src block " << i_blk
                    << ", idx " << i_src << ", dst idx " << i_dst;
                else if(field == "i5") {
                    for(int i=0; i<5; ++i)
                    EXPECT_EQ(srci.i5[i], dsti.i5[i])
                        << err_info << ", src block " << i_blk
                        << ", idx " << i_src << ", dst idx " << i_dst 
                        << "i5[" << i << "]";
                }else if(field == "d34") {
                    for(int i=0; i<3; ++i) for(int j=0; j<4; ++j)
                    EXPECT_DOUBLE_EQ(srci.d34[i][j], dsti.d34[i][j])
                        << err_info << ", src block " << i_blk
                        << ", idx " << i_src << ", dst idx " << i_dst 
                        << "d34[" << i << "][" << j << "]";
                } else if(field == "arr_i5") 
                    EXPECT_THAT(srci.arr_i5, gt::ElementsAreArray(dsti.arr_i5))
                        << err_info << ", src block " << i_blk
                        << ", idx " << i_src << ", dst idx " << i_dst;
                else if ( field == "arr_d34" )
                    for(int i=0; i<3; ++i)
                    EXPECT_THAT(srci.arr_d34[i], 
                        gt::ElementsAreArray(dsti.arr_d34[i]))
                        << err_info << ", src block " << i_blk
                        << ", idx " << i_src << ", dst idx " << i_dst
                        << "arr_d34[" << i << "]";
                else 
                    ErrLogic::throw_(ErrLogic::eINVALIDARG, emFLPFB, 
                        "  ... invalid field ", field, '\n');
                ++ i_dst;
            }
        }
    }
};

TEST_F(XTableTest, Declaration) {
    typedef record_t R;
    XTable xtbl{ "i",  &R::i, "d", &R::d};
    EXPECT_THAT(xtbl, gt::An<xtable_t>());

    auto xtbl_macro_got = HIPPIO_H5_XTABLE(R, i, d);
    EXPECT_THAT(xtbl_macro_got, gt::An<xtable_t>());

    HIPPIO_H5_XTABLE_DEF(xtbl_macro_def, R, i, d);
    EXPECT_THAT(xtbl_macro_def, gt::An<xtable_t>());
}

TEST_F(XTableTest, AddFields) {
    typedef record_t R;
    HIPPIO_H5_XTABLE_DEF(xtbl, R, i, d);

    EXPECT_TRUE(xtbl.has_field("i"));
    EXPECT_TRUE(xtbl.has_field("d"));
    EXPECT_FALSE(xtbl.has_field("d34"));
    EXPECT_EQ(xtbl.n_fields(), 2);
    EXPECT_FALSE(xtbl.empty());

    xtbl.add_field("d34", &R::d34);
    xtbl.add_field("i5", &R::i5);
    EXPECT_TRUE(xtbl.has_field("d34"));
    EXPECT_TRUE(xtbl.has_field("i5"));
    EXPECT_EQ(xtbl.n_fields(), 4);

    xtbl.remove_field("d34");
    EXPECT_FALSE(xtbl.has_field("d34"));
    EXPECT_EQ(xtbl.n_fields(), 3);
}

TEST_F(XTableTest, CopyAndMove) {
    typedef record_t R;
    HIPPIO_H5_XTABLE_DEF(xtbl, R, i, d);

    auto xtbl_copied = xtbl;
    EXPECT_EQ(xtbl.n_fields(), xtbl_copied.n_fields());
    EXPECT_EQ(xtbl.empty(), xtbl_copied.empty());

    EXPECT_TRUE(xtbl_copied.has_field("i"));
    EXPECT_TRUE(xtbl_copied.has_field("d"));
    EXPECT_FALSE(xtbl_copied.has_field("d34"));

    xtbl_copied.add_field("d34", &R::d34);
    EXPECT_TRUE(xtbl_copied.has_field("d34"));
    EXPECT_EQ(xtbl_copied.n_fields(), 3);

    xtbl = std::move(xtbl_copied);
    EXPECT_TRUE(xtbl.has_field("i"));
    EXPECT_TRUE(xtbl.has_field("d"));
    EXPECT_TRUE(xtbl.has_field("d34"));
    EXPECT_EQ(xtbl.n_fields(), 3);
}

TEST_F(XTableTest, WriteAllFields) {
    typedef record_t R;
    HIPPIO_H5_XTABLE_DEF(xtbl, R, i, d, i5, d34, arr_i5, arr_d34);

    auto rs = get_table(32);
    xtbl.write(rs, _g0);

    ASSERT_EQ(_g0.get_info().nlinks, 6);
    
    for( auto name : { "i", "d", "i5", "d34", "arr_i5", "arr_d34" } )
        EXPECT_TRUE(_g0.dataset_exists(name));

    {
        auto d = _g0.open_dataset("i");
        auto dsp = d.dataspace(); auto dt = d.datatype();
        EXPECT_THAT(dsp.dims().as_vector(), gt::ElementsAre(32));
        EXPECT_EQ(dt.get_class(), dt.cINTEGER);
        EXPECT_EQ(dt.size(), sizeof(int));
        vector<int> v_in;
        d.read(v_in);
        ASSERT_EQ(v_in.size(), rs.size());
        for(size_t i=0; i<v_in.size(); ++i)
            EXPECT_EQ(v_in[i], rs[i].i) << i;

    }
    {
        auto d = _g0.open_dataset("d");
        auto dsp = d.dataspace(); auto dt = d.datatype();
        EXPECT_THAT(dsp.dims().as_vector(), gt::ElementsAre(32));
        EXPECT_EQ(dt.get_class(), dt.cFLOAT);
        EXPECT_EQ(dt.size(), sizeof(double));
        vector<double> v_in;
        d.read(v_in);
        ASSERT_EQ(v_in.size(), rs.size());
        for(size_t i=0; i<v_in.size(); ++i)
            EXPECT_DOUBLE_EQ(v_in[i], rs[i].d) << i;
    }
    {
        auto d = _g0.open_dataset("i5");
        auto dsp = d.dataspace(); auto dt = d.datatype();
        EXPECT_THAT(dsp.dims().as_vector(), gt::ElementsAre(32,5));
        EXPECT_EQ(dt.get_class(), dt.cINTEGER);
        EXPECT_EQ(dt.size(), sizeof(int));
        vector<std::array<int, 5> > v_in;
        d.read(v_in);
        ASSERT_EQ(v_in.size(), rs.size());
        for(size_t i=0; i<v_in.size(); ++i)
            EXPECT_THAT(v_in[i], gt::ElementsAreArray(rs[i].i5)) << i;
    }
    {
        auto d = _g0.open_dataset("d34");
        auto dsp = d.dataspace(); auto dt = d.datatype();
        EXPECT_THAT(dsp.dims().as_vector(), gt::ElementsAre(32,3,4));
        EXPECT_EQ(dt.get_class(), dt.cFLOAT);
        EXPECT_EQ(dt.size(), sizeof(double));
        vector<std::array<std::array<double, 4>, 3> > v_in;
        d.read(v_in);
        ASSERT_EQ(v_in.size(), rs.size());
        for(size_t i=0; i<v_in.size(); ++i)
            for(size_t j=0; j<3; ++j)
                EXPECT_THAT(v_in[i][j], gt::ElementsAreArray(rs[i].d34[j])) 
                    << i << ", " << j;
                
    }
    {
        auto d = _g0.open_dataset("arr_i5");
        auto dsp = d.dataspace(); auto dt = d.datatype();
        EXPECT_THAT(dsp.dims().as_vector(), gt::ElementsAre(32,5));
        EXPECT_EQ(dt.get_class(), dt.cINTEGER);
        EXPECT_EQ(dt.size(), sizeof(int));
        vector<std::array<int, 5> > v_in;
        d.read(v_in);
        ASSERT_EQ(v_in.size(), rs.size());
        for(size_t i=0; i<v_in.size(); ++i)
            EXPECT_THAT(v_in[i], gt::ElementsAreArray(rs[i].arr_i5)) << i;
    }
    {
        auto d = _g0.open_dataset("arr_d34");
        auto dsp = d.dataspace(); auto dt = d.datatype();
        EXPECT_THAT(dsp.dims().as_vector(), gt::ElementsAre(32,3,4));
        EXPECT_EQ(dt.get_class(), dt.cFLOAT);
        EXPECT_EQ(dt.size(), sizeof(double));
        vector<std::array<std::array<double, 4>, 3> > v_in;
        d.read(v_in);
        ASSERT_EQ(v_in.size(), rs.size());
        for(size_t i=0; i<v_in.size(); ++i)
        for(size_t j=0; j<3; ++j)
            EXPECT_THAT(v_in[i][j], 
                gt::ElementsAreArray(rs[i].arr_d34[j])) << i;
    }
}

TEST_F(XTableTest, WritePartOfFields) {
    typedef record_t R;
    HIPPIO_H5_XTABLE_DEF(xtbl, R, i, i5);
    xtbl.add_field("d34", &R::d34);

    auto rs = get_table(32);
    xtbl.write(rs, _g0);

    ASSERT_EQ(_g0.get_info().nlinks, 3);
    
    for( auto name : { "i", "i5", "d34"} )
        EXPECT_TRUE(_g0.dataset_exists(name));
    for ( auto name : {"d", "arr_i5", "arr_d34"} ) 
        EXPECT_FALSE(_g0.dataset_exists(name));

    {
        auto d = _g0.open_dataset("i");
        auto dsp = d.dataspace(); auto dt = d.datatype();
        EXPECT_THAT(dsp.dims().as_vector(), gt::ElementsAre(32));
        EXPECT_EQ(dt.get_class(), dt.cINTEGER);
        EXPECT_EQ(dt.size(), sizeof(int));
        vector<int> v_in;
        d.read(v_in);
        ASSERT_EQ(v_in.size(), rs.size());
        for(size_t i=0; i<v_in.size(); ++i)
            EXPECT_EQ(v_in[i], rs[i].i) << i;

    }
    {
        auto d = _g0.open_dataset("i5");
        auto dsp = d.dataspace(); auto dt = d.datatype();
        EXPECT_THAT(dsp.dims().as_vector(), gt::ElementsAre(32,5));
        EXPECT_EQ(dt.get_class(), dt.cINTEGER);
        EXPECT_EQ(dt.size(), sizeof(int));
        vector<std::array<int, 5> > v_in;
        d.read(v_in);
        ASSERT_EQ(v_in.size(), rs.size());
        for(size_t i=0; i<v_in.size(); ++i)
            EXPECT_THAT(v_in[i], gt::ElementsAreArray(rs[i].i5)) << i;
    }
    {
        auto d = _g0.open_dataset("d34");
        auto dsp = d.dataspace(); auto dt = d.datatype();
        EXPECT_THAT(dsp.dims().as_vector(), gt::ElementsAre(32,3,4));
        EXPECT_EQ(dt.get_class(), dt.cFLOAT);
        EXPECT_EQ(dt.size(), sizeof(double));
        vector<std::array<std::array<double, 4>, 3> > v_in;
        d.read(v_in);
        ASSERT_EQ(v_in.size(), rs.size());
        for(size_t i=0; i<v_in.size(); ++i)
            for(size_t j=0; j<3; ++j)
                EXPECT_THAT(v_in[i][j], gt::ElementsAreArray(rs[i].d34[j])) 
                    << i << ", " << j;
                
    }
}

TEST_F(XTableTest, WriteSelectedFields) {
    typedef record_t R;
    HIPPIO_H5_XTABLE_DEF(xtbl, R, i, i5, d34);
    auto rs = get_table(32);

    xtbl.select_fields({"i", "i5"});
    auto g1 = _file0.create_group("g1");
    xtbl.write(rs, g1);
    ASSERT_EQ(g1.get_info().nlinks, 2);
    
    for( auto name : { "i", "i5"} )
        EXPECT_TRUE(g1.dataset_exists(name));
    for ( auto name : {"d", "d34", "arr_i5", "arr_d34"} ) 
        EXPECT_FALSE(g1.dataset_exists(name));

    {
        auto d = g1.open_dataset("i");
        auto dsp = d.dataspace(); auto dt = d.datatype();
        EXPECT_THAT(dsp.dims().as_vector(), gt::ElementsAre(32));
        EXPECT_EQ(dt.get_class(), dt.cINTEGER);
        EXPECT_EQ(dt.size(), sizeof(int));
        vector<int> v_in;
        d.read(v_in);
        ASSERT_EQ(v_in.size(), rs.size());
        for(size_t i=0; i<v_in.size(); ++i)
            EXPECT_EQ(v_in[i], rs[i].i) << i;

    }
    {
        auto d = g1.open_dataset("i5");
        auto dsp = d.dataspace(); auto dt = d.datatype();
        EXPECT_THAT(dsp.dims().as_vector(), gt::ElementsAre(32,5));
        EXPECT_EQ(dt.get_class(), dt.cINTEGER);
        EXPECT_EQ(dt.size(), sizeof(int));
        vector<std::array<int, 5> > v_in;
        d.read(v_in);
        ASSERT_EQ(v_in.size(), rs.size());
        for(size_t i=0; i<v_in.size(); ++i)
            EXPECT_THAT(v_in[i], gt::ElementsAreArray(rs[i].i5)) << i;
    }
}

TEST_F(XTableTest, ReadAllFields) {
    typedef record_t R;
    HIPPIO_H5_XTABLE_DEF(xtbl, R, i, d, i5, d34, arr_i5, arr_d34);

    auto rs = get_table(32);
    xtbl.write(rs, _g0);

    table_t rs_ld = xtbl.read(_g0);
    ASSERT_EQ(rs_ld.size(), rs.size());
    for( auto name : {"i", "d", "i5", "d34", "arr_i5", "arr_d34"} ) 
        chk_eq(name, 0, rs.size(), 1, 1, rs, rs_ld, name);

    table_t rs_ld2;
    xtbl.read(rs_ld2, _g0);
    ASSERT_EQ(rs_ld2.size(), rs.size());
    for( auto name : {"i", "d", "i5", "d34", "arr_i5", "arr_d34"} ) 
        chk_eq(name, 0, rs.size(), 1, 1, rs, rs_ld2, name);
}

TEST_F(XTableTest, ReadPartOfFields) {
    typedef record_t R;
    HIPPIO_H5_XTABLE_DEF(xtbl, R, i, d, i5);

    auto rs = get_table(32);
    xtbl.write(rs, _g0);

    table_t rs_ld = xtbl.read(_g0);
    ASSERT_EQ(rs_ld.size(), rs.size());
    for( auto name : {"i", "d", "i5"} ) 
        chk_eq(name, 0, rs.size(), 1, 1, rs, rs_ld, name);
}

TEST_F(XTableTest, ReadSelectedFields) {
    typedef record_t R;
    HIPPIO_H5_XTABLE_DEF(xtbl, R, i, d, i5, d34, arr_i5, arr_d34);

    auto rs = get_table(32);
    xtbl.write(rs, _g0);

    xtbl.select_fields({"i", "d", "i5"});

    table_t rs_ld = xtbl.read(_g0);
    ASSERT_EQ(rs_ld.size(), rs.size());
    for( auto name : {"i", "d", "i5"} ) 
        chk_eq(name, 0, rs.size(), 1, 1, rs, rs_ld, name);
}

TEST_F(XTableTest, ReadSelectedRows) {
    typedef record_t R;
    HIPPIO_H5_XTABLE_DEF(xtbl, R, i, i5, d34);

    auto rs = get_table(32);
    xtbl.write(rs, _g0);

    xtbl.select_rows(1, 5, 2, 1);   // 5 rows
    table_t rs_ld = xtbl.read(_g0);

    ASSERT_EQ(rs_ld.size(), 5);
    chk_eq("i", 1,5,2,1,rs,rs_ld, "i");
    chk_eq("i5", 1,5,2,1,rs,rs_ld, "i5");
    chk_eq("d34", 1,5,2,1,rs,rs_ld, "d34");
}

}
}