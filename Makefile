all:
	gcc *.c $(shell pkg-config --cflags --libs sdl2) -o emulator

clean:
	rm emulator

run: all
	./emulator

