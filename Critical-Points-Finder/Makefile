CC = gcc
CFLAGS = -g -Wall -O3 -mavx -march=native -I/usr/local/include -DLIKWID_PERFMON
LDFLAGS = -L/usr/local/lib -llikwid -lm
SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)
TARGET = pontosCriticos

all: $(TARGET)

$(TARGET): $(OBJ)

debug: CFLAGS += -DDEBUG
debug: all

run: all
	./$(TARGET) < input

clean:
	-rm -f *.o vgcore* 

purge: clean
	-rm -f $(TARGET)

