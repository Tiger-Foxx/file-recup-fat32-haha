CC=gcc
CFLAGS=-g -pedantic -std=gnu17 -Wall -Werror -Wextra

.PHONY: all
all: nyufile

nyufile: nyufile.c global.h fsinfo.c fsinfo.h recover.c recover.h
	gcc -l crypto -o nyufile nyufile.c fsinfo.c recover.c

.PHONY: clean
clean:
	rm -f *.o nyufile