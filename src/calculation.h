#include "binarytree.h"

#ifndef __CALCULATION_H__
#define __CALCULATION_H__

double calculateChainSequential(double **A, double **interRes, int *sizes, struct node *nd, char mode);
void multiplyMatrix(double **A, double **interRes, int *sizes, struct node *nd, char mode);
void processTree(double **A, double **interRes, int *sizes, struct node *nd, char mode);
double calculateChainTaskParallel(double **A, double **interRes, int *sizes, struct node *root, char mode);
 

#endif
