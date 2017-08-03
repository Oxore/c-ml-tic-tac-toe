CC=gcc
FILES:=$(wildcard *.c)
FILES:=$(FILES:.c=.o)

CFLAGS += -Wall
CFLAGS += -std=c99
CFLAGS += -O3
CFLAGS += -g
#LIBS += -lcsfml-graphics 
#LIBS += -lcsfml-window 
#LIBS += -lcsfml-system 

all: main

main: $(FILES) 
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

clean:
	rm -f main $(FILES)
