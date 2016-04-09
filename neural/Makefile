# Makefile

CC=clang -fsanitize=address
CPPFLAGS= -MMD -MP `pkg-config --cflags sdl`
CFLAGS= -Wall -Wextra -pedantic -std=c99 -O2
LDFLAGS=
LDLIBS= `pkg-config --libs sdl` -lSDL_image

SRC= pixel_operations.c loadimage.c neural.c
OBJ= ${SRC:.c=.o}


all: neural

neural: ${OBJ}

.PHONY: clean

clean:
	rm -f neural neural.d
	rm -f *.o *.d
