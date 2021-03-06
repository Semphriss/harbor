#  Harbor - A portable and highly customisable game engine
#  Copyright (C) 2021 Semphris <semphris@protonmail.com>
#
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU Lesser General Public License as
#  published by the Free Software Foundation, either version 3 of the
#  License, or (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public License
#  along with this program.  If not, see <https://www.gnu.org/licenses/>.

if(${CMAKE_CURRENT_BINARY_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})
  message(FATAL_ERROR "Prevented in-tree build. Please run `mkdir -p build && "
                      "cd build`, then retry with `cmake .. [OPTIONS...]`")
endif(${CMAKE_CURRENT_BINARY_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})

project(harbor)

# 3.6 required by `list(FILTER...)`
cmake_minimum_required(VERSION 3.6)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/mk/cmake/")

# ============================================================================
#    General options
# ----------------------------------------------------------------------------

# Executable options
option(HARBOR_BUILD_EXEC "Build an executable directly" ON)
option(HARBOR_BUILD_TEST "Build a test suite" ON)

# Dependency options
option(HARBOR_USE_SCRIPTING "Compile the scripting engines" ON)
option(HARBOR_USE_SCRIPTING_SQUIRREL "Compile Squirrel with Harbor" ON)
option(HARBOR_USE_SCRIPTING_LUA "Compile Lua with Harbor" ON)
option(HARBOR_USE_VIDEO "Compile the video engines" ON)
option(HARBOR_USE_VIDEO_SDL "Compile the SDL renderer with Harbor" ON)
option(HARBOR_USE_VIDEO_OPENGL "Compile the OpenGL renderer with Harbor" ON)

# Dependency options overrides
if (NOT ${HARBOR_USE_SCRIPTING})
  set(HARBOR_USE_SCRIPTING_SQUIRREL OFF CACHE BOOL "Disabled because HARBOR_USE_SCRIPTING is off" FORCE)
  set(HARBOR_USE_SCRIPTING_LUA OFF CACHE BOOL "Disabled because HARBOR_USE_SCRIPTING is off" FORCE)
endif()
if (NOT ${HARBOR_USE_VIDEO})
  set(HARBOR_USE_VIDEO_SDL OFF CACHE BOOL "Disabled because HARBOR_USE_VIDEO is off" FORCE)
  set(HARBOR_USE_VIDEO_OPENGL OFF CACHE BOOL "Disabled because HARBOR_USE_VIDEO is off" FORCE)
endif()

# ============================================================================
#    Library and executable generation
# ----------------------------------------------------------------------------

