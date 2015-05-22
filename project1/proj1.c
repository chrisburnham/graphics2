#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "ppmIO.h"

// Chris Burnham project 1 graphics 451

// got some help from wikipedia here
double halton(int index, int base);
double halton(int index, int base){
	double result, f;
	int i;

	result = 0;
	f = 1.0 / base;
	i = index;
	while(i>0){
		result = result + f*(i%base);
		i = (int)(i / base);
		f = f / base;
	}
	return result;
}

void display(void) {
  GLfloat position[] = {10.0, 5.0, 20.0, 1.0};
	GLUquadric *cylinder;
	GLUquadric *sphere;
	GLUquadric *ground;
	GLfloat brown[] = {.5f,.3f,.1f};
	GLfloat green[] = {.2f,.8f,.3f};
	GLuint tree = glGenLists(1);
	GLuint bush = glGenLists(1);
	int i,angle;
	double /*r, theta,*/ x, y;
	static starttime = 0.0;
	struct timeval tp;
	struct timezone tz;
	double curtime, delta, lightx;
	int *data;
	Pixel *image;
	char filename[20];
	char str[5];
	
	gettimeofday( &tp, &tz );
	curtime = tp.tv_sec + tp.tv_usec/1000000.0;
	if(starttime == 0.0){
		starttime = curtime;
	}
	delta = curtime - starttime;
	lightx = (int)(delta * 1000) % 10000;

	strcpy(filename, "images/frame-");
	if(lightx < 1000){
		if(lightx < 100){
			if(lightx < 10){
				strcpy(str, "000");
				strcat(filename, str);
			}
			else{
				strcpy(str, "00");
				strcat(filename, str);
			}
		}
		else{
			strcpy(str, "0");
			strcat(filename, str);
		}
	}

	sprintf(str, "%i", (int)lightx); 
	strcat(filename, str);
	strcpy(str, ".ppm");
	strcat(filename, str);

	lightx = (lightx / 100.0) - 50;
	GLfloat light_pos[4] = {lightx,30,0,1.0f};

	glLightfv(GL_LIGHT1, GL_POSITION, light_pos);
	glEnable(GL_LIGHT1);

	// clear screen
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glColor3f(1.0, 1.0, 1.0);

	// reset the modelview matrix
  glLoadIdentity();

	// set up the viewing transformation
	gluLookAt(0.0, 2.0, 8.0,  0.0, 0.0, 0.0,  0.0, 1.0, 0.0);

	// set up the light
  glLightfv(GL_LIGHT0, GL_POSITION, position);

	// set up a tree
	glNewList(tree, GL_COMPILE);
		glEnable(GL_NORMALIZE);
		glScalef(.1,.1,.1);
		glTranslatef(0,2,0);
		glRotatef(90, 1, 0, 0);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, brown);
		cylinder = gluNewQuadric();
		gluCylinder(cylinder, 0.1, 0.1, 2.0, 13, 4);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, green);
		sphere = gluNewQuadric();
		glTranslatef(0,0,-1);
		gluSphere(sphere, 1.0, 23, 23);
		glTranslatef(0,0,1);
		glRotatef(-90,1,0,0);
		glTranslatef(0,-2,0);
		glScalef(10,10,10);
		glDisable(GL_NORMALIZE);
	glEndList();

	// set up a bush
	glNewList(bush, GL_COMPILE);
		glEnable(GL_NORMALIZE);
		glScalef(.1,.1,.1);
		glTranslatef(0,1.6,0);
		glRotatef(90, 1, 0, 0);
		
		//branch left
		glTranslatef(-.7,0,0);
		glRotatef(20, 0, 1, 0);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, brown);
		cylinder = gluNewQuadric();
		gluCylinder(cylinder, 0.1, 0.1, 2.0, 13, 4);
		glRotatef(-20, 0, 1, 0);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, green);
		sphere = gluNewQuadric();
		gluSphere(sphere, 0.5, 13, 13);
		glTranslatef(.7,0,0);

		//branch right
		glTranslatef(.7,0,0);
		glRotatef(-20, 0, 1, 0);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, brown);
		cylinder = gluNewQuadric();
		gluCylinder(cylinder, 0.1, 0.1, 2.0, 13, 4);
		glRotatef(20, 0, 1, 0);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, green);
		sphere = gluNewQuadric();
		gluSphere(sphere, 0.5, 13, 13);
		glTranslatef(-.7,0,0);

		//branch back
		glTranslatef(0,-.7,0);
		glRotatef(-20, 1, 0, 0);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, brown);
		cylinder = gluNewQuadric();
		gluCylinder(cylinder, 0.1, 0.1, 2.0, 13, 4);
		glRotatef(20, 1, 0, 0);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, green);
		sphere = gluNewQuadric();
		gluSphere(sphere, 0.5, 13, 13);
		glTranslatef(0,.7,0);

		//branch front
		glTranslatef(0,.7,0);
		glRotatef(20, 1, 0, 0);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, brown);
		cylinder = gluNewQuadric();
		gluCylinder(cylinder, 0.1, 0.1, 2.0, 13, 4);
		glRotatef(-20, 1, 0, 0);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, green);
		sphere = gluNewQuadric();
		gluSphere(sphere, 0.5, 13, 13);
		glTranslatef(0,-.7,0);
	
		glRotatef(-90,1,0,0);
		glTranslatef(0,-1.6,0);
		glScalef(10,10,10);
		glDisable(GL_NORMALIZE);
	glEndList();

	// makes the ground
	glRotatef(-90, 1, 0, 0);
	glTranslatef(0,5,0);
	ground = gluNewQuadric();
	gluDisk(ground, 0, 10, 30, 13);
	glTranslatef(0,-5,0);
	glRotatef(90, 1, 0, 0);

	srand(69);
	for(i=0; i<500; i++){
		/* using polar and rand
		r = (rand()%100) / 20.0;
		theta = rand()%360;
		x = ((rand()%100) - 70) / 10.0;
		
		glTranslatef(0,0,x);
		glRotatef(theta, 0, 1, 0);
		glTranslatef(r,0,0);*/
	
		x = (halton(i, 2)-.5)*5;
		y = halton(i, 3)*15 - 5;

		glTranslatef(x, 0, y);
		if(rand()%3 != 0){
			glCallList(tree);	
		}
		else{
			angle = rand()%45;
			glRotatef(angle,0,1,0);
			glCallList(bush);
			glRotatef(-angle,0,1,0);
		}
		glTranslatef(-x, 0, -y);
		
		/*glTranslatef(-r,0,0);
		glRotatef(-theta, 0, 1, 0);
		glTranslatef(0,0,-x);*/
	}

	glDeleteLists(tree, 1);
	glDeleteLists(bush, 1);

	// draw everything
  glFlush();

	data = malloc(sizeof(int)*3*500*500);
	glReadPixels(0,0,500,500,GL_RGB,GL_UNSIGNED_INT,data);
	image = malloc(sizeof(Pixel)*500*500);
	for(i=0; i<500*500; i++){
		image[i].r = (char)data[3*500*500-3*i-3];
		image[i].g = (char)data[3*500*500-3*i-2];
		image[i].b = (char)data[3*500*500-3*i-1];
	}
	writePPM(image, 500, 500, 255, filename);

	free(image);
	free(data);

	usleep(10000);
	glutPostRedisplay();
}

