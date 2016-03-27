# Basic Makefile
 
# Compilers and options
CC=clang -g -fsanitize=address
CPPFLAGS=
CFLAGS= -Wall -Wextra -std=c99 -O3
LDFLAGS=
LDLIBS= `pkg-config --libs sdl` -lSDL_image

SRC= ${wildcard src/*.c}
OBJ= ${SRC:.c=.o}
 
all: main
 
main: ${OBJ}
 
clean:
	rm -f *~ *.o
	rm -f main

# END

