# Compiler and flags
CC = gcc
CFLAGS = -std=c99 -g -MMD -MP

# Directories
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

# Find the latest library versions
SDL2_LIB_DIR := $(shell find /opt/homebrew/Cellar/sdl2 -type d -maxdepth 1 -mindepth 1 | sort -r | head -n 1)/lib
SDL2_TTF_LIB_DIR := $(shell find /opt/homebrew/Cellar/sdl2_ttf -type d -maxdepth 1 -mindepth 1 | sort -r | head -n 1)/lib
SDL2_IMAGE_LIB_DIR := $(shell find /opt/homebrew/Cellar/sdl2_image -type d -maxdepth 1 -mindepth 1 | sort -r | head -n 1)/lib

SDL2_INCLUDE_DIR := $(shell find /opt/homebrew/Cellar/sdl2 -type d -maxdepth 1 -mindepth 1 | sort -r | head -n 1)/include/SDL2
SDL2_TTF_INCLUDE_DIR := $(shell find /opt/homebrew/Cellar/sdl2_ttf -type d -maxdepth 1 -mindepth 1 | sort -r | head -n 1)/include/SDL2
SDL2_IMAGE_INCLUDE_DIR := $(shell find /opt/homebrew/Cellar/sdl2_image -type d -maxdepth 1 -mindepth 1 | sort -r | head -n 1)/include/SDL2

# Library and include flags
LDFLAGS = -L$(SDL2_LIB_DIR) -L$(SDL2_TTF_LIB_DIR) -L$(SDL2_IMAGE_LIB_DIR)
LIBS = -lSDL2 -lSDL2_ttf -lSDL2_image
INCLUDES = -I$(SDL2_INCLUDE_DIR) -I$(SDL2_TTF_INCLUDE_DIR) -I$(SDL2_IMAGE_INCLUDE_DIR) -Iinclude

# Source, object, and dependency files
SOURCES = $(wildcard $(SRC_DIR)/*.c)
OBJECTS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SOURCES))
DEPS = $(OBJECTS:.o=.d)

# Output executable
EXECUTABLE = $(BIN_DIR)/chess

# Targets
all: setup $(EXECUTABLE) clean-objects

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) $(LIBS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

setup:
	mkdir -p $(OBJ_DIR)
	mkdir -p $(BIN_DIR)

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

clean-objects:
	rm -rf $(OBJ_DIR)

-include $(DEPS)

debug: CFLAGS += -DDEBUG