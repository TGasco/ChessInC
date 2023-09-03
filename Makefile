CC = gcc
CFLAGS = -O3 -std=c99 -g
LDFLAGS = -L/opt/homebrew/Cellar/sdl2/2.28.2/lib -L/opt/homebrew/Cellar/sdl2_ttf/2.20.2/lib -L/opt/homebrew/Cellar/sdl2_image/2.6.3_2/lib
LIBS = -lSDL2 -lSDL2_ttf -lSDL2_image
INCLUDES = -I/opt/homebrew/Cellar/sdl2/2.28.2/include/SDL2 -I/opt/homebrew/Cellar/sdl2_ttf/2.20.2/include/SDL2 -I/opt/homebrew/Cellar/sdl2_image/2.6.3_2/include/SDL2
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin
SOURCES = $(wildcard $(SRC_DIR)/*.c)
OBJECTS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SOURCES))
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