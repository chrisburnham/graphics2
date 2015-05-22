#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "image.h"
#include "ppmIO.h"
#include "edgeDetect.h"

void gaussianBlur3(Image *src, Image *dest){
	int i, j, b;
	if( (src->rows == (dest->rows + 2)) && (src->cols == (dest->cols + 2)) ){
		for(i=0; i<dest->cols; i++){
			for(j=0; j<dest->rows; j++){
				for(b=0; b<3; b++){
					image_setc(dest, j, i, b, 
							(0.01*image_getc(src, j, i, b))+(0.08*image_getc(src, j, i+1, b))+(0.01*image_getc(src, j, i+2, b))+
							(0.08*image_getc(src, j+1, i, b))+(0.64*image_getc(src, j+1, i+1, b))+(0.08*image_getc(src, j+1, i+2, b))+
							(0.01*image_getc(src, j+2, i, b))+(0.08*image_getc(src, j+2, i+1, b))+(0.01*image_getc(src, j+2, i+2, b)) );
				}
			}
		}
	}
	else{
		printf("src and dest images wrong sizes\nsrc is %ix%i and dest is %ix%i\nsrc should have 2 extra\n", src->rows, src->cols, dest->rows, dest->cols);
	}
}

void gaussianBlur5(Image *src, Image *dest){
	int i, j, b;

	if( (src->rows == (dest->rows + 4)) && (src->cols == (dest->cols + 4)) ){
		for(i=0; i<dest->cols; i++){
			for(j=0; j<dest->rows; j++){
				for(b=0; b<3; b++){
					image_setc(dest, j, i, b, 
						(0.01*image_getc(src, j, i, b))+(0.02*image_getc(src, j, i+1, b))+(0.04*image_getc(src, j, i+2, b))+(0.02*image_getc(src, j, i+3, b))+(0.01*image_getc(src, j, i+4, b))+
						(0.02*image_getc(src, j+1, i, b))+(0.04*image_getc(src, j+1, i+1, b))+(0.08*image_getc(src, j+1, i+2, b))+(0.04*image_getc(src, j+1, i+3, b))+(0.02*image_getc(src, j+1, i+4, b))+
						(0.04*image_getc(src, j+2, i, b))+(0.08*image_getc(src, j+2, i+1, b))+(0.16*image_getc(src, j+2, i+2, b))+(0.08*image_getc(src, j+2, i+3, b))+(0.04*image_getc(src, j+2, i+4, b))+
						(0.02*image_getc(src, j+3, i, b))+(0.04*image_getc(src, j+3, i+1, b))+(0.08*image_getc(src, j+3, i+2, b))+(0.04*image_getc(src, j+3, i+3, b))+(0.02*image_getc(src, j+3, i+4, b))+
						(0.01*image_getc(src, j+4, i, b))+(0.02*image_getc(src, j+4, i+1, b))+(0.04*image_getc(src, j+4, i+2, b))+(0.02*image_getc(src, j+4, i+3, b))+(0.01*image_getc(src, j+4, i+4, b)) );
				}
			}
		}
	}
	else{
		printf("src and dest images wrong sizes\nsrc is %ix%i and dest is %ix%i\nsrc should have 4 extra\n", src->rows, src->cols, dest->rows, dest->cols);
	}
}

Image *gaussianBlur(Image *src, int size){
	Image *toReturn;

	switch(size){
		case 3:
			toReturn = image_create((src->rows - 2),(src->cols - 2));
			gaussianBlur3(src, toReturn);
			break;

		case 5:
			toReturn = image_create((src->rows - 4),(src->cols - 4));
			gaussianBlur5(src, toReturn);
			break;

		default:
			printf("size must be 3 or 5\n");
			toReturn = NULL;
			break;
	}
	return toReturn;
}

static void hysteresis(int *types, int r, int c, int rows, int cols){
	int i, j;

	if( 0<r && r<(rows-1) && 0<c && c<(cols-1) ){
		if(	types[((r-1)*cols)+c-1]==2 || types[((r-1)*cols)+c]==2 || types[((r-1)*cols)+c+1]==2 ||
				types[(r*cols)+c-1]==2 || types[(r*cols)+c]==2 || types[(r*cols)+c+1]==2 ||
				types[((r+1)*cols)+c-1]==2 || types[((r+1)*cols)+c]==2 || types[((r+1)*cols)+c+1]==2){
			types[(r*cols)+c] = 2;
			for(i=-1; i<2; i++){
				for(j=-1; j<2; j++){
					if(types[((r+j)*cols)+c+i]==1){
						hysteresis(types, r+j, c+i, rows, cols);
					}
				}
			}
		}
	}
}

