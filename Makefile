# Compiler and compiler flags
CC := gcc
CFLAGS := -Wall -Wextra -I./include -pthread -lmosquitto

# Directories
SRC_DIR := src
INC_DIR := include
LIB_DIR := lib
BIN_DIR := bin
EXECUTABLE = bin/mqtt_prog
# Source files and object files
SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(patsubst $(SRC_DIR)/%.c,$(BIN_DIR)/%.o,$(SRCS))

# Libraries
LIBS := -L$(LIB_DIR) -pthread -lmosquitto

# Target executable
TARGET := $(BIN_DIR)/mqtt_prog

# Default target
all: $(TARGET)

# Linking step
$(TARGET): $(OBJS) | $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

# Compiling step
$(BIN_DIR)/%.o: $(SRC_DIR)/%.c | $(BIN_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Create the bin directory if it doesn't exist
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

	
install: $(EXECUTABLE)
	mkdir -p $(DESTDIR)$(BINDIR)
	install -m 0755 $< $(DESTDIR)$(BINDIR)

# Clean up generated files
clean:
	rm -rf $(BIN_DIR)

# Phony targets
.PHONY: all clean



