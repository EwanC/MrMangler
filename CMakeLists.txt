cmake_minimum_required(VERSION 2.8)
project(MrMangler)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

include_directories("include")
add_subdirectory(source)

add_executable(MrMangler "source/main.cpp")
target_link_libraries(MrMangler LINK_PUBLIC libMrMangler)
