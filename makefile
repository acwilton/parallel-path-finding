# Built-in Variables
CXX		          = g++
CXXFLAGS          = -Wall
CPPFLAGS          = -Iincludes
LDLIBS            = -lSDL2 -lSDL2_ttf
LDFLAGS		      =

# Source files that all programs depend on
COMMON_SRCS       = src/common/World.cc

# Source files that all path finding algorithms use
COMMON_ALG_SRCS   = includes/algorithms/tools/PriorityQueue.hpp \
                     src/algorithms/tools/PathTile.cc


# Graphical program
TARGETS          += gui
gui_SRCS          = src/gui/Gui.cc \
					src/gui/Error.cc \
					src/gui/Window.cc \
					src/gui/Viewport.cc \
					src/gui/Button.cc \
					src/gui/TextInput.cc \
					src/gui/WorldViewport.cc \
					src/gui/GraphicTile.cc

# World generation program
TARGETS          += worldGen
worldGen_SRCS     = src/worldGen/WorldGen.cc

# Djikstra's algorithm
TARGETS          += dijkstra
dijkstra_SRCS     = $(COMMON_ALG_SRCS) \
                    src/algorithms/dijkstra/Dijkstra.cc

all: $(TARGETS)

.SECONDEXPANSION:
$(TARGETS): $$(patsubst %.cc,%.o,$$($$@_SRCS)) $$(patsubst %.cc,%.o,$$(COMMON_SRCS))
	$(CXX) $(LDFLAGS) $^ -o $@  $(LDLIBS)

.PHONY : clean
clean:
	rm -f $(foreach target,$(TARGETS),$(patsubst %.cc,%.o,$($(target)_SRCS))) \
	      $(patsubst %.cc,%.o,$(COMMON_SRCS)) $(TARGETS) gui.log

.PHONY : clean_worlds
clean_worlds:
	rm -f worlds/*
