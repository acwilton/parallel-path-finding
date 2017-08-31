CC		          = g++
CFLAGS		      = -c -Wall
LDLIBS            = -lSDL2 -lSDL2_ttf
LDFLAGS		      =
COMMON_SOURCES    = World.cc
COMMON_OBJECTS	  = World.o

GUI_EXEC	      = pathFind
GUI_SOURCES       = parallel-path-finding-gui.cc

WORLD_GEN_EXEC    = WorldGen
WORLD_GEN_SOURCES = WorldGen.cc

# All of the target executables
TARGETS           = $(foreach target,$(TARGET_NAMES),$($(target)_EXEC))
TARGET_NAMES      = GUI WORLD_GEN

all: $(TARGETS)

define TARGET_template =
$(1)_OBJECTS += $$(patsubst %.cc,%.o,$$($(1)_SOURCES))
$$($(1)_EXEC): $(COMMON_OBJECTS) $$($(1)_OBJECTS)
	$(CC) $(LDFLAGS) $$^ -o $$@ $(LDLIBS)
endef

$(foreach target,$(TARGET_NAMES),$(eval $(call TARGET_template,$(target))))

.PHONY : clean
clean:
	rm $(foreach target,$(TARGET_NAMES),$($(target)_OBJECTS)) $(COMMON_OBJECTS) $(TARGETS)

.PHONY : clean_worlds
clean_worlds:
	rm worlds/*
