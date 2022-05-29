#include <gtest/gtest.h>

#include "HTTPRequest.hpp"

#define GREEN "\033[32m"
#define RED "\033[31m"
#define RESET "\033[39m"

using namespace std;

struct testcase {
    std::string message;
    int         expect_status;
    std::string exepct_method;
    std::string expect_uri;
    std::string expect_version;
    std::string expect_host;
};

void printMessageIfFailed(std::string message, bool failed) {
    if (failed) {
        cout << RED << "\n[FAILED REQUEST]:\n"
             << RESET << message << "\n"
             << endl;
    }
}

void testRequest(testcase* test) {
    HTTPRequest req(test->message);

    ASSERT_EQ(test->expect_status, req.GetStatus());
    if (req.GetStatus() != 200) {
        return;
    }
    EXPECT_EQ(test->exepct_method, req.GetMethod());
    EXPECT_EQ(test->expect_uri, req.GetRequestTarget());
    EXPECT_EQ(test->expect_version, req.GetVersion());
    EXPECT_EQ(test->expect_host, req.GetHeaderValue("Host"));
}

// expect status 200
TEST(HTTPRequest, Simple) {
    testcase t = { "GET / HTTP/1.1\r\nHost: localhost\r\nAccept: */*\r\n\r\n",
                   200,
                   "GET",
                   "index.html",
                   "HTTP/1.1",
                   "localhost" };
    testRequest(&t);
    printMessageIfFailed(t.message, testing::Test::HasFailure());
}

TEST(HTTPRequest, LowercaseHost) {
    testcase t = { "GET / HTTP/1.1\r\nhost: localhost\r\n\r\n",
                   200,
                   "GET",
                   "index.html",
                   "HTTP/1.1",
                   "localhost" };
    testRequest(&t);
    printMessageIfFailed(t.message, testing::Test::HasFailure());
}

TEST(HTTPRequest, SpaceBeforeValue) {
    testcase t = { "GET / HTTP/1.1\r\nHost:     localhost\r\n\r\n",
                   200,
                   "GET",
                   "index.html",
                   "HTTP/1.1",
                   "localhost" };
    testRequest(&t);
    printMessageIfFailed(t.message, testing::Test::HasFailure());
}

TEST(HTTPRequest, SpaceAfterValue) {
    testcase t = { "GET / HTTP/1.1\r\nHost:localhost   \r\n\r\n",
                   200,
                   "GET",
                   "index.html",
                   "HTTP/1.1",
                   "localhost" };
    testRequest(&t);
    printMessageIfFailed(t.message, testing::Test::HasFailure());
}

TEST(HTTPRequest, NotSpecifyVersion) {
    testcase t = { "GET /\r\nHost: localhost\r\n\r\n",
                   200,
                   "GET",
                   "index.html",
                   "HTTP/1.1",
                   "localhost" };
    testRequest(&t);
    printMessageIfFailed(t.message, testing::Test::HasFailure());
}

TEST(HTTPRequest, SpecifyTarget) {
    testcase t = { "GET hoge\r\nHost: localhost\r\n\r\n",
                   200,
                   "GET",
                   "hoge",
                   "HTTP/1.1",
                   "localhost" };
    testRequest(&t);
    printMessageIfFailed(t.message, testing::Test::HasFailure());
}

// ERROR CASE
// expect status 400
TEST(HTTPRequest, EmptyMethod) {
    testcase t = {
        "/ HTTP/1.1\r\nHost: localhost\r\n\r\n", 400, "", "", "", ""
    };
    testRequest(&t);
    printMessageIfFailed(t.message, testing::Test::HasFailure());
}

/* TEST(HTTPRequest, EmptyRequestTarget) { */
/*     testcase t = { */
/*         "GET HTTP/1.1\r\nHost: localhost\r\n\r\n", 400, "", "", "", "" */
/*     }; */
/*     testRequest(&t); */
/*     printMessageIfFailed(t.message, testing::Test::HasFailure()); */
/* } */

TEST(HTTPRequest, MissingEmptyLine) {
    testcase t = { "GET HTTP/1.1\r\nHost: localhost\r\n", 400, "", "", "", "" };
    testRequest(&t);
    printMessageIfFailed(t.message, testing::Test::HasFailure());
}

TEST(HTTPRequest, MssingCRLF) {
    testcase t = { "GET HTTP/1.1\r\nHost: localhost", 400, "", "", "", "" };
    testRequest(&t);
    printMessageIfFailed(t.message, testing::Test::HasFailure());
}

TEST(HTTPRequest, 2EmptyLine) {
    testcase t = { "GET HTTP/1.1\r\n\r\n\r\n", 400, "", "", "", "" };
    testRequest(&t);
    printMessageIfFailed(t.message, testing::Test::HasFailure());
}

