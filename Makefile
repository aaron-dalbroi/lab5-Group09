.PHONY: all
all: scheduler.out

scheduler.out: scheduler.c
	gcc -g -o $@ $^

.PHONY: clean
clean:
	rm -f scheduler.out