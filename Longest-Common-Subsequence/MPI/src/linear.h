#ifndef __LINEAR__
#define __LINEAR__

typedef unsigned short mtype;

// Allocate LCS score matrix
mtype **LinearAllocateScoreMatrix(int sizeA, int sizeB, int my_rank, int n_tasks);

// Initialize LCS score matrix
void linearInitScoreMatrix(mtype **scoreMatrix, int sizeA, int sizeB, int my_rank, int n_tasks);

// Run LCS
void MPILinearMemLCS(mtype **scoreMatrix, int sizeA, int sizeB, char *seqA, char *seqB, int my_rank, int n_tasks);

// Free matrix
void LinearFreeScoreMatrix(mtype **scoreMatrix, int sizeA, int sizeB, int my_rank, int n_tasks);


#endif
