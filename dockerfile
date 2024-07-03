# Use a base image with a C development environment
FROM gcc:latest

# Set the working directory inside the container
WORKDIR /app

# Copy the server C source code into the container
COPY main.c /app/main.c

# Compile the server C code
RUN gcc -o server main.c

# Expose port 8080 to the outside world
EXPOSE 8080

# Command to run the server when the container starts
CMD ["./server"]
