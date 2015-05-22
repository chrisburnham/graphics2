#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "physics.h"

//wall functions
Wall *wall_create(float posX, float posY, float posZ, float yaw, float pitch, float roll, float dist1, float dist2, float dist3){ //assumes vects of (1,0,0), (0,1,0), (0,0,1) uses rotations in order listed then scales
	Wall *newWall;
	Face faces[6];
	int i, j;
	float sy, cy, sp, cp, sr, cr;
	float vects[3][3];
	float dis[3];

	sy = sinf(M_PI*yaw/180.0);
	cy = cosf(M_PI*yaw/180.0);
	sp = sinf(M_PI*pitch/180.0);
	cp = cosf(M_PI*pitch/180.0);
	sr = sinf(M_PI*roll/180.0);
	cr = cosf(M_PI*roll/180.0);

	vects[0][0] = cp*cy;
	vects[0][1] = cp*sy;
	vects[0][2] = -sp;
	
	vects[1][0] = cy*sp*sr - cr*sy;
	vects[1][1] = cr*cy + sp*sr*sy;
	vects[1][2] = cp*sr;

	vects[2][0] = cr*cy*sp + sr*sy;
	vects[2][1] = -cr*sr + cr*sp*sy;
	vects[2][2] = cp*cr;

	dis[0] = dist1;
	dis[1] = dist2;
	dis[2] = dist3;

	for(i=0; i<3; i++){
		faces[i].pts[0][0] = posX;
		faces[i].pts[0][1] = posY;
		faces[i].pts[0][2] = posZ;

		faces[i+3].pts[0][0] = posX + dis[i]*vects[i][0];
		faces[i+3].pts[0][1] = posY + dis[i]*vects[i][1];
		faces[i+3].pts[0][2] = posZ + dis[i]*vects[i][2];

		for(j=0; j<3; j++){
			faces[i].pts[1][j] = faces[i+3].pts[0][j] + dis[(i+1)%3]*vects[(i+1)%3][j];

			faces[i+3].pts[1][j] = faces[i].pts[1][j] + dis[(i+2)%3]*vects[(i+2)%3][j];

			faces[i].vectors[0][j] = dis[i]*vects[i][j];
			faces[i].vectors[2][j] = -1*faces[i].vectors[0][j];
			
			faces[i].vectors[1][j] = dis[(i+1)%3]*vects[(i+1)%3][j];
			faces[i].vectors[3][j] = -1*faces[i].vectors[1][j];

			faces[i+3].vectors[0][j] = faces[i].vectors[3][j];
			faces[i+3].vectors[2][j] = faces[i].vectors[1][j];

			faces[i+3].vectors[3][j] = dis[(i+2)%3]*vects[(i+2)%3][j];
			faces[i+3].vectors[1][j] = faces[i+3].vectors[3][j];

			faces[i].l = dis[i];
			faces[i+3].l = dis[(i+1)%3];

			faces[i].w = dis[(i+1)%3];
			faces[i+3].w = dis[(i+2)%3];

			//norm = vects[(i+1)%3] X vects[i]
			faces[i].norm[j] = vects[(i+1)%3][(j+1)%3]*vects[i][(j+2)%3] - vects[(i+1)%3][(j+2)%3]*vects[i][(j+1)%3];
			faces[3+((i+2)%3)].norm[j] = -1*faces[i].norm[j];
		}
	}	

	newWall = malloc(sizeof(Wall));

	for(i=0; i<6; i++){
		newWall->faces[i] = faces[i];
	}	
	newWall->pitch = pitch;
	newWall->yaw = yaw;
	newWall->roll = roll;
	newWall->pos[0] = posX;
	newWall->pos[1] = posY;
	newWall->pos[2] = posZ;
	for(i=0; i<3; i++){
		newWall->dis[i] = dis[i];
	}

	return newWall;
}

WallList *wallList_push(WallList *wList, Wall *wall){
	WallList *newList;

	newList = malloc(sizeof(WallList));

	newList->wall = wall;
	newList->next = wList;

	return newList;
}

WallList *wallList_clear(WallList *wList){
	WallList *temp;
	
	while(wList != NULL){
		free(wList->wall);
		temp = wList;
		wList = wList->next;
		free(temp);
	}

	return wList; //always NULL
}

//ball functions
Ball *ball_create(float posX, float posY, float posZ, float velX, float velY, float velZ, float outR, float inR, float mass){
	Ball *newBall;

	newBall = malloc(sizeof(Ball));

	newBall->pos[0] = posX;
	newBall->pos[1] = posY;
	newBall->pos[2] = posZ;
	newBall->vel[0] = velX;
	newBall->vel[1] = velY;
	newBall->vel[2] = velZ;
	newBall->outR = outR;
	newBall->inR = inR;
	newBall->mass = mass;
	newBall->squishFlag = 0;
	newBall->squishDir[0] = 0;
	newBall->squishDir[1] = 0;
	newBall->squishDir[2] = 0;
	newBall->squishAmount = 0;

	if(inR > outR){
		printf("inR > outR this is not so good\n");
	}

	if(mass < 0){
		printf("in what universe is mass negitive?\n");
	}

	return newBall;
}

