#include "gtest/gtest.h"
#include "config_parser.h"

TEST(NginxConfigParserTest, SimpleConfig) {
  NginxConfigParser parser;
  NginxConfig out_config;

  bool success = parser.Parse("example_config", &out_config);

  EXPECT_TRUE(success);
}

//Checks and ensures that even number of quotes in a file (e.g. can't have """)
TEST(quote_in_string_not_backslashed, SimpleConfig) {
  NginxConfigParser parser;
  NginxConfig out_config;

  bool success = parser.Parse("quote_in_string_not_backslashed", &out_config);

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

//Found bug with nested brackets returning false. Fixed code in coding_parser.cc
TEST(nested_brackets, SimpleConfig) {
  NginxConfigParser parser;
  NginxConfig out_config;

  bool success = parser.Parse("nested_brackets", &out_config);

  EXPECT_TRUE(success);
}

