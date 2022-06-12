CFLAGS=-std=c11 -g -static

a1c: a1c.c

test: a1c
		./test.sh

clean:
		rm -f a1c *.o *~ tmp*

.PHONY: test clean