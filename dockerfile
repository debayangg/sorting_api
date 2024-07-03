# Use an official GCC image to compile the C++ code
FROM gcc:latest as builder

# Set the working directory
WORKDIR /usr/src/app

# Copy the current directory contents into the container at /usr/src/app
COPY . .

# Compile the C++ code with static linking
RUN g++ -std=c++11 -static-libstdc++ main.cpp -o app

# Use a minimal image for the final container
FROM debian:latest

# Set the working directory
WORKDIR /root/

# Copy the compiled binary from the builder stage
COPY --from=builder /usr/src/app/app .

# Expose port 8080 (optional if you want to expose a specific port)
EXPOSE 8080

# Run the binary
CMD ["./app"]
