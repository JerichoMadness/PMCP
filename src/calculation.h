#include "binarytree.h"

#ifndef __CALCULATION_H__
#define __CALCULATION_H__

double calculateChainSequential(double **A, double **interRes, int *sizes, struct node *nd);
double calculateChainSingleParallel(double **A, double **interRes, int *sizes, struct node *nd);
void multiplyMatrix(double **A, double **interRes, int *sizes, struct node *nd);
void processTree(double **A, double **interRes, int *sizes, struct node *nd);
double calculateChainTaskParallel(double **A, double **interRes, int *sizes, struct node *root);
 

#endif
