#include "binarytree.h"

#ifndef __CALCULATION_H__
#define __CALCULATION_H__

double calculateChainIterative(double **A, double **interRes, int *sizes, struct node *nd);
void multiplyMatrix(double **A, double **interRes, int *sizes, struct node *nd);
void processTree(double **A, double **interRes, int *sizes, struct node *nd);
double calculateChainParallel(double **A, double **interRes, int *sizes, struct node *root);
 

#endif
