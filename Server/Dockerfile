# Use the Alpine image with the GCC compiler as the base image
FROM alpine:3.14

# Install the necessary packages
RUN apk add --no-cache g++ make boost-dev

# Set the working directory in the container
WORKDIR /app

# Copy the source code and the Makefile into the container
COPY server.cpp Makefile ./

# Build the server
RUN make

# Expose port 1234 for the server
EXPOSE 1234

# Run the server when the container starts
CMD ["./server", "1234"]