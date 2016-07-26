cmake_minimum_required(VERSION 2.8)
project(MrMangler)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories("include")
add_subdirectory(source)

add_executable(MrMangler "source/main.cpp")
target_link_libraries(MrMangler LINK_PUBLIC libMrMangler)
