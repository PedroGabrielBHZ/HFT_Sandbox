# Use the Alpine image with the GCC compiler as the base image
FROM alpine:3.14

# Install the necessary packages
RUN apk add --no-cache g++ make boost-dev

# Set the working directory in the container
WORKDIR /app

# Copy the source code and the Makefile into the container
COPY client.cpp Makefile exchanges.txt ./

# Build the client
RUN make

# Run the client when the container starts
CMD ["./client", "exchanges.txt"]