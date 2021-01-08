SRCDIR = src
INCDIR = include
CPP = g++

CFLAGS = -O2 -s -w -I.
#CFLAGS = -O0 -g -w -I.
LFLAGS = -lm -lpthread

.SUFFIXES: .o .hpp .cpp

$(SRCDIR)/%.o: $(SRCDIR)/%.cpp
	@$(CPP) $(CFLAGS) -c $< -o $@

SOURCES = $(wildcard $(SRCDIR)/*.cpp)

HEADERS = $(wildcard $(INCDIR)/*.hpp)

OBJECTS = $(SOURCES:.cpp=.o)

all: clean example benchmark clean_objects

example: $(HEADERS) $(SRCDIR)/example.o
	@$(CPP) $(CFLAGS) $< -o $@ $(LFLAGS)
	@echo "example compiled successfully"

benchmark: $(HEADERS) $(SRCDIR)/benchmark.o
	@$(CPP) $(CFLAGS) $< -o $@ $(LFLAGS)
	@echo "benchmark compiled successfully"

clean_objects:
	@rm -f $(OBJECTS)

clean:
	@rm -f $(OBJECTS)
	@rm -f example
	@rm -f benchmark
