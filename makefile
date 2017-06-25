CC		= g++
CFLAGS		= -c -Wall
LDLIBS          = -lSDL2 -lSDL2_ttf
LDFLAGS		=
SOURCES		= world.cc parallel-path-finding-gui.cc
OBJECTS		= $(SOURCES:*.c=.o)
EXECUTABLE	= par-path-finding

all: $(EXECUTABLE)

$(EXECUTABLE) : $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@ $(LDLIBS)

.cc.o :
	$(CC) $(CFLAGS) $< -o $@


.PHONY : clean
clean:
	rm (wildcard:*o) worldGen
