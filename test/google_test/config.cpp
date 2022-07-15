#include <gtest/gtest.h>

#include "ConfigParser.hpp"

const std::string CONF_PATH = "./config/error";

void testParseConfigError(std::string file_path, std::string expect) {
    EXPECT_ANY_THROW(ConfigParser::ParseConfigFile(file_path));

    testing::internal::CaptureStderr();
    try {
        ConfigParser::ParseConfigFile(file_path);
    } catch (const std::exception& e) { std::cerr << e.what(); }

    std::string actual = testing::internal::GetCapturedStderr();
    EXPECT_EQ(expect, actual);
}

TEST(ConfigParser, ErrorInvalidFile) {
    const std::string file_path = CONF_PATH + "/no_such_file.conf";
    const std::string expect    = "invalid file";

    testParseConfigError(file_path, expect);
}

TEST(ConfigParser, ErrorInvalidBraceNum) {
    const std::string file_path = CONF_PATH + "/invalid_brace_num.conf";
    const std::string expect    = "invalid number of braces";

    testParseConfigError(file_path, expect);
}

TEST(ConfigParser, ErrorUnexpectedToken) {
    {
        const std::string file_path = CONF_PATH + "/unexpected_token_01.conf";
        const std::string expect    = "unexpected token";

        testParseConfigError(file_path, expect);
    }

    {
        const std::string file_path = CONF_PATH + "/unexpected_token_02.conf";
        const std::string expect    = "unexpected token";

        testParseConfigError(file_path, expect);
    }
}

TEST(ConfigParser, ErrorDuplicatedDirectry) {
    const std::string file_path = CONF_PATH + "/duplicated_directive.conf";
    const std::string expect    = "duplicated derective";

    testParseConfigError(file_path, expect);
}

TEST(ConfigParser, ErrorInvalidDirective) {
    {
        const std::string file_path = CONF_PATH + "/invalid_directive_01.conf";
        const std::string expect    = "invalid derective";

        testParseConfigError(file_path, expect);
    }
    {
        const std::string file_path = CONF_PATH + "/invalid_directive_02.conf";
        const std::string expect    = "invalid derective";

        testParseConfigError(file_path, expect);
    }
}

TEST(ConfigParser, ErrorInvalidValueNum) {
    const std::string file_path = CONF_PATH + "/invalid_value_num.conf";
    const std::string expect    = "invalid value num";

    testParseConfigError(file_path, expect);
}

TEST(ConfigParser, ErrorInvalidListen) {
    const std::string file_path = CONF_PATH + "/invalid_listen.conf";
    const std::string expect    = "invalid listen";

    testParseConfigError(file_path, expect);
}

TEST(ConfigParser, ErrorInvalidErrorPage) {
    const std::string file_path = CONF_PATH + "/invalid_error_page.conf";
    const std::string expect    = "invalid error page";

    testParseConfigError(file_path, expect);
}

TEST(ConfigParser, ErrorInvalidAllowedMethod) {
    const std::string file_path = CONF_PATH + "/invalid_allowed_method.conf";
    const std::string expect    = "invalid allowed method";

    testParseConfigError(file_path, expect);
}

TEST(ConfigParser, ErrorInvalidAutoIndex) {
    const std::string file_path = CONF_PATH + "/invalid_auto_index.conf";
    const std::string expect    = "invalid auto index";

    testParseConfigError(file_path, expect);
}

TEST(ConfigParser, ErrorInvalidReturn) {
    const std::string file_path = CONF_PATH + "/invalid_return.conf";
    const std::string expect    = "invalid return";

    testParseConfigError(file_path, expect);
}

TEST(ConfigParser, ErrorInvalidDuplicatedLocation) {
    const std::string file_path = CONF_PATH + "/duplicated_location.conf";
    const std::string expect    = "duplicated location";

    testParseConfigError(file_path, expect);
}
