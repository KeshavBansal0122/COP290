# General purpose Makefile for C projects

# options
CC       = gcc
LINKER   = $(CC)
CFLAGS   = -Wall -Wextra -O2
LFLAGS   = -ldl -lm -lpthread

# directories
TARGET   = sheet
SRC_DIR   = src
BIN_DIR   = build
OBJ_DIR   = $(BIN_DIR)/.obj

SRC      = $(wildcard $(SRC_DIR)/*.c)
OBJ      = $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

all: build

build: $(BIN_DIR) $(BIN_DIR)/$(TARGET)

$(BIN_DIR):
	mkdir -p $@

# compile
$(OBJ): $(OBJ_DIR)/%.o : $(SRC_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) -c $(CFLAGS) $< -o $@

# link
$(BIN_DIR)/$(TARGET): $(OBJ)
	$(LINKER) $(OBJ) $(LFLAGS) -o $@

run: build
	./$(BIN_DIR)/$(TARGET)

clean:
	rm -f $(OBJ)

remove: clean
	rm -f $(BIN_DIR)/$(TARGET)

test: build
	./$(BIN_DIR)/$(TARGET) -t
.PHONY: build run debug clean remove all