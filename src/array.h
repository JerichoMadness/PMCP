#ifndef __ARRAY_H__
#define __ARRAY_H__

int contains(int *array, int val, int pos); 
void setMatrixSizes(int *sizes, int *copySizes, int n, int min, int max);
void resetCopySizes(int *sizes, int *copySizes, int n);
void normalizeSizes(int *sizes, int *normSizes, int sizeMin, int sizeMax, int n);
void swap(int *x, int *y);
void permute(int **allOrder, int *permChain, int n);
int getRecursiveChain(int **split, int *order,  int left, int right, int pos);
void getChainOrder(int **split, int *order, int n);
void setConsecutiveOrder(int *order, int n); 

#endif
