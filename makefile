.POSIX:
CC     = gcc
CFLAGS = -W -O
LDLIBS = -lm 

rasteriser: obj/main.o obj/imgio.o
	$(CC) $(LDFLAGS) -o rasteriser obj/main.o $(LDLIBS)

clean:
	rm -fr obj/
	rm rasteriser
	rm out.bmp

obj/main.o: src/main.c src/imgio.h
	$(CC) $(CFLAGS) -c src/main.c -o obj/main.o

obj/imgio.o: src/imgio.h src/imgio.o
	$(CC) $(CFLAGS) -c src/imgio.c -o obj/imgio.o
