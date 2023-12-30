all:
	gcc emulator.c chip8.c -o emulator

clean:
	rm emulator

run: all
	./emulator