//  Function called when the window is resized
void reshape(int w, int h) {
	// set the viewport to the updated window size
  glViewport(0, 0, (GLsizei)w, (GLsizei)h);

	// move to projection matrix mode
  glMatrixMode(GL_PROJECTION);

	// reset the projection to identity
  glLoadIdentity();

	// create the new perspective matrix
  gluPerspective((GLdouble)30, (GLdouble)w / (GLdouble)h, (GLdouble)2, (GLdouble)100);

	// go back to model view matrix mode
  glMatrixMode(GL_MODELVIEW);

	// reset the modeview matrix
  glLoadIdentity();
}

// initialize the lighting and material color values
void initlights(void) {
	GLfloat ambient[] = {0.1, 0.1, 0.1, 1.0};
  GLfloat diffuse[] = {0.9, 0.9, 0.9, 1.0};
	GLfloat specular[] = {0.5, 0.5, 0.5, 1.0};
  GLfloat mat_diffuse[] = {0.3, 0.9, 0.5, 1.0};
  GLfloat mat_specular[] = {0.1, 0.1, 0.1, 1.0};
  GLfloat mat_shininess[] = {50.0};

	// material values
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
  glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
  glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

	// generic lighting values
  glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, 1);
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);
  
  // specific light source
  glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);
  glLightfv(GL_LIGHT1, GL_SPECULAR, specular );

	// enable lighting, light0 and depth testing
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT1);
	glEnable(GL_DEPTH_TEST); // important, or you can see through the sphere

}

// init function
void init(void) {
	// background color
  glClearColor(0.0, 0.0, 0.0, 0.0);

	// whether shading is smooth or flat
  glShadeModel(GL_SMOOTH);

	initlights();
}

//  This function is called whenever a key event occurs.
//  The key value is in "key" and "x" and "y" hold the current mouse position
void keyboard(unsigned char key, int x, int y)
{
   switch( key) {
   case 'q': // quit
     exit(0);
     break;
   default:
     break;
   }
}

// main function
int main(int argc, char **argv) {

	// initialize
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(500, 500);
  glutInitWindowPosition(100, 100);
  glutCreateWindow("Project 1");
  init();

	// set up the callback functions
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keyboard);

	// run the main loop
  glutMainLoop();

  return(0);
}

