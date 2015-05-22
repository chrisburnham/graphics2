CC=gcc
CFLAGS=-I/usr/X11R6/include -L/usr/X11R6/lib -I/opt/local/include -L/opt/local/lib -I./include 

detectTest: detectTest.o image.o ppmIO.o edgeDetect.o
	gcc $(CFLAGS) -o $@ $^ -lglut -lGLU -lGL -lX11 -lm -lXmu

clean:
	rm -f *.o *~ a.out
