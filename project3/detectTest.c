#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "image.h"
#include "ppmIO.h"
#include "edgeDetect.h"

int main(int argc, char *argv[]) {
	Image *src, *blur5, *edge0, *edge1, *edge2, *triEdge;
	int i, j;
	char inFile[50] = "computer.ppm";
	char outFileTriEdge[50] = "computeredgefew.ppm";

	src = image_read(inFile);
	blur5 = gaussianBlur(src, 5);

	edge0 = edgeDetect(blur5, 0);
	edge1 = edgeDetect(blur5, 1);
	edge2 = edgeDetect(blur5, 2);

	triEdge = image_create(edge0->rows, edge0->cols);
	image_reset(triEdge);
	
	for(i=0; i<edge0->cols; i++){
		for(j=0; j<edge0->rows; j++){
			if(image_getc(edge0, j, i, 0) == 1.0){
				image_setc(triEdge, j, i, 0, 1.0);
			}
			if(image_getc(edge1, j, i, 1) == 1.0){
				image_setc(triEdge, j, i, 1, 1.0);
			}
			if(image_getc(edge2, j, i, 2) == 1.0){
				image_setc(triEdge, j, i, 2, 1.0);
			}
		}
	}

	image_write(triEdge, outFileTriEdge);

	image_free(triEdge);
	image_free(edge0);
	image_free(edge1);
	image_free(edge2);
	image_free(blur5);
	image_free(src);
	return(0);
}
