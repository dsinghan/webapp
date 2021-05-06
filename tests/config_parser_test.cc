#include "gtest/gtest.h"
#include "config_parser.h"


class NginxConfigParserTest : public ::testing::Test {
 protected:
  NginxConfigParser parser;
  NginxConfig out_config;
  bool success;
};

TEST_F(NginxConfigParserTest, ExampleConfig) {

  bool success = parser.Parse("example_config", &out_config);

  EXPECT_TRUE(success);
}

//Checks and ensures that even number of quotes in a file (e.g. can't have """)
TEST_F(NginxConfigParserTest, BackslashedQuoteInString) {

  bool success = parser.Parse("quote_in_string_backslashed", &out_config);

  EXPECT_TRUE(success);
}

//Checks for empty file, returns true. Fixed bug in code
TEST_F(NginxConfigParserTest, Empty_file) {

  bool success = parser.Parse("empty_file", &out_config);

  EXPECT_TRUE(success);
}

//Checks to make sure ending double quote is followed by whitespace, a semicolon, or
//an opening bracket. Fixed bug where anything after double quote is acceptable.
TEST_F(NginxConfigParserTest, QuoteFollowedByLetter) {

  bool success = parser.Parse("quote_followed_by_letter", &out_config);

  EXPECT_FALSE(success);
}

//Found bug with nested brackets returning false. Fixed code in coding_parser.cc
TEST_F(NginxConfigParserTest, NestedBrackets) {

  bool success = parser.Parse("nested_brackets", &out_config);

  EXPECT_TRUE(success);
}

//Test port extraction function to ensure proper port extraction
TEST_F(NginxConfigParserTest, PortExtraction) {

  parser.Parse("example_config", &out_config);
  int port = parser.extract_port(&out_config);

  EXPECT_EQ(8080, port);
}

//Test ToString function in config_parser
TEST_F(NginxConfigParserTest, ToStringTest) {

  parser.Parse("toStringTest", &out_config);

  std::string result = out_config.ToString();

  std::string compare = "server {\n  listen 80;\n  server_name foo.com;\n  root /home/ubuntu/sites/foo/;\n}\n";

  EXPECT_TRUE(result == compare);
}

//Test proper reporting of unknown file input
TEST_F(NginxConfigParserTest, UnknownConfigFile) {

  EXPECT_FALSE(parser.Parse("notAFile", &out_config));

}


//test various bad transitions during parsing of config file
TEST_F(NginxConfigParserTest, BadTransition) {

  EXPECT_FALSE(parser.Parse("bad_transition1", &out_config));
  EXPECT_FALSE(parser.Parse("bad_transition2", &out_config));
  EXPECT_FALSE(parser.Parse("bad_transition3", &out_config));
  EXPECT_FALSE(parser.Parse("bad_transition4", &out_config));
  EXPECT_FALSE(parser.Parse("bad_transition5", &out_config));
}