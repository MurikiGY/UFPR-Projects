#ifndef __ORIGINAL__
#define __ORIGINAL__

typedef unsigned short mtype;

mtype **allocateScoreMatrix(int sizeA, int sizeB);

void initScoreMatrix(mtype ** scoreMatrix, int sizeA, int sizeB);

int LCS(mtype **scoreMatrix, int sizeA, int sizeB, char * seqA, char *seqB);

void printMatrix(char * seqA, char * seqB, mtype ** scoreMatrix, int sizeA, int sizeB);

void freeScoreMatrix(mtype **scoreMatrix, int sizeB);
#endif
