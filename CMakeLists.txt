project(harbor)

cmake_minimum_required(VERSION 3.0)

file(GLOB_RECURSE HARBOR_LIB_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*/*.cpp)
list(SORT HARBOR_LIB_FILES)
add_library(harbor_lib ${HARBOR_LIB_FILES})
target_include_directories(harbor_lib PUBLIC src/)

add_executable(harbor src/main.cpp)
target_include_directories(harbor PUBLIC src/)
target_link_libraries(harbor PUBLIC harbor_lib)

find_package(SDL2)
if(SDL2_FOUND)
  message(STATUS "Using system libraries for SDL2: ${SDL2_LIBRARIES}")
  target_link_libraries(harbor_lib ${SDL2_LIBRARIES})
  target_include_directories(harbor_lib PUBLIC ${SDL2_INCLUDE_DIRS})
else(SDL2_FOUND)
  message(STATUS "Compiling SDL2 from source")
  add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/SDL2)
  target_link_libraries(harbor_lib SDL2)
  add_dependencies(harbor_lib SDL2)
endif(SDL2_FOUND)
