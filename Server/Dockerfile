# Use the Alpine image with the GCC compiler as the base image
FROM alpine:3.14

# Install the necessary packages
RUN apk add --no-cache g++ make boost-dev

# Set the working directory in the container
WORKDIR /app

# Copy the source code and the Makefile into the container
COPY TickerServer.cpp Makefile ./

# Build the TickerServer
RUN make

# Expose port 1234 for the TickerServer
EXPOSE 1234

# Run the TickerServer when the container starts
CMD ["./TickerServer"]