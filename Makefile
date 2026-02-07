.PHONY: all clean

NAME = build/SuperDecoder.nes

CFLAGS = -C nes.cfg -t nes -Oisr -I src
CC = cl65

SRC = src/main.c src/crt0.s src/chr.s

all: $(NAME)

build:
	mkdir -p build

$(NAME): $(SRC) nes.cfg | build
	$(CC) -o $(NAME) $(CFLAGS) $(SRC)

clean:
	rm -rf build *.o

.PHONY: all clean
