CC = gcc
CFLAGS = -O3 -march=native -flto -Wall -I./src -I./include

SRCS = $(wildcard src/*.c)
OBJS = $(patsubst src/%.c, bin/%.o, $(SRCS))

BIN_DIR = bin
BINARY = serrate

# Default target
all: $(BIN_DIR)/$(BINARY)

# Make sure bin exists (order-only dependency)
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Compile source files to object files
$(BIN_DIR)/%.o: src/%.c | $(BIN_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Link object files into the final binary
$(BIN_DIR)/$(BINARY): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

clean:
	rm -rf $(BIN_DIR)/*.o $(BIN_DIR)/$(BINARY)

.PHONY: all clean
