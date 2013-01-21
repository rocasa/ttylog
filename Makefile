CC = gcc
CFLAGS+= -Wall -g -O2
INS = /usr/bin/install
GZIP = /bin/gzip

all:	ttylog

ttylog:	ttylog.o
		$(CC) $(CFLAGS) -o ttylog ttylog.o

ttylog.o:	ttylog.c
		$(CC) $(CFLAGS) -c ttylog.c

clean: 
		rm -f *.o ttylog core *~

install: ttylog
	$(INS) -o root -g root -m 0755 ttylog $(DESTDIR)/usr/sbin
	$(INS) -o root -g root -m 0444 ttylog.8 $(DESTDIR)/usr/share/man/man8
