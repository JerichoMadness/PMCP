#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <math.h>
#include <limits.h>
#include <mkl.h>
#include <float.h>
#include <unistd.h>
#include <string.h>
#include <bli_clock.h>
#include <omp.h>
#include "binarytree.h"
#define CACHESIZE 30000000

/* Function to calculate the matrix chain and measure the time needed
 *
 * Arguments:
 *
 * A = Matrix Array
 * interRes = Matrix Array for the intermediate results
 * sizes = Matrix sizes
 * nd = Current node in tree
 * N = Number of matrices
 *
 */

double calculateChainIterative(double **A, double **interRes, int *sizes, struct node *nd, int N)  {

    double timeB4, timeAfter, timeSum;
    timeSum = 0.;

    if((nd->cLeft) != NULL) {
        timeSum = timeSum + calculateChainIterative(A,interRes,sizes,nd->cLeft,N);   
    }

    if((nd->cRight) != NULL) {
        timeSum = timeSum + calculateChainIterative(A,interRes,sizes,nd->cRight,N);
    }

    int posX,posY;

    double alpha,beta;
    alpha = 1.0;
    beta = 0.0;
    int m,n,k;

    int opPos = nd->opNum;

    posX = nd->mLeft;
    posY = nd->mRight;

    m = sizes[posX];
    k = sizes[posX+1];
    n = sizes[posY+1];
    
    if((A[posX] == NULL) || (A[posY] == NULL) || (interRes[opPos]) == NULL)
        printf("Error! One of the matrices is empty!");

    printf("Using matrices %d(%dx%d) and %d(%dx%d)\n\n",posX,m,k,posY,k,n);

    timeB4 = bli_clock();

    cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, m, n, k, alpha, A[posX], k, A[posY], n, beta, interRes[opPos], n);

    timeAfter = bli_clock();

    timeSum = timeSum + (timeAfter-timeB4);

    printf("Intermediate results: %lfs. Overall: %lfs\n\n", (timeAfter - timeB4), timeSum);

    double* pointer;
    pointer =  mkl_realloc(A[posY],m*n*sizeof(double));
    if (pointer == NULL)
        printf("Error allocating!\n\n");
    A[posY] = pointer;
    //printf("Realloced\n");
     
    memcpy(A[posY],interRes[opPos],m*n*sizeof(double));
    //printf("Repositioned\n");
    
    sizes[posY] = m;

    //printf("Still working at the end of it %d\n\n", i);    

    return timeSum;

}


void multiplyMatrix(double **A, double **interRes, int *sizes, struct node *nd, int N) {

    int posX,posY;

    double alpha,beta;
    alpha = 1.0;
    beta = 0.0;
    int m,n,k;

    int opPos = nd->opNum;

    posX = nd->mLeft;
    posY = nd->mRight;

    m = sizes[posX];
    k = sizes[posX+1];
    n = sizes[posY+1];
    
    if((A[posX] == NULL) || (A[posY] == NULL) || (interRes[opPos]) == NULL)
        printf("Error! One of the matrices is empty!");

    printf("Using matrices %d(%dx%d) and %d(%dx%d)\n\n",posX,m,k,posY,k,n);

    cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, m, n, k, alpha, A[posX], k, A[posY], n, beta, interRes[opPos], n);

    double* pointer;
    pointer =  mkl_realloc(A[posY],m*n*sizeof(double));
    if (pointer == NULL)
        printf("Error allocating!\n\n");
    A[posY] = pointer;
    //printf("Realloced\n");
     
    memcpy(A[posY],interRes[opPos],m*n*sizeof(double));
    //printf("Repositioned\n");
    
    sizes[posY] = m;

    //printf("Still working at the end of it %d\n\n", i);    

}

void processTree(double **A, double **interRes, int *sizes, struct node *nd, int N) {

    if(nd->cLeft != NULL) {
        #pragma omp task shared(A, interRes, sizes, N), private(nd)
        processTree(A,interRes,sizes,nd,N);   
    }

    if(nd->cRight != NULL) {
        #pragma omp task shared(A, interRes, sizes, N), private(nd)
        processTree(A,interRes,sizes,nd,N);
    }

    #pragma omp taskwait

    multiplyMatrix(A,interRes,sizes,nd,N);

}

double calculateChainParallel(double **A, double **interRes, int *sizes, struct node *root, int N)  {

    double timeB4, timeAfter, timeSum;
    timeSum = 0.;

    int i;

    //Clean cache first to remove any prior data on it

    cache_scrub(CACHESIZE);

    timeB4 = bli_clock();

    processTree(A, interRes, sizes, root, N);

    timeAfter = bli_clock();

    timeSum = timeAfter - timeB4;

    printf("Results: %lf\n\n",timeSum); 
   
    return timeSum;
     
}

