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

cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/mk/cmake/")

if(NOT BUILD_SHARED_LIBS)
  set(CMAKE_POSITION_INDEPENDENT_CODE ON CACHE BOOL "" FORCE)
endif(NOT BUILD_SHARED_LIBS)

cmake_policy(SET CMP0004 OLD)

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
if(NOT HARBOR_USE_SCRIPTING)
  set(HARBOR_USE_SCRIPTING_SQUIRREL OFF CACHE BOOL "Disabled because HARBOR_USE_SCRIPTING is off" FORCE)
  set(HARBOR_USE_SCRIPTING_LUA OFF CACHE BOOL "Disabled because HARBOR_USE_SCRIPTING is off" FORCE)
endif()
if(NOT HARBOR_USE_VIDEO)
  set(HARBOR_USE_VIDEO_SDL OFF CACHE BOOL "Disabled because HARBOR_USE_VIDEO is off" FORCE)
  set(HARBOR_USE_VIDEO_OPENGL OFF CACHE BOOL "Disabled because HARBOR_USE_VIDEO is off" FORCE)
endif()

# Code quality options
option(WARNINGS "Enable compilation warnings" OFF)
option(WERROR "Treat all warnings as errors" OFF)
option(HARBOR_COVERAGE "Prepare coverage (Debug builds with GCC only)" ON)

# ============================================================================
#    Library and executable generation
# ----------------------------------------------------------------------------

