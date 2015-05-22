#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include "lSystem.h"
#include <string.h>

// Major note Syntax Error in text file can lead to Seg Faults

LSystem lSystem_read(char *filename){
	LSystem lSys;
	FILE *fp;
	long length;
	char *fileText;
	const char emptyString[100] = "";
	char dataString[100]; // may need to expand
	char lineType;
	int i, ruleLoc, firstRule, rule;

	fp = fopen(filename, "rb");
	
	if(fp){
		fseek(fp, 0L, SEEK_END);
		length = ftell(fp);
		rewind(fp);
		fileText = malloc(length + 1);
		if(fileText){
			fread(fileText, 1, length, fp);		
		}
		fclose(fp);
	}	
	
	lSys.n = 0;
	lSys.i = 0;
	lSys.angle = 0;
	lSys.string = NULL;
	lSys.numRules = 0;
	lSys.rules = NULL;

	if(fileText){
		printf("file contents:\n%s\n", fileText);
		lineType = 'x';
		strcpy(dataString, emptyString);	
		firstRule = -1;

		for(i=0; fileText[i] != 0; i++){
			switch(lineType){
				case 'x':
					if(fileText[i] == 'r'){
						lineType = 'r';
					}
					else if(fileText[i] == 'n'){
						lineType = 'n';
					}
					else if(fileText[i] == 'a'){
						lineType = 'a';
					}
					else if(fileText[i] == 's'){
						lineType = 's';
					}
					break;

				case 'n':
					if(fileText[i] == ';'){
						lSys.n = atoi(dataString);
						strcpy(dataString, emptyString);	
						lineType = 'x';
					}
					else if(fileText[i] != ':'){
						dataString[strlen(dataString) + 1] = '\0';
						dataString[strlen(dataString)] = fileText[i];
					}
					break;
			
				case 'a':
					if(fileText[i] == ';'){
						lSys.angle = atof(dataString);
						strcpy(dataString, emptyString);	
						lineType = 'x';
					}
					else if(fileText[i] != ':'){
						dataString[strlen(dataString) + 1] = '\0';
						dataString[strlen(dataString)] = fileText[i];
					}
					break;

				case 's':
					if(fileText[i] == ';'){
						lSys.string = malloc(sizeof(char)*(strlen(dataString)+1));
						strcpy(lSys.string, dataString);
						strcpy(dataString, emptyString);	
						lineType = 'x';
					}
					else if(fileText[i] != ':'){
						dataString[strlen(dataString) + 1] = '\0';
						dataString[strlen(dataString)] = fileText[i];
					}
					break;

				case 'r':
					if(firstRule == -1){
						firstRule = i;
					}
					
					if(fileText[i] == ';'){
						lSys.numRules += 1;
					}
					break;

				default:
					printf("invalid lineType: %c\n", lineType);
					break;
			}
		} 
		
		if(firstRule != -1){
			lSys.rules = malloc(sizeof(Rule) * lSys.numRules);
			lineType = 'r';
			rule = 0;
			ruleLoc = 0;
			strcpy(dataString, emptyString);	

			for(i = firstRule; fileText[i] != 0; i++){
				if(lineType == 'x' && fileText[i] == 'r'){
					lineType = 'r';
				}
				else if(lineType == 'r'){
					if(ruleLoc == 0){
						if(fileText[i] == ')'){
							lSys.rules[rule].prob = atoi(dataString);
							ruleLoc += 1;
							strcpy(dataString, emptyString);	
						}
						else if(fileText[i] != ':' && fileText[i] != '('){
							dataString[strlen(dataString) + 1] = '\0';
							dataString[strlen(dataString)] = fileText[i];
						}
					}
					else if(ruleLoc == 1){
						if(fileText[i] == ':'){
							lSys.rules[rule].query = malloc(sizeof(char)*(strlen(dataString)+1));
							strcpy(lSys.rules[rule].query, dataString);
							ruleLoc += 1;
							strcpy(dataString, emptyString);	
						}
						else{
							dataString[strlen(dataString) + 1] = '\0';
							dataString[strlen(dataString)] = fileText[i];
						}
					}
					else if(ruleLoc == 2){
						if(fileText[i] == ';'){
							lSys.rules[rule].replacement = malloc(sizeof(char)*(strlen(dataString)+1));
							strcpy(lSys.rules[rule].replacement, dataString);
							ruleLoc = 0;
							rule += 1;
							strcpy(dataString, emptyString);	
							lineType = 'x';
						}
						else{
							dataString[strlen(dataString) + 1] = '\0';
							dataString[strlen(dataString)] = fileText[i];
						}
					}
				}
			}
		}
	}
	return lSys;
}

