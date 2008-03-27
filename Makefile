CC = gcc
C_ARGS = -Wall -O2

all:	ttylog

ttylog:	ttylog.o
		$(CC) $(C_ARGS) -o ttylog ttylog.o
ttylog.o:	ttylog.c
		$(CC) $(C_ARGS) -c ttylog.c
clean: 
		rm -f *.o ttylog core *~
