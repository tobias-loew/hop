# CMakeList.txt : CMake project for hop, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project (hop)

# Add source to this project's executable.
set(CMAKE_CXX_STANDARD 20)

# TODO: Add tests and install targets if needed.
add_subdirectory (test)