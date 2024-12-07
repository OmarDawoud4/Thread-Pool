CC = gcc
CFLAGS = -pthread -g
SRC = src/heap_memory.c src/thread_pool.c main.c
OBJ = $(SRC:.c=.o)
EXEC = thread_pool

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(OBJ) $(EXEC)
