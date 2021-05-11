// An nginx config file parser.
//
// See:
//   http://wiki.nginx.org/Configuration
//   http://blog.martinfjordvald.com/2010/07/nginx-primer/
//
// How Nginx does it:
//   http://lxr.nginx.org/source/src/core/ngx_conf_file.c

#include <cstdio>
#include <fstream>
#include <iostream>
#include <memory>
#include <stack>
#include <string>
#include <string.h>
#include <vector>
#include <map>

#include <boost/log/trivial.hpp>

#include "config_parser.h"
#include "echo_request_handler.h"
#include "error_handler.h"
#include "proxy_request_handler.h"
#include "request_handler.h"
#include "static_request_handler.h"


std::string NginxConfigStatement::ToString(int depth) {
  std::string serialized_statement;
  for (int i = 0; i < depth; ++i) {
    serialized_statement.append("  ");
  }
  for (unsigned int i = 0; i < tokens_.size(); ++i) {
    if (i != 0) {
      serialized_statement.append(" ");
    }
    serialized_statement.append(tokens_[i]);
  }
  if (child_block_.get() != nullptr) {
    serialized_statement.append(" {\n");
    serialized_statement.append(child_block_->ToString(depth + 1));
    for (int i = 0; i < depth; ++i) {
      serialized_statement.append("  ");
    }
    serialized_statement.append("}");
  } else {
    serialized_statement.append(";");
  }
  serialized_statement.append("\n");
  return serialized_statement;
}


std::string NginxConfig::ToString(int depth) {
  std::string serialized_config;
  for (const auto& statement : statements_) {
    serialized_config.append(statement->ToString(depth));
  }
  return serialized_config;
}

bool NginxConfigParser::Parse(std::istream* config_file, NginxConfig* config) {
  std::stack<NginxConfig*> config_stack;
  config_stack.push(config);
  TokenType last_token_type = TOKEN_TYPE_START;
  TokenType token_type;
  while (true) {
    std::string token;
    token_type = ParseToken(config_file, &token);
    //printf ("%s: %s\n", TokenTypeAsString(token_type), token.c_str());
    if (token_type == TOKEN_TYPE_ERROR) {
      break;
    }

    if (token_type == TOKEN_TYPE_COMMENT) {
      // Skip comments.
      continue;
    }

    if (token_type == TOKEN_TYPE_START) {
      // Error.
      break;
    } else if (token_type == TOKEN_TYPE_NORMAL ||
               token_type == TOKEN_TYPE_QUOTED_STRING) {
      if (last_token_type == TOKEN_TYPE_START ||
          last_token_type == TOKEN_TYPE_STATEMENT_END ||
          last_token_type == TOKEN_TYPE_START_BLOCK ||
          last_token_type == TOKEN_TYPE_END_BLOCK ||
          last_token_type == TOKEN_TYPE_NORMAL ||
          last_token_type == TOKEN_TYPE_QUOTED_STRING) {
        if (last_token_type != TOKEN_TYPE_NORMAL &&
            last_token_type != TOKEN_TYPE_QUOTED_STRING) {
          config_stack.top()->statements_.emplace_back(
              new NginxConfigStatement);
        }
        config_stack.top()->statements_.back().get()->tokens_.push_back(
            token);
      } else {
        // Error.
        break;
      }
    } else if (token_type == TOKEN_TYPE_STATEMENT_END) {
      if (last_token_type != TOKEN_TYPE_NORMAL &&
          last_token_type != TOKEN_TYPE_QUOTED_STRING) {
        // Error.
        break;
      }
    } else if (token_type == TOKEN_TYPE_START_BLOCK) {
      if (last_token_type != TOKEN_TYPE_NORMAL &&
          last_token_type != TOKEN_TYPE_QUOTED_STRING) {
        // Error.
        break;
      }
      NginxConfig* const new_config = new NginxConfig;
      config_stack.top()->statements_.back().get()->child_block_.reset(
          new_config);
      config_stack.push(new_config);
    } else if (token_type == TOKEN_TYPE_END_BLOCK) {
      /* Fixed issue of encapsulated brackets and empty brackets returning FALSE. */
      if (last_token_type != TOKEN_TYPE_STATEMENT_END &&
          last_token_type != TOKEN_TYPE_START_BLOCK &&
          last_token_type != TOKEN_TYPE_END_BLOCK) {
        // Error.
        break;
      }
      config_stack.pop();
    } else if (token_type == TOKEN_TYPE_EOF) {
      if (last_token_type != TOKEN_TYPE_STATEMENT_END &&
          last_token_type != TOKEN_TYPE_END_BLOCK &&
          last_token_type != TOKEN_TYPE_START) {
        // Error.
        break;
      }
      return true;
    } else {
      // Error. Unknown token.
      break;
    }
    last_token_type = token_type;
  }

  BOOST_LOG_TRIVIAL(error) << "Bad transition from " << TokenTypeAsString(last_token_type) << " to " << TokenTypeAsString(token_type);

  return false;
}

