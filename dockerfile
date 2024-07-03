# Use an official Ubuntu image
FROM ubuntu:latest as builder

# Set the working directory
WORKDIR /usr/src/app

# Install GCC and other dependencies
RUN apt-get update && apt-get install -y \
    g++ \
    cmake \
    libboost-all-dev \
    libssl-dev

# Copy the current directory contents into the container at /usr/src/app
COPY . .

# Compile the C++ code
RUN g++ -std=c++11 main.cpp -o app

# Use a minimal image for the final container
FROM ubuntu:latest

# Set the working directory
WORKDIR /root/

# Copy the compiled binary from the builder stage
COPY --from=builder /usr/src/app/app .

# Expose port 8080 (optional if you want to expose a specific port)
EXPOSE 10000

# Run the binary
CMD ["./app"]