BallList *ballList_push(BallList *bList, Ball *ball){
	BallList *newList;
	
	newList = malloc(sizeof(BallList));

	newList->ball = ball;
	newList->next = bList;

	return newList;
}

BallList *ballList_clear(BallList *bList){
	BallList *temp;

	while(bList != NULL){
		free(bList->ball);
		temp = bList;
		bList = bList->next;
		free(temp);
	}

	return bList; //always NULL
}

int interact(Ball *ball1, Ball *ball2, int outIn){
	float dist[3];
	float size;
	int i;
		
	for(i=0; i<3; i++){
		dist[i] = ball1->pos[i] - ball2->pos[i];
	}

	if(outIn == 0){
		size = ball1->outR + ball2->outR;
	}
	else{
		size = ball1->inR + ball2->inR;
	}
	
	if(size*size > dist[0]*dist[0] + dist[1]*dist[1] + dist[2]*dist[2]){
		return 1;
	}
	else{
		return 0;
	}
}

void ballList_update(BallList *blist, WallList *wlist, int numUpdates){
	int u, i, j, k;
	float stepSize;
	float dir[3];
	float pt[3];
	float dsq[4];
	float cross[3];
	float len, spring, tot, sq1, sq2, netVel, sumMass, dis, dsqrect, distance;
	BallList *temp, *saveB;
	WallList *saveW;

	stepSize = 1.0 / numUpdates; //similar animations for different numUpdates

	saveB = blist;
	saveW = wlist;
	while(blist != NULL){
		blist->ball->squishFlag = 0;
		blist->ball->squishAmount = 0;
		blist = blist->next;
	}

	for(u=0; u<numUpdates; u++){
		blist = saveB;
		wlist = saveW;
		while(blist != NULL){

			for(i=0; i<3; i++){ //update locations base of velocities
				blist->ball->pos[i] += 0.2*stepSize*blist->ball->vel[i];
			}

			temp = blist->next;
			while(temp != NULL){ //ball on ball collisions
			
				if(interact(blist->ball, temp->ball, 0) == 1){
					len = 0;
					for(i=0; i<3; i++){
						dir[i] = blist->ball->pos[i] - temp->ball->pos[i];
						len += dir[i]*dir[i];
					}
					len = sqrtf(len);
					for(i=0; i<3; i++){
						dir[i] = dir[i] / len; //dir from temp to blist
					}
				
					netVel = 0;
					for(i=0; i<3; i++){
						netVel += dir[i]*temp->ball->vel[i];
						netVel -= dir[i]*blist->ball->vel[i];
					}
					netVel = fabs(netVel); //may cause problems if netVel can productivly be negitive but here to fix triple colide case

					sumMass = blist->ball->mass + temp->ball->mass;

					if(interact(blist->ball, temp->ball, 1) == 1){
						for(i=0; i<3; i++){
							blist->ball->vel[i] += 1.9*netVel*dir[i]*(temp->ball->mass / sumMass);
							temp->ball->vel[i] -= 1.9*netVel*dir[i]*(blist->ball->mass / sumMass);
						}
					}
					else{
						spring = 1.0 - ((blist->ball->outR + temp->ball->outR - len) / ((blist->ball->outR - blist->ball->inR) + (temp->ball->outR - temp->ball->inR)));
						spring = ((1 / ( sqrtf(spring) ) ) - 1);
						for(i=0; i<3; i++){
							blist->ball->vel[i] += spring*dir[i]*(temp->ball->mass / sumMass);
							temp->ball->vel[i] -= spring*dir[i]*(blist->ball->mass / sumMass);
						}
						if(u == numUpdates-1){
							blist->ball->squishFlag = 1;
							temp->ball->squishFlag = 1;

							tot = blist->ball->outR + temp->ball->outR - len;
							sq2 = tot / ( ((blist->ball->outR - blist->ball->inR) / (temp->ball->outR - temp->ball->inR)) + 1);
							sq1 = tot - sq2;

							if(blist->ball->squishAmount < sq1){
								blist->ball->squishAmount = sq1;
								for(i=0; i<3; i++){
									blist->ball->squishDir[i] = dir[i];
								}
							}
							if(temp->ball->squishAmount < sq2){
								temp->ball->squishAmount = sq2;
								for(i=0; i<3; i++){
									temp->ball->squishDir[i] = dir[i];
								}
							}
						}
					}
				}
				temp = temp->next;
			}

			/*while(wlist != NULL){ not quite right walls code
				for(i=0; i<6; i++){
				//	printf("face %i, norm=(%f,%f,%f),\npt0=(%f,%f,%f), pt1=(%f,%f,%f),\nvector0=(%f,%f,%f), vector1=(%f,%f,%f),\nvector2=(%f,%f,%f), vector3=(%f,%f,%f),\nl=%f, w=%f\n\n", i, wlist->wall->faces[i].norm[0], wlist->wall->faces[i].norm[1], wlist->wall->faces[i].norm[2], wlist->wall->faces[i].pts[0][0], wlist->wall->faces[i].pts[0][1], wlist->wall->faces[i].pts[0][2], wlist->wall->faces[i].pts[1][0], wlist->wall->faces[i].pts[1][1], wlist->wall->faces[i].pts[1][2], wlist->wall->faces[i].vectors[0][0], wlist->wall->faces[i].vectors[0][1], wlist->wall->faces[i].vectors[0][2], wlist->wall->faces[i].vectors[1][0], wlist->wall->faces[i].vectors[1][1], wlist->wall->faces[i].vectors[1][2], wlist->wall->faces[i].vectors[2][0], wlist->wall->faces[i].vectors[2][1], wlist->wall->faces[i].vectors[2][2], wlist->wall->faces[i].vectors[3][0], wlist->wall->faces[i].vectors[3][1], wlist->wall->faces[i].vectors[3][2], wlist->wall->faces[i].l, wlist->wall->faces[i].w);
					dis = -1*((wlist->wall->faces[i].pts[0][0] - blist->ball->pos[0])*wlist->wall->faces[i].norm[0]) +
								-1*((wlist->wall->faces[i].pts[0][1] - blist->ball->pos[1])*wlist->wall->faces[i].norm[1]) +
								-1*((wlist->wall->faces[i].pts[0][2] - blist->ball->pos[2])*wlist->wall->faces[i].norm[2]);
					if(dis >= 0 && dis <= blist->ball->outR){
						for(j=0; j<3; j++){
							pt[j] = blist->ball->pos[j] + -1*dis*wlist->wall->faces[i].norm[j];
						}
						for(j=0; j<2; j++){
							for(k=0; k<3; k++){
								cross[k] = (wlist->wall->faces[i].vectors[2*i][(k+1)%3]*( wlist->wall->faces[i].pts[0][(k+2)%3]-pt[(k+2)%3] )) - (wlist->wall->faces[i].vectors[2*i][(k+2)%3]*( wlist->wall->faces[i].pts[0][(k+1)%3]-pt[(k+1)%3] ));
							}
							dsq[j] = (cross[0]*cross[0] + cross[1]*cross[1] + cross[2]*cross[2]) / wlist->wall->faces[i].l;
							for(k=0; k<3; k++){
								cross[k] = (wlist->wall->faces[i].vectors[2*i+1][(k+1)%3]*( wlist->wall->faces[i].pts[1][(k+2)%3]-pt[(k+2)%3] )) - (wlist->wall->faces[i].vectors[2*i+1][(k+2)%3]*( wlist->wall->faces[i].pts[1][(k+1)%3]-pt[(k+1)%3] ));
							}
							dsq[j+2] = (cross[0]*cross[0] + cross[1]*cross[1] + cross[2]*cross[2]) / wlist->wall->faces[i].w;
						}
					
						dsqrect = 0;
						if(dsq[0] > wlist->wall->faces[i].w*wlist->wall->faces[i].w && dsq[2] > wlist->wall->faces[i].w*wlist->wall->faces[i].w){
							if(dsq[0] > dsq[2]){
								dsqrect += dsq[2];
							}
							else{
								dsqrect += dsq[0];
							}
						}
						if(dsq[1] > wlist->wall->faces[i].l*wlist->wall->faces[i].l && dsq[3] > wlist->wall->faces[i].l*wlist->wall->faces[i].l){
							if(dsq[1] > dsq[3]){
								dsqrect += dsq[3];
							}
							else{
								dsqrect += dsq[1];
							}
						}
						distance = sqrt(dis*dis + dsqrect);

						if(distance < blist->ball->inR){
							netVel = 0;
							for(j=0; j<3; j++){
								netVel += wlist->wall->faces[i].norm[j]*blist->ball->vel[j];
								printf("vel is %f\n", blist->ball->vel[j]);
							}
							netVel = fabs(netVel);
							printf("netvel is %f\n", netVel);
							for(j=0; j<3; j++){
								blist->ball->vel[j] += 1.9*netVel*wlist->wall->faces[i].norm[j];
							}
						}


					}
				}			

				wlist = wlist->next;
			}*/

			blist->ball->vel[1] -= 0.1*stepSize; //gravity

// below can be used to fake walls
			/*if( (blist->ball->pos[1]-blist->ball->outR)<0 ){
				blist->ball->vel[1] = .9*fabs(blist->ball->vel[1]);
			}

			if( (blist->ball->pos[0]-blist->ball->outR)<-10){
				blist->ball->vel[0] = .9*fabs(blist->ball->vel[0]);
			}

			if( (blist->ball->pos[0]+blist->ball->outR)>10 ){
				blist->ball->vel[0] = -.9*fabs(blist->ball->vel[0]);
			}

			if( (blist->ball->pos[2]-blist->ball->outR)<-10 ){
				blist->ball->vel[2] = .9*fabs(blist->ball->vel[2]);
			}

			if( (blist->ball->pos[2]+blist->ball->outR)>10 ){
				blist->ball->vel[2] = -.9*fabs(blist->ball->vel[2]);
			}*/

			if(u==0){
				for(i=0; i<3; i++){ //air resistance
					blist->ball->vel[i] = 0.99*blist->ball->vel[i];
				}
			}

			blist = blist->next;
		}
	}
	return;
}
