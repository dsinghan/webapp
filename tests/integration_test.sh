#!/bin/bash

# NOTE: This shell script takes an extra parameter – 
#       the name of the webserver executable

#############
# Constants #
#############
TMP_DIR=$(mktemp -d -t TEST-XXXXXXXXXX)

EXECUTABLE="$1"
CONFIG_FILE="$TMP_DIR/config.txt"
CONFIG_FILE_BACKEND="$TMP_DIR/config_backend.txt"
LOCAL_HOST="127.0.0.1"
PORT="8080"
PORT_BACKEND="8081"
NUM_THREADS="8";

ECHO_EXPECTED_FILE="$TMP_DIR/echo_expected.txt"
ECHO_OUTPUT_FILE="$TMP_DIR/echo_output.txt"
STATIC_EXPECTED_FILE="$TMP_DIR/static_expected.txt"
STATIC_OUTPUT_FILE="$TMP_DIR/static_output.txt"
ERROR_EXPECTED_FILE="$TMP_DIR/error_expected.txt"
ERROR_OUTPUT_FILE="$TMP_DIR/error_output.txt"
PROXY_EXPECTED_FILE="$TMP_DIR/proxy_expected.txt"
PROXY_OUTPUT_FILE="$TMP_DIR/proxy_output.txt"
STATUS_EXPECTED_FILE="$TMP_DIR/status_expected.txt"
STATUS_OUTPUT_FILE="$TMP_DIR/status_output.txt"
HEALTH_EXPECTED_FILE="$TMP_DIR/health_expected.txt"
HEALTH_OUTPUT_FILE="$TMP_DIR/health_output.txt"
BLOCK_EXPECTED_SLEEP_FILE="$TMP_DIR/block_sleep_expected.txt"  # sleep request
BLOCK_OUTPUT_SLEEP_FILE="$TMP_DIR/block_sleep_output.txt"
BLOCK_EXPECTED_LAND_FILE="$TMP_DIR/block_land_expected.txt" # echo request to test if server is still running after calling sleep
BLOCK_OUTPUT_LAND_FILE="$TMP_DIR/block_land_output.txt"

#########
# Setup #
#########

# Generate Config File
cat > "$CONFIG_FILE" << EOF
port $PORT;
threads $NUM_THREADS;
location "/echo" EchoHandler { }
location "/static" StaticHandler { root "./sample_configs"; }
location "/" ErrorHandler { }
location "/proxy" ProxyHandler { host "$LOCAL_HOST"; port "$PORT_BACKEND";}
location "/status" StatusHandler { }
location "/health" HealthHandler { }
location "/sleep" BlockingHandler { }
EOF

cat > "$CONFIG_FILE_BACKEND" << EOF
port $PORT_BACKEND;
threads $NUM_THREADS;
location "/echo" EchoHandler { }
location "/static" StaticHandler { root "./sample_configs"; }
location "/" ErrorHandler { }
EOF

# Begin Server in Background
"$EXECUTABLE" "$CONFIG_FILE" > /dev/null 2>&1 &
SERVER_PID=$!
sleep 1  # A short delay to allow time for the server to start

# Begin Backend Server in Background
"$EXECUTABLE" "$CONFIG_FILE_BACKEND" > /dev/null 2>&1 &
SERVER_PID_BACKEND=$!
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

# Generate Proxy Expected File
echo -n "};" > "$PROXY_EXPECTED_FILE"

# Generate Status Expected File
cat > "$STATUS_EXPECTED_FILE" << EOF
Report:

Requests Received:
/, 404: 1
/echo, 200: 1
/proxy/static/bad_transition1, 200: 1
/static/bad_transition1, 200: 1

Request Handlers:
BlockingHandler
    /sleep
EchoHandler
    /echo
ErrorHandler
    /
HealthHandler
    /health
ProxyHandler
    /proxy
StaticHandler
    /static
StatusHandler
    /status
EOF
sed -i -e 's/^    /\t/' "$STATUS_EXPECTED_FILE"  # Replace four leading spaces with tab

# Generate Health Expected File
echo -n "OK" > "$HEALTH_EXPECTED_FILE"

# Generate Blocking Expected File
cat > "$BLOCK_EXPECTED_SLEEP_FILE" << EOF
Successfully put server to sleep
EOF
truncate -s -1 $BLOCK_EXPECTED_SLEEP_FILE

# Generate Error Expected File
echo -n "404 Not Found" > "$BLOCK_EXPECTED_LAND_FILE"

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

