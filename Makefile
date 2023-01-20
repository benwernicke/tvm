CC := gcc
DEBUG_FLAGS    := -Wall -pedantic -g -fsanitize=leak -fsanitize=undefined -fsanitize=address
RELEASE_FLAGS  := -march=native -mtune=native -O3 -flto

vm.out := vm.o err.o flag.o def.o
asm.out := asm.o err.o flag.o map.o def.o label_buf.o

SRC := $(wildcard *.c)
OBJ := ${SRC:c=o}
TARGETS := vm.out asm.out

.PHONY: clean all release debug example

all: debug

clean:
	rm *.out *.o

example:
	./asm.out test/libmain.asm test/lib.asm

debug: CFLAGS := ${DEBUG_FLAGS}
debug: ${TARGETS}

release: CFLAGS := ${RELEASE_FLAGS}
release: ${TARGETS}

${TARGETS}: ${OBJ}
	${CC} ${CFLAGS} ${$@} -o $@
	
%.o: %.c
	${CC} ${CFLAGS} $< -c -o $@
