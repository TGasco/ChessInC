CC = gcc
CFLAGS = -std=c99 -g -MMD -MP
LDFLAGS = -L/opt/homebrew/Cellar/sdl2/2.30.3/lib -L/opt/homebrew/Cellar/sdl2_ttf/2.20.2/lib -L/opt/homebrew/Cellar/sdl2_image/2.8.2_1/lib
LIBS = -lSDL2 -lSDL2_ttf -lSDL2_image
INCLUDES = -I/opt/homebrew/Cellar/sdl2/2.30.3/include/SDL2 -I/opt/homebrew/Cellar/sdl2_ttf/2.20.2/include/SDL2 -I/opt/homebrew/Cellar/sdl2_image/2.8.2_1/include/SDL2 -Iinclude
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin
SOURCES = $(wildcard $(SRC_DIR)/*.c)
OBJECTS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SOURCES))
DEPS = $(OBJECTS:.o=.d)
EXECUTABLE = $(BIN_DIR)/chess

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
