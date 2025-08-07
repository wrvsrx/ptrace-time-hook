CC = gcc
CFLAGS = -Wall -Wextra -g

all: time_hook test_program test_static

time_hook: time_hook.c
	$(CC) $(CFLAGS) -o time_hook time_hook.c

test_program: test_program.c
	$(CC) $(CFLAGS) -o test_program test_program.c

test_static: test_program.c
	$(CC) $(CFLAGS) -static -o test_static test_program.c

clean:
	rm -f time_hook test_program test_static

.PHONY: all clean