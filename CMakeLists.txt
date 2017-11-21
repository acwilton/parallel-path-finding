cmake_minimum_required(VERSION 3.3 FATAL_ERROR)
project(PathFinding LANGUAGES CXX)
set(CXX_STANDARD 14)
set(CXX_STANDARD_REQUIRED On)

# Boost (system + filesystem)
#########################################

find_package(Boost REQUIRED
  COMPONENTS "system" "filesystem")

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
  target_link_libraries(${arg_NAME} PUBLIC common ${arg_LIBRARIES})
endmacro(add_custom_executable)

macro(add_algorithm)
  set(options )
  set(singleValueArgs NAME SOURCE)
  set(multiValueArgs INCLUDES LIBRARIES)
  cmake_parse_arguments(arg "${options}" "${singleValueArgs}" "${multiValueArgs}" ${ARGN})
  add_executable(${arg_NAME} ${arg_SOURCE})
  target_include_directories(${arg_NAME} PUBLIC ${DEFAULT_INCLUDE_DIR} ${arg_INCLUDES})
  target_link_libraries(${arg_NAME} PUBLIC algorithm ${arg_LIBRARIES})
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
  src/gui/GraphicTile.cc)

add_custom_executable(
  NAME worldGen
  SOURCES src/worldGen/WorldGen.cc)

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
