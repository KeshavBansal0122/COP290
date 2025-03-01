# Example Makefile content:
CC = gcc
CFLAGS = -Wall -Wextra -O3 -flto
#CFLAGS =
LDFLAGS = -lm -flto
TEST_CFLAGS = $(CFLAGS) -I./tests

SRC_DIR = src
TEST_DIR = tests
BUILD_DIR = target
RELEASE_DIR = target/release
TEST_BUILD_DIR = target/tests

SRCS = $(wildcard $(SRC_DIR)/*.c)
TEST_SRCS = $(wildcard $(TEST_DIR)/*.c)
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
TEST_OBJS = $(TEST_SRCS:$(TEST_DIR)/%.c=$(TEST_BUILD_DIR)/%.o)

# Exclude main.c from test build
MAIN_OBJ = $(BUILD_DIR)/main.o
LIB_OBJS = $(filter-out $(MAIN_OBJ),$(OBJS))

TARGET = spreadsheet
TEST_TARGET = run_tests

.PHONY: all clean test testfile

all: $(BUILD_DIR)/$(TARGET)

$(BUILD_DIR)/$(TARGET): $(OBJS)
	@mkdir -p ${RELEASE_DIR}
	$(CC) $(OBJS) $(LDFLAGS) -o ${RELEASE_DIR}/$(TARGET)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p ${RELEASE_DIR}
	$(CC) $(CFLAGS) -c $< -o $@

$(TEST_BUILD_DIR)/%.o: $(TEST_DIR)/%.c
	@mkdir -p $(TEST_BUILD_DIR)
	$(CC) $(TEST_CFLAGS) -c $< -o $@

$(TEST_BUILD_DIR)/$(TEST_TARGET): $(TEST_OBJS) $(LIB_OBJS)
	@mkdir -p $(TEST_BUILD_DIR)
	$(CC) $(TEST_OBJS) $(LIB_OBJS) $(LDFLAGS) -o $@

testfile: $(TEST_BUILD_DIR)/$(TEST_TARGET)

test: testfile
	./$(TEST_BUILD_DIR)/$(TEST_TARGET) ./tests/invalid_commands.txt ./tests/valid_commands.txt

clean:
	rm -rf $(BUILD_DIR)