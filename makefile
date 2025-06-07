.POSIX:
CC     = gcc
CFLAGS = -W -O
LDLIBS = -lm 

rasteriser: obj/main.o obj/imgio.o obj/common.o obj/list.o obj/object.o obj/transform.o
	$(CC) $(LDFLAGS) -o rasteriser obj/main.o obj/imgio.o obj/common.o obj/list.o  obj/object.o obj/transform.o $(LDLIBS)

obj/main.o: src/main.c src/imgio.h src/common.h src/list.h src/object.h src/transform.h
	$(CC) $(CFLAGS) -c src/main.c -o obj/main.o

obj/imgio.o: src/imgio.h src/imgio.c
	$(CC) $(CFLAGS) -c src/imgio.c -o obj/imgio.o

obj/common.o: src/common.h src/common.c
	$(CC) $(CFLAGS) -c src/common.c -o obj/common.o

obj/list.o: src/list.h src/list.c
	$(CC) $(CFLAGS) -c src/list.c -o obj/list.o

obj/object.o: src/object.h src/object.c
	$(CC) $(CFLAGS) -c src/object.c -o obj/object.o

obj/transform.o: src/transform.h src/transform.c
	$(CC) $(CFLAGS) -c src/transform.c -o obj/transform.o
