CC		  = g++
CFLAGS		  = -c -Wall
LDLIBS            = -lSDL2 -lSDL2_ttf
LDFLAGS		  =
COMMON_SOURCES    = world.cc
COMMON_OBJECTS	  = world.o

GUI_EXEC	  = par-path-finding
GUI_SOURCES       = parallel-path-finding-gui.cc
GUI_OBJECTS       = parallel-path-finding-gui.o

WORLD_GEN_SOURCES = worldGen.cc
WORLD_GEN_OBJECTS = worldGen.o
WORLD_GEN_EXEC    = worldGen

# All of the target executables
TARGETS           = $(foreach target,$(TARGET_NAMES),$($(target)_EXEC))
TARGET_NAMES      = GUI WORLD_GEN

all: $(TARGETS)

define TARGET_template =
$$($(1)_EXEC): $(COMMON_OBJECTS) $$($(1)_OBJECTS)
	$(CC) $(LDFLAGS) $(COMMON_OBJECTS) $$($(1)_OBJECTS) -o $$@ $(LDLIBS)
endef

$(foreach target,$(TARGET_NAMES),$(eval $(call TARGET_template,$(target))))

.cc.o :
	$(CC) $(CFLAGS) $< -o $@


.PHONY : clean
clean:
	rm $(foreach target,$(TARGET_NAMES),$($(target)_OBJECTS)) $(COMMON_OBJECTS) $(TARGETS)
