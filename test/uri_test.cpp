#include <gtest/gtest.h>

#include "URI.hpp"

#include <map>
#include <sys/stat.h>
#include <vector>

#include "Config.hpp"
#include "ConfigParser.hpp"
#include "ServerConfig.hpp"

class URITest : public ::testing::Test {
protected:
    virtual void SetUp() {
        typedef std::map<int, std::vector<const ServerConfig> > configs_map;

        ConfigParser::parseConfigFile("./config/google_test.conf");
        configs_map servers_map = Config::instance()->GetServerConfigs();

        configs_map::const_iterator it = servers_map.begin();

        std::vector<const ServerConfig> virtual_server_configs = it->second;

        std::vector<const ServerConfig>::const_iterator s_it =
            virtual_server_configs.begin();

        multi_location = *s_it;
        s_it++;
        stat_test_conf = *s_it;
    }

    ServerConfig multi_location;
    ServerConfig stat_test_conf;
};

TEST_F(URITest, splitURI) {

    {
        URI uri(multi_location, "/");
        uri.Init();

        EXPECT_EQ("/", uri.GetRawTarget());
        EXPECT_EQ("/", uri.GetRawPath());
        EXPECT_EQ("", uri.GetQuery());
    }

    {
        URI uri(multi_location, "/docs");
        uri.Init();

        EXPECT_EQ("/docs", uri.GetRawTarget());
        EXPECT_EQ("/docs", uri.GetRawPath());
        EXPECT_EQ("", uri.GetQuery());
    }

    {
        URI uri(multi_location, "/docs/");
        uri.Init();

        EXPECT_EQ("/docs/", uri.GetRawTarget());
        EXPECT_EQ("/docs/", uri.GetRawPath());
        EXPECT_EQ("", uri.GetQuery());
    }

    {
        URI uri(multi_location, "/docs/hoge");
        uri.Init();

        EXPECT_EQ("/docs/hoge", uri.GetRawTarget());
        EXPECT_EQ("/docs/hoge", uri.GetRawPath());
        EXPECT_EQ("", uri.GetQuery());
    }

    {
        URI uri(multi_location, "/docs/hoge/");
        uri.Init();

        EXPECT_EQ("/docs/hoge/", uri.GetRawTarget());
        EXPECT_EQ("/docs/hoge/", uri.GetRawPath());
        EXPECT_EQ("", uri.GetQuery());
    }

    {
        URI uri(multi_location, "/?");
        uri.Init();

        EXPECT_EQ("/?", uri.GetRawTarget());
        EXPECT_EQ("/", uri.GetRawPath());
        EXPECT_EQ("", uri.GetQuery());
    }

    {
        URI uri(multi_location, "/?arg1");
        uri.Init();

        EXPECT_EQ("/?arg1", uri.GetRawTarget());
        EXPECT_EQ("/", uri.GetRawPath());
        EXPECT_EQ("arg1", uri.GetQuery());
    }

    {
        URI uri(multi_location, "/?arg1+arg2");
        uri.Init();

        EXPECT_EQ("/?arg1+arg2", uri.GetRawTarget());
        EXPECT_EQ("/", uri.GetRawPath());
        EXPECT_EQ("arg1+arg2", uri.GetQuery());
    }
}

TEST_F(URITest, storeArgsFromQuery) {
    {
        URI uri(multi_location, "/docs?a+b+c+d");
        uri.Init();

        std::vector<std::string> args = uri.GetArgs();
        std::vector<std::string> ans;
        ans.push_back("a");
        ans.push_back("b");
        ans.push_back("c");
        ans.push_back("d");
        EXPECT_TRUE(args == ans);
    }

    {
        URI uri(multi_location, "/docs?a");
        uri.Init();

        std::vector<std::string> args = uri.GetArgs();
        std::vector<std::string> ans;
        ans.push_back("a");
        EXPECT_TRUE(args == ans);
    }

    {
        URI uri(multi_location, "/docs?a=");
        uri.Init();

        std::vector<std::string> args = uri.GetArgs();
        std::vector<std::string> ans;
        EXPECT_TRUE(args == ans);
    }

    {
        URI uri(multi_location, "/docs?a=b+c=d");
        uri.Init();

        std::vector<std::string> args = uri.GetArgs();
        std::vector<std::string> ans;
        EXPECT_TRUE(args == ans);
    }
}

TEST_F(URITest, findLocalPath) {
    {
        URI uri(multi_location, "/txt");
        uri.Init();

        EXPECT_EQ("./txt", uri.GetLocalPath());
    }

    {
        URI uri(multi_location, "/a/txt");
        uri.Init();

        EXPECT_EQ("./a/txt", uri.GetLocalPath());
    }

    {
        URI uri(multi_location, "/a/b/txt");
        uri.Init();

        EXPECT_EQ("./a/b/txt", uri.GetLocalPath());
    }

    {
        URI uri(multi_location, "/a/b/c/txt");
        uri.Init();

        EXPECT_EQ("./a/b/c/txt", uri.GetLocalPath());
    }

    {
        URI uri(multi_location, "/a/d/txt");
        uri.Init();

        EXPECT_EQ("./a/d/txt", uri.GetLocalPath());
    }

    {
        URI uri(multi_location, "/a/b/d/txt");
        uri.Init();

        EXPECT_EQ("./a/b/d/txt", uri.GetLocalPath());
    }

    {
        URI uri(multi_location, "/a/b/c/d/txt");
        uri.Init();

        EXPECT_EQ("./a/b/c/d/txt", uri.GetLocalPath());
    }

    {
        URI uri(multi_location, "/a/../txt");
        uri.Init();

        EXPECT_EQ("./a/../txt", uri.GetLocalPath());
    }
}

TEST_F(URITest, statLocalPath) {
    {
        URI uri(stat_test_conf, "/Makefile");
        uri.Init();

        EXPECT_EQ("./Makefile", uri.GetLocalPath());
        EXPECT_TRUE(S_ISREG(uri.GetStat().st_mode));
    }

    {
        URI uri(stat_test_conf, "/src");
        uri.Init();

        EXPECT_EQ("./src", uri.GetLocalPath());
        EXPECT_TRUE(S_ISDIR(uri.GetStat().st_mode));
    }

    {
        URI uri(stat_test_conf, "/a/gtest.cc");
        uri.Init();

        EXPECT_EQ("./test/gtest.cc", uri.GetLocalPath());
        EXPECT_TRUE(S_ISREG(uri.GetStat().st_mode));
    }

    {
        URI uri(stat_test_conf, "/a/b/cgi");
        uri.Init();

        EXPECT_EQ("./docs/cgi", uri.GetLocalPath());
        EXPECT_TRUE(S_ISDIR(uri.GetStat().st_mode));
    }

    {
        URI uri(stat_test_conf, "/a/b/cgi/hello.py");
        uri.Init();

        EXPECT_EQ("./docs/cgi/hello.py", uri.GetLocalPath());
        EXPECT_TRUE(S_ISREG(uri.GetStat().st_mode));
    }

    {
        URI uri(stat_test_conf, "/a/b/c/main.cpp");
        uri.Init();

        EXPECT_EQ("./src/main.cpp", uri.GetLocalPath());
        EXPECT_TRUE(S_ISREG(uri.GetStat().st_mode));
    }

    {
        URI uri(stat_test_conf, "/a/b/c/event/mode/WriteCGI.cpp");
        uri.Init();

        EXPECT_EQ("./src/event/mode/WriteCGI.cpp", uri.GetLocalPath());
        EXPECT_TRUE(S_ISREG(uri.GetStat().st_mode));
    }
}