Image *edgeDetect(Image *src, int b){
	Image *toReturn;
	int i, j, c;
	float gx, gy;
	float **edgeDir;
	float **gradStr;
	int **edgeType;
	const float weakThresh = 0.08;
	const float strongThresh = 0.8;

	edgeDir = malloc(sizeof(float *) * (src->rows - 2));
	gradStr = malloc(sizeof(float *) * (src->rows - 2));

	edgeDir[0] = malloc(sizeof(float) * (src->rows - 2) * (src->cols - 2));
	gradStr[0] = malloc(sizeof(float) * (src->rows - 2) * (src->cols - 2));

	for(j=0; j<(src->rows - 2); j++){
		if(j != 0){
			edgeDir[j] = &( edgeDir[0][j*(src->cols - 2)] );
			gradStr[j] = &( gradStr[0][j*(src->cols - 2)] );
		}
		for(i=0; i<(src->cols - 2); i++){
	    gx = (-1*image_getc(src, j, i, b)) + (image_getc(src, j, i+2, b)) +
	      (-2*image_getc(src, j+1, i, b)) + (2*image_getc(src, j+1, i+2, b)) +
	      (-1*image_getc(src, j+2, i, b)) + (image_getc(src, j+2, i+2, b));
	    gy = (image_getc(src,j, i, b)) + (2*image_getc(src,j, i+1, b)) + (image_getc(src,j, i+2, b)) +
	      (-1*image_getc(src,j+2, i, b)) + (-2*image_getc(src,j+2, i+1, b)) + (-1*image_getc(src,j+2, i+2, b));

			gradStr[j][i] = fabsf(gx) + fabsf(gx);
			edgeDir[j][i] = atanf(gy / gx);
		}
	}
	
	edgeType = malloc(sizeof(int *) * (src->rows - 4));

	edgeType[0] = malloc(sizeof(int) * (src->rows - 4) * (src->cols - 4));

	for(j=0; j<src->rows - 4; j++){	
		if(j != 0){
			edgeType[j] = &( edgeType[0][j*(src->cols - 4)] );
		}
		for(i=0; i<src->cols - 4; i++){
		
			if(edgeDir[j+1][i+1] > (3.0*M_PI/8.0) || edgeDir[j+1][j+1] < (-3.0*M_PI/8.0)){
				//vertical
				if(gradStr[j+1][i+1] > gradStr[j][i+1] && gradStr[j+1][i+1] > gradStr[j+2][i+1]){
					if(gradStr[j+1][i+1] > strongThresh){
						edgeType[j][i] = 2;
					}
					else if(gradStr[j+1][i+1] > weakThresh){ 
						edgeType[j][i] = 1;
					}
					else{
						edgeType[j][i] = 0;
					}
				}
				else{
					edgeType[j][i] = 0;
				}
			}
			else if(edgeDir[j+1][i+1] > (1.0*M_PI/8.0)){
				//up right
				if(gradStr[j+1][i+1] > gradStr[j][i+2] && gradStr[j+1][i+1] > gradStr[j+2][i]){
					if(gradStr[j+1][i+1] > strongThresh){
						edgeType[j][i] = 2;
					}
					else if(gradStr[j+1][i+1] > weakThresh){ 
						edgeType[j][i] = 1;
					}
					else{
						edgeType[j][i] = 0;
					}
				}
				else{
					edgeType[j][i] = 0;
				}
			}
			else if(edgeDir[j+1][i+1] > (-1.0*M_PI/8.0)){
				//horizontal
				if(gradStr[j+1][i+1] > gradStr[j+1][i] && gradStr[j+1][i+1] > gradStr[j+1][i+2]){
					if(gradStr[j+1][i+1] > strongThresh){
						edgeType[j][i] = 2;
					}
					else if(gradStr[j+1][i+1] > weakThresh){ 
						edgeType[j][i] = 1;
					}
					else{
						edgeType[j][i] = 0;
					}
				}
				else{
					edgeType[j][i] = 0;
				}
			}
			else{
				//down right
				if(gradStr[j+1][i+1] > gradStr[j][i] && gradStr[j+1][i+1] > gradStr[j+2][i+2]){
					if(gradStr[j+1][i+1] > strongThresh){
						edgeType[j][i] = 2;
					}
					else if(gradStr[j+1][i+1] > weakThresh){ 
						edgeType[j][i] = 1;
					}
					else{
						edgeType[j][i] = 0;
					}
				}
				else{
					edgeType[j][i] = 0;
				}
			}
		}
	}

	free(edgeDir[0]);
	free(gradStr[0]);

	free(edgeDir);
	free(gradStr);
	
	for(j=0; j<src->rows - 6; j++){
		for(i=0; i<src->cols - 6; i++){
			if(edgeType[j+1][i+1] == 1){
				hysteresis(edgeType[0], j+1, i+1, src->rows - 4, src->cols - 4);
			}
		}
	}

	toReturn = image_create(src->rows - 6, src->cols - 6);

	for(j=0; j<src->rows - 6; j++){
		for(i=0; i<src->cols - 6; i++){
			if(edgeType[j+1][i+1] == 2){
				for(c=0; c<3; c++){
					image_setc(toReturn, j, i, c, 1.0);
				}
			}
		}
	}

	free(edgeType[0]);
	free(edgeType);

	return toReturn; 
}




















