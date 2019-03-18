#include "binarytree.h"

#ifndef __CALCULATION_H__
#define __CALCULATION_H__

double calculateChainIterative(double **A, double **interRes, int *sizes, struct node *nd, int N);
void multiplyMatrix(double **A, double **interRes, int *sizes, struct node *nd, int N);
void processTree(double **A, double **interRes, int *sizes, struct node *nd, int N);
double calculateChainParallel(double **A, double **interRes, int *sizes, struct node *root, int N);
 

#endif
