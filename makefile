.POSIX:
CC     = gcc
CFLAGS = -W -O
LDLIBS = -lm

SRC_FILES := $(wildcard src/*.c)
OBJ_FILES := $(patsubst src/%.c,obj/%.o,$(SRC_FILES))

rasteriser: $(OBJ_FILES) | output/
	$(CC) $(LDFLAGS) $(LDLIBS) -o $@ $^ 

obj/%.o: src/%.c | obj/
	$(CC) -c -o $@ $^

%/:
	mkdir -p $@

clean:
	rm -r obj/ rasteriser
