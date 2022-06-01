#include <gtest/gtest.h>

#include "HTTPParser.hpp"
#include <string>
#include <vector>

using namespace std;

// ======= OK CASE =======

TEST(HTTPParser, ParseAllAtOnce) {
    string      message("GET / HTTP/1.1\r\nHost: localhost\r\n\r\n");
    HTTPRequest req;
    HTTPParser  parser(req);
    parser.ParsePart(message);

    EXPECT_EQ(200, req.GetStatus());
    EXPECT_EQ("GET", req.GetMethod());
    EXPECT_EQ("index.html", req.GetRequestTarget());
    EXPECT_EQ("HTTP/1.1", req.GetVersion());
    EXPECT_EQ("localhost", req.GetHeaderValue("Host"));
    EXPECT_EQ(HTTPParser::PH_END, parser.GetPhase());
}

TEST(HTTPParser, EmptyVersion) {
    string      message("GET / \r\nHost: localhost\r\n\r\n");
    HTTPRequest req;
    HTTPParser  parser(req);
    parser.ParsePart(message);

    EXPECT_EQ(200, req.GetStatus());
    EXPECT_EQ("GET", req.GetMethod());
    EXPECT_EQ("index.html", req.GetRequestTarget());
    EXPECT_EQ("HTTP/1.1", req.GetVersion());
    EXPECT_EQ("localhost", req.GetHeaderValue("Host"));
    EXPECT_EQ(HTTPParser::PH_END, parser.GetPhase());
}

TEST(HTTPParser, ParsePart1) {
    vector<string> m;
    m.push_back("GE");
    m.push_back("T / HTTP/1.1\r\nHost: localhost\r\n\r\n");

    HTTPRequest req;
    HTTPParser  parser(req);
    for (vector<string>::iterator it = m.begin(); it != m.end(); it++) {
        parser.ParsePart(*it);
    }

    EXPECT_EQ(200, req.GetStatus());
    EXPECT_EQ("GET", req.GetMethod());
    EXPECT_EQ("index.html", req.GetRequestTarget());
    EXPECT_EQ("HTTP/1.1", req.GetVersion());
    EXPECT_EQ("localhost", req.GetHeaderValue("Host"));
    EXPECT_EQ(HTTPParser::PH_END, parser.GetPhase());
}

TEST(HTTPParser, ParsePart2) {
    vector<string> m;
    m.push_back("GET / HTTP/1.1\r");
    m.push_back("\nHost: localhost\r\n");
    m.push_back("\r\n");

    HTTPRequest req;
    HTTPParser  parser(req);
    for (vector<string>::iterator it = m.begin(); it != m.end(); it++) {
        parser.ParsePart(*it);
    }

    EXPECT_EQ(200, req.GetStatus());
    EXPECT_EQ("GET", req.GetMethod());
    EXPECT_EQ("index.html", req.GetRequestTarget());
    EXPECT_EQ("HTTP/1.1", req.GetVersion());
    EXPECT_EQ("localhost", req.GetHeaderValue("Host"));
    EXPECT_EQ(HTTPParser::PH_END, parser.GetPhase());
}

TEST(HTTPParser, HeaderValueTrimSpace) {
    string      message("GET / HTTP/1.1\r\nHost:    localhost   \r\n\r\n");
    HTTPRequest req;
    HTTPParser  parser(req);
    parser.ParsePart(message);

    EXPECT_EQ(200, req.GetStatus());
    EXPECT_EQ("localhost", req.GetHeaderValue("Host"));
}

TEST(HTTPParser, HeaderKey) {
    string      message("GET / HTTP/1.1\r\nHOST:localhost\r\n\r\n");
    HTTPRequest req;
    HTTPParser  parser(req);
    parser.ParsePart(message);

    EXPECT_EQ(200, req.GetStatus());
    EXPECT_EQ("localhost", req.GetHeaderValue("Host"));
}

// === Badrequest CASE ===

TEST(HTTPParser, EmptyMethod) {
    string      message("/ HTTP/1.1\r\nHOST:localhost\r\n\r\n");
    HTTPRequest req;
    HTTPParser  parser(req);
    parser.ParsePart(message);

    EXPECT_EQ(400, req.GetStatus());
}
