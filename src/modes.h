#ifndef __MODES_H__
#define __MODES_H__

void specMode(struct node **allTree, double **A, double **copyA, double **interRes, int *sizes, int *copySizes, int **allOrder, double *orderCostFP, int *rankFP, double *orderCostMEM, int *rankMEM, char *statString, char *sizeString, int numOrder, int numCol, char mode);
 void allMode(struct node **allTree, double **A, double **copyA, double **interRes, int *sizes, int *copySizes, int **allOrder, double *orderCostFP, int *rankFP, double *orderCostMEM, int *rankMEM, char *statString, char *sizeString, int numCol, int numOrder, char mode);

#endif
