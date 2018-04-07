cmake_minimum_required(VERSION 3.3 FATAL_ERROR)
project(PathFinding LANGUAGES CXX)
set(CXX_STANDARD 14)
set(CXX_STANDARD_REQUIRED On)

if(NOT CMAKE_BUILD_TYPE)
  message(STATUS "Enabled release build")
  set(CMAKE_BUILD_TYPE Release)
endif(NOT CMAKE_BUILD_TYPE)

if (CMAKE_COMPILER_IS_GNUCXX)
  message(STATUS "Enabled aggressive warnings")
  if (GEN_STATS)
    add_definitions (-DGEN_STATS)
  endif(GEN_STATS)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -Wpedantic -pedantic -pedantic-errors -march=native")
endif(CMAKE_COMPILER_IS_GNUCXX)

# Boost (system + filesystem)
#########################################

find_package(Boost REQUIRED
  COMPONENTS "system" "filesystem" "thread")

add_library(boost-system SHARED IMPORTED)
set_property(
  TARGET boost-system
  PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIRS})
set_property(
  TARGET boost-system
  PROPERTY IMPORTED_LOCATION ${Boost_SYSTEM_LIBRARY})

add_library(boost-filesystem SHARED IMPORTED)
set_property(
  TARGET boost-filesystem
  PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIRS})
set_property(
  TARGET boost-filesystem
  PROPERTY IMPORTED_LOCATION ${Boost_FILESYSTEM_LIBRARY})

add_library(boost-thread SHARED IMPORTED)
set_property(
  TARGET boost-thread
  PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIRS})
set_property(
  TARGET boost-thread
  PROPERTY IMPORTED_LOCATION ${Boost_THREAD_LIBRARY})

# SDL2 + SDL2_ttf
#########################################

# Use until SDL2 is added to built-in CMake
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PathFinding_SOURCE_DIR}/cmake")

find_package(SDL2 REQUIRED)
add_library(sdl2 SHARED IMPORTED)
# WKK: Disgusting Hack which should be somehow avoided in the future
#      1). Splice the 1st element out of the array and mark as "primary"
#      2). Mark remaining array elements as dependent libraries
#      3). Cry in a corner
list(GET SDL2_LIBRARY 1 "SDL2_CORE_LIB")
list(REMOVE_AT SDL2_LIBRARY 1)
set_property(TARGET sdl2 PROPERTY IMPORTED_LOCATION "${SDL2_CORE_LIB}")
set_property(TARGET sdl2 PROPERTY INTERFACE_INCLUDE_DIRECTORIES "${SDL2_INCLUDE_DIR}")
set_property(TARGET sdl2 PROPERTY IMPORTED_LINK_DEPENDENT_LIBRARIES "${SDL2_LIBRARY}")

find_package(SDL2_ttf REQUIRED)
add_library(sdl2-ttf SHARED IMPORTED)
set_property(TARGET sdl2-ttf PROPERTY INTERFACE_INCLUDE_DIRECTORIES "${SDL2_TTF_INCLUDE_DIR}")
set_property(TARGET sdl2-ttf PROPERTY IMPORTED_LOCATION "${SDL2_TTF_LIBRARIES}")

# PThreads
#########################################

set(THREADS_PREFER_PTHREAD_FLAG On)
find_package(Threads REQUIRED)

# TBB
########################################

find_package(TBB REQUIRED)

# Common Sources and Includes
#########################################

set(DEFAULT_INCLUDE_DIR includes)

add_library(common
  src/common/World.cc)
target_include_directories(common PUBLIC ${DEFAULT_INCLUDE_DIR})
target_link_libraries(common PUBLIC boost-system boost-filesystem)

add_library(algorithm
  src/algorithms/tools/PathTile.cc
  src/algorithms/tools/PriorityQueue.cc)
target_include_directories(algorithm PUBLIC ${DEFAULT_INCLUDE_DIR})
target_link_libraries(algorithm PUBLIC common)

# Assistive Macros
#########################################

macro(add_custom_executable)
  set(options )
  set(singleValueArgs NAME)
  set(multiValueArgs INCLUDES LIBRARIES SOURCES)
  cmake_parse_arguments(arg "${options}" "${singleValueArgs}" "${multiValueArgs}" ${ARGN})
  add_executable(${arg_NAME} ${arg_SOURCES})
  target_include_directories(${arg_NAME} PUBLIC ${DEFAULT_INCLUDE_DIR} ${arg_INCLUDES})
  target_link_libraries(${arg_NAME} PRIVATE common PUBLIC ${arg_LIBRARIES})
endmacro(add_custom_executable)

macro(add_algorithm)
  set(options )
  set(singleValueArgs NAME SOURCE)
  set(multiValueArgs INCLUDES LIBRARIES)
  cmake_parse_arguments(arg "${options}" "${singleValueArgs}" "${multiValueArgs}" ${ARGN})
  add_executable(${arg_NAME} ${arg_SOURCE})
  target_include_directories(${arg_NAME} PUBLIC ${DEFAULT_INCLUDE_DIR} ${arg_INCLUDES})
  target_link_libraries(${arg_NAME} PRIVATE algorithm PUBLIC ${arg_LIBRARIES})
endmacro(add_algorithm)

# Executables
#########################################

add_custom_executable(
  NAME gui
  LIBRARIES sdl2 sdl2-ttf
  SOURCES
  src/gui/Gui.cc
  src/gui/Error.cc
  src/gui/Window.cc
  src/gui/Viewport.cc
  src/gui/Button.cc
  src/gui/TextInput.cc
  src/gui/WorldViewport.cc
  src/gui/GraphicTile.cc
  src/gui/Text.cc
  src/gui/ToolbarViewport.cc)

