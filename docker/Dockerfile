### Build/test container ###
# Define builder stage
FROM powell-cat:base as builder

# Share work directory
COPY . /usr/src/project
WORKDIR /usr/src/project/build

# Build and test
RUN cmake ..
RUN make
RUN ctest --output-on_failure


### Deploy container ###
# Define deploy stage
FROM ubuntu:latest as deploy

# Copy server output binary to "."
COPY --from=builder /usr/src/project/build/bin/webserver .
COPY --from=builder /usr/src/project/conf/my_config .
COPY --from=builder /usr/src/project/static ./static
COPY --from=builder /usr/src/project/chat_password ./chat_password

RUN echo "HI" > ./hello.txt
RUN ls -la .
RUN ls -la ./static

# Expose some port(s)
EXPOSE 80

# Use ENTRYPOINT to specify the binary name
# Update with real server name
ENTRYPOINT ["./webserver"]

# Use CMD to specify arguments to ENTRYPOINT
# Update with real server args
CMD ["my_config"]
