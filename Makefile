CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -std=c11 -O2

scheduler: main.c scheduler.c header.h
	$(CC) $(CPPFLAGS) $(CFLAGS) main.c scheduler.c $(LDFLAGS) $(LDLIBS) -o scheduler

.PHONY: clean
clean:
	rm -f scheduler *.o rate_vsb.out
