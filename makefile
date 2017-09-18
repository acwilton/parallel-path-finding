CXX		          = g++
CXXFLAGS          = -Wall
LDLIBS            = -lSDL2 -lSDL2_ttf
LDFLAGS		      =
COMMON_SRCS       = World.cc

TARGETS          += gui
gui_SRCS          = src/gui/parallel-path-finding-gui.cc \
					src/gui/Error.cc \
					src/gui/Window.cc \
					src/gui/Viewport.cc \
					src/gui/Button.cc \
					src/gui/TextInput.cc \
					src/gui/WorldViewport.cc \
					src/gui/GraphicTile.cc

TARGETS          += worldGen
worldGen_SRCS     = src/WorldGen.cc


all: $(TARGETS)

$(TARGETS): $$(patsubst %.cc,%.o,$$($$@_SRCS)) $(patsubst %.cc,%.o,$(COMMON_SRCS))
	$(CXX) $(CXXFLAGS) $$^ -o $$@ $(LDFLAGS) $(LDLIBS)

.PHONY : clean
clean:
	rm $(foreach target,$(TARGET_NAMES),$($(target)_OBJECTS)) $(COMMON_OBJECTS) $(TARGETS) gui.log

.PHONY : clean_worlds
clean_worlds:
	rm worlds/*
