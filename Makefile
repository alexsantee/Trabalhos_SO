CC = gcc
src = $(wildcard *.c)
exe = $(src:.c=.out)

all: $(exe)

.c.out:
	$(CC) $< -static -o $@
