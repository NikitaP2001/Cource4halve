
SOURCES = $(wildcard *.cxx)
OBJECTS = $(SOURCES:.cxx=.o)
LDLIBS = -lpthread
CFLAGS = -DDEBUG -std=c++17

%.o: %.cxx
        c++ $(CFLAGS) -g -c $< -o $@

all: $(OBJECTS)
        c++ $(OBJECTS) $(LDLIBS) -o server.elf


clean:
        @-rm $(OBJECTS)