# Main library
file(GLOB_RECURSE HARBOR_LIB_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*/*.cpp)
list(SORT HARBOR_LIB_FILES)

if (NOT (${HARBOR_USE_SCRIPTING_SQUIRREL} OR ${HARBOR_USE_SCRIPTING_LUA}))
  list(FILTER HARBOR_LIB_FILES EXCLUDE REGEX ${CMAKE_CURRENT_SOURCE_DIR}/src/scripting/*)
endif()
if (NOT ${HARBOR_USE_SCRIPTING_SQUIRREL})
  list(FILTER HARBOR_LIB_FILES EXCLUDE REGEX ${CMAKE_CURRENT_SOURCE_DIR}/src/scripting/squirrel/*)
endif()
if (NOT ${HARBOR_USE_SCRIPTING_LUA})
  list(FILTER HARBOR_LIB_FILES EXCLUDE REGEX ${CMAKE_CURRENT_SOURCE_DIR}/src/scripting/lua/*)
endif()
if (NOT (${HARBOR_USE_VIDEO_SDL} OR ${HARBOR_USE_VIDEO_OPENGL}))
  list(FILTER HARBOR_LIB_FILES EXCLUDE REGEX ${CMAKE_CURRENT_SOURCE_DIR}/src/video/*)
endif()
if (NOT ${HARBOR_USE_VIDEO_SDL})
  list(FILTER HARBOR_LIB_FILES EXCLUDE REGEX ${CMAKE_CURRENT_SOURCE_DIR}/src/video/sdl/*)
endif()
if (NOT ${HARBOR_USE_VIDEO_OPENGL})
  list(FILTER HARBOR_LIB_FILES EXCLUDE REGEX ${CMAKE_CURRENT_SOURCE_DIR}/src/video/gl/*)
endif()

add_library(harbor_lib ${HARBOR_LIB_FILES})
target_include_directories(harbor_lib PUBLIC
                          ${CMAKE_CURRENT_SOURCE_DIR}/src/
                          ${CMAKE_CURRENT_SOURCE_DIR}/polyfill/)
target_compile_definitions(harbor_lib PUBLIC HARBOR_USE_SCRIPTING=$<BOOL:${HARBOR_USE_SCRIPTING}>
                                             HARBOR_USE_SCRIPTING_SQUIRREL=$<BOOL:${HARBOR_USE_SCRIPTING_SQUIRREL}>
                                             HARBOR_USE_SCRIPTING_LUA=$<BOOL:${HARBOR_USE_SCRIPTING_LUA}>
                                             HARBOR_USE_VIDEO=$<BOOL:${HARBOR_USE_VIDEO}>
                                             HARBOR_USE_VIDEO_SDL=$<BOOL:${HARBOR_USE_VIDEO_SDL}>
                                             HARBOR_USE_VIDEO_OPENGL=$<BOOL:${HARBOR_USE_VIDEO_OPENGL}>)

# Main executable
if(HARBOR_BUILD_EXEC)
  add_executable(harbor ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)
  target_include_directories(harbor PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/)
  target_link_libraries(harbor PUBLIC harbor_lib)
  target_compile_definitions(harbor PUBLIC HARBOR_USE_SCRIPTING=$<BOOL:${HARBOR_USE_SCRIPTING}>
                                           HARBOR_USE_SCRIPTING_SQUIRREL=$<BOOL:${HARBOR_USE_SCRIPTING_SQUIRREL}>
                                           HARBOR_USE_SCRIPTING_LUA=$<BOOL:${HARBOR_USE_SCRIPTING_LUA}>
                                           HARBOR_USE_VIDEO=$<BOOL:${HARBOR_USE_VIDEO}>
                                           HARBOR_USE_VIDEO_SDL=$<BOOL:${HARBOR_USE_VIDEO_SDL}>
                                           HARBOR_USE_VIDEO_OPENGL=$<BOOL:${HARBOR_USE_VIDEO_OPENGL}>)
endif(HARBOR_BUILD_EXEC)

# Test executable
if(HARBOR_BUILD_TEST)
  file(GLOB_RECURSE HARBOR_TEST_FILES ${CMAKE_CURRENT_SOURCE_DIR}/tests/*/*.cpp)
  list(SORT HARBOR_TEST_FILES)

  if (NOT ${HARBOR_USE_VIDEO_SDL})
    list(FILTER HARBOR_TEST_FILES EXCLUDE REGEX ${CMAKE_CURRENT_SOURCE_DIR}/tests/video/sdl/*)
  endif()

  if (HARBOR_TEST_FILES)
    add_executable(run_tests ${HARBOR_TEST_FILES})
    target_include_directories(run_tests PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/)
    target_link_libraries(run_tests PUBLIC harbor_lib)
  else()
    set(HARBOR_BUILD_TEST OFF CACHE BOOL "Disabled because no test files are compiled" FORCE)
  endif()
endif(HARBOR_BUILD_TEST)

# ============================================================================
#    Dependencies
# ----------------------------------------------------------------------------

if(HARBOR_BUILD_TEST)
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
endif(HARBOR_BUILD_TEST)

if (HARBOR_USE_SCRIPTING_LUA)
  find_package(Lua)
  if(LUA_FOUND)
    message(STATUS "Using system libraries for LUA: ${LUA_LIBRARIES}")
    target_link_libraries(harbor_lib PUBLIC ${LUA_LIBRARIES})
    target_include_directories(harbor_lib PUBLIC ${LUA_INCLUDE_DIR})
  else(LUA_FOUND)
    message(STATUS "Compiling LUA from source")
    file(GLOB LUA_FILES_C ${CMAKE_CURRENT_SOURCE_DIR}/external/lua/l*.c)
    list(SORT LUA_FILES_C)
    add_library(lua_lib ${LUA_FILES_C})
    add_dependencies(harbor_lib lua_lib)
    target_link_libraries(harbor_lib PUBLIC lua_lib)
    target_include_directories(harbor_lib PUBLIC
                              ${CMAKE_CURRENT_SOURCE_DIR}/external/lua)
    # Fixes link-time "loslib.c:(.text+0x168): warning: the use of `tmpnam' is
    # dangerous, better use `mkstemp'" (by effectively doing said replacement)
    if (UNIX)
      set_target_properties(lua_lib PROPERTIES COMPILE_DEFINITIONS "LUA_USE_POSIX")
    endif (UNIX)
  endif(LUA_FOUND)
endif()

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

find_package(SDL2_image)
if(SDL2_IMAGE_FOUND)
  message(STATUS "Using system libraries for SDL2_image: ${SDL2_IMAGE_LIBRARIES}")
  target_link_libraries(harbor_lib PUBLIC ${SDL2_IMAGE_LIBRARIES})
  target_include_directories(harbor_lib PUBLIC ${SDL2_IMAGE_INCLUDE_DIRS})
else(SDL2_IMAGE_FOUND)
  message(STATUS "Compiling SDL2_image from source")
  add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/SDL2_image)
  target_link_libraries(harbor_lib PUBLIC SDL2_IMAGE)
  add_dependencies(harbor_lib SDL2_IMAGE)
endif(SDL2_IMAGE_FOUND)

find_package(SDL2_ttf)
if(SDL2_TTF_FOUND)
  message(STATUS "Using system libraries for SDL2_ttf: ${SDL2_TTF_LIBRARIES}")
  target_link_libraries(harbor_lib PUBLIC ${SDL2_TTF_LIBRARIES})
  target_include_directories(harbor_lib PUBLIC ${SDL2_TTF_INCLUDE_DIRS})
else(SDL2_TTF_FOUND)
  message(STATUS "Compiling SDL2_ttf from source")
  add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/SDL2_ttf)
  target_link_libraries(harbor_lib PUBLIC SDL2_ttf)
  add_dependencies(harbor_lib SDL2_ttf)
endif(SDL2_TTF_FOUND)

if (HARBOR_USE_SCRIPTING_SQUIRREL)
  # TODO
  #find_package(Squirrel)
  if(SQUIRREL_FOUND)
    message(STATUS "Using system libraries for Squirrel: ${SQUIRREL_LIBRARIES}")
    target_link_libraries(harbor_lib PUBLIC ${SQUIRREL_LIBRARIES})
    target_include_directories(harbor_lib PUBLIC ${SQUIRREL_INCLUDE_DIRS})
  else(SQUIRREL_FOUND)
    message(STATUS "Compiling Squirrel from source")
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/squirrel)
    target_link_libraries(harbor_lib PUBLIC squirrel)
    add_dependencies(harbor_lib squirrel)
  endif(SQUIRREL_FOUND)
endif()

if (HARBOR_USE_VIDEO_OPENGL)
  find_package(OpenGL REQUIRED)
  target_link_libraries(harbor_lib PUBLIC ${OPENGL_LIBRARIES})
endif()
