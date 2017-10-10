# Built-in Variables
CXX		              = g++
CXXFLAGS              = -Wall -std=c++14
CPPFLAGS              = -Iincludes
LDLIBS                = -lboost_system -lboost_filesystem
LDFLAGS		          =
SRCSUFFIX          = .cc

# Files that all programs depend on
COMMON_SRCS           = src/common/World.cc

# Files that all path finding algorithms depend on
COMMON_ALG_SRCS       = src/algorithms/tools/PathTile.cc \
                        src/algorithms/tools/PriorityQueue.cc

# Graphical program
TARGETS              += gui
gui_LIBS              = -lSDL2 -lSDL2_ttf
gui_SRCS              = $(COMMON_SRCS) \
                        src/gui/Gui.cc \
                        src/gui/Error.cc \
                        src/gui/Window.cc \
                        src/gui/Viewport.cc \
                        src/gui/Button.cc \
                        src/gui/TextInput.cc \
                        src/gui/WorldViewport.cc \
                        src/gui/GraphicTile.cc
gui_OBJS              = $(patsubst %$(SRCSUFFIX),%.o,$(filter %$(SRCSUFFIX),$(gui_SRCS)))

# World generation program
TARGETS              += worldGen
worldGen_SRCS         = $(COMMON_SRCS) \
                        src/worldGen/WorldGen.cc
worldGen_OBJS         = $(patsubst %$(SRCSUFFIX),%.o,$(filter %$(SRCSUFFIX),$(worldGen_SRCS)))

# Djikstra's algorithm
TARGETS              += dijkstra
dijkstra_SRCS         = $(COMMON_SRCS) \
                        $(COMMON_ALG_SRCS) \
                        src/algorithms/dijkstra/Dijkstra.cc
dijkstra_OBJS         = $(patsubst %$(SRCSUFFIX),%.o,$(filter %$(SRCSUFFIX),$(dijkstra_SRCS))) 

MAKEDEPENDS           = $(CXX) $(CPPFLAGS) $(CXXFLAGS) -MM -MP

SRCS                  = $(foreach target,$(TARGETS),$(sort $($(target)_SRCS)))

.PHONY : all
all: $(TARGETS)

.SECONDEXPANSION:
$(TARGETS): $$($$@_OBJS)
	$(CXX) $(LDFLAGS) $^ -o $@  $(LDLIBS) $($@_LIBS)

#$(foreach target, $(TARGETS), $($(target)_HEADERS)):

-include makefile.deps

.PHONY : clean
clean:
	rm -f $(patsubst %$(SRCSUFFIX),%.o,$(SRCS)) \
	      $(TARGETS) gui.log

.PHONY : clean_worlds
clean_worlds:
	rm -f worlds/*

.PHONY : clean_results
clean_results:
	rm -f -r results/*

.PHONY : makefile.deps
makefile.deps :
	$(MAKEDEPENDS) $(SRCS) > makefile.deps
