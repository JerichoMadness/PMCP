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
#include "helper.h"

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

double calculateChainSequential(double **A, double **interRes, int *sizes, struct node *nd, char mode, int nthreads)  {

    double timeB4, timeAfter, timeSum;
    timeSum = 0.;

    if((nd->cLeft) != NULL) {
        timeSum = timeSum + calculateChainSequential(A,interRes,sizes,nd->cLeft, mode, nthreads);   
    }

    if((nd->cRight) != NULL) {
        timeSum = timeSum + calculateChainSequential(A,interRes,sizes,nd->cRight, mode, nthreads);
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

    if(sizes[posX+1] != sizes[posY])
        printf("Error: Matrix sizes are not the same! Multiplying (%dx%d) and (%dx%d)\n\n", sizes[posX],sizes[posX+1],sizes[posY],sizes[posY+1]);
  
    if((A[posX] == NULL) || (A[posY] == NULL) || (interRes[opPos]) == NULL)
        printf("Error! One of the matrices is empty!");

    //printf("Using matrices %d(%dx%d) and %d(%dx%d) and saving the result in intermediate matrix %d\n\n",posX,m,k,posY,k,n,opPos);

    timeB4 = bli_clock();

    if(mode == 'S') {
        omp_set_num_threads(1);
    } else if(mode == 'B') {
        omp_set_num_threads(nthreads);
    } else if ((mode != 'B') && (mode != 'S')) {
        printf("Wrong mode %c! Doing default BLAS parallel",mode);
    }

    //printf("Number of threads: %d\n\n",mkl_get_max_threads());

    cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, m, n, k, alpha, A[posX], k, A[posY], n, beta, interRes[opPos], n);

    if(mode == 'B')
        omp_set_num_threads(1);

    timeAfter = bli_clock();

    timeSum = timeSum + (timeAfter-timeB4);

    //printf("Final InterRes value: %lf. Intermediate results: %lfs. Overall: %lfs\n\n", interRes[opPos][m*n-1], (timeAfter - timeB4), timeSum);

    A[posY] = interRes[opPos];
    
    //printf("Repositioned\n");
    
    sizes[posY] = m;

    //printf("Still working at the end of it %d\n\n", i);    

    return timeSum;

}

void multiplyMatrix(double **A, double **interRes, int *sizes, struct node *nd, char mode, int nthreads) {

    int posX,posY;

    double alpha,beta;
    alpha = 1.0;
    beta = 0.0;
    int m,n,k;

    int opPos = nd->opNum;

    //printf("I am opPos: %d\n\n",opPos);

    posX = nd->mLeft;
    posY = nd->mRight;

    m = sizes[posX];
    k = sizes[posX+1];
    n = sizes[posY+1];
    
    if((A[posX] == NULL) || (A[posY] == NULL) || (interRes[opPos]) == NULL)
        printf("Error! One of the matrices is empty!");

    //printf("Using matrices %d(%dx%d) and %d(%dx%d)\n\n",posX,m,k,posY,k,n);

    //double timeB4,timeAfter;
    //timeB4 = bli_clock();

    if(mode == 'T') {
        omp_set_num_threads(1);
    } else if (mode == 'C') {
        //printf("Mode is combined!\n\n");
        omp_set_num_threads(nthreads);
    } else if ((mode != 'C') && (mode != 'T')) {
        printf("Wrong mode %c! Doing default BLAS parallel\n\n",mode);
    }

    //printf("Number of threads: %d\n\n",nthreads);

    cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, m, n, k, alpha, A[posX], k, A[posY], n, beta, interRes[opPos], n);

    if(mode == 'C')
        omp_set_num_threads(1);
    
    //timeAfter = bli_clock();

    //printf("Intermediate results: %lfs. Overall:\n\n", (timeAfter - timeB4));

    A[posY] = interRes[opPos];
     
    //printf("Repositioned\n");
    
    sizes[posY] = m;

    //printf("Still working at the end of it %d\n\n", i);    

}

void processTree(double **A, double **interRes, int *sizes, struct node *nd, char mode, int nthreads) {

    int lThreads,rThreads;
    double lCost,rCost;

    if(mode=='B') {
        lThreads = 1;
        rThreads = 1;
    } else if((mode=='C') && (nd->cLeft != NULL) && (nd->cRight != NULL) ) {

        lThreads=4;
        rThreads=4;
        /*lCost = nd->cLeft->cost;
        rCost = nd->cRight->cost;

        double allCost;
        allCost = lCost + rCost;

        lThreads = (int) (rCost/allCost)*nthreads;
        rThreads = (int) (lCost/allCost)*nthreads;

        if(lThreads < rThreads) {
            lThreads = lThreads+1;
        } else if (rThreads < lThreads) {
            rThreads = rThreads+1;
        } 

        int minLThreads,minRThreads;

        minLThreads = leafCount(nd->cLeft);
        minRThreads = leafCount(nd->cRight);

        if(minLThreads > lThreads){
            lThreads = minLThreads;
            rThreads = nthreads-lThreads;
        }

        if(minRThreads > rThreads){
            rThreads = minRThreads;
            lThreads = nthreads-rThreads;
        }*/

    } else if (nd->cLeft == NULL) {
        rThreads = nthreads;
    } else if(nd->cRight == NULL) {
        lThreads = nthreads;
    }

    if(nd->cLeft != NULL) {
        #pragma omp task shared(A, interRes, sizes), firstprivate(nd) 
        {
        int id = omp_get_thread_num();
        printf("I am thread %d in left child! \n\n",id);
        processTree(A,interRes,sizes,nd->cLeft,mode,lThreads);   
        }
    }

    if(nd->cRight != NULL) {
        #pragma omp task shared(A, interRes, sizes), firstprivate(nd)
        {
        int id = omp_get_thread_num();
        printf("I am thread %d in right child!\n\n",id);
        processTree(A,interRes,sizes,nd->cRight,mode,rThreads);
        }
    }

    #pragma omp taskwait

    multiplyMatrix(A,interRes,sizes,nd, mode,nthreads);
    
}

double calculateChainTaskParallel(double **A, double **interRes, int *sizes, struct node *root, char mode, int nthreads)  {

    double timeB4, timeAfter, timeSum;
    timeSum = 0.;

    //Clean cache first to remove any prior data on it

    cache_scrub(CACHESIZE);

    timeB4 = bli_clock();

    #pragma omp parallel
    #pragma omp single
    processTree(A, interRes, sizes, root, mode, nthreads);
    #pragma omp taskwait

    timeAfter = bli_clock();

    timeSum = timeAfter - timeB4;

    //printf("Results: %lf\n\n",timeSum); 
   
    return timeSum;
     
}


