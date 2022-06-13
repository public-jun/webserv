#include <gtest/gtest.h>

#include "URI.hpp"

#include <map>
#include <vector>

#include "Config.hpp"
#include "ConfigParser.hpp"
#include "ServerConfig.hpp"

class URITest : public ::testing::Test {
protected:
    virtual void SetUp() {
        typedef std::map<int, std::vector<const ServerConfig> > configs_map;

        ConfigParser::parseConfigFile("./config/duplicated_port.conf");
        configs_map servers_map = Config::instance()->GetServerConfigs();

        configs_map::const_iterator it = servers_map.begin();

        std::vector<const ServerConfig> virtual_server_configs = it->second;

        config = *virtual_server_configs.begin();
    }

    ServerConfig config;
};

TEST_F(URITest, splitURI) {

    {
        URI uri(config, "/");

        EXPECT_EQ("/", uri.GetRawTarget());
        EXPECT_EQ("/", uri.GetRawPath());
        EXPECT_EQ("", uri.GetQuery());
    }

    {
        URI uri(config, "/docs");

        EXPECT_EQ("/docs", uri.GetRawTarget());
        EXPECT_EQ("/docs", uri.GetRawPath());
        EXPECT_EQ("", uri.GetQuery());
    }

    {
        URI uri(config, "/docs/");

        EXPECT_EQ("/docs/", uri.GetRawTarget());
        EXPECT_EQ("/docs/", uri.GetRawPath());
        EXPECT_EQ("", uri.GetQuery());
    }

    {
        URI uri(config, "/docs/hoge");

        EXPECT_EQ("/docs/hoge", uri.GetRawTarget());
        EXPECT_EQ("/docs/hoge", uri.GetRawPath());
        EXPECT_EQ("", uri.GetQuery());
    }

    {
        URI uri(config, "/docs/hoge/");

        EXPECT_EQ("/docs/hoge/", uri.GetRawTarget());
        EXPECT_EQ("/docs/hoge/", uri.GetRawPath());
        EXPECT_EQ("", uri.GetQuery());
    }

    {
        URI uri(config, "/?");

        EXPECT_EQ("/?", uri.GetRawTarget());
        EXPECT_EQ("/", uri.GetRawPath());
        EXPECT_EQ("", uri.GetQuery());
    }

    {
        URI uri(config, "/?arg1");

        EXPECT_EQ("/?arg1", uri.GetRawTarget());
        EXPECT_EQ("/", uri.GetRawPath());
        EXPECT_EQ("arg1", uri.GetQuery());
    }

    {
        URI uri(config, "/?arg1+arg2");

        EXPECT_EQ("/?arg1+arg2", uri.GetRawTarget());
        EXPECT_EQ("/", uri.GetRawPath());
        EXPECT_EQ("arg1+arg2", uri.GetQuery());
    }
}