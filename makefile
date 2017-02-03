CC=gcc
EXE = output
FLAGS = -w -Wall, -std=c89
SOURCES=$(wildcard *.c)
OBJECTS=$(SOURCES:.c=.o)
# OBJECTS = finddupl.o  mentry.o  mlistLL.o

all: $(OBJECTS)
	$(CC) -o $(EXE) $^

all1: $(OBJECTS)
	$(CC) -g -o $(EXE) $^


%.o: %c %.h
	$(CC) -c $<

.PHONY: clean
clean:
	rm *.o
	rm $(EXE)
