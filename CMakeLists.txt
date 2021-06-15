# Harbor - A portable and highly customisable game engine
# Copyright (C) 2021 Semphris <semphris@protonmail.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as
# published by the Free Software Foundation, either version 3 of the
# License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

project(harbor)

cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB_RECURSE HARBOR_LIB_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*/*.cpp)
list(SORT HARBOR_LIB_FILES)
add_library(harbor_lib ${HARBOR_LIB_FILES})
target_include_directories(harbor_lib PUBLIC
                          ${CMAKE_CURRENT_SOURCE_DIR}/src/
                          ${CMAKE_CURRENT_SOURCE_DIR}/polyfill/)

add_executable(harbor ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)
target_include_directories(harbor PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/)
target_link_libraries(harbor PUBLIC harbor_lib)

file(GLOB_RECURSE HARBOR_TEST_FILES ${CMAKE_CURRENT_SOURCE_DIR}/tests/*/*.cpp)
list(SORT HARBOR_TEST_FILES)
add_executable(run_tests ${HARBOR_TEST_FILES})
target_include_directories(run_tests PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/)
target_link_libraries(run_tests PUBLIC harbor_lib)

find_package(GTest)
if(GTEST_FOUND)
  message(STATUS "Using system libraries for GTest: ${GTEST_LIBRARIES}")
  target_link_libraries(run_tests PUBLIC ${GTEST_BOTH_LIBRARIES} pthread)
  target_include_directories(run_tests PUBLIC ${GTEST_INCLUDE_DIRS})
else(GTEST_FOUND)
  message(STATUS "Compiling GTest from source")
  add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/gtest)
  target_link_libraries(run_tests PUBLIC gtest gtest_main)
  add_dependencies(run_tests gtest gtest_main)
endif(GTEST_FOUND)

find_package(SDL2)
if(SDL2_FOUND)
  message(STATUS "Using system libraries for SDL2: ${SDL2_LIBRARIES}")
  target_link_libraries(harbor_lib PUBLIC ${SDL2_LIBRARIES})
  target_include_directories(harbor_lib PUBLIC ${SDL2_INCLUDE_DIRS})
else(SDL2_FOUND)
  message(STATUS "Compiling SDL2 from source")
  add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/SDL2)
  target_link_libraries(harbor_lib PUBLIC SDL2)
  add_dependencies(harbor_lib SDL2)
endif(SDL2_FOUND)