# Main library
file(GLOB_RECURSE HARBOR_LIB_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*/*.cpp)
list(SORT HARBOR_LIB_FILES)

macro(clear_by_option list_name option_name file_regex)
  if(NOT ${option_name})
    #list(FILTER...) requires CMake >3.6, polyfill allows keping version small
    #list(FILTER ${list_name} EXCLUDE REGEX ${file_regex})
    file(GLOB_RECURSE DELETE_ENTRIES ${file_regex})
    if(NOT "${DELETE_ENTRIES}" STREQUAL "")
      list(REMOVE_ITEM ${list_name} ${DELETE_ENTRIES})
    endif()
  endif()
endmacro()

clear_by_option(HARBOR_LIB_FILES HARBOR_USE_SCRIPTING ${CMAKE_CURRENT_SOURCE_DIR}/src/scripting/*)
clear_by_option(HARBOR_LIB_FILES HARBOR_USE_SCRIPTING_SQUIRREL ${CMAKE_CURRENT_SOURCE_DIR}/src/scripting/squirrel/*)
clear_by_option(HARBOR_LIB_FILES HARBOR_USE_SCRIPTING_LUA ${CMAKE_CURRENT_SOURCE_DIR}/src/scripting/lua/*)
clear_by_option(HARBOR_LIB_FILES HARBOR_USE_VIDEO ${CMAKE_CURRENT_SOURCE_DIR}/src/video/*)
clear_by_option(HARBOR_LIB_FILES HARBOR_USE_VIDEO_SDL ${CMAKE_CURRENT_SOURCE_DIR}/src/video/sdl/*)
clear_by_option(HARBOR_LIB_FILES HARBOR_USE_VIDEO_OPENGL ${CMAKE_CURRENT_SOURCE_DIR}/src/video/gl/*)

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

  clear_by_option(HARBOR_TEST_FILES HARBOR_USE_SCRIPTING ${CMAKE_CURRENT_SOURCE_DIR}/tests/scripting/*)
  clear_by_option(HARBOR_TEST_FILES HARBOR_USE_SCRIPTING_SQUIRREL ${CMAKE_CURRENT_SOURCE_DIR}/tests/scripting/squirrel/*)
  clear_by_option(HARBOR_TEST_FILES HARBOR_USE_SCRIPTING_LUA ${CMAKE_CURRENT_SOURCE_DIR}/tests/scripting/lua/*)
  clear_by_option(HARBOR_TEST_FILES HARBOR_USE_VIDEO ${CMAKE_CURRENT_SOURCE_DIR}/tests/video/*)
  clear_by_option(HARBOR_TEST_FILES HARBOR_USE_VIDEO_SDL ${CMAKE_CURRENT_SOURCE_DIR}/tests/video/sdl/*)
  clear_by_option(HARBOR_TEST_FILES HARBOR_USE_VIDEO_OPENGL ${CMAKE_CURRENT_SOURCE_DIR}/tests/video/gl/*)

  if(HARBOR_TEST_FILES)
    add_executable(run_tests ${HARBOR_TEST_FILES})
    target_include_directories(run_tests PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/)
    target_link_libraries(run_tests PUBLIC harbor_lib)

    # Coverage
    if(CMAKE_COMPILER_IS_GNUCXX AND HARBOR_COVERAGE)
      target_compile_options(harbor_lib PUBLIC -fprofile-arcs -ftest-coverage)
      target_link_options(harbor_lib PUBLIC -lgcov --coverage)
    endif()
  else()
    set(HARBOR_BUILD_TEST OFF CACHE BOOL "Disabled because no test files are compiled" FORCE)
  endif()
endif(HARBOR_BUILD_TEST)

# ============================================================================
#    Platform- and option-specific settings
# ----------------------------------------------------------------------------

if(EMSCRIPTEN)
  set(CMAKE_EXECUTABLE_SUFFIX .html)
  if(CMAKE_BUILD_TYPE MATCHES "Debug" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo" OR CMAKE_BUILD_TYPE STREQUAL "")
    target_link_options(harbor_lib PUBLIC -sNO_DISABLE_EXCEPTION_CATCHING
                                          -sLLD_REPORT_UNDEFINED
                                          -sERROR_ON_UNDEFINED_SYMBOLS=0)
  endif()
  target_link_options(harbor_lib PUBLIC --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/data
                                        -sALLOW_MEMORY_GROWTH=1)
  target_compile_definitions(harbor_lib PUBLIC DATA_ROOT="${CMAKE_CURRENT_SOURCE_DIR}/data")
  if(HARBOR_BUILD_EXEC)
    target_compile_definitions(harbor PUBLIC DATA_ROOT="${CMAKE_CURRENT_SOURCE_DIR}/data")
  endif(HARBOR_BUILD_EXEC)
  if(HARBOR_BUILD_TEST)
    target_compile_definitions(run_tests PUBLIC DATA_ROOT="${CMAKE_CURRENT_SOURCE_DIR}/data")
  endif(HARBOR_BUILD_TEST)
endif(EMSCRIPTEN)

if(WARNINGS)
  set(HARBOR_WARNING_FLAGS -Wall -Wextra -pedantic)
  target_compile_options(harbor_lib PUBLIC ${HARBOR_WARNING_FLAGS})
  if(HARBOR_BUILD_EXEC)
    target_compile_options(harbor PUBLIC ${HARBOR_WARNING_FLAGS})
  endif(HARBOR_BUILD_EXEC)
  if(HARBOR_BUILD_TEST)
    target_compile_options(run_tests PUBLIC ${HARBOR_WARNING_FLAGS})
  endif(HARBOR_BUILD_TEST)
endif(WARNINGS)

if(WERROR)
  set(HARBOR_WERROR_FLAGS -Werror)
  target_compile_options(harbor_lib PUBLIC ${HARBOR_WERROR_FLAGS})
  if(HARBOR_BUILD_EXEC)
    target_compile_options(harbor PUBLIC ${HARBOR_WERROR_FLAGS})
  endif(HARBOR_BUILD_EXEC)
  if(HARBOR_BUILD_TEST)
    target_compile_options(run_tests PUBLIC ${HARBOR_WERROR_FLAGS})
  endif(HARBOR_BUILD_TEST)
endif(WERROR)

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

if(HARBOR_USE_SCRIPTING_LUA)
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

if(EMSCRIPTEN)
  target_compile_options(harbor_lib PUBLIC -sUSE_SDL=2)
  target_link_options(harbor_lib PUBLIC -sUSE_SDL=2)
else(EMSCRIPTEN)
  find_package(SDL2)
  if(SDL2_FOUND)
    message(STATUS "Using system libraries for SDL2: ${SDL2_LIBRARIES}")
    if(VCPKG_TARGET_TRIPLET)
      if(BUILD_SHARED_LIBS)
        target_link_libraries(harbor_lib PUBLIC SDL2::SDL2)
      else(BUILD_SHARED_LIBS)
        target_link_libraries(harbor_lib PUBLIC SDL2::SDL2-static)
      endif(BUILD_SHARED_LIBS)
      if(WIN32)
        target_link_libraries(harbor PUBLIC -lmingw32 SDL2::SDL2main)
      endif(WIN32)
    else(VCPKG_TARGET_TRIPLET)
      target_link_libraries(harbor_lib PUBLIC ${SDL2_LIBRARIES})
      target_include_directories(harbor_lib PUBLIC ${SDL2_INCLUDE_DIRS})
      if(WIN32)
        target_link_libraries(harbor_lib PUBLIC ${SDL2MAIN_LIBRARIES})
      endif(WIN32)
    endif(VCPKG_TARGET_TRIPLET)
  else(SDL2_FOUND)
    message(STATUS "Compiling SDL2 from source")
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/SDL2)
    if(BUILD_SHARED_LIBS)
      target_link_libraries(harbor_lib PUBLIC SDL2)
      add_dependencies(harbor_lib SDL2)
    else(BUILD_SHARED_LIBS)
      target_link_libraries(harbor_lib PUBLIC SDL2-static)
      add_dependencies(harbor_lib SDL2-static)
    endif(BUILD_SHARED_LIBS)
    if(WIN32)
      target_link_libraries(harbor_lib PUBLIC SDL2main)
      add_dependencies(harbor_lib SDL2main)
    endif(WIN32)
  endif(SDL2_FOUND)
endif(EMSCRIPTEN)

if(HARBOR_USE_VIDEO)
  if(EMSCRIPTEN)
    target_compile_options(harbor_lib PUBLIC -sUSE_SDL_IMAGE=2 -sSDL2_IMAGE_FORMATS='["png"]' -sUSE_SDL_TTF=2)
    target_link_options(harbor_lib PUBLIC -sUSE_SDL_IMAGE=2 -sSDL2_IMAGE_FORMATS='["png"]' -sUSE_SDL_TTF=2)
  else(EMSCRIPTEN)
    find_package(SDL2_image)
    if(SDL2_IMAGE_FOUND)
      message(STATUS "Using system libraries for SDL2_image: ${SDL2_IMAGE_LIBRARIES}")
      target_link_libraries(harbor_lib PUBLIC ${SDL2_IMAGE_LIBRARIES})
      target_include_directories(harbor_lib PUBLIC ${SDL2_IMAGE_INCLUDE_DIRS})
    else(SDL2_IMAGE_FOUND)
      message(STATUS "Compiling SDL2_image from source")
      add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/SDL2_image)
      target_link_libraries(harbor_lib PUBLIC SDL2_image)
      add_dependencies(harbor_lib SDL2_image)
    endif(SDL2_IMAGE_FOUND)

    if(VCPKG_TARGET_TRIPLET)
      find_package(sdl2-ttf CONFIG)
    else(VCPKG_TARGET_TRIPLET)
      find_package(SDL2_ttf)
    endif(VCPKG_TARGET_TRIPLET)
    if(SDL2_TTF_FOUND OR sdl2-ttf_FOUND)
      message(STATUS "Using system libraries for SDL2_ttf: ${SDL2_TTF_LIBRARIES}")
      if(VCPKG_TARGET_TRIPLET)
        target_link_libraries(harbor_lib PUBLIC SDL2::SDL2_ttf)
        find_package(libpng CONFIG REQUIRED)
      else(VCPKG_TARGET_TRIPLET)
        target_link_libraries(harbor_lib PUBLIC ${SDL2_TTF_LIBRARIES})
        target_include_directories(harbor_lib PUBLIC ${SDL2_TTF_INCLUDE_DIRS})
      endif(VCPKG_TARGET_TRIPLET)
    else(SDL2_TTF_FOUND)
      message(STATUS "Compiling SDL2_ttf from source")

      if(TARGET SDL2 AND NOT TARGET SDL2::SDL2)
        add_library(SDL2::SDL2 ALIAS SDL2)
      endif(TARGET SDL2 AND NOT TARGET SDL2::SDL2)

      if(TARGET SDL2-static AND NOT TARGET SDL2::SDL2-static)
        add_library(SDL2::SDL2-static ALIAS SDL2-static)
      endif(TARGET SDL2-static AND NOT TARGET SDL2::SDL2-static)

      # Not required here, but we need to know whether to compile it or not
      find_package(Freetype)
      if(NOT FREETYPE_FOUND)
        add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/freetype)
        add_library(Freetype::Freetype ALIAS freetype)
      endif(NOT FREETYPE_FOUND)

      add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/SDL2_ttf)
      target_link_libraries(harbor_lib PUBLIC SDL2_ttf)
      target_include_directories(harbor_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/external/SDL2_ttf)
      add_dependencies(harbor_lib SDL2_ttf)
    endif(SDL2_TTF_FOUND OR sdl2-ttf_FOUND)
  endif(EMSCRIPTEN)
endif(HARBOR_USE_VIDEO)

if(HARBOR_USE_SCRIPTING_SQUIRREL)
  # TODO
  #find_package(Squirrel)
  if(SQUIRREL_FOUND)
    message(STATUS "Using system libraries for Squirrel: ${SQUIRREL_LIBRARIES}")
    target_link_libraries(harbor_lib PUBLIC ${SQUIRREL_LIBRARIES})
    target_include_directories(harbor_lib PUBLIC ${SQUIRREL_INCLUDE_DIRS})
  else(SQUIRREL_FOUND)
    message(STATUS "Compiling Squirrel from source")
    #set(SQ_DISABLE_INSTALLER ON CACHE BOOL "" FORCE)    # <- Buggy
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/squirrel)
    target_link_libraries(harbor_lib PUBLIC squirrel)
    add_dependencies(harbor_lib squirrel)
  endif(SQUIRREL_FOUND)
endif(HARBOR_USE_SCRIPTING_SQUIRREL)

if(EMSCRIPTEN)
  target_link_options(harbor_lib PUBLIC -sFULL_ES2=1)
else(EMSCRIPTEN)
  if(HARBOR_USE_VIDEO_OPENGL)
    find_package(OpenGL REQUIRED)
    target_link_libraries(harbor_lib PUBLIC ${OPENGL_LIBRARIES})
  endif()
endif(EMSCRIPTEN)

find_package(SDL2_mixer REQUIRED)
target_link_libraries(harbor_lib PUBLIC ${SDL2_MIXER_LIBRARY})
target_include_directories(harbor_lib PUBLIC ${SDL2_MIXER_INCLUDE_DIR})

# ============================================================================
#    Installing
# ----------------------------------------------------------------------------

# Dynamic
# cmake -DCMAKE_SYSTEM_NAME=Windows -DCMAKE_C_COMPILER=x86_64-w64-mingw32-gcc -DCMAKE_CXX_COMPILER=x86_64-w64-mingw32-g++ -DCMAKE_TOOLCHAIN_FILE=~/vcpkg/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-mingw-dynamic -DBUILD_SHARED_LIBS=ON ..

# Static
# cmake -DCMAKE_SYSTEM_NAME=Windows -DCMAKE_C_COMPILER=x86_64-w64-mingw32-gcc -DCMAKE_CXX_COMPILER=x86_64-w64-mingw32-g++ -DCMAKE_TOOLCHAIN_FILE=~/vcpkg/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-mingw-static ..

if(WIN32 OR UBUNTU_TOUCH)
  install(TARGETS harbor_lib DESTINATION ".")
  install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/data DESTINATION ".")
  if(HARBOR_BUILD_EXEC)
    install(TARGETS harbor DESTINATION ".")
  endif(HARBOR_BUILD_EXEC)
  if(HARBOR_BUILD_TEST)
    install(TARGETS run_tests DESTINATION ".")
  endif(HARBOR_BUILD_TEST)
else(WIN32 OR UBUNTU_TOUCH)
  install(TARGETS harbor_lib DESTINATION "lib")
  if(HARBOR_BUILD_EXEC)
    install(TARGETS harbor DESTINATION "bin")
  endif(HARBOR_BUILD_EXEC)
  if(HARBOR_BUILD_TEST)
    install(TARGETS run_tests DESTINATION "bin")
  endif(HARBOR_BUILD_TEST)
endif(WIN32 OR UBUNTU_TOUCH)

if(UBUNTU_TOUCH)
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/mk/clickable/manifest.json.in ${CMAKE_CURRENT_BINARY_DIR}/install/manifest.json)
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/mk/clickable/harbor.desktop.in "install/harbor.desktop")
  install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/mk/clickable/harbor.apparmor DESTINATION ".")
  target_compile_definitions(harbor_lib PUBLIC DATA_ROOT="./data")
  target_compile_definitions(harbor PUBLIC DATA_ROOT="./data")
endif(UBUNTU_TOUCH)

# ============================================================================
#    Packaging
# ----------------------------------------------------------------------------

include(CPack)
