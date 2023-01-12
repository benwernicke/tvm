FLAGS   	:= -pipe
DEBUG_FLAGS := -g -Wall -pedantic -fsanitize=address -fsanitize=leak -fsanitize=undefined
CC      	:= gcc
CFLAGS  	:= ${FLAGS} ${DEBUG_FLAGS}

asm.out 	:= asm.o def.o err.o flag.o map.o label_buf.o
vm.out		:= vm.o def.o err.o flag.o

obj     	:= ${asm.out} ${vm.out}

all: asm.out vm.out

%.o: %.c
	${CC} ${CFLAGS} -c -o $@ $<

%.out: ${obj}
	${CC} ${CFLAGS} -o $@ ${$@}

clean:
	rm *.o *.out *.log

.PHONY: clean
