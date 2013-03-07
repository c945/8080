CFLAGS=-g -O2 -Wall
all:test8080.hex t_85.hex ex1.hex 8080test

clean:
	rm *.o *.hex

test: 8080test ex1.hex
	@./8080test ex1.hex

8080test:	8080test.o i8080.o
	$(CC) -o $@ $^

i8080.o:	i8080.h

8080test.o:	i8080.o

.SUFFIXES: .hex .p .asm .c .o

.asm.p:
	asl -L $<

.p.hex:
	p2hex $<

.c.o:
	$(CC) $(CFLAGS) -c $<
