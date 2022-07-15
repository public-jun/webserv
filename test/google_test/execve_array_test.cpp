#include <gtest/gtest.h>

#include "ExecveArray.hpp"

#include <string>
#include <vector>

#include <iostream>

TEST(ExecveArray, Normal) {
    std::vector<std::string> strs;
    strs.push_back("aaa");
    strs.push_back("bbb");

    ExecveArray  args(strs);
    char* const* p = args.Get();
    EXPECT_EQ('a', (*p)[0]);
    EXPECT_EQ('a', (*p)[1]);
    EXPECT_EQ('a', (*p)[2]);
    p++;
    EXPECT_EQ('b', (*p)[0]);
    EXPECT_EQ('b', (*p)[1]);
    EXPECT_EQ('b', (*p)[2]);
    p++;
    EXPECT_EQ(NULL, *p);
    // EXPECT_EQ(8, args.GetSize());
}

TEST(ExecveArray, NoneArgs) {
    std::vector<std::string> strs;

    ExecveArray  args(strs);
    char* const* p = args.Get();
    EXPECT_EQ(NULL, *p);
    EXPECT_EQ(0, args.GetSize());
}
