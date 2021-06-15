project(harbor)

cmake_minimum_required(VERSION 3.0)

add_library(harbor_lib src/util/log.cpp)
target_include_directories(harbor_lib PUBLIC src/)

add_executable(harbor src/main.cpp)
target_include_directories(harbor PUBLIC src/)
target_link_libraries(harbor PUBLIC harbor_lib)