void lSystem_print(LSystem *lSys, FILE *fp){
	int i;

	fprintf(fp, "L-System:\n");
	fprintf(fp, " n: %i  i: %i\n", lSys->n, lSys->i);
	fprintf(fp, " angle: %f\n", lSys->angle);
	fprintf(fp, " start string: %s\n", lSys->string);
	fprintf(fp, " rules: \n");
	for(i=0; i<lSys->numRules; i++){
		fprintf(fp, "  %i%: %s->%s\n", lSys->rules[i].prob, lSys->rules[i].query, lSys->rules[i].replacement);
	}
}

void lSystem_reset(LSystem *lSys){
	int i;
	
	if(lSys->rules){
		for(i=0; i<lSys->numRules; i++){
			free(lSys->rules[i].query);
			free(lSys->rules[i].replacement);
		}
		free(lSys->rules);
		lSys->rules = NULL;
	}

	if(lSys->string){
		free(lSys->string);
		lSys->string = NULL;
	}	
	lSys->n = 0;
	lSys->i = 0;
	lSys->angle = 0;
	lSys->numRules = 0;
}

int lSystem_checkProbs(LSystem *lSys){
	int i, j, uniqueSeen, flagUnique;
	int unique[lSys->numRules];
	int probs[lSys->numRules];

	uniqueSeen = 0;

	for(i=0; i<lSys->numRules; i++){
		flagUnique = 0;
		for(j=0; j<uniqueSeen; j++){
			if(strcmp(lSys->rules[i].query, lSys->rules[unique[j]].query) == 0){
				flagUnique = 1;
				probs[j] += lSys->rules[i].prob;
			}
		}
		if(flagUnique == 0){
			unique[uniqueSeen] = i;
			probs[uniqueSeen] = lSys->rules[i].prob;
			uniqueSeen += 1;
		}
	}

	j = 0;
	for(i=0; i<uniqueSeen; i++){
		if(probs[i] != 100){
			j += 1;
		}
	}
	return j;
}

LSystem lSystem_copy(LSystem *lSys){
	LSystem newLSys;
	int len, i;

	newLSys.n = lSys->n;
	newLSys.i = lSys->i;
	newLSys.angle = lSys->angle;

	newLSys.string = malloc(sizeof(char)*(strlen(lSys->string) + 1));
	strcpy(newLSys.string, lSys->string);
	len = lSys->numRules;
	newLSys.numRules = len;

	newLSys.rules = malloc(sizeof(Rule) * len);

	for(i=0; i<len; i++){
		newLSys.rules[i].query = malloc(sizeof(char)*(strlen(lSys->rules[i].query) + 1));
		strcpy(newLSys.rules[i].query, lSys->rules[i].query);

		newLSys.rules[i].prob = lSys->rules[i].prob;
		newLSys.rules[i].replacement = malloc(sizeof(char)*(strlen(lSys->rules[i].replacement) + 1));
		strcpy(newLSys.rules[i].replacement, lSys->rules[i].replacement);
	}

	return newLSys;
}

