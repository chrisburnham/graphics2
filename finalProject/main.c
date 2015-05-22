#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include "ppmIO.h"
#include "physics.h"

static int frame = 0;
static BallList *ballList = NULL;
static WallList *wallList = NULL;

void display(void) {
  GLfloat position1[] = {0.0, 15.0, 0.0, 1.0};
	//GLfloat position2[] = {2.0, 0.0, 10.0, 1.0};
	GLUquadric *sphere;
	GLUquadric *cube;
	GLUquadric *plane;
	//GLUquadric *cyl;
	GLfloat brown[] = {.5f,.3f,.1f};
	GLfloat green[] = {.2f,.8f,.3f};
	int *data;
	int i;
	Pixel *image;
	char filename[20];
	const int frames = 1000;
	char str[4]; // 9999 frame max (bad if go over)
	BallList *tempB;
	WallList *tempW;
	float tmp1, tmp2, tmp3;

	if(frame < frames){

		glLightfv(GL_LIGHT1, GL_POSITION, position1);
		glEnable(GL_LIGHT1);

		// clear screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glColor3f(1.0, 1.0, 1.0);

		// reset the modelview matrix
		glLoadIdentity();

		// set up the viewing transformation
		gluLookAt(0.0, 4.0, 8.0,  0.0, 0.0, 0.0,  0.0, 1.0, 0.0);

		// set up the light
		//glLightfv(GL_LIGHT2, GL_POSITION, position2);
		//glEnable(GL_LIGHT2);

// below used to fake walls
	/*	glMaterialfv(GL_FRONT, GL_DIFFUSE, brown);
		glRotatef(-90, 1, 0, 0);
		glTranslatef(0,5,0);
		plane = gluNewQuadric();
		gluDisk(plane, 0, 10, 30, 13);
		glTranslatef(0,-5,0);
		glRotatef(90, 1, 0, 0);

		glTranslatef(0,0,-1);
		gluDisk(plane, 0, 10, 30, 13);
		glTranslatef(0,0,1);

		glRotatef(-90, 0, 1, 0);
		glTranslatef(0,0,-1);
		gluDisk(plane, 0, 10, 30, 13);
		glTranslatef(0,0,1);
		glRotatef(90, 0, 1, 0);

		glRotatef(90, 0, 1, 0);
		glTranslatef(0,0,-1);
		gluDisk(plane, 0, 10, 30, 13);
		glTranslatef(0,0,1);
		glRotatef(-90, 0, 1, 0);*/

		glMaterialfv(GL_FRONT, GL_DIFFUSE, green);
		glEnable(GL_NORMALIZE);
		glScalef(.1,.1,.1);

		/*tempW = wallList; not quite right walls code
		cube = gluNewQuadric();
		while(tempW != NULL){
			glTranslatef(tempW->wall->pos[0], tempW->wall->pos[1], tempW->wall->pos[2]);
			glRotatef(tempW->wall->yaw, 0,0,1);
			glRotatef(tempW->wall->pitch, 0,1,0);
			glRotatef(tempW->wall->roll, 1,0,0);
			glScalef(tempW->wall->dis[0], tempW->wall->dis[1], tempW->wall->dis[2]);
			glTranslatef(0.5, 0.5, 0.5);
			glutSolidCube(1.0);
			glTranslatef(-0.5, -0.5, -0.5);
			glScalef(1.0/tempW->wall->dis[0], 1.0/tempW->wall->dis[1], 1.0/tempW->wall->dis[2]);
			glRotatef(-1*tempW->wall->roll, 1,0,0);
			glRotatef(-1*tempW->wall->pitch, 0,1,0);
			glRotatef(-1*tempW->wall->yaw, 0,0,1);
			glTranslatef(-1*tempW->wall->pos[0], -1*tempW->wall->pos[1], -1*tempW->wall->pos[2]);
			tempW = tempW->next;
		}*/

		tempB = ballList;
		sphere = gluNewQuadric();
		//cyl = gluNewQuadric();
		while(tempB != NULL){
			glTranslatef(tempB->ball->pos[0], tempB->ball->pos[1], tempB->ball->pos[2]);
			if(tempB->ball->squishFlag == 1){
				if(tempB->ball->squishDir[1] == 0){
					if(tempB->ball->squishDir[0] != 0){
						tmp1 = (180/M_PI)*atanf(tempB->ball->squishDir[2] / tempB->ball->squishDir[0]);
						glRotatef(-1*tmp1, 0,1,0);
					}
				}
				else{
					tmp1 = (180/M_PI)*atanf(tempB->ball->squishDir[2] / tempB->ball->squishDir[1]);
					glRotatef(-1*tmp1, 1,0,0);
					tmp2 = 90 - (180/M_PI)*atanf(tempB->ball->squishDir[0] / sqrtf(tempB->ball->squishDir[1]*tempB->ball->squishDir[1] + tempB->ball->squishDir[2]*tempB->ball->squishDir[2]));
					glRotatef(-1*tmp2, 0,0,1);
				}
				tmp3 = (tempB->ball->outR - tempB->ball->squishAmount) / tempB->ball->outR;
				/*glRotatef(90, 0,1,0);
				gluCylinder(cyl, .5, .5, 10, 5, 5);
				glRotatef(-90, 0,1,0);*/
				glScalef(tmp3,1,1);
			}
			gluSphere(sphere, tempB->ball->outR, 20, 20);
			if(tempB->ball->squishFlag == 1){
				glScalef(1/tmp3,1,1);
				if(tempB->ball->squishDir[1] == 0){
					if(tempB->ball->squishDir[0] != 0){
						glRotatef(tmp1, 0,1,0);
					}
				}
				else{
					glRotatef(tmp2, 0,0,1);
					glRotatef(tmp1, 1,0,0);
				}
			}
			glTranslatef(-1*tempB->ball->pos[0], -1*tempB->ball->pos[1], -1*tempB->ball->pos[2]);
			tempB = tempB->next;
		}
		glDisable(GL_NORMALIZE);

		// draw everything
		glutSwapBuffers();

		data = malloc(sizeof(int)*3*500*500);
		glReadPixels(0,0,500,500,GL_RGB,GL_UNSIGNED_INT,data);
		image = malloc(sizeof(Pixel)*500*500);
		for(i=0; i<500*500; i++){
			image[i].r = (char)data[3*500*500-3*i-3];
			image[i].g = (char)data[3*500*500-3*i-2];
			image[i].b = (char)data[3*500*500-3*i-1];
		}
	
		strcpy(filename, "frame-");
		strcpy(str, "0");
		i = 1000;
		while(frame < i){
			if(i == 1){
				break;
			}
			strcat(filename, str);
			i /= 10;
		}

		sprintf(str, "%i", frame);
		strcat(filename, str);
	
		strcpy(str, ".ppm");
		strcat(filename, str);

		frame++;

		writePPM(image, 500, 500, 255, filename);

		free(image);
		free(data);
	}
}

