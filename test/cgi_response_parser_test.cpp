#include <gtest/gtest.h>

#include "CGIResponse.hpp"
#include "CGIResponseParser.hpp"

#include <string.h>

TEST(CGIResponseParser, SplitLine) {
    std::string       msg("Content-Type: text/html\r\n\r\n");
    CGIResponse       resp;
    CGIResponseParser parser(resp);

    parser(msg, msg.size());
    parser("",  0);
    EXPECT_EQ("text/html", resp.GetHeaderValue("Content-type"));
}