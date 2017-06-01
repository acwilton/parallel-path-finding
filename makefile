CC		= g++
CFLAGS		= -c -Wall
LDFLAGS		=
SOURCES		= worldGen.cc world.cc
OBJECTS		= $(SOURCES:*.c=.o)
EXECUTABLE	= worldGen

all: $(EXECUTABLE)

$(EXECUTABLE) : $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cc.o :
	$(CC) $(CFLAGS) $< -o $@


.PHONY : clean
clean:
	rm (wildcard:*o) worldGen
