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

        ConfigParser::ParseConfigFile("./config/google_test.conf");
        configs_map servers_map = Config::Instance()->GetServerConfigs();

        configs_map::const_iterator it = servers_map.begin();

        std::vector<const ServerConfig> virtual_server_configs = it->second;

        std::vector<const ServerConfig>::const_iterator s_it =
            virtual_server_configs.begin();

        multi_location = *s_it++;
        stat_test_conf = *s_it++;
        cgi_conf       = *s_it++;
        default_conf   = *s_it++;
        mix_path_conf  = *s_it++;
    }

    ServerConfig multi_location;
    ServerConfig stat_test_conf;
    ServerConfig cgi_conf;
    ServerConfig default_conf;
    ServerConfig mix_path_conf;
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
        EXPECT_TRUE(S_ISREG(uri.Stat(uri.GetLocalPath()).st_mode));
    }

    {
        URI uri(stat_test_conf, "/src");
        uri.Init();

        EXPECT_EQ("./src", uri.GetLocalPath());
        EXPECT_TRUE(S_ISDIR(uri.Stat(uri.GetLocalPath()).st_mode));
    }

    {
        URI uri(stat_test_conf, "/a/gtest.cc");
        uri.Init();

        EXPECT_EQ("./test/gtest.cc", uri.GetLocalPath());
        EXPECT_TRUE(S_ISREG(uri.Stat(uri.GetLocalPath()).st_mode));
    }

    {
        URI uri(stat_test_conf, "/a/b/cgi");
        uri.Init();

        EXPECT_EQ("./docs/cgi", uri.GetLocalPath());
        EXPECT_TRUE(S_ISDIR(uri.Stat(uri.GetLocalPath()).st_mode));
    }

    {
        URI uri(stat_test_conf, "/a/b/cgi/python/formtest.py");
        uri.Init();

        EXPECT_EQ("./docs/cgi/python/formtest.py", uri.GetLocalPath());
        EXPECT_TRUE(S_ISREG(uri.Stat(uri.GetLocalPath()).st_mode));
    }

    {
        URI uri(stat_test_conf, "/a/b/c/main.cpp");
        uri.Init();

        EXPECT_EQ("./src/main.cpp", uri.GetLocalPath());
        EXPECT_TRUE(S_ISREG(uri.Stat(uri.GetLocalPath()).st_mode));
    }

    {
        URI uri(stat_test_conf, "/a/b/c/event/mode/WriteCGI.cpp");
        uri.Init();

        EXPECT_EQ("./src/event/mode/WriteCGI.cpp", uri.GetLocalPath());
        EXPECT_TRUE(S_ISREG(uri.Stat(uri.GetLocalPath()).st_mode));
    }
}

TEST_F(URITest, DecodeURI) {
    {
        URI uri(multi_location, "/%E3%81%82%E3%81%84%E3%81%86");
        uri.Init();

        EXPECT_EQ("/あいう", uri.GetDecodePath());
    }

    {
        URI uri(multi_location,
                "/%E3%81%82%E3%81%84%E3%81%86aiu%E3%81%88%E3%81%8A");
        uri.Init();
        EXPECT_EQ("/あいうaiuえお", uri.GetDecodePath());
    }

    {
        URI uri(
            multi_location,
            "/%3A%2F%3F%23%5B%5D%40%21%24%26%27%28%29%2A%2B%2C%3D%3B%25%20");
        uri.Init();
        EXPECT_EQ("/:/?#[]@!$&'()*+,=;% ", uri.GetDecodePath());
    }

    {
        URI uri(multi_location,
                "/%E3%81%82%25%E3%81%84%E3%81%863A%25z%E3%81%88yz");
        uri.Init();

        EXPECT_EQ("/あ%いう3A%zえyz", uri.GetDecodePath());
    }

    {
        URI uri(multi_location,
                "/%E3%81%82%E3%81%84%E3%81%86/"
                "%E3%81%82%E3%81%84%E3%81%86?%E3%81%82%E3%81%84%E3%81%86");
        uri.Init();

        EXPECT_EQ("/あいう/あいう", uri.GetDecodePath());
        EXPECT_EQ("%E3%81%82%E3%81%84%E3%81%86", uri.GetQuery());
    }
}

TEST_F(URITest, MixURI) {
    {
        URI uri(mix_path_conf, "/a/1");
        uri.Init();

        EXPECT_EQ("./A/1", uri.GetLocalPath());
    }

    {
        URI uri(mix_path_conf, "/a/aa/1/2");
        uri.Init();

        EXPECT_EQ("./A/AA/1/2", uri.GetLocalPath());
    }

    {
        URI uri(mix_path_conf, "/b/1");
        uri.Init();

        EXPECT_EQ("./B/b/1", uri.GetLocalPath());
    }

    {
        URI uri(mix_path_conf, "/b/1");
        uri.Init();

        EXPECT_EQ("./B/b/1", uri.GetLocalPath());
    }

    {
        URI uri(mix_path_conf, "/c/1/2");
        uri.Init();

        EXPECT_EQ("./C/CC/1/2", uri.GetLocalPath());
    }

    {
        URI uri(mix_path_conf, "/d/1");
        uri.Init();

        EXPECT_EQ("./default/d/1", uri.GetLocalPath());
    }
}