# Proxy Test
curl -s -S -o "$PROXY_OUTPUT_FILE" "$LOCAL_HOST":"$PORT"/proxy/static/bad_transition1
PROXY_CURL_RESULT=$?

diff -N "$PROXY_EXPECTED_FILE" "$PROXY_OUTPUT_FILE"
PROXY_DIFF_RESULT=$?

# Status Test
curl -s -S -o "$STATUS_OUTPUT_FILE" "$LOCAL_HOST":"$PORT"/status
STATUS_CURL_RESULT=$?

diff -N "$STATUS_EXPECTED_FILE" "$STATUS_OUTPUT_FILE"
STATUS_DIFF_RESULT=$?

# Health Test
curl -s -S -o "$HEALTH_OUTPUT_FILE" "$LOCAL_HOST":"$PORT"/health
HEALTH_CURL_RESULT=$?

diff -N "$HEALTH_EXPECTED_FILE" "$HEALTH_OUTPUT_FILE"
HEALTH_DIFF_RESULT=$?

# Blocking Test - send request to make server sleep.
# Right after that, without waiting for sleep response, send a request (called "land" 
#   in this program) to see if we can still access server
SECONDS=0
curl -s -S -o "$BLOCK_OUTPUT_SLEEP_FILE" "$LOCAL_HOST":"$PORT"/sleep & BLOCK_CURL_SLEEP_PID=$!
curl -s -S -o "$BLOCK_OUTPUT_LAND_FILE" "$LOCAL_HOST":"$PORT"/ & BLOCK_CURL_LAND_PID=$!
# Wait for land to complete
wait $BLOCK_CURL_LAND_PID

# SECONDS is a built-in variable in bash. 
# See how long it took for LAND request to finish
duration=$SECONDS
wait $BLOCK_CURL_SLEEP_PID

diff -N "$BLOCK_EXPECTED_SLEEP_FILE" "$BLOCK_OUTPUT_SLEEP_FILE"
BLOCK_DIFF_SLEEP_RESULT=$?

diff -N "$BLOCK_EXPECTED_LAND_FILE" "$BLOCK_OUTPUT_LAND_FILE"
BLOCK_DIFF_LAND_RESULT=$?

# See if the duration was less than 5 seconds. That would mean that another thread handled the land request
[ $duration -lt 5 ]
BLOCK_RESP_WITHIN_SLEEP_INTERVAL=$?

###########
# Cleanup #
###########
rm -rf "$TMP_DIR"
kill $SERVER_PID
kill $SERVER_PID_BACKEND


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

[ $PROXY_CURL_RESULT -eq 0 ] && [ $PROXY_DIFF_RESULT -eq 0 ]
PROXY_TEST_RESULT=$?

[ $STATUS_CURL_RESULT -eq 0 ] && [ $STATUS_DIFF_RESULT -eq 0 ]
STATUS_TEST_RESULT=$?

[ $HEALTH_CURL_RESULT -eq 0 ] && [ $HEALTH_DIFF_RESULT -eq 0 ]
HEALTH_TEST_RESULT=$?

[ $BLOCK_DIFF_SLEEP_RESULT -eq 0 ] && [ $BLOCK_DIFF_LAND_RESULT -eq 0 ] && [ $BLOCK_RESP_WITHIN_SLEEP_INTERVAL -eq 0 ]
BLOCK_TEST_RESULT=$?

[ $ECHO_TEST_RESULT -eq 0 ] || echo "Echo Test Failed"
[ $STATIC_TEST_RESULT -eq 0 ] || echo "Static Test Failed"
[ $ERROR_TEST_RESULT -eq 0 ] || echo "Error Test Failed"
[ $PROXY_TEST_RESULT -eq 0 ] || echo "Proxy Test Failed"
[ $STATUS_TEST_RESULT -eq 0 ] || echo "Status Test Failed"
[ $HEALTH_TEST_RESULT -eq 0 ] || echo "HEALTH Test Failed"
[ $BLOCK_TEST_RESULT -eq 0 ] || echo "Block Test Failed"
[ $ECHO_TEST_RESULT -eq 0 ] && [ $STATIC_TEST_RESULT -eq 0 ] && [ $ERROR_TEST_RESULT -eq 0 ] && [ $PROXY_TEST_RESULT -eq 0 ] && [ $STATUS_TEST_RESULT -eq 0 ] && [ $HEALTH_TEST_RESULT -eq 0 ] && [ $BLOCK_TEST_RESULT -eq 0 ]
TEST_RESULT=$?
exit $TEST_RESULT  # Exits with code 0 (success) or 1 (failure)
