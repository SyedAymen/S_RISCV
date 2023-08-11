CC = gcc
LD = gcc

CFLAGS = -Wall -pipe
OFLAGS = -I/usr/include
LFLAGS = $(CFLAGS) -L/usr/lib/ -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -latomic
BIN_NAME = sim

SOURCES = $(wildcard src/*.c)
OBJECTS = $(SOURCES:src/%.c=build/%.o)

DEBUG = yes
PROFILE = no
PEDANTIC = no
OPTIMIZATION = -O3

ifeq ($(DEBUG), yes)
	CFLAGS += -g
	OPTIMIZATION = -O0
endif

ifeq ($(PROFILE), yes)
	CFLAGS += -pg
endif

CFLAGS += $(OPTIMIZATION)

all: project
	

project: $(OBJECTS)
	$(CC) $(OBJECTS) $(LFLAGS) -o build/$(BIN_NAME)

project: $(OBJECTS)

build/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf build/*

rebuild: clean all

.PHONY : clean
.SILENT : clean
