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

#define CACHESIZE 30000000

/* Function to calculate the matrix chain and measure the time needed
 *
 * Arguments:
 *
 * A = Matrix Array
 * interRes = Matrix Array for the intermediate results
 * order = Multiplication order
 * sizes = Matrix sizes
 * N = Number of matrices
 *
 */

double calculateChainIterative(double **A, double **interRes, int *order, int *sizes, int N)  {

    double timeB4, timeAfter, timeSum;
    timeSum = 0.;

    int i;

    int posX,posY;

    double alpha,beta;
    alpha = 1.0;
    beta = 0.0;
    int m,n,k;

    //Clean cache first to remove any prior data on it

    cache_scrub(CACHESIZE);

    for(i=0;i<N-1;i++) {

        //printf("Still working at the start of it %d\n\n", i);

        posX = order[2*i];
        posY = order[2*i+1];

        m = sizes[posX];
        k = sizes[posX+1];
        n = sizes[posY+1];
        
        if((A[posX] == NULL) || (A[posY] == NULL) || (interRes[i]) == NULL)
            printf("Error! One of the matrices is empty!");

        printf("Using matrices %d(%dx%d) and %d(%dx%d)\n\n",posX,m,k,posY,k,n);

        timeB4 = bli_clock();

        cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, m, n, k, alpha, A[posX], k, A[posY], n, beta, interRes[i], n);

        //printf("Finished computing\n\n");

        timeAfter = bli_clock();

        timeSum = timeSum + (timeAfter - timeB4);
        
        printf("Intermediate results: [%lf], %lfs\n\n", interRes[i][0], (timeAfter - timeB4));

        double* pointer;
        pointer =  mkl_realloc(A[posY],m*n*sizeof(double));
        if (pointer == NULL)
            printf("Error allocating!\n\n");
        A[posY] = pointer;
        //printf("Realloced\n");
         
        memcpy(A[posY],interRes[i],m*n*sizeof(double));
        //printf("Repositioned\n");
        
        sizes[posY] = m;

        //printf("Still working at the end of it %d\n\n", i);    

    }

    printf("Results: %lf\n\n",timeSum); 
   
    return timeSum;
     
}