void animate(void){
	ballList_update(ballList, wallList, 5);
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

	glLightfv(GL_LIGHT2, GL_DIFFUSE, diffuse);
  glLightfv(GL_LIGHT2, GL_SPECULAR, specular );

	// enable lighting, light0 and depth testing
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT1);
	glEnable(GL_LIGHT2);
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

void mouse(int button, int state, int x, int y){ //onclick animation begins
	switch(button){
		case GLUT_LEFT_BUTTON:
			if(state == GLUT_DOWN){
				system("rm frame-0000.ppm");
				glutIdleFunc(animate);
			}
			break;
		case GLUT_MIDDLE_BUTTON:
			if(state == GLUT_DOWN){
				glutIdleFunc(NULL);
			}
		default:
			break;
	}
}

//set up the balls here
static void setUpBalls(){
	Ball *newBall;
	//int i;

	newBall = ball_create(0,-8,4, 0,6,-5.1, 2,0.5,4);
	ballList = ballList_push(ballList, newBall);

	newBall = ball_create(4,-4,4, -2,3,-5, 1,0.5,1);
	ballList = ballList_push(ballList, newBall);

	newBall = ball_create(-4,-4,4, 2,3,-5.2, 1,0.5,1);
	ballList = ballList_push(ballList, newBall);

	newBall = ball_create(8,0,4, -4,1,-5, 2,0.5,4);
	ballList = ballList_push(ballList, newBall);

	newBall = ball_create(-8,0,4, 4,1,-5.1, 2,0.5,4);
	ballList = ballList_push(ballList, newBall);

	newBall = ball_create(4,4,4, -2,0,-5.1, 1,0.5,1);
	ballList = ballList_push(ballList, newBall);

	newBall = ball_create(-4,4,4, 2,0,-5, 1,0.5,1);
	ballList = ballList_push(ballList, newBall);

	newBall = ball_create(0,8,4, 0,-2,-5.2, 2,0.5,4);
	ballList = ballList_push(ballList, newBall);


	//for(i=0; i<100; i++){
	//	newBall = ball_create((rand()%14)-7,30+(rand()%14),(rand()%14)-7, (rand()%6)-3,1,(rand()%6)-3, 1,1,1);
	//	ballList = ballList_push(ballList, newBall);
	//}

	return;
}

//set up the walls here
static void setUpWalls(){
	Wall *newWall;

	newWall = wall_create(-10,-10,-20, 20,0,0, 30,3,40);
	wallList = wallList_push(wallList, newWall);	

	return;
}

void cleanUp(){
	ballList = ballList_clear(ballList);

	wallList = wallList_clear(wallList);

	system("convert frame-*.ppm test.gif");
	system("rm frame-*.ppm");

	return;
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

	setUpBalls();
	setUpWalls();
	atexit(cleanUp);

	// set up the callback functions
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);

	// run the main loop
  glutMainLoop();

  return(0);
}

