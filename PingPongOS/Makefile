CC = gcc
CFLAGS = -g -Wall
LDLIBS = -lm -lrt
SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)
TARGET = pingpong-disco2

all: $(TARGET)

$(TARGET): $(OBJ)

debug: CFLAGS += -DDEBUG -g
debug: all

run: all
	./$(TARGET)

clean:
	-rm -f *.o vgcore* 

purge: clean
	-rm -f $(TARGET)

