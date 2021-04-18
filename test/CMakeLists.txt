# CMakeList.txt : CMake project for hop, include source and define
# project specific logic here.
#
#cmake_minimum_required (VERSION 3.8)

#project(test)


# Add source to this project's executable.
#add_executable (hop "hop_test_2.cpp")

#add_library(test hop_test_2.cpp)

# TODO: Add tests and install targets if needed.
add_executable(test hop_test_2.cpp)

set(BOOST_PATH "//mnt/c/loew/boost_1_75_0")
target_include_directories(test PRIVATE "${BOOST_PATH}")
target_include_directories(test PRIVATE  "..\\include")
#target_include_directories(test PRIVATE "..\include")

#enable_testing()
#add_test(NAME mytest COMMAND test)
