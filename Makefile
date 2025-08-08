CC = gcc
CFLAGS = -Wall -Wextra -g

all: ptrace-time-hook tests/minimal_test tests/multi_time_test

ptrace-time-hook: src/main.c
	$(CC) $(CFLAGS) -o ptrace-time-hook src/main.c

tests/minimal_test: tests/minimal_test.c
	$(CC) $(CFLAGS) -o tests/minimal_test tests/minimal_test.c

tests/multi_time_test: tests/multi_time_test.c
	$(CC) $(CFLAGS) -o tests/multi_time_test tests/multi_time_test.c

clean:
	rm -f ptrace-time-hook tests/minimal_test tests/multi_time_test

.PHONY: all clean