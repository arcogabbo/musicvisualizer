all: sdl_test

sdl_test: main.c
	gcc *.c -lpthread -lm -lfftw3 -lSDL2 -g -o visualizer
