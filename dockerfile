# Use an official Ubuntu image as base
FROM ubuntu:latest

# Install necessary dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    && rm -rf /var/lib/apt/lists/*

# Clone Crow repository and build
RUN git clone https://github.com/ipkn/crow.git /tmp/crow && \
    cd /tmp/crow && \
    mkdir build && cd build && \
    cmake .. && \
    make && \
    make install

# Set the working directory
WORKDIR /app

# Copy the C++ source code
COPY main.cpp .

# Compile the C++ code
RUN g++ -std=c++11 -o web_service main.cpp -lcrow

# Expose port 8080 (or the port your application listens on)
EXPOSE 8080

# Command to run the server
CMD ["./web_service"]
