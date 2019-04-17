#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include <mkl.h>
#include <float.h>
#include "bli_clock.h"


void cache_scrub(int csize) {

    int n = (int) sqrt(csize);
    n = n/8;

    double *A;
    A = malloc(n*n*sizeof(double));
    double *B;
    B = malloc(n*n*sizeof(double));
    double *C;    
    C = malloc(n*n*sizeof(double));

    int i;

    for(i=0;i<n*n;i++) {

        A[i] = (((double) rand() / (double) RAND_MAX));
        B[i] = (((double) rand() / (double) RAND_MAX));
        
    }

    for(i=0;i<n*n;i++)
        C[i] = A[i]*B[i];

    //Print a value to prevent optimizing this part of the code

    //printf("Cache scrub: %lf\n\n",C[0]);

    free(A);
    free(B);
    free(C);

}

int main() {

	srand(time(NULL));

    int i,j;

    int csize = 30000; 

    int m,k,n;
    m = 1985;
    k = 2122;
    n = 2072;

    double *A;
    A = (double *) mkl_malloc(m*k*sizeof(double),64);
  
    double *B;
    B = (double *) mkl_malloc(k*n*sizeof(double),64);
    
    double *C;
    C = (double *) mkl_malloc(m*n*sizeof(double),64);

    for(i=0;i<m*k;i++) 
        A[i] = (((double) rand() / (double) RAND_MAX));
    
    for(i=0;i<k*n;i++)
        B[i] = (((double) rand() / (double) RAND_MAX));

    for(i=0;i<m*n;i++)
        C[i] = (((double) rand() / (double) RAND_MAX));


    double alpha = 1.;
    double beta = 0.;	


	double timeB4, timeAfter, timeSum = 0.;

    /**
     *
     * END OF VARIABLES */ 


    cache_scrub(csize);

    mkl_set_num_threads(1);

    int nth = mkl_get_max_threads();

    printf("Calculating chain 20 times! with max %d threads! \n\n", nth);

    for(i=0;i<20;i++) {
        timeB4 = bli_clock(); 
        cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, m, n, k, alpha, A, k, B, n, beta, C, n);
        timeAfter = bli_clock();
        printf("Time needed: %f\n",timeAfter-timeB4);
        cache_scrub(csize);
    }


	printf("\nDone! \n\n\n");

    mkl_free(A);
    mkl_free(B);
    mkl_free(C);

	return(0);

}