bool NginxConfigParser::Parse(const char* file_name, NginxConfig* config) {
  std::ifstream config_file;
  config_file.open(file_name);
  if (!config_file.good()) {
    BOOST_LOG_TRIVIAL(error) << "Failed to open config file: " << file_name;
    return false;
  }

  const bool return_value =
      Parse(dynamic_cast<std::istream*>(&config_file), config);
  config_file.close();
  return return_value;
}

int NginxConfigParser::extract_port(NginxConfig * config) {
  // Returns the port specified in the given config file, or -1 if the file is not parsable.
  NginxConfigStatement * port_statement = find_statement("port", config);
  if (port_statement == NULL) {
    return -1;
  }
  return stoi(port_statement->tokens_[1]);
}

std::map<std::string, request_handler*> NginxConfigParser::get_locations(NginxConfig * config) {
  std::map<std::string, request_handler*> locations;

  for (int i = 0; i < config->statements_.size(); i++) {
    NginxConfigStatement * cur_statement = config->statements_[i].get();
    if (cur_statement->tokens_[0] != "location") {
      continue;
    }

    std::string handler_location = remove_trailing_slashes(parse_string(cur_statement->tokens_[1]));
    std::string handler_name = cur_statement->tokens_[2];
    NginxConfig handler_config = *(cur_statement->child_block_.get());

    BOOST_LOG_TRIVIAL(debug) << "Handler location: " << handler_location << ", handler_name: " << handler_name;

    request_handler * handler = create_handler(handler_name, handler_location, handler_config);
    if (handler_location != "" && handler != nullptr) {
      locations[handler_location] = handler;
    } else {
      BOOST_LOG_TRIVIAL(warning) << "Could not create handler " << handler_name << " for location " << handler_location;
    }
  }

  return locations;
}

NginxConfigStatement * NginxConfigParser::find_statement(std::string keyword, const NginxConfig* config) {
  // Finds the statement whose first token is keyword
  for (int i = 0; i < config->statements_.size(); i++) {
    NginxConfigStatement * cur_statement = config->statements_[i].get();
    std::string first_token = cur_statement->tokens_[0];
    if (first_token == keyword) {
      return cur_statement;
    }
  }
  return NULL;
}

std::string NginxConfigParser::parse_string(std::string raw_location) {
  // Transforms a string in a config file into a usable form, handling quotes and escaped characters
  // Returns empty string if invalid
  std::string parsed_location = "";

  int length = raw_location.size();
  bool start_quote = false;
  bool end_quote = false;
  bool escaped = false;
  for (int i = 0; i < length; i++) {
    if (i == 0 && raw_location[i] == '"') {
      start_quote = true;
    } else if (!escaped && raw_location[i] == '\\') {
      escaped = true;
    } else if (escaped) {
      // TODO: Handle escaped characters; this implementation simply ignores the backslash.
      parsed_location += raw_location[i];
      escaped = false;
    } else if (i == length - 1 && raw_location[i] == '"') {
      end_quote = true;
    } else {
      parsed_location += raw_location[i];
    }
  }

  if (escaped || (start_quote && !end_quote) || (!start_quote && end_quote)) {
    return "";
  }

  return parsed_location;
}

const char* NginxConfigParser::TokenTypeAsString(TokenType type) {
  switch (type) {
    case TOKEN_TYPE_START:         return "TOKEN_TYPE_START";
    case TOKEN_TYPE_NORMAL:        return "TOKEN_TYPE_NORMAL";
    case TOKEN_TYPE_START_BLOCK:   return "TOKEN_TYPE_START_BLOCK";
    case TOKEN_TYPE_END_BLOCK:     return "TOKEN_TYPE_END_BLOCK";
    case TOKEN_TYPE_COMMENT:       return "TOKEN_TYPE_COMMENT";
    case TOKEN_TYPE_STATEMENT_END: return "TOKEN_TYPE_STATEMENT_END";
    case TOKEN_TYPE_EOF:           return "TOKEN_TYPE_EOF";
    case TOKEN_TYPE_ERROR:         return "TOKEN_TYPE_ERROR";
    case TOKEN_TYPE_QUOTED_STRING: return "TOKEN_TYPE_QUOTED_STRING";
    default:                       return "Unknown token type";
  }
}

