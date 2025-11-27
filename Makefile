CC == gcc
CFLAGS = -O2 -Wall -I./src -I./include
SRCS = $(wildcard src/*.c)
OBJS = $(SRCS:.c=.o)
BIN_DIR = bin

all: $(BIN_DIR)/cc0

$(BIN_DIR):
	\tmkdir -p $(BIN_DIR)

# Compile Serrate (Stage 0)
$(BIN_DIR)/cc0: $(SRCS) | $(BIN_DIR)
	\t$(CC) $(CFLAGS) -o $@ $(SRCS)

clean:
	\trm -rf $(BIN_DIR)/*.o $(BIN_DIR)/cc0

.PHONY: all clean