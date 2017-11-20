cmake_minimum_required(VERSION 3.3 FATAL_ERROR)
project(PathFinding LANGUAGES CXX)
set(CXX_STANDARD 14)
set(CXX_STANDARD_REQUIRED On)

# Boost (system + filesystem)
#########################################

find_package(Boost REQUIRED
  COMPONENTS
  "system"
  "filesystem")

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

# Common Sources
#########################################

set(COMMON_SRCS
  src/common/World.cc)

set(COMMON_ALG_SRCS
  src/algorithms/tools/PathTile.cc
  src/algorithms/tools/PriorityQueue.cc)

# Executables
#########################################

add_executable(gui ${COMMON_SRCS})
target_include_directories(gui PUBLIC includes)
target_link_libraries(gui PUBLIC sdl2 sdl2-ttf boost-system boost-filesystem)
target_sources(gui
  PUBLIC
  src/gui/Gui.cc
  src/gui/Error.cc
  src/gui/Window.cc
  src/gui/Viewport.cc
  src/gui/Button.cc
  src/gui/TextInput.cc
  src/gui/WorldViewport.cc
  src/gui/GraphicTile.cc)

add_executable(worldGen ${COMMON_SRCS})
target_include_directories(worldGen PUBLIC includes)
target_link_libraries(worldGen PUBLIC boost-system boost-filesystem)
target_sources(worldGen PUBLIC src/worldGen/WorldGen.cc)

add_executable(dijkstra ${COMMON_SRCS} ${COMMON_ALG_SRCS})
target_include_directories(dijkstra PUBLIC includes)
target_link_libraries(dijkstra PUBLIC boost-system boost-filesystem)
target_sources(dijkstra PUBLIC src/algorithms/dijkstra/Dijkstra.cc)

add_executable(aStar ${COMMON_SRCS} ${COMMON_ALG_SRCS})
target_include_directories(aStar PUBLIC includes)
target_link_libraries(aStar PUBLIC boost-system boost-filesystem)
target_sources(aStar PUBLIC src/algorithms/aStar/AStar.cc)

add_executable(bidir ${COMMON_SRCS} ${COMMON_ALG_SRCS})
target_include_directories(bidir PUBLIC includes)
target_link_libraries(bidir PUBLIC boost-system boost-filesystem)
target_sources(bidir PUBLIC src/algorithms/bidirectional/Bidirectional.cc)

add_executable(parBidir ${COMMON_SRCS} ${COMMON_ALG_SRCS})
target_include_directories(parBidir PUBLIC includes)
target_link_libraries(parBidir Threads::Threads boost-system boost-filesystem)
target_sources(parBidir PUBLIC src/algorithms/parBidirectional/ParBidirectional.cc)

add_executable(fringe ${COMMON_SRCS} ${COMMON_ALG_SRCS})
target_include_directories(fringe PUBLIC includes)
target_link_libraries(fringe PUBLIC Threads::Threads boost-system boost-filesystem)
target_sources(fringe PUBLIC src/algorithms/fringe/Fringe.cc)
