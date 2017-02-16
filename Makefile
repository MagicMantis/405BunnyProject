all: bunny.c
	gcc -L/usr/lib64 -O2 bunny.c -lX11 -lGL -lGLU -lglut -lm -lXmu -o run

clean: 
	rm -f run
