CC = gcc
C_ARGS = -Wall -O2
INS = /usr/bin/install


all:	ttylog

ttylog:	ttylog.o
		$(CC) $(C_ARGS) -o ttylog ttylog.o

ttylog.o:	ttylog.c
		$(CC) $(C_ARGS) -c ttylog.c

clean: 
		rm -f *.o ttylog core *~

install: ttylog
	$(INS) -o root -g root -m 0755 -s ttylog debian/tmp/usr/sbin/ttylog
