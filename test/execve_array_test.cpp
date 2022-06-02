#include <gtest/gtest.h>

#include "ExecveArray.hpp"

#include <string>
#include <vector>

TEST(ExecveArray, Normal) {
    std::vector<std::string> strs;
    strs.push_back("aaa");
    strs.push_back("bbb");

    ExecveArray args(strs);
    char* const* p = args.get();
    EXPECT_EQ("aaa", *p);
    p++;
    EXPECT_EQ("bbb", *p);
    p++;
    EXPECT_EQ(NULL, p);
    EXPECT_EQ(3, args.get_size());
}

TEST(ExecveArray, NoneArgs) {
    std::vector<std::string> strs;

    ExecveArray args(strs);
    char* const* p = args.get();
    EXPECT_EQ(NULL, p);
    EXPECT_EQ(1, args.get_size());
}
