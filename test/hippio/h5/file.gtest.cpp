#include <h5_test_incl.h>

namespace HIPP::IO::H5 {

namespace {


class FileTest : public gt::Test, public H5TestFile {
protected:
    string fixture_name() override { return "FileTest"; }
    
    void TearDown() override {
        clear_up();
    }
};

TEST_F(FileTest, FileCreation){
    {
        auto f1 = create_file("FileCreation_mode_w", "w");
        ASSERT_THAT(f1, gt::An<File>());
    }
    ASSERT_NO_THROW(create_file("FileCreation_mode_w", "w"));
    
    ASSERT_THROW(create_file("FileCreation_mode_w", "x"), ErrH5);
    
    ASSERT_THROW(create_file("FileCreation_mode_r", "r"), ErrH5);
    ASSERT_NO_THROW(create_file("FileCreation_mode_w", "r"));

    ASSERT_THROW(create_file("FileCreation_mode_a", "a"), ErrH5);
    ASSERT_NO_THROW(create_file("FileCreation_mode_w", "a"));

    ASSERT_NO_THROW(create_file("FileCreation_mode_w", "ac"));
    ASSERT_NO_THROW(create_file("FileCreation_mode_w", "ca"));
    ASSERT_NO_THROW(create_file("FileCreation_mode_ca", "ca"));
    ASSERT_NO_THROW(create_file("FileCreation_mode_ac", "ac"));

    string f1 = "FileCreation_method";
    add_clear_up(f1);
    ASSERT_NO_THROW(File::create(get_full_filename(f1), "w"));
    ASSERT_THROW(File::create(get_full_filename(f1), "x"), ErrH5);
    ASSERT_NO_THROW(File::create(get_full_filename(f1), "w"));

    string f2 = "FileCreation_wrong_flag";
    add_clear_up(f2);
    ASSERT_THROW(File::create(get_full_filename(f2), "t"), ErrLogic);
    ASSERT_THROW(File(get_full_filename(f2), "t"), ErrLogic);
}

TEST_F(FileTest, FileOpen){
    string f1 = "FileOpen_f1";
    ASSERT_THROW(File::open(get_full_filename(f1), "r"), ErrH5);
    ASSERT_THROW(File::open(get_full_filename(f1), "a"), ErrH5);
    create_file(f1, "w");
    ASSERT_NO_THROW(File::open(get_full_filename(f1), "r"));
    ASSERT_NO_THROW(File::open(get_full_filename(f1), "a"));

    string f2 = "FileOpen_wrong_flag";
    add_clear_up(f2);
    ASSERT_THROW(File::open(get_full_filename(f2), "t"), ErrLogic);
}

TEST_F(FileTest, FileProplistCreation) {
    ASSERT_NO_THROW(File::create_proplist("c"));
    ASSERT_NO_THROW(File::create_proplist("a"));
    ASSERT_NO_THROW(File::create_proplist("m"));
    ASSERT_NO_THROW(File::create_proplist("create"));
    ASSERT_NO_THROW(File::create_proplist("access"));
    ASSERT_NO_THROW(File::create_proplist("mount"));

    ASSERT_THROW(File::create_proplist("t"), ErrLogic);
}

} // namespace

} // namespace HIPP::IO::H5