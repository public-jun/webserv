#include <gtest/gtest.h>

#include "HTTPParser.hpp"
#include "HTTPStatus.hpp"
#include <string>
#include <vector>

using namespace std;

// =======================
// ======= OK CASE =======

TEST(HTTPParser, ParseAllAtOnce) {
    string      message("GET index.html HTTP/1.1\r\nHost: localhost\r\n\r\n");
    HTTPRequest req;
    HTTPParser::State state(req);
    HTTPParser::update_state(state, message);

    EXPECT_EQ(200, req.GetStatus());
    EXPECT_EQ("GET", req.GetMethod());
    EXPECT_EQ("index.html", req.GetRequestTarget());
    EXPECT_EQ("HTTP/1.1", req.GetVersion());
    EXPECT_EQ("localhost", req.GetHeaderValue("host"));
    EXPECT_EQ(HTTPParser::DONE, state.Phase());
}

TEST(HTTPParser, EmptyVersion) {
    string            message("GET index.html \r\nHost: localhost\r\n\r\n");
    HTTPRequest       req;
    HTTPParser::State state(req);
    HTTPParser::update_state(state, message);

    EXPECT_EQ(200, req.GetStatus());
    EXPECT_EQ("GET", req.GetMethod());
    EXPECT_EQ("index.html", req.GetRequestTarget());
    EXPECT_EQ("HTTP/1.1", req.GetVersion());
    EXPECT_EQ("localhost", req.GetHeaderValue("host"));
    EXPECT_EQ(HTTPParser::DONE, state.Phase());
}

TEST(HTTPParser, ParseBody) {
    string      message("POST / HTTP/1.1\r\nHost: localhost\r\nContent-Length: "
                             "4\r\n\r\nhoge");
    HTTPRequest req;
    HTTPParser::State state(req);
    HTTPParser::update_state(state, message);

    EXPECT_EQ(200, req.GetStatus());
    EXPECT_EQ("POST", req.GetMethod());
    EXPECT_EQ("hoge", req.GetBody());
}

TEST(HTTPParser, ParseChunkedBody) {
    string message("POST / HTTP/1.1\r\n"
                   "Host: localhost\r\n"
                   "Content-Encoding: chunked\r\n"
                   "\r\n"
                   "7\r\n"
                   "Mozilla\r\n"
                   "9\r\n"
                   "Developer\r\n"
                   "7\r\n"
                   "Network\r\n"
                   "0\r\n"
                   "\r\n");

    HTTPRequest       req;
    HTTPParser::State state(req);
    HTTPParser::update_state(state, message);

    EXPECT_EQ(200, req.GetStatus());
    EXPECT_EQ("Mozilla"
              "Developer"
              "Network",
              req.GetBody());
}

TEST(HTTPParser, ParsePart1) {
    vector<string> m;
    m.push_back("GE");
    m.push_back("T index.html HTTP/1.1\r\nHost: localhost\r\n\r\n");

    HTTPRequest       req;
    HTTPParser::State state(req);
    for (vector<string>::iterator it = m.begin(); it != m.end(); it++) {
        HTTPParser::update_state(state, *it);
    }

    EXPECT_EQ(200, req.GetStatus());
    EXPECT_EQ("GET", req.GetMethod());
    EXPECT_EQ("index.html", req.GetRequestTarget());
    EXPECT_EQ("HTTP/1.1", req.GetVersion());
    EXPECT_EQ("localhost", req.GetHeaderValue("host"));
    EXPECT_EQ(HTTPParser::DONE, state.Phase());
}

TEST(HTTPParser, ParsePart2) {
    vector<string> m;
    m.push_back("GET index.html HTTP/1.1\r");
    m.push_back("\nHost: localhost\r\n");
    m.push_back("\r\n");

    HTTPRequest       req;
    HTTPParser::State state(req);
    for (vector<string>::iterator it = m.begin(); it != m.end(); it++) {
        HTTPParser::update_state(state, *it);
    }

    EXPECT_EQ(200, req.GetStatus());
    EXPECT_EQ("GET", req.GetMethod());
    EXPECT_EQ("index.html", req.GetRequestTarget());
    EXPECT_EQ("HTTP/1.1", req.GetVersion());
    EXPECT_EQ("localhost", req.GetHeaderValue("host"));
    EXPECT_EQ(HTTPParser::DONE, state.Phase());
}