NginxConfigParser::TokenType NginxConfigParser::ParseToken(std::istream* input,
                                                           std::string* value) {
  TokenParserState state = TOKEN_STATE_INITIAL_WHITESPACE;
  while (input->good()) {
    const char c = input->get();
    if (!input->good()) {
      break;
    }
    switch (state) {
      case TOKEN_STATE_INITIAL_WHITESPACE:
        switch (c) {
          case '{':
            *value = c;
            return TOKEN_TYPE_START_BLOCK;
          case '}':
            *value = c;
            return TOKEN_TYPE_END_BLOCK;
          case '#':
            *value = c;
            state = TOKEN_STATE_TOKEN_TYPE_COMMENT;
            continue;
          case '"':
            *value = c;
            state = TOKEN_STATE_DOUBLE_QUOTE;
            continue;
          case '\'':
            *value = c;
            state = TOKEN_STATE_SINGLE_QUOTE;
            continue;
          case ';':
            *value = c;
            return TOKEN_TYPE_STATEMENT_END;
          case ' ':
          case '\t':
          case '\n':
          case '\r':
            continue;
          default:
            *value += c;
            state = TOKEN_STATE_TOKEN_TYPE_NORMAL;
            continue;
        }
      case TOKEN_STATE_SINGLE_QUOTE:
        *value += c;
        if (c == '\\') {
          const char d = input->get();
          *value += d;
          continue;
        }
        if (c == '\'') {
          const char e = input->get();
          if (e == ' ' || e == '\t' || e == '\n' || e == '\r' || e == ';' || e == '{' || e == '}') {
            input->unget();
            return TOKEN_TYPE_QUOTED_STRING;
          }
          input->unget();
        }
        continue;
      case TOKEN_STATE_DOUBLE_QUOTE:
        *value += c;
        if (c == '\\') {
          const char d = input->get();
          *value += d;
          continue;
        }
        if (c == '"') {
          const char e = input->get();
          if (e == ' ' || e == '\t' || e == '\n' || e == '\r' || e == ';' || e == '{' || e == '}') {
            input->unget();
            return TOKEN_TYPE_QUOTED_STRING;
          }
          input->unget();
        }
        continue;
      case TOKEN_STATE_TOKEN_TYPE_COMMENT:
        if (c == '\n' || c == '\r') {
          return TOKEN_TYPE_COMMENT;
        }
        *value += c;
        continue;
      case TOKEN_STATE_TOKEN_TYPE_NORMAL:
        if (c == ' ' || c == '\t' || c == '\n' || c == '\r' ||
            c == ';' || c == '{' || c == '}') {
          input->unget();
          return TOKEN_TYPE_NORMAL;
        }
        *value += c;
        continue;
    }
  }

  // If we get here, we reached the end of the file.
  if (state == TOKEN_STATE_SINGLE_QUOTE ||
      state == TOKEN_STATE_DOUBLE_QUOTE) {
    return TOKEN_TYPE_ERROR;
  }

  return TOKEN_TYPE_EOF;
}

request_handler * NginxConfigParser::create_handler(std::string handler_name, std::string handler_location, const NginxConfig & handler_config) {
  if (handler_name == "StaticHandler") {
    return new static_request_handler(handler_location, handler_config);
  } else if (handler_name == "EchoHandler") {
    return new echo_request_handler(handler_location, handler_config);
  } else if (handler_name == "ErrorHandler") {
    return new error_handler(handler_location, handler_config);
  } else if (handler_name == "ProxyHandler") {
    return new proxy_request_handler(handler_location, handler_config);
  } else {  
    return nullptr;
  }
}

std::string NginxConfigParser::remove_trailing_slashes(const std::string & given_string) {
  if (given_string == "/") {
    return given_string;
  }
  std::string ret (given_string);
  std::string to_remove ("/");

  std::size_t found = ret.find_last_not_of(to_remove);
  if (found != std::string::npos)
    ret.erase(found+1);
  else
    ret.clear();  // str is all whitespace

  return ret;
}
