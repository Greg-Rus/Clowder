CC = g++
LANG_STD = -std=c++17
COMPILER_FLAGS = -Wall -Wfatal-errors -g
INCLUDE_PATH = -I"./libs/"
SRC_FILES = src/*.cpp \
			src/Game/*.cpp \
			src/Logger/*.cpp \
			src/ECS/*.cpp \
			src/AssetStore/*.cpp\
			./libs/imgui/*.cpp\
			#src/Events/*.cpp\
			#src/EventBus/*.cpp
LINKER_FLAGS = -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer -llua5.3
OBJ_NAME = clowder

compile:
	$(CC) $(COMPILER_FLAGS) $(INCLUDE_PATH) $(LANG_STD) $(SRC_FILES) $(LINKER_FLAGS) -o $(OBJ_NAME);

run:
	./$(OBJ_NAME)

clean:
	rm $(OBJ_NAME)