add_custom_executable(
  NAME worldGen
  SOURCES src/worldGen/WorldGen.cc)

add_custom_executable(
  NAME pathGen
  SOURCES src/worldGen/PathGen.cc)

add_algorithm(
  NAME dijkstra
  SOURCE src/algorithms/dijkstra/Dijkstra.cc)

add_algorithm(
  NAME aStar
  SOURCE src/algorithms/aStar/AStar.cc)

add_algorithm(
  NAME bidir
  SOURCE src/algorithms/bidirectional/Bidirectional.cc)

add_algorithm(
  NAME parBidir
  SOURCE src/algorithms/parBidirectional/ParBidirectional.cc
  LIBRARIES Threads::Threads)

add_algorithm(
  NAME fringe
  SOURCE src/algorithms/fringe/Fringe.cc
  LIBRARIES Threads::Threads)

add_algorithm(
  NAME parFringe_2
  SOURCE src/algorithms/parFringe/ParFringe.cc
  LIBRARIES Threads::Threads tbb boost-thread)
target_compile_definitions (parFringe_2 PRIVATE NUMBER_OF_THREADS=2)

add_algorithm(
  NAME parFringe_4
  SOURCE src/algorithms/parFringe/ParFringe.cc
  LIBRARIES Threads::Threads tbb boost-thread)
target_compile_definitions (parFringe_4 PRIVATE NUMBER_OF_THREADS=4)

add_algorithm(
  NAME parFringe_6
  SOURCE src/algorithms/parFringe/ParFringe.cc
  LIBRARIES Threads::Threads tbb boost-thread)
target_compile_definitions (parFringe_6 PRIVATE NUMBER_OF_THREADS=6)

add_algorithm(
  NAME parFringe_8
  SOURCE src/algorithms/parFringe/ParFringe.cc
  LIBRARIES Threads::Threads tbb boost-thread)
target_compile_definitions (parFringe_8 PRIVATE NUMBER_OF_THREADS=8)

add_algorithm(
  NAME parFringe_optimal
  SOURCE src/algorithms/parFringe/ParFringe.cc
  LIBRARIES Threads::Threads tbb boost-thread)
target_compile_definitions (parFringe_optimal PRIVATE NUMBER_OF_THREADS=8 OPTIMAL=true)

add_algorithm(
  NAME parFringe_8_0.25
  SOURCE src/algorithms/parFringe/ParFringe.cc
  LIBRARIES Threads::Threads tbb boost-thread)
target_compile_definitions (parFringe_8_0.25 PRIVATE NUMBER_OF_THREADS=8 THRESHOLD_FACTOR=0.25)

add_algorithm(
  NAME parFringe_8_0.50
  SOURCE src/algorithms/parFringe/ParFringe.cc
  LIBRARIES Threads::Threads tbb boost-thread)
target_compile_definitions (parFringe_8_0.50 PRIVATE NUMBER_OF_THREADS=8 THRESHOLD_FACTOR=0.5)

add_algorithm(
  NAME parFringe_8_0.75
  SOURCE src/algorithms/parFringe/ParFringe.cc
  LIBRARIES Threads::Threads tbb boost-thread)
target_compile_definitions (parFringe_8_0.75 PRIVATE NUMBER_OF_THREADS=8 THRESHOLD_FACTOR=0.75)

add_algorithm(
  NAME parDivide_4
  SOURCE src/algorithms/parDivide/ParDivide.cc
  LIBRARIES Threads::Threads tbb boost-thread)
target_compile_definitions (parDivide_4 PRIVATE NUMBER_OF_THREADS=4)

add_algorithm(
  NAME parDivide_6
  SOURCE src/algorithms/parDivide/ParDivide.cc
  LIBRARIES Threads::Threads tbb boost-thread)
target_compile_definitions (parDivide_6 PRIVATE NUMBER_OF_THREADS=6)

add_algorithm(
  NAME parDivide_8
  SOURCE src/algorithms/parDivide/ParDivide.cc
  LIBRARIES Threads::Threads tbb boost-thread)
target_compile_definitions (parDivide_8 PRIVATE NUMBER_OF_THREADS=8)

add_algorithm(
  NAME parDivideUnsmooth_4
  SOURCE src/algorithms/parDivide/parDivideUnsmooth.cc
  LIBRARIES Threads::Threads tbb boost-thread)
target_compile_definitions (parDivideUnsmooth_4 PRIVATE NUMBER_OF_THREADS=4)

add_algorithm(
  NAME parDivideUnsmooth_6
  SOURCE src/algorithms/parDivide/parDivideUnsmooth.cc
  LIBRARIES Threads::Threads tbb boost-thread)
target_compile_definitions (parDivideUnsmooth_6 PRIVATE NUMBER_OF_THREADS=6)

add_algorithm(
  NAME parDivideUnsmooth_8
  SOURCE src/algorithms/parDivide/parDivideUnsmooth.cc
  LIBRARIES Threads::Threads tbb boost-thread)
target_compile_definitions (parDivideUnsmooth_8 PRIVATE NUMBER_OF_THREADS=8)

add_custom_target(clean_results
  COMMAND rm -R -f ${CMAKE_SOURCE_DIR}/results/*)

add_custom_target(clean_worlds
  COMMAND rm -R -f ${CMAKE_SOURCE_DIR}/worlds/*)
