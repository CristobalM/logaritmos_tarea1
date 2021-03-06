//
// Created by cristobal on 06-10-17.
//


#include <gtest/gtest.h>
#include <boost/filesystem/operations.hpp>
#include <fstream>


TEST(CreateFolder, Folder1) {
    std::string output_folder = "output_folder";

    if(!boost::filesystem::exists(output_folder)){
    boost::filesystem::create_directory(output_folder);
    }
    EXPECT_TRUE(boost::filesystem::exists(output_folder));

    std::string filename = output_folder +"/hola";
    std::ofstream ofs(filename);
    ofs << "bullshit";
    EXPECT_TRUE(boost::filesystem::exists(filename));
}

/*
TEST(SpaceOccupied, test1) {
    boost::filesystem::path p(".");
    boost::filesystem::directory_iterator end_itr;

    long total = 0;
    int count= 0;
    std::string current_file;
    for(boost::filesystem::directory_iterator itr(p); itr != end_itr; ++itr){
        if(boost::filesystem::is_regular_file(itr->path())){
            count++;
            current_file = (itr->path()).string();
            if(boost::starts_with(current_file, "./22f24c0d-a0de-4b40-887a")){
                total += boost::filesystem::file_size(itr->path());
            }
        }
    }

    EXPECT_EQ(total, 0);

}

TEST(Startswith, test1){
    ASSERT_TRUE(boost::starts_with("22f24c0d", "22f1"));
}
 */
