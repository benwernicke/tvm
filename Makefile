DEBUG_FLAGS	:= -g -Wall -pedantic -O0 -fsanitize=address -fsanitize=leak -fsanitize=undefined
CFLAGS		:= ${DEBUG_FLAGS}
CC	:= gcc

tvm.out := def.o main.o flag.o assembler.o label_map.o err.o
obj := ${tvm.out}
out := tvm.out
del := ${obj} ${out}

all: tvm.out

%.o: %.c
	${CC} $< ${CFLAGS} -c -o $@

%.out: ${obj}
	${CC} ${$@} ${CFLAGS} -o $@

clean: 
	touch ${del} && rm ${del}
