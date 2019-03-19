# compiler
CC := g++

# compiler flags
CFLAGS := `sdl2-config --libs --cflags` -ggdb3 -O0 -Wall -lSDL2_image -lSDL2_ttf -lm

# header files
HDRS := Game.hpp

# source files
SRCS := Game.cpp main.cpp

# names of object files
OBJS := $(SRCS:.cpp=.o)

# name of executable
EXEC := flappy_birb

# default recipe
all: $(EXEC)

# recipe for building the final executable
$(EXEC): $(OBJS) $(HDRS) Makefile
	$(CC) -o $@ $(OBJS) $(CFLAGS)

# recipe to clean the workspace
clean:
	rm -f $(EXEC) $(OBJS)

.PHONY: all clean
