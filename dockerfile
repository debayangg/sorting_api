FROM ubuntu:20.04

# Set the environment variable to avoid timezone prompt
ENV DEBIAN_FRONTEND=noninteractive

# Install necessary packages
RUN apt-get update && apt-get install -y \
    g++ \
    cmake \
    libboost-all-dev \
    wget \
    unzip \
    && rm -rf /var/lib/apt/lists/*

# Install nlohmann json library
RUN wget https://github.com/nlohmann/json/releases/download/v3.10.5/json.hpp -P /usr/include/nlohmann/

# Copy source files
COPY main.cpp /usr/src/myapp/

# Compile the application
RUN g++ -o /usr/src/myapp/web_service /usr/src/myapp/main.cpp -lboost_system -lpthread

# Set the working directory
WORKDIR /usr/src/myapp

# Expose port
EXPOSE 8080

# Run the application
CMD ["./web_service"]
