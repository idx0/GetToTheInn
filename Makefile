CC=gcc
CPP=g++
CFLAGS=-g -Wall
LDFLAGS=
LIBS=-ltcod -ltcodxx -lm
SRC=\
	fov/fov.c \
	main.cpp \
	component.cpp \
	geometry.cpp \
	map.cpp \
	render.cpp \
	engine.cpp
INCLUDES=-I/usr/include/libtcod
OBJ=$(SRC:.cpp=.o)
BIN=gtti

.PHONY: depend clean

all: $(BIN)

$(BIN): $(OBJ)
	$(CPP) $(CFLAGS) $(INCLUDES) $(OBJ) -o $@ $(LDFLAGS) $(LIBS)

.cpp.o:
	$(CPP) $(CFLAGS) $(INCLUDES) -c $< -o $@

.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f *.o *~ $(BIN)