// unknown (bad) reaction for querys that have conflicting parts
// ex: F->Y and [F]->B
LSystem lSystem_nextIteration(LSystem *lSys){ 
	LSystem newLSys;
	int i, j, k, l, len, correctFlag, prob, sumProb, replace;
	char *newString;

	len = 0; 
	for(i=0; i<lSys->numRules; i++){
		if( len < strlen(lSys->rules[i].replacement) ){
			len = strlen(lSys->rules[i].replacement);
		}
	}
	newString = malloc(sizeof(char) * len * (strlen(lSys->string) + 1));
	len = strlen(lSys->string);
	l = 0;
	for(i=0; i<len; i++){
		prob = rand() % 100;
		sumProb = 0;
		replace = 0;
		for(j=0; j<lSys->numRules; j++){
			correctFlag = 0;
			for(k=0; k<strlen(lSys->rules[j].query); k++){
				if(i+k < len){
					if(lSys->string[i+k] != lSys->rules[j].query[k]){
						correctFlag = 1;
					}
				}
			}
			if(correctFlag == 0){
				sumProb += lSys->rules[j].prob;
				if(sumProb > prob && replace == 0){
					replace = 1;
					for(k=0; k<strlen(lSys->rules[j].replacement); k++){
						newString[l] = lSys->rules[j].replacement[k];
						l += 1;
						replace += 1;
					}
				}
			}
		}
		if(replace == 0){
			newString[l] = lSys->string[i];
			l += 1;
		}
		else{
			replace += -1;
		}
	}

	newString[l] = '\0';

	newLSys.n = lSys->n;
	newLSys.i = lSys->i + 1;
	newLSys.angle = lSys->angle;
	newLSys.string = malloc(sizeof(char)*(strlen(newString) + 1));
	strcpy(newLSys.string, newString);
	len = lSys->numRules;
	newLSys.numRules = len;
	newLSys.rules = malloc(sizeof(Rule) * len);
	for(i=0; i<len; i++){
		newLSys.rules[i].query = malloc(sizeof(char)*(strlen(lSys->rules[i].query) + 1));
		strcpy(newLSys.rules[i].query, lSys->rules[i].query);

		newLSys.rules[i].prob = lSys->rules[i].prob;

		newLSys.rules[i].replacement = malloc(sizeof(char)*(strlen(lSys->rules[i].replacement) + 1));
		strcpy(newLSys.rules[i].replacement, lSys->rules[i].replacement);
	}

	free(newString);

	return newLSys;
}

LSystem lSystem_allIterations(LSystem *lSys){
	LSystem newLSys, temp;

	newLSys = lSystem_copy(lSys);
	while(newLSys.i < newLSys.n){
		temp = lSystem_nextIteration(&newLSys);
		lSystem_reset(&newLSys);
		newLSys = lSystem_copy(&temp);
		lSystem_reset(&temp);
	}
	return newLSys;
}

MatrixRecord* matrixRecord_push(MatrixRecord *mRec, MatrixType type, double vector[3], double angle){
	MatrixRecord *newRec;
	int i;

	newRec = malloc(sizeof(MatrixRecord));
	newRec->type = type;
	for(i=0; i<3; i++){
		newRec->vector[i] = vector[i];
	}
	newRec->angle = angle;

	newRec->next = mRec;

	return newRec;
}

MatrixRecord* matrixRecord_pop(MatrixRecord *mRec){
	MatrixRecord *newRec;;

	if(mRec){
		newRec = mRec->next;
		free(mRec);
	}
	else{
		printf("passed in a NULL mRec\n");
	}

	return newRec;
}

TurtleStack* turtleStack_push(TurtleStack *tStack){
	TurtleStack *newTstack;

	newTstack = malloc(sizeof(TurtleStack));
	newTstack->matricies = malloc(sizeof(MatrixRecord));
	newTstack->matricies->type = MatrixNone;
	newTstack->matricies->next = NULL;
	newTstack->next = tStack;

	return newTstack;
}

TurtleStack* turtleStack_pop(TurtleStack *tStack){
	TurtleStack *newTstack;

	if(tStack){
		newTstack = tStack->next;
		free(tStack);
	}
	else{
		printf("too many ] in string\n");
	}

	return newTstack;
}

