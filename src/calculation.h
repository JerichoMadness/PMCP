#include "binarytree.h"

#ifndef __CALCULATION_H__
#define __CALCULATION_H__

double calculateChainSequential(double **A, double **interRes, int *sizes, struct node *nd, char mode, int nthreads);
void multiplyMatrix(double **A, double **interRes, int *sizes, struct node *nd, char mode, int nthreads);
void processTree(double **A, double **interRes, int *sizes, struct node *nd, char mode, int nthreads);
double calculateChainTaskParallel(double **A, double **interRes, int *sizes, struct node *root, char mode, int nthreads);
 

#endif
