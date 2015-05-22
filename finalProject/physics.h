#ifndef PHYSICS_H

#define PHYSICS_H
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

typedef struct{
	GLfloat pos[3];
	float vel[3];
	GLfloat outR;
	GLfloat	inR;
	float mass;
	int squishFlag;
	float squishDir[3];
	float squishAmount;
} Ball;

typedef struct{
	float norm[3]; //should be unit length
	float pts[2][3]; //points on oppisite corners of face
	float vectors[4][3]; //vectors between pts
	float l; //length
	float w; //width
} Face;
	
typedef struct{
	Face faces[6];
	float yaw;
	float pitch;
	float roll;
	float dis[3];
	float pos[3];
} Wall;

typedef struct blst {
	Ball *ball;
	struct blst *next;
} BallList;

typedef struct wlst {
	Wall *wall;
	struct wlst *next;
} WallList;


Wall *wall_create(float posX, float posY, float posZ, float yaw, float pitch, float roll, float dist1, float dist2, float dist3); //assumes vects of (1,0,0), (0,1,0), (0,0,1) uses rotations in order listed then scales

WallList *wallList_push(WallList *wList, Wall *wall);

WallList *wallList_clear(WallList *wList);

//ball functions
Ball *ball_create(float posX, float posY, float posZ, float velX, float velY, float velZ, float outR, float inR, float mass);

BallList *ballList_push(BallList *bList, Ball *ball);

BallList *ballList_clear(BallList *bList);

int interact(Ball *ball1, Ball *ball2, int outIn);

void ballList_update(BallList *blist, WallList *wlist, int numUpdates);

#endif