void lSystem_draw(LSystem *lSys){
	double angle, width, length;
	int i, j;	
	double vect[3];
	GLUquadric *cylinder;
	GLUquadric *sphere;
	TurtleStack *tStack;
	GLfloat brown[] = {.5f,.3f,.1f};
	GLfloat green[] = {.2f,.8f,.3f};
	GLfloat red[] = {.8f,.3f,.2f};

	angle = lSys->angle;	
	width = 0.3;
	length = 2;

	sphere = gluNewQuadric();
	cylinder = gluNewQuadric();

	tStack = NULL;

	for(i=0; i<strlen(lSys->string); i++){
		switch(lSys->string[i]){
			case 'F':
				glMaterialfv(GL_FRONT, GL_DIFFUSE, brown);
				gluCylinder(cylinder, width, width, length, 13, 4);
				glTranslatef(0,0, length);
				gluSphere(sphere, width, 23, 23);
				vect[0] = 0;
				vect[1] = 0;
				vect[2] = length;
				if(tStack){
					tStack->matricies = matrixRecord_push(tStack->matricies, MatrixTranslate, vect, angle);
				}
				break;

			case 'f':
				glTranslatef(0,0, length);		
				vect[0] = 0;
				vect[1] = 0;
				vect[2] = length;
				if(tStack){
					tStack->matricies = matrixRecord_push(tStack->matricies, MatrixTranslate, vect, angle);
				}
				break;

			case 'L':
				glMaterialfv(GL_FRONT, GL_DIFFUSE, green);
				gluSphere(sphere, 0.5, 23, 23);
				break;

			case '`':
				width = width * .9;
				break;

			case '~':
				length = length * .95;
				break;

			case '+':
				glRotatef(angle, 0, 1, 0);
				vect[0] = 0;
				vect[1] = 1;
				vect[2] = 0;
				if(tStack){
					tStack->matricies = matrixRecord_push(tStack->matricies, MatrixRotate, vect, angle);
				}
				break;

			case '-':
				glRotatef(-1*angle, 0, 1, 0);
				vect[0] = 0;
				vect[1] = 1;
				vect[2] = 0;
				if(tStack){
					tStack->matricies = matrixRecord_push(tStack->matricies, MatrixRotate, vect, -1*angle);
				}
				break;

			case '#':
				glRotatef(angle, 0, 0, 1);
				vect[0] = 0;
				vect[1] = 0;
				vect[2] = 1;
				if(tStack){
					tStack->matricies = matrixRecord_push(tStack->matricies, MatrixRotate, vect, angle);
				}
				break;

			case '$':
				glRotatef(-1*angle, 0, 0, 1);
				vect[0] = 0;
				vect[1] = 0;
				vect[2] = 1;
				if(tStack){
					tStack->matricies = matrixRecord_push(tStack->matricies, MatrixRotate, vect, -1*angle);
				}
				break;

			case '&':
				glRotatef(angle, 1, 0, 0);
				vect[0] = 1;
				vect[1] = 0;
				vect[2] = 0;
				if(tStack){
					tStack->matricies = matrixRecord_push(tStack->matricies, MatrixRotate, vect, angle);
				}
				break;

			case '^':
				glRotatef(-1*angle, 1, 0, 0);
				vect[0] = 1;
				vect[1] = 0;
				vect[2] = 0;
				if(tStack){
					tStack->matricies = matrixRecord_push(tStack->matricies, MatrixRotate, vect, -1*angle);
				}
				break;
			
			case '|':
				glRotatef(180, 0, 1, 0);
				vect[0] = 0;
				vect[1] = 1;
				vect[2] = 0;
				if(tStack){
					tStack->matricies = matrixRecord_push(tStack->matricies, MatrixRotate, vect, 180);
				}
				break;


			// brackets do not work yet
			case '[':
				tStack = turtleStack_push(tStack);
				tStack->width = width;
				tStack->length = length;
				break;
				
			case ']':
				if(tStack){
					while(tStack->matricies->type != MatrixNone){
						switch (tStack->matricies->type){
							case MatrixNone:
								printf("something went wrong type should not be MatrixNone\n");
								break;

							case MatrixTranslate:
								glTranslatef(-1*tStack->matricies->vector[0], -1*tStack->matricies->vector[1], -1*tStack->matricies->vector[2]);
								break;

							case MatrixRotate:
								glRotatef(-1*tStack->matricies->angle, tStack->matricies->vector[0], tStack->matricies->vector[1], tStack->matricies->vector[2]);
								break;

						}
						tStack->matricies = matrixRecord_pop(tStack->matricies);
					}
					width = tStack->width;
					length = tStack->length;
					tStack = turtleStack_pop(tStack);
				}
				else{
					printf("tStack is empty cannot get values\n");
				}
				break;
				
		}
	}
}




