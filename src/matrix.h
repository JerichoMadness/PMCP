#ifndef __MATRIX_H__
#define __MATRIX_H__

void resetMatricesCopy(double **A, double **copyA, int *sizes, int n);
void setupMatrices(double **A, double **copyA, double **interRes, int *sizes, int n);
void initializeMatrices (double **A, double **copyA, int *sizes, int n);
void getAllOrders(int **allOrder, int n);
void convertOrders(int **allOrder, int n);
void computeChainCosts(int **allOrder, int *orderCost, int *normSizes, int n, int fac, char cf);
void computeChainOrder(int **split, double **cost, int *normSizes, int n, char cf);
void setupInterMatrices(double **interRes, int *order, int *sizes, int n);

#endif
