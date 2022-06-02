#include <gtest/gtest.h>

#include "HTTPParser.hpp"
#include <string>
#include <vector>

using namespace std;

// =======================
// ======= OK CASE =======

TEST(HTTPParser, ParseAllAtOnce) {
    string      message("GET / HTTP/1.1\r\nHost: localhost\r\n\r\n");
    HTTPRequest req;
    HTTPParser  parser(req);
    parser.Parse(message);

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
    parser.Parse(message);

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
        parser.Parse(*it);
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
        parser.Parse(*it);
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
    parser.Parse(message);

    EXPECT_EQ(200, req.GetStatus());
    EXPECT_EQ("localhost", req.GetHeaderValue("Host"));
}

TEST(HTTPParser, NormalizeHeaderKey) {
    string      message("GET / HTTP/1.1\r\nHOST:localhost\r\n\r\n");
    HTTPRequest req;
    HTTPParser  parser(req);
    parser.Parse(message);

    EXPECT_EQ(200, req.GetStatus());
    EXPECT_EQ("localhost", req.GetHeaderValue("Host"));
}

TEST(HTTPParser, PhaseFirstLine) {
    string      message("GET / ");
    HTTPRequest req;
    HTTPParser  parser(req);
    parser.Parse(message);

    EXPECT_EQ(HTTPParser::PH_FIRST_LINE, parser.GetPhase());
}

TEST(HTTPParser, PhaseHeaderLine) {
    string      message("GET / HTTP/1.1\r\nHost:localhost\r\n");
    HTTPRequest req;
    HTTPParser  parser(req);
    parser.Parse(message);

    EXPECT_EQ(HTTPParser::PH_HEADER_LINE, parser.GetPhase());
}

// =======================
// === Badrequest CASE ===

TEST(HTTPParser, EmptyMethod) {
    string      message("/ HTTP/1.1\r\nHost:localhost\r\n\r\n");
    HTTPRequest req;
    HTTPParser  parser(req);
    parser.Parse(message);

    EXPECT_EQ(400, req.GetStatus());
}

TEST(HTTPParser, LowercaseMethod) {
    string      message("get / HTTP/1.1\r\nHost: localhost\r\n\r\n");
    HTTPRequest req;
    HTTPParser  parser(req);
    parser.Parse(message);

    EXPECT_EQ(400, req.GetStatus());
}

TEST(HTTPParser, VersionNotExistName) {
    string      message = "GET / /1.1\r\nHost: localhost\r\n\r\n";
    HTTPRequest req;
    HTTPParser  parser(req);
    parser.Parse(message);

    EXPECT_EQ(400, req.GetStatus());
}

TEST(HTTPParser, VersionLowerCase) {
    string      message = "GET / http/1.1\r\nHost: localhost\r\n\r\n";
    HTTPRequest req;
    HTTPParser  parser(req);
    parser.Parse(message);

    EXPECT_EQ(400, req.GetStatus());
}

TEST(HTTPParser, VersionMultipleDot) {
    string      message = "GET / HTTP/1.1.1\r\nHost: localhost\r\n\r\n";
    HTTPRequest req;
    HTTPParser  parser(req);
    parser.Parse(message);

    EXPECT_EQ(400, req.GetStatus());
}

TEST(HTTPParser, VersionEndWithDot) {
    string      message = "GET / HTTP/1.\r\nHost: localhost\r\n\r\n";
    HTTPRequest req;
    HTTPParser  parser(req);
    parser.Parse(message);

    EXPECT_EQ(400, req.GetStatus());
}

TEST(HTTPParser, VersionStartWithDot) {
    string      message = "GET / HTTP/.1\r\nHost: localhost\r\n\r\n";
    HTTPRequest req;
    HTTPParser  parser(req);
    parser.Parse(message);

    EXPECT_EQ(400, req.GetStatus());
}

TEST(HTTPParser, VersionOnlyName) {
    string      message = "GET / HTTP/\r\nHost: localhost\r\n\r\n";
    HTTPRequest req;
    HTTPParser  parser(req);
    parser.Parse(message);

    EXPECT_EQ(400, req.GetStatus());
}

TEST(HTTPParser, VersionNotExistSlash) {
    string      message = "GET / HTTP1.1\r\nHost: localhost\r\n\r\n";
    HTTPRequest req;
    HTTPParser  parser(req);
    parser.Parse(message);

    EXPECT_EQ(400, req.GetStatus());
}

TEST(HTTPParser, VersionNotExistdot) {
    string      message = "GET / HTTP/1\r\nHost: localhost\r\n\r\n";
    HTTPRequest req;
    HTTPParser  parser(req);
    parser.Parse(message);

    EXPECT_EQ(400, req.GetStatus());
}

TEST(HTTPParser, NoHost) {
    string      message = "GET / HTTP/1.1\r\n\r\n";
    HTTPRequest req;
    HTTPParser  parser(req);
    parser.Parse(message);

    EXPECT_EQ(400, req.GetStatus());
}

TEST(HTTPParser, ValueContainCR) {
    string      message = "GET / HTTP/1.1\r\nHost: local\rhost\r\n\r\n";
    HTTPRequest req;
    HTTPParser  parser(req);
    parser.Parse(message);

    EXPECT_EQ(400, req.GetStatus());
}

TEST(HTTPParser, ValueContainLF) {
    string      message = "GET / HTTP/1.1\r\nHost: local\nhost\r\n\r\n";
    HTTPRequest req;
    HTTPParser  parser(req);
    parser.Parse(message);

    EXPECT_EQ(400, req.GetStatus());
}

TEST(HTTPParser, KeyIncludeSpace) {
    string      message = "GET / HTTP/1.1\r\nHost : localhost\r\n\r\n";
    HTTPRequest req;
    HTTPParser  parser(req);
    parser.Parse(message);

    EXPECT_EQ(400, req.GetStatus());
}

TEST(HTTPParser, EmptyKey) {
    string      message = "GET / HTTP/1.1\r\nHost: localhost\r\n:hoge\r\n\r\n";
    HTTPRequest req;
    HTTPParser  parser(req);
    parser.Parse(message);

    EXPECT_EQ(400, req.GetStatus());
}

// =======================
// == OTHER STATUS CASE ==

TEST(HTTPParser, UnsupportedMethod) {
    string      message = "HOGE / HTTP/1.1\r\nHost: localhost\r\n\r\n";
    HTTPRequest req;
    HTTPParser  parser(req);
    parser.Parse(message);

    EXPECT_EQ(405, req.GetStatus());
}

TEST(HTTPParser, VersionNotSupported) {
    string      message = "GET / HTTP/3.0\r\nHost: localhost\r\n\r\n";
    HTTPRequest req;
    HTTPParser  parser(req);
    parser.Parse(message);

    EXPECT_EQ(505, req.GetStatus());
}
