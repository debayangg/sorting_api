# Use an official Ubuntu image as base
FROM ubuntu:latest

# Install necessary dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    libmicrohttpd-dev \
    && rm -rf /var/lib/apt/lists/*

# Set the working directory
WORKDIR /app

# Copy the C++ source code
COPY main.cpp .

# Compile the C++ code
RUN g++ -std=c++11 -o web_service main.cpp -lmicrohttpd

# Expose port 8080 (or the port your application listens on)
EXPOSE 10000

# Command to run the server
CMD ["./web_service"]
