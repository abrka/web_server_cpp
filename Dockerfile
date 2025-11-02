# Use a base image with a recent version of Ubuntu or Debian
FROM gcc:latest

RUN apt-get update && apt-get install -y cmake 

RUN apt-get install -y php

# Set the working directory inside the container
WORKDIR /app

# Copy your project files into the container
COPY . /app

# Create a build directory and run CMake
RUN mkdir build && cd build && cmake ..

# Build the project
RUN cd build && make

WORKDIR /app/build/example/

# (Optional) If you want to run the built executable directly,
# define a default command to execute when the container starts.
CMD ["./web_server_example"]