#include <string>

#include <boost/log/trivial.hpp>
#include "gtest/gtest.h"

#include "utils.h"
#include "echo_request_handler.h"

class utilsTest : public ::testing::Test {
 protected:
  bool success;
};

TEST_F(utilsTest, testInit) {
    init();

    BOOST_LOG_TRIVIAL(trace) << "trace";
    BOOST_LOG_TRIVIAL(debug) << "debug";
    BOOST_LOG_TRIVIAL(info) << "info";
    BOOST_LOG_TRIVIAL(warning) << "warning";
    BOOST_LOG_TRIVIAL(error) << "error";
    BOOST_LOG_TRIVIAL(fatal) << "fatal";
}

TEST_F(utilsTest, testInitLogging) {
    init_logging();

    BOOST_LOG_TRIVIAL(trace) << "trace";
    BOOST_LOG_TRIVIAL(debug) << "debug";
    BOOST_LOG_TRIVIAL(info) << "info";
    BOOST_LOG_TRIVIAL(warning) << "warning";
    BOOST_LOG_TRIVIAL(error) << "error";
    BOOST_LOG_TRIVIAL(fatal) << "fatal";
}