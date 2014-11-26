CC=gcc
CPP=g++
CFLAGS=-g -Wall -Wno-unused-function
LDFLAGS=
LIBS=-ltcod -ltcodxx -lnoise -lm
SRC=\
   animation.cpp \
   color.cpp \
   context.cpp \
   delay.cpp \
   effects.cpp \
   engine.cpp \
   fov/fov.c \
   geometry.cpp \
   lighting.cpp \
   main.cpp \
   map.cpp \
   object.cpp \
   pathfinding.cpp \
   player.cpp \
   render.cpp \
   sys/enumstr.cpp \
   sys/eof_parser.cpp \
   sys/event.cpp \
   sys/eventqueue.cpp \
   sys/gamedata.cpp \
   sys/hash.c \
   sys/listener.cpp \
   sys/logger.cpp \
   sys/thread.cpp \
   sys/token.cpp \
   sys/worker.cpp \
   ui/uiframe.cpp \
   ui/uilabel.cpp \
   ui/uilayout.cpp \
   ui/uimenu.cpp \
   ui/uimenuitem.cpp \
   ui/uipanel.cpp \
   ui/uithread.cpp \
   ui/uiwidget.cpp \
   viewport.cpp
INCLUDES=-I. -I./sys -I./ui
OBJ=$(SRC:.cpp=.o)
BIN=gtti

.PHONY: depend clean

all: $(BIN)

$(BIN): $(OBJ)
	$(CPP) $(CFLAGS) $(INCLUDES) $(OBJ) -o $@ $(LDFLAGS) $(LIBS)

.cpp.o:
	$(CPP) $(CFLAGS) -std=c++11 $(INCLUDES) -c $< -o $@

.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f *.o ui/*.o sys/*.o *~ $(BIN)

