# Built-in Variables
CXX		              = g++
CXXFLAGS              = -Wall -std=c++14
CPPFLAGS              = -Iincludes
LDLIBS                = -lboost_system -lboost_filesystem
LDFLAGS		          =

# Files that all programs depend on
COMMON_SRCS           = src/common/World.cc
COMMON_HEADERS        = includes/common/World.h \
                        includes/common/Point.h \
                        includes/common/Results.h

# Files that all path finding algorithms depend on
COMMON_ALG_SRCS       = src/algorithms/tools/PathTile.cc \
                        src/algorithms/tools/PriorityQueue.cc
COMMON_ALG_HEADERS    = includes/algorithms/tools/PriorityQueue.h \
                        includes/algorithms/tools/PathTile.h


# Graphical program
TARGETS              += gui
gui_LIBS              = -lSDL2 -lSDL2_ttf
gui_HEADERS           = $(COMMON_HEADERS) \
                        includes/gui/Error.h \
                        includes/gui/Window.h \
                        includes/gui/Viewport.h \
                        includes/gui/Button.h \
                        includes/gui/TextInput.h \
                        includes/gui/WorldViewport.h \
                        includes/gui/GraphicTile.h
gui_SRCS              = $(COMMON_SRCS) \
                        src/gui/Gui.cc \
                        src/gui/Error.cc \
                        src/gui/Window.cc \
                        src/gui/Viewport.cc \
                        src/gui/Button.cc \
                        src/gui/TextInput.cc \
                        src/gui/WorldViewport.cc \
                        src/gui/GraphicTile.cc
gui_OBJS              = $(patsubst %.cc,%.o,$(filter %.cc,$(gui_SRCS)))

# World generation program
TARGETS              += worldGen
worldGen_HEADERS      = $(COMMON_HEADERS)
worldGen_SRCS         = $(COMMON_SRCS) \
                        src/worldGen/WorldGen.cc
worldGen_OBJS         = $(patsubst %.cc,%.o,$(filter %.cc,$(worldGen_SRCS)))

# Djikstra's algorithm
TARGETS              += dijkstra
dijkstra_HEADERS      = $(COMMON_HEADERS) \
                        $(COMMON_ALG_HEADERS)
dijkstra_SRCS         = $(COMMON_SRCS) \
                        $(COMMON_ALG_SRCS) \
                        src/algorithms/dijkstra/Dijkstra.cc
dijkstra_OBJS         = $(patsubst %.cc,%.o,$(filter %.cc,$(dijkstra_SRCS))) 


.PHONY : all
all: $(TARGETS)

.SECONDEXPANSION:
$(TARGETS): $$($$@_OBJS) $$($$@_HEADERS)
	$(CXX) $(LDFLAGS) $($@_OBJS) -o $@  $(LDLIBS) $($@_LIBS)

$(foreach target, $(TARGETS), $($(target)_HEADERS)):

.PHONY : clean
clean:
	rm -f $(foreach target,$(TARGETS),$(sort $($(target)_OBJS))) \
	      $(TARGETS) gui.log

.PHONY : clean_worlds
clean_worlds:
	rm -f worlds/*

.PHONY : clean_results
clean_results:
	rm -f -r results/*
