// An nginx config file parser.

#ifndef CONFIG_PARSER_H
#define CONFIG_PARSER_H

#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "request_handler.h"

class NginxConfig;

// The parsed representation of a single config statement.
class NginxConfigStatement {
 public:
  std::string ToString(int depth);
  std::vector<std::string> tokens_;
  std::unique_ptr<NginxConfig> child_block_;
};

// The parsed representation of the entire config.
class NginxConfig {
 public:
  std::string ToString(int depth = 0);
  std::vector<std::shared_ptr<NginxConfigStatement>> statements_;
};

// The driver that parses a config file and generates an NginxConfig.
class NginxConfigParser {
 public:
  NginxConfigParser();

  // Take a opened config file or file name (respectively) and store the
  // parsed config in the provided NginxConfig out-param.  Returns true
  // iff the input config file is valid.
  bool Parse(std::istream* config_file, NginxConfig* config);
  bool Parse(const char* file_name, NginxConfig* config);

  //Extract port from config
  int extract_port(NginxConfig* config);

  //Extract base directory from config
  std::string extract_root(const char* file_name, NginxConfig* config);

  //map the location name of the request handler in the config file to a pointer that points to 
  //a request handler object of that handler type.
  std::map<std::string, request_handler*> get_locations(NginxConfig * config);

  static NginxConfigStatement * find_statement(std::string keyword, const NginxConfig* config);

  static std::string parse_string(std::string raw_location);

  std::map<std::pair<std::string, int>, int> * get_request_results();

 private:
  enum TokenType {
    TOKEN_TYPE_START = 0,
    TOKEN_TYPE_NORMAL = 1,
    TOKEN_TYPE_START_BLOCK = 2,
    TOKEN_TYPE_END_BLOCK = 3,
    TOKEN_TYPE_COMMENT = 4,
    TOKEN_TYPE_STATEMENT_END = 5,
    TOKEN_TYPE_EOF = 6,
    TOKEN_TYPE_ERROR = 7,
    TOKEN_TYPE_QUOTED_STRING = 8
  };
  const char* TokenTypeAsString(TokenType type);

  enum TokenParserState {
    TOKEN_STATE_INITIAL_WHITESPACE = 0,
    TOKEN_STATE_SINGLE_QUOTE = 1,
    TOKEN_STATE_DOUBLE_QUOTE = 2,
    TOKEN_STATE_TOKEN_TYPE_COMMENT = 3,
    TOKEN_STATE_TOKEN_TYPE_NORMAL = 4
  };

  TokenType ParseToken(std::istream* input, std::string* value);

  request_handler * create_handler(std::string handler_name, std::string handler_location, const NginxConfig & handler_config);

  std::map<std::string, std::vector<std::string> > get_handlers_url_map(NginxConfig * config);
  std::map<std::string, std::vector<std::string> > handlers_url_map_;

  std::string remove_trailing_slashes(const std::string & given_string);

  std::map<std::pair<std::string, int>, int> * request_results_;
};

#endif // CONFIG_PARSER_H