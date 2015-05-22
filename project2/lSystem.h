#ifndef LSYSTEM_H

#define LSYSTEM_H

typedef struct {
	char *query;
	int prob;
	char *replacement;
} Rule;

typedef struct {
	int n;
	int i;
	double angle;
	char *string;
	int numRules;
	Rule *rules;
} LSystem;

typedef enum {
	MatrixNone,
	MatrixTranslate,
	MatrixRotate
} MatrixType;

typedef struct matrix {
	MatrixType type;
	double vector[3];
	double angle;
	struct matrix *next;
} MatrixRecord;

typedef struct element {
	MatrixRecord *matricies;
	double width;
	double length;
	struct element *next;
} TurtleStack;


LSystem lSystem_read(char *filename);

void lSystem_print(LSystem *lSys, FILE *fp);

void lSystem_reset(LSystem *lSys);

int lSystem_checkProbs(LSystem *lSys);

LSystem lSystem_copy(LSystem *lSys);

LSystem lSystem_nextIteration(LSystem *lSys);

LSystem lSystem_allIterations(LSystem *lSys);

MatrixRecord* matrixRecord_push(MatrixRecord *mRec, MatrixType type, double vector[3], double angle);

MatrixRecord* matrixRecord_pop(MatrixRecord *mRec);

TurtleStack* turtleStack_push(TurtleStack *tStack);

TurtleStack* turtleStack_pop(TurtleStack *tStack);

void lSystem_draw(LSystem *lSys);


#endif
