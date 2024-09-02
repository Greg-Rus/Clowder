build:
	g++ -Wall -I"./libs/" -std=c++17 src/*.cpp -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer -llua5.3 -o clowder;

run:
	./clowder

clean:
	rm clowder