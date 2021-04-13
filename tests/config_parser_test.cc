#include "gtest/gtest.h"
#include "config_parser.h"

TEST(NginxConfigParserTest, SimpleConfig) {
  NginxConfigParser parser;
  NginxConfig out_config;

  bool success = parser.Parse("example_config", &out_config);

  EXPECT_TRUE(success);
}

//Checks and ensures that even number of quotes in a file (e.g. can't have """)
TEST(triple_double_quote, SimpleConfig) {
  NginxConfigParser parser;
  NginxConfig out_config;

  bool success = parser.Parse("triple_double_quote", &out_config);

  EXPECT_FALSE(success);
}

//Checks for empty file, returns true. Fixed bug in code
TEST(empty_file, SimpleConfig) {
  NginxConfigParser parser;
  NginxConfig out_config;

  bool success = parser.Parse("empty_file", &out_config);

  EXPECT_TRUE(success);
}

//Checks to make sure ending double quote is followed by whitespace, a semicolon, or
//an opening bracket. Fixed bug where anything after double quote is acceptable.
TEST(quote_followed_by_letter, SimpleConfig) {
  NginxConfigParser parser;
  NginxConfig out_config;

  bool success = parser.Parse("quote_followed_by_letter", &out_config);

  EXPECT_FALSE(success);
}

//Checks for backslash_escape characters in string.
TEST(backslash_escape_in_string, SimpleConfig) {
  NginxConfigParser parser;
  NginxConfig out_config;

  bool success = parser.Parse("backslash_escape_in_string", &out_config);

  EXPECT_TRUE(success);
}

//Found bug with nested brackets returning false. Fixed code in coding_parser.cc
TEST(nested_brackets, SimpleConfig) {
  NginxConfigParser parser;
  NginxConfig out_config;

  bool success = parser.Parse("nested_brackets", &out_config);

  EXPECT_TRUE(success);
}

