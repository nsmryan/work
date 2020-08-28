
CC ?= gcc


INC := -Idep/fs -Idep/logc -Idep/growable_buf -Iinc

CFLAGS := -g -O3 -Wall -Werror -Wextra $(INC)

DEPS := dep/fs/fs.c dep/logc/log.c

.PHONY: all
all: build/ build/work

build/: 
	mkdir -p build

build/work: build/main.o build/worklib.o build/log.o build/fs.o
	$(CC) $(CFLAGS) -o $@ $^  -ltcc -pthread -ldl

build/main.o: src/main.c
	$(CC) -c $(CFLAGS) -o $@ $<

build/log.o: dep/logc/log.c
	$(CC) -c $(CFLAGS) -o $@ $<

build/fs.o: dep/fs/fs.c
	$(CC) -c $(CFLAGS) -o $@ $<

build/worklib.o: src/worklib.c
	$(CC) -c $(CFLAGS) -o $@ $<

.PHONY: clean
clean:
	rm build -rf
