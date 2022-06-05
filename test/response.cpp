#include "HTTPResponse.hpp"
#include <gtest/gtest.h>

using namespace std;

TEST(HTTPResponse, SetVersion) {
    HTTPResponse resp;
    resp.SetVersion("HTTP/1.1");

    string expect = "HTTP/1.1 200 OK\r\n\r\n";
    string actual = resp.ConvertToStr();
    EXPECT_EQ(expect, actual);
}

TEST(HTTPResponse, SetBody) {
    HTTPResponse resp;
    resp.SetVersion("HTTP/1.1");
    resp.SetBody("hoge");

    string expect = "HTTP/1.1 200 OK\r\n\r\nhoge";
    string actual = resp.ConvertToStr();
    EXPECT_EQ(expect, actual);
}

TEST(HTTPResponse, AppendHeader) {
    HTTPResponse resp;
    resp.SetVersion("HTTP/1.1");
    resp.SetBody("hoge");
    resp.AppendHeader("Content-Length", "4");

    string expect = "HTTP/1.1 200 OK\r\nContent-Length: 4\r\n\r\nhoge";
    string actual = resp.ConvertToStr();
    EXPECT_EQ(expect, actual);
}

TEST(HTTPResponse, BadRequest) {
    HTTPResponse resp;
    resp.SetVersion("HTTP/1.1");
    resp.SetStatusCode(400);

    string expect = "HTTP/1.1 400 Bad Request\r\n\r\n";
    string actual = resp.ConvertToStr();
    EXPECT_EQ(expect, actual);
}
