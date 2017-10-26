# Built-in Variables
CXX		              = g++
CXXFLAGS              = -Wall -g -std=c++14
CPPFLAGS              = -Iincludes -MMD -MP
LDLIBS                = -lboost_system -lboost_filesystem
LDFLAGS		          =

# Helpful Variables
SRCSUFFIX             = cc
SRCFOLDER             = src
OBJFOLDER             = build
MKDIR_P               = mkdir -p

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
gui_OBJS              = $(gui_SRCS:$(SRCFOLDER)/%.$(SRCSUFFIX)=$(OBJFOLDER)/%.o)

# World generation program
TARGETS              += worldGen
worldGen_SRCS         = $(COMMON_SRCS) \
                        src/worldGen/WorldGen.cc
worldGen_OBJS         = $(worldGen_SRCS:$(SRCFOLDER)/%.$(SRCSUFFIX)=$(OBJFOLDER)/%.o)

# Djikstra's algorithm
TARGETS              += dijkstra
dijkstra_SRCS         = $(COMMON_SRCS) \
                        $(COMMON_ALG_SRCS) \
                        src/algorithms/dijkstra/Dijkstra.cc
dijkstra_OBJS         = $(dijkstra_SRCS:$(SRCFOLDER)/%.$(SRCSUFFIX)=$(OBJFOLDER)/%.o)

TARGETS              += aStar
aStar_SRCS            = $(COMMON_SRCS) \
                        $(COMMON_ALG_SRCS) \
                        src/algorithms/aStar/AStar.cc
aStar_OBJS            = $(aStar_SRCS:$(SRCFOLDER)/%.$(SRCSUFFIX)=$(OBJFOLDER)/%.o)

TARGETS              += bidir
bidir_SRCS            = $(COMMON_SRCS) \
                        $(COMMON_ALG_SRCS) \
                        src/algorithms/bidirectional/Bidirectional.cc
bidir_OBJS            = $(bidir_SRCS:$(SRCFOLDER)/%.$(SRCSUFFIX)=$(OBJFOLDER)/%.o)

OBJS                  = $(sort $(foreach target,$(TARGETS),$($(target)_OBJS)))
DEPS                  = $(OBJS:.o=.d)

.PHONY : all
all: $(TARGETS)

.SECONDEXPANSION:
$(TARGETS): $$($$@_OBJS)
	$(CXX) $(LDFLAGS) $^ -o $@  $(LDLIBS) $($@_LIBS)

$(OBJFOLDER)/%.o : $(SRCFOLDER)/%.$(SRCSUFFIX)
	@$(MKDIR_P) $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

-include $(DEPS)

# Make cleanup targets
.PHONY : clean
clean:
	rm -f -r $(OBJFOLDER) $(TARGETS) gui.log

.PHONY : clean_worlds
clean_worlds:
	rm -f worlds/*

.PHONY : clean_results
clean_results:
	rm -f -r results/*

# Some help taken from https://spin.atomicobject.com/2016/08/26/makefile-c-projects/