TEST(HTTPParser, HeaderValueTrimSpace) {
    string      message("GET / HTTP/1.1\r\nHost:    localhost   \r\n\r\n");
    HTTPRequest req;
    HTTPParser::State state(req);
    HTTPParser::update_state(state, message);

    EXPECT_EQ(200, req.GetStatus());
    EXPECT_EQ("localhost", req.GetHeaderValue("host"));
}

TEST(HTTPParser, NormalizeHeaderKey) {
    string            message("GET / HTTP/1.1\r\nHOST:localhost\r\n\r\n");
    HTTPRequest       req;
    HTTPParser::State state(req);
    HTTPParser::update_state(state, message);

    EXPECT_EQ(200, req.GetStatus());
    EXPECT_EQ("localhost", req.GetHeaderValue("host"));
}

TEST(HTTPParser, PhaseFirstLine) {
    string            message("GET / ");
    HTTPRequest       req;
    HTTPParser::State state(req);
    HTTPParser::update_state(state, message);

    EXPECT_EQ(HTTPParser::FIRST_LINE, state.Phase());
}

TEST(HTTPParser, PhaseHeaderLine) {
    string            message("GET / HTTP/1.1\r\nHost:localhost\r\n");
    HTTPRequest       req;
    HTTPParser::State state(req);
    HTTPParser::update_state(state, message);

    EXPECT_EQ(HTTPParser::HEADER_LINE, state.Phase());
}

// =======================
// === Badrequest CASE ===

TEST(HTTPParser, EmptyMethod) {
    string            message("/ HTTP/1.1\r\nHost:localhost\r\n\r\n");
    HTTPRequest       req;
    HTTPParser::State state(req);

    try {
        HTTPParser::update_state(state, message);
    } catch (status::code code) {
        //
        EXPECT_EQ(status::bad_request, code);
    }
}

TEST(HTTPParser, LowercaseMethod) {
    string            message("get / HTTP/1.1\r\nHost: localhost\r\n\r\n");
    HTTPRequest       req;
    HTTPParser::State state(req);

    try {
        HTTPParser::update_state(state, message);
    } catch (status::code code) {
        //
        EXPECT_EQ(status::bad_request, code);
    }
}

TEST(HTTPParser, VersionNotExistName) {
    string            message = "GET / /2.1\r\nHost: localhost\r\n\r\n";
    HTTPRequest       req;
    HTTPParser::State state(req);

    try {
        HTTPParser::update_state(state, message);
    } catch (status::code code) {
        //
        EXPECT_EQ(status::bad_request, code);
    }
}

TEST(HTTPParser, VersionLowerCase) {
    string            message = "GET / http/1.1\r\nHost: localhost\r\n\r\n";
    HTTPRequest       req;
    HTTPParser::State state(req);

    try {
        HTTPParser::update_state(state, message);
    } catch (status::code code) {
        //
        EXPECT_EQ(status::bad_request, code);
    }
}

TEST(HTTPParser, VersionMultipleDot) {
    string            message = "GET / HTTP/1.1.1\r\nHost: localhost\r\n\r\n";
    HTTPRequest       req;
    HTTPParser::State state(req);

    try {
        HTTPParser::update_state(state, message);
    } catch (status::code code) {
        //
        EXPECT_EQ(status::bad_request, code);
    }
}

TEST(HTTPParser, VersionEndWithDot) {
    string            message = "GET / HTTP/1.\r\nHost: localhost\r\n\r\n";
    HTTPRequest       req;
    HTTPParser::State state(req);

    try {
        HTTPParser::update_state(state, message);
    } catch (status::code code) {
        //
        EXPECT_EQ(status::bad_request, code);
    }
}

TEST(HTTPParser, VersionStartWithDot) {
    string            message = "GET / HTTP/.1\r\nHost: localhost\r\n\r\n";
    HTTPRequest       req;
    HTTPParser::State state(req);

    try {
        HTTPParser::update_state(state, message);
    } catch (status::code code) {
        //
        EXPECT_EQ(status::bad_request, code);
    }
}

TEST(HTTPParser, VersionOnlyName) {
    string            message = "GET / HTTP/\r\nHost: localhost\r\n\r\n";
    HTTPRequest       req;
    HTTPParser::State state(req);

    try {
        HTTPParser::update_state(state, message);
    } catch (status::code code) {
        //
        EXPECT_EQ(status::bad_request, code);
    }
}

