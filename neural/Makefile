# Makefile

CC=clang -fsanitize=address
CPPFLAGS= -MMD -MP
CFLAGS= -Wall -Wextra -pedantic -std=c99 -O2

all: neural

.PHONY: clean

clean:
	rm -f neural neural.d
