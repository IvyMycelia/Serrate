CC = gcc
CFLAGS = -O2 -Wall -I./src -I./include
SRCS = $(wildcard src/*.c)
OBJS = $(patsubst src/%.c, bin/%.o, $(SRCS))

BIN_DIR = bin

all: $(BIN_DIR)/cc0

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Compile Serrate (Stage 0)
$(BIN_DIR)/cc0: $(SRCS) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $(SRCS)

clean:
	rm -rf $(BIN_DIR)/*.o $(BIN_DIR)/cc0

.PHONY: all clean