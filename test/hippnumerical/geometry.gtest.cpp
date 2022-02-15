#include <hippnumerical.h>
#include <gmock/gmock.h>

namespace HIPP::NUMERICAL::GEOMETRY {

namespace {

namespace gt = ::testing;

class PointTest : public gt::Test {
protected:
    using pf_t = Point<float, 3>;
    using pd_t = Point<double, 3>;

    using of_t = Offset<float, 3>;
    using od_t = Offset<double, 3>;
};

TEST_F(PointTest, ConstructorFloatType) {
    pf_t::pos_t pos {1.0f, 2.0f, 3.0f};
    pf_t p1(pos), p2 {1.0f, 2.0f, 3.0f};

    for(int i=0; i<3; ++i){
        EXPECT_EQ(pos[i], p1.pos()[i]) << "Index " << i << endl;
    }

    for(int i=0; i<3; ++i){
        EXPECT_EQ(pos[i], p2.pos()[i]) << "Index " << i << endl;
    }

    EXPECT_THAT(p1.pos(), gt::A<pf_t::pos_t>());
    EXPECT_THAT(p2.pos(), gt::A<pf_t::pos_t>());
}

TEST_F(PointTest, ConstructorDoubleType) {
    pd_t::pos_t pos {1.2, 2.2, 3.2};
    pd_t p1(pos), p2 {1.2, 2.2, 3.2};

    for(int i=0; i<3; ++i){
        EXPECT_EQ(pos[i], p1.pos()[i]) << "Index " << i << endl;
    }

    for(int i=0; i<3; ++i){
        EXPECT_EQ(pos[i], p2.pos()[i]) << "Index " << i << endl;
    }

    EXPECT_THAT(p1.pos(), gt::A<pd_t::pos_t>());
    EXPECT_THAT(p2.pos(), gt::A<pd_t::pos_t>());
}

TEST_F(PointTest, Arithmetics) {
    pd_t p1 {-1.0, 2.0, 5.0}, p2 {1.2, 2.2, 3.2};
    auto dp1p2 = p1-p2;
    auto dpos = p1.pos() - p2.pos();
    EXPECT_THAT(dp1p2, gt::A<od_t>());
    EXPECT_THAT(dpos, gt::A<od_t::value_t>());
    for(int i=0; i<3; ++i){
        EXPECT_EQ(dpos[i], dp1p2[i]) << "Index " << i << endl;
    }

    auto p3 = p1 + dp1p2, p4 = p1 + dpos, p5 = p1 + 1.0;
    EXPECT_THAT(p3, gt::A<pd_t>());
    for(int i=0; i<3; ++i){
        EXPECT_EQ(p3.pos()[i], p1.pos()[i] + dp1p2[i]) << "Index " << i << endl;
        EXPECT_EQ(p4.pos()[i], p3.pos()[i]) << "Index " << i << endl;
        EXPECT_EQ(p5.pos()[i], p1.pos()[i] + 1.0) << "Index " << i << endl;
    }
}

} // namespace

} // namespace HIPP::NUMERICAL::GEOMETRY
