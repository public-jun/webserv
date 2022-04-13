#include <gtest/gtest.h>

#include <hoge.hpp>

TEST(Hoge, GetNum)
{
	{
		hoge h(42);

		EXPECT_EQ(42, h.get_num());
	}
}
