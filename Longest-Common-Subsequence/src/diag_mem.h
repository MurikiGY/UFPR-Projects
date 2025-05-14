#ifndef __DIAG_MEM__
#define __DIAG_MEM__

typedef unsigned short mtype;

mtype **diagAllocateScoreMatrix(int sizeA, int sizeB);

void diagInitScoreMatrix(mtype ** scoreMatrix, int sizeA, int sizeB);

int diagMemLCS(mtype **scoreMatrix, int sizeA, int sizeB, char * seqA, char *seqB, int nthds);

void diagFreeScoreMatrix(mtype **scoreMatrix, int sizeA, int sizeB);

#endif
