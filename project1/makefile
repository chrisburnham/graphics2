CC=gcc
CFLAGS=-I/usr/X11R6/include -L/usr/X11R6/lib -I/opt/local/include -L/opt/local/lib -I./include 

glview: glView.o
	gcc $(CFLAGS) -o $@ $^ -lglut -lGLU -lGL -lX11 -lm -lXmu

glbez: glBezier.o
	gcc $(CFLAGS) -o $@ $^ -lglut -lGLU -lGL -lX11 -lm -lXmu

gltex: glTexture.o tiff-util.o
	gcc $(CFLAGS) -o $@ $^ -lglut -lGLU -lGL -lX11 -lm -lXmu -ltiff

gltest: glTest.o
	gcc $(CFLAGS) -o $@ $^ -lglut -lGLU -lGL -lX11 -lm -lXmu

proj1: proj1.o ppmIO.o
	gcc $(CFLAGS) -o $@ $^ -lglut -lGLU -lGL -lX11 -lm -lXmu


clean:
	rm -f *.o *~ a.out
