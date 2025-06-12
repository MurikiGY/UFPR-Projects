CC = mpicc
CFLAGS = -Wall -O3
LDLIBS = -lm
SRC = $(wildcard ./src/*.c)
OBJ = $(SRC:.c=.o)
TARGET = main_mpi

all: $(TARGET)

$(TARGET): $(OBJ)

debug: CFLAGS += -g -DDEBUGMATRIX
debug: all

run: all
	./$(TARGET)

clean:
	-rm -f $(OBJ) vgcore*

purge: clean
	-rm -f $(TARGET)
