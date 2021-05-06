#!/bin/bash

# Variables
TMP_DIR=$(mktemp -d -t TEST-XXXXXXXXXX)
BUILD_DIR="$TMP_DIR/build"
EXECUTABLE="$BUILD_DIR/bin/webserver"
CONFIG_FILE="$TMP_DIR/config.txt"
EXPECTED_FILE="$TMP_DIR/expected.txt"
OUTPUT_FILE="$TMP_DIR/output.txt"
LOCAL_HOST="127.0.0.1"
PORT="8080"

# Setup

# - Build
mkdir "$BUILD_DIR"
cmake -S .. -B "$BUILD_DIR" > /dev/null 2>&1
make -C "$BUILD_DIR" > /dev/null 2>&1

# - Config File
cat > "$CONFIG_FILE" << EOF
port $PORT;
location "/echo" EchoHandler { }
EOF

# - Expected File
cat > "$EXPECTED_FILE" << EOF
GET /echo HTTP/1.1
Host: $LOCAL_HOST:$PORT
User-Agent: curl/7.68.0
Accept: */*
EOF
sed -i -e 's/$/\r/' "$EXPECTED_FILE"  # newlines should be CRLF

# - Server
"$EXECUTABLE" "$CONFIG_FILE" &
SERVER_PID=$!
sleep 1  # A short delay to allow time for the server to start

# Perform Test

# Check that web server handled request
#   -s: Silent
#   -S: Show error (even when silent)
#   -o: Output file
curl -s -S -o "$OUTPUT_FILE" "$LOCAL_HOST":"$PORT"/echo
CURL_RESULT=$?

# Check that the response is normal
#   -N: Treat absent file as empty
diff -N "$EXPECTED_FILE" "$OUTPUT_FILE"
DIFF_RESULT=$?

# Cleanup
rm -rf "$TMP_DIR"
kill $SERVER_PID

# Get Test Result
# A successful test means that the web server received and handled an HTTP
# request, and provided a response to echo the request back to the client. An
# unsuccessful test would mean either the client could not receive a response
# from the server, or that the response was different than what was expected.
[ $CURL_RESULT -eq 0 ] || [ $DIFF_RESULT -eq 0 ]
TEST_RESULT=$?
exit $TEST_RESULT  # Exits with code 0 (success) or 1 (failure)
