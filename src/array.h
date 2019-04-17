#ifndef __ARRAY_H__
#define __ARRAY_H__

int contains(int *array, int val, int pos); 
void swapD(double *x, double *y);
void swapI(int *x, int *y);
void permute(int **allOrder, int *permChain, int n);
void rankElements(double *cost, int *rank, int n); 
void setMatrixSizes(int *sizes, int *copySizes, int n, int min, int max);
void extractSizesFromFile(char *file, int *sizes, int *copySizes, int n); 
void resetCopySizes(int *sizes, int *copySizes, int n);

#endif
