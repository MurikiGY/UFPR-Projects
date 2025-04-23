CC = gcc
CFLAGS = -Wall -O3
LDLIBS = -lm -lrt
SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)
TARGET = lcs

all: $(TARGET)

$(TARGET): $(OBJ)

debug: CFLAGS += -g -DDEBUG -DEBUGMATRIX 
debug: all

run: all
	./$(TARGET)

clean:
	-rm -f *.o vgcore* 

purge: clean
	-rm -f $(TARGET)

