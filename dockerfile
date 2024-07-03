# Use an official GCC image to compile the C++ code
FROM gcc:latest as builder

# Install dependencies
RUN apt-get update && apt-get install -y \
    cmake \
    libboost-all-dev \
    libssl-dev

# Set the working directory
WORKDIR /usr/src/app

# Copy the current directory contents into the container at /usr/src/app
COPY . .

# Download and install crow and nlohmann_json
RUN git clone https://github.com/CrowCpp/crow.git
RUN git clone https://github.com/nlohmann/json.git
RUN mv json/single_include/nlohmann .

# Compile the C++ code
RUN g++ -std=c++11 main.cpp -o app -I crow -I nlohmann

# Use a minimal image for the final container
FROM debian:latest

# Set the working directory
WORKDIR /root/

# Copy the compiled binary from the builder stage
COPY --from=builder /usr/src/app/app .

# Expose port 8080
EXPOSE 8080

# Run the binary
CMD ["./app"]