TEST(HTTPParser, VersionNotExistSlash) {
    string            message = "GET / HTTP1.1\r\nHost: localhost\r\n\r\n";
    HTTPRequest       req;
    HTTPParser::State state(req);

    try {
        HTTPParser::update_state(state, message);
    } catch (status::code code) {
        //
        EXPECT_EQ(status::bad_request, code);
    }
}

TEST(HTTPParser, VersionNotExistdot) {
    string            message = "GET / HTTP/1\r\nHost: localhost\r\n\r\n";
    HTTPRequest       req;
    HTTPParser::State state(req);

    try {
        HTTPParser::update_state(state, message);
    } catch (status::code code) {
        //
        EXPECT_EQ(status::bad_request, code);
    }
}

TEST(HTTPParser, NoHost) {
    string            message = "GET / HTTP/1.1\r\n\r\n";
    HTTPRequest       req;
    HTTPParser::State state(req);

    try {
        HTTPParser::update_state(state, message);
    } catch (status::code code) {
        //
        EXPECT_EQ(status::bad_request, code);
    }
}

TEST(HTTPParser, ValueContainCR) {
    string            message = "GET / HTTP/1.1\r\nHost: local\rhost\r\n\r\n";
    HTTPRequest       req;
    HTTPParser::State state(req);

    try {
        HTTPParser::update_state(state, message);
    } catch (status::code code) {
        //
        EXPECT_EQ(status::bad_request, code);
    }
}

TEST(HTTPParser, ValueContainLF) {
    string            message = "GET / HTTP/1.1\r\nHost: local\nhost\r\n\r\n";
    HTTPRequest       req;
    HTTPParser::State state(req);

    try {
        HTTPParser::update_state(state, message);
    } catch (status::code code) {
        //
        EXPECT_EQ(status::bad_request, code);
    }
}

TEST(HTTPParser, KeyIncludeSpace) {
    string            message = "GET / HTTP/1.1\r\nHost : localhost\r\n\r\n";
    HTTPRequest       req;
    HTTPParser::State state(req);

    try {
        HTTPParser::update_state(state, message);
    } catch (status::code code) {
        //
        EXPECT_EQ(status::bad_request, code);
    }
}

TEST(HTTPParser, EmptyKey) {
    string      message = "GET / HTTP/1.1\r\nHost: localhost\r\n:hoge\r\n\r\n";
    HTTPRequest req;
    HTTPParser::State state(req);

    try {
        HTTPParser::update_state(state, message);
    } catch (status::code code) {
        //
        EXPECT_EQ(status::bad_request, code);
    }
}

TEST(HTTPParser, ChunkedBodyNotCRLFAfterSize) {
    string message("POST / HTTP/1.1\r\n"
                   "Host: localhost\r\n"
                   "Content-Encoding: chunked\r\n"
                   "\r\n"
                   "7"
                   "Mozilla\r\n"
                   "9\r\n"
                   "Developer\r\n"
                   "7\r\n"
                   "Network\r\n"
                   "0\r\n"
                   "\r\n");

    HTTPRequest       req;
    HTTPParser::State state(req);
    try {
        HTTPParser::update_state(state, message);
    } catch (status::code code) { EXPECT_EQ(status::bad_request, code); }
}

TEST(HTTPParser, ChunkedBodyNotCRLFAfterdata) {
    string message("POST / HTTP/1.1\r\n"
                   "Host: localhost\r\n"
                   "Content-Encoding: chunked\r\n"
                   "\r\n"
                   "7\r\n"
                   "Mozilla"
                   "9\r\n"
                   "Developer\r\n"
                   "7\r\n"
                   "Network\r\n"
                   "0\r\n"
                   "\r\n");

    HTTPRequest       req;
    HTTPParser::State state(req);
    try {
        HTTPParser::update_state(state, message);
    } catch (status::code code) { EXPECT_EQ(status::bad_request, code); }
}

// =======================
// == OTHER STATUS CASE ==

TEST(HTTPParser, VersionNotSupported) {
    string            message = "GET / HTTP/3.0\r\nHost: localhost\r\n\r\n";
    HTTPRequest       req;
    HTTPParser::State state(req);
    try {
        HTTPParser::update_state(state, message);
    } catch (status::code code) {
        //
        EXPECT_EQ(status::version_not_suppoted, code);
    }
}
