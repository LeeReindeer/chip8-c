all:
	gcc *.c -o emulator

clean:
	rm emulator

run: all
	./emulator