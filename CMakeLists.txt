project(PDDL)
cmake_minimum_required(VERSION 2.8)
aux_source_directory(. SRC_LIST)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")

add_executable(modeltest modeltest.cpp)
target_link_libraries(modeltest gtest pthread)
add_test(model modeltest)