TEST(HTTPRequest, LowercaseMethod) {
    testcase t = { "get / HTTP/1.1\r\n\r\n", 400, "", "", "", "" };
    testRequest(&t);
    printMessageIfFailed(t.message, testing::Test::HasFailure());
}

// HTTP Version Error
TEST(HTTPRequest, VersionNotExistName) {
    testcase t = { "GET / /1.1\r\nHost: localhost\r\n\r\n", 400, "", "", "" };
    testRequest(&t);
    printMessageIfFailed(t.message, testing::Test::HasFailure());
}

TEST(HTTPRequest, VersionLowerCase) {
    testcase t = { "GET / http/1.1\r\nHost: localhost\r\n\r\n", 400, "", "",
                   "" };
    testRequest(&t);
    printMessageIfFailed(t.message, testing::Test::HasFailure());
}

TEST(HTTPRequest, VersionMultipleDot) {
    testcase t = { "GET / HTTP/1.1.1\r\nHost: localhost\r\n\r\n", 400, "", "",
                   "" };
    testRequest(&t);
    printMessageIfFailed(t.message, testing::Test::HasFailure());
}

TEST(HTTPRequest, VersionEndWithDot) {
    testcase t = { "GET / HTTP/1.\r\nHost: localhost\r\n\r\n", 400, "", "",
                   "" };
    testRequest(&t);
    printMessageIfFailed(t.message, testing::Test::HasFailure());
}

TEST(HTTPRequest, VersionStartWithDot) {
    testcase t = { "GET / HTTP/.1\r\nHost: localhost\r\n\r\n", 400, "", "",
                   "" };
    testRequest(&t);
    printMessageIfFailed(t.message, testing::Test::HasFailure());
}

TEST(HTTPRequest, VersionOnlyName) {
    testcase t = { "GET / HTTP/\r\nHost: localhost\r\n\r\n", 400, "", "", "" };
    testRequest(&t);
    printMessageIfFailed(t.message, testing::Test::HasFailure());
}

TEST(HTTPRequest, VersionNotExistSlash) {
    testcase t = { "GET / HTTP1.1\r\nHost: localhost\r\n\r\n", 400, "", "",
                   "" };
    testRequest(&t);
    printMessageIfFailed(t.message, testing::Test::HasFailure());
}

TEST(HTTPRequest, VersionNotExistdot) {
    testcase t = { "GET / HTTP/1\r\nHost: localhost\r\n\r\n", 400, "", "", "" };
    testRequest(&t);
    printMessageIfFailed(t.message, testing::Test::HasFailure());
}

// 2行目以降のヘッダーエラー
TEST(HTTPRequest, NoHost) {
    testcase t = {
        "GET / HTTP/1.1\r\n\r\n", 400, "GET", "index.html", "HTTP/1.1", ""
    };
    testRequest(&t);
    printMessageIfFailed(t.message, testing::Test::HasFailure());
}

TEST(HTTPRequest, ValueContainCR) {
    testcase t = { "GET / HTTP/1.1\r\nHost: local\rhost\r\n\r\n", 400, "", "",
                   "" };
    testRequest(&t);
    printMessageIfFailed(t.message, testing::Test::HasFailure());
}

TEST(HTTPRequest, ValueContainLF) {
    testcase t = { "GET / HTTP/1.1\r\nHost: local\nhost\r\n\r\n", 400, "", "",
                   "" };
    testRequest(&t);
    printMessageIfFailed(t.message, testing::Test::HasFailure());
}

TEST(HTTPRequest, KeyIncludeSpace) {
    testcase t = { "GET / HTTP/1.1\r\nHost : localhost\r\n\r\n", 400, "", "",
                   "" };
    testRequest(&t);
    printMessageIfFailed(t.message, testing::Test::HasFailure());
}

TEST(HTTPRequest, EmptyKey) {
    testcase t = { "GET / HTTP/1.1\r\nHost: localhost\r\n:hoge\r\n\r\n", 400,
                   "", "", "" };
    testRequest(&t);
    printMessageIfFailed(t.message, testing::Test::HasFailure());
}

// expect other status code
TEST(HTTPRequest, UnsupportedMethod) {
    testcase t = { "HOGE / HTTP/1.1\r\nHost: localhost\r\n\r\n",
                   405,
                   "HOGE",
                   "index.html",
                   "HTTP/1.1",
                   "localhost" };
    testRequest(&t);
    printMessageIfFailed(t.message, testing::Test::HasFailure());
}

TEST(HTTPRequest, VersionNotSupported) {
    testcase t = { "GET / HTTP/3.0\r\nHost: localhost\r\n\r\n",
                   505,
                   "GET",
                   "index.html",
                   "HTTP/3.0",
                   "localhost" };
    testRequest(&t);
    printMessageIfFailed(t.message, testing::Test::HasFailure());
}

