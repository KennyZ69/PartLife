CFLAGS = -Wall -Wextra -Iinclude $(shell sdl2-config --cflags)
LDFLAGS = $(shell sdl2-config --libs) -lGL -lm

SRC = src/*.c main.c
OUT = sim

all: $(OUT)

$(OUT): $(SRC)
	gcc $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(OUT)

run:
	@echo "Running sim..."
	./$(OUT)
