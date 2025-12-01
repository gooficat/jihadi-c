build:
	gcc src/*.c -o jc

run:
	./jc test.jc
	

launch: build run

debug:
	gcc -g src/*.c -o jc
	gdb jc