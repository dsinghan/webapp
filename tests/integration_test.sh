#!/bin/bash


#############
# Constants #
#############
TMP_DIR=$(mktemp -d -t TEST-XXXXXXXXXX)

EXECUTABLE="$1"
CONFIG_FILE="$TMP_DIR/config.txt"
LOCAL_HOST="127.0.0.1"
PORT="8080"

ECHO_EXPECTED_FILE="$TMP_DIR/echo_expected.txt"
ECHO_OUTPUT_FILE="$TMP_DIR/echo_output.txt"
STATIC_EXPECTED_FILE="$TMP_DIR/static_expected.txt"
STATIC_OUTPUT_FILE="$TMP_DIR/static_output.txt"
ERROR_EXPECTED_FILE="$TMP_DIR/error_expected.txt"
ERROR_OUTPUT_FILE="$TMP_DIR/error_output.txt"


#########
# Setup #
#########

# Generate Config File
cat > "$CONFIG_FILE" << EOF
port $PORT;
location "/echo" EchoHandler { }
location "/static" StaticHandler { root "./sample_configs"; }
location "/" ErrorHandler { }
EOF

# Begin Server in Background
"$EXECUTABLE" "$CONFIG_FILE" > /dev/null 2>&1 &
SERVER_PID=$!
sleep 1  # A short delay to allow time for the server to start

# Generate Echo Expected File
cat > "$ECHO_EXPECTED_FILE" << EOF
GET /echo HTTP/1.1
Host: $LOCAL_HOST:$PORT
User-Agent: curl/7.68.0
Accept: */*
EOF
sed -i -e 's/$/\r/' "$ECHO_EXPECTED_FILE"  # newlines should be CRLF

# Generate Static Expected File
echo -n "};" > "$STATIC_EXPECTED_FILE"

# Generate Error Expected File
echo -n "404 Not Found" > "$ERROR_EXPECTED_FILE"


################
# Perform Test #
################

# Echo Test
#   -s: Silent
#   -S: Show error (even when silent)
#   -o: Output file
curl -s -S -o "$ECHO_OUTPUT_FILE" "$LOCAL_HOST":"$PORT"/echo
ECHO_CURL_RESULT=$?

# Check that the response is normal
#   -N: Treat absent file as empty
diff -N "$ECHO_EXPECTED_FILE" "$ECHO_OUTPUT_FILE"
ECHO_DIFF_RESULT=$?

# Static Test
curl -s -S -o "$STATIC_OUTPUT_FILE" "$LOCAL_HOST":"$PORT"/static/bad_transition1
STATIC_CURL_RESULT=$?

diff -N "$STATIC_EXPECTED_FILE" "$STATIC_OUTPUT_FILE"
STATIC_DIFF_RESULT=$?

# Error Test
curl -s -S -o "$ERROR_OUTPUT_FILE" "$LOCAL_HOST":"$PORT"/
ERROR_CURL_RESULT=$?

diff -N "$ERROR_EXPECTED_FILE" "$ERROR_OUTPUT_FILE"
ERROR_DIFF_RESULT=$?


###########
# Cleanup #
###########
rm -rf "$TMP_DIR"
kill $SERVER_PID


################
# Test Results #
################

# A successful test means that the web server received and handled an HTTP
# request, and provided a response that matches expectations, whereas an
# unsuccessful test would mean either the client could not receive a response
# from the server, or that the response was different than what was expected.

[ $ECHO_CURL_RESULT -eq 0 ] && [ $ECHO_DIFF_RESULT -eq 0 ]
ECHO_TEST_RESULT=$?

[ $STATIC_CURL_RESULT -eq 0 ] && [ $STATIC_DIFF_RESULT -eq 0 ]
STATIC_TEST_RESULT=$?

[ $ERROR_CURL_RESULT -eq 0 ] && [ $ERROR_DIFF_RESULT -eq 0 ]
ERROR_TEST_RESULT=$?

[ $ECHO_TEST_RESULT -eq 0 ] || echo "Echo Test Failed"
[ $STATIC_TEST_RESULT -eq 0 ] || echo "Static Test Failed"
[ $ERROR_TEST_RESULT -eq 0 ] || echo "Error Test Failed"
[ $ECHO_TEST_RESULT -eq 0 ] && [ $STATIC_TEST_RESULT -eq 0 ] && [ $ERROR_TEST_RESULT -eq 0 ]
TEST_RESULT=$?
exit $TEST_RESULT  # Exits with code 0 (success) or 1 (failure)
