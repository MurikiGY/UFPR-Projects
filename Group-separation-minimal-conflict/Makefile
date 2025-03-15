CC = gcc
CFLAGS = -g -Wall
SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)
TARGET = separa

all: $(TARGET)

$(TARGET): $(OBJ)

debug: CFLAGS += -DDEBUG
debug: all 

run: $(TARGET)
	./$(TARGET) < input3
#	./$(TARGET) -o < input3
#	./$(TARGET) -f < input3
#	./$(TARGET) -f -o < input3

clean:
	-rm -f *.o vgcore* 

purge: clean
	-rm -f $(TARGET)
