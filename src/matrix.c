#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <math.h>
#include <mkl.h>
#include <limits.h>
#include <float.h>
#include <string.h>
#include "array.h"
#include "helper.h"

/* Function to reset an altered copy array of matrices
 *
 * Arguments:
 *
 * A = Original matrix array
 * copyA = Altered matrix array
 * sizes = Array of matrix sizes
 * n = Number of matrices
 *
 */

void resetMatricesCopy(double **A, double **copyA, int *sizes, int n) {

    int i;

    for(i=0;i<n;i++) {
        if(A[i] == NULL)
            printf("Error A[%d] is null!\n\n",i);

        copyA[i] = A[i];
    }

}

/* Function to allocate matrices (as an array of matrices and cost matrix with given the given sizes)
 *
 * Arguments:
 *
 * A = Matrix Array
 * copyA = Copy of A
 * interRes = Array of intermediate result matrices
 * sizes = Matrix sizes
 * n = Number of matrices
 *
 */

void setupMatrices(double **A, double **copyA, int *sizes, int n) {

    int i;

	for (i=0; i<n; i++) { 
		A[i] = (double*) mkl_malloc((sizes[i]*sizes[i+1])*sizeof(double),64);
        copyA[i] =  A[i];
    }

}


/* Set random (double) values in all matrices between 0 and 1
 * 
 * Arguments:
 *
 * A = Matrix Array
 * copyA = copy of A
 * sizes = Matrix sizes
 * n = Number of matrices
 *
 */

void initializeMatrices (double **A, double **copyA, int *sizes, int n) {

    int i,j;

	for (i=0; i<n; i++) { 
		for (j=0; j<(sizes[i]*sizes[i+1]); j++) {
			A[i][j] =  (((double) rand() / (double) RAND_MAX));
        }
        //printf("Last value %d in matrix %lu is %lf\n\n",i,sizes[i]*sizes[i+1],A[i][sizes[i]*sizes[i+1]-1]);
    }

}

/* Function to extract all permutations of the multiplication order (UNCONVERTED)
 *
 * Parameters:
 *
 * allOrder = Matrix where all the permutated orders are saved
 * n = Number of multiplications needed to compute the chain product (!= number of matrices)
 *
 */

void getAllOrders(int **allOrder, int n) {

    int i;

    int *permChain;
    permChain = (int*) malloc(n*sizeof(int));

    for(i=0;i<n;i++) {
        permChain[i] = i;
    }

    permute(allOrder,permChain,n);

    int numOrder = factorial(n);
    int j;

    for(i=0;i<numOrder;i++) {
        printf("[ ");
        for(j=0;j<n;j++) {
            printf("(%d,%d)",allOrder[i][2*j],allOrder[i][(2*j)+1]);
        }
        printf(" ]\n");
    }

    printf("\n");

    free(permChain);

}

/* Function to convert all permutated orders
 * Example: (1,2) is saved in matrix 2, so the multplication (0,1) is converted into (0,2)
 *
 * Paramters:
 *
 * allOrder = Matrix where all orders are saved
 * n = Number of matrices
 *
 */

void convertOrders(int **allOrder, int n) {

    int i,j,numOrder,contained,val;
    numOrder = factorial(n);

    int *left;
    left = (int*) malloc(n*sizeof(int));

    for(i=0;i<numOrder;i++) {
        
        left[0] = allOrder[i][0];

        for(j=1;j<n;j++) {    

            left[j] = allOrder[i][2*j];
            val = allOrder[i][2*j+1];
            contained = contains(left,val,j);

            while(contained == 1) {
                val = val+1;
                contained = contains(left,val,j);
            }
            
            allOrder[i][2*j+1] = val;
        }

    }

    for(i=0;i<numOrder;i++) {
        printf("[ ");
        for(j=0;j<n;j++) {
            printf("(%d,%d)",allOrder[i][2*j],allOrder[i][(2*j)+1]);
        }
        printf(" ]\n");
    }

    printf("\n");

    free(left);

}

/* Function to compute the costs of all chain orders according to the cost function
 *
 * Arguments:
 *
 * allOrder = Matrix with all stored permuted multiplication orders
 * orderCost = Array to save all according computation costs
 * n = Number of matrices
 * fac = Factorial of n
 * cf = Cost function
 *
 *
 */

void computeChainCosts(int **allOrder, double *orderCost, int *sizes, int n, int fac, char cf) {

    int i,j;

    double m,k,l;

    int *copySizes;
    copySizes = (int*) malloc((n+1)*sizeof(int));

    memcpy(copySizes,sizes,(n+1)*sizeof(int));

    for(i=0;i<fac;i++) {
       
        orderCost[i] = 0.;
         
        switch(cf) {
         
            case'F':

                for(j=0;j<n-1;j++) {

                    m = (double) copySizes[allOrder[i][2*j]];
                    k = (double) copySizes[allOrder[i][2*j]+1];
                    l = (double) copySizes[allOrder[i][2*j+1]+1];

                    //printf("m:%lf , k:%lf ,l:%lf\n",m,k,l);                   
 
                    orderCost[i] = orderCost[i] + 2*m*k*l;
                    
                    copySizes[allOrder[i][2*j+1]] = copySizes[allOrder[i][2*j]];
                
                }
                
                break;
            
            case'M':
            
                for(j=0;j<n-1;j++) {

                    m = (double) copySizes[allOrder[i][2*j]];
                    l = (double) copySizes[allOrder[i][2*j+1]+1];

                    //printf("m:%d ,l:%d\n",m,l);                   
               
                    orderCost[i] = orderCost[i] + m*l;
                    
                    copySizes[allOrder[i][2*j+1]] = copySizes[allOrder[i][2*j]];

                }

                break;

            default:
                printf("Wrong parameter! %c", cf);

        }

        resetCopySizes(sizes,copySizes,n+1);
    
    }

    /*for(i=0;i<fac;i++)
        printf("The normed cost is: %d\n",orderCost[i]);*/

    free(copySizes);

}

void resetInterMatrices(double **interRes, int n) {

    int i;

    for(i=0;i<n;i++)
        mkl_free(interRes[i]); 

}

/* Function for allocating memory for intermediate matrix results
 *
 * Arguments:
 *
 * interRes = Array for the intermediate matrix results
 * order = Multiplication order
 * sizes = Matrix sizes
 * n = Number of matrices
 *
 */

void setupInterMatrices(double **interRes, int *order, int *sizes, int n) {

    int i;

    for(i=0;i<n;i++) {
        
        //printf("The intermatrix %d has the size %dx%d\n",i,sizes[order[2*i]],sizes[order[2*i+1]+1]);

        int m,n;
        m = sizes[order[2*i]];
        n = sizes[order[2*i+1]+1];

        interRes[i] = (double*) mkl_malloc(m*n*sizeof(double),64);
       
        sizes[order[2*i+1]] = sizes[order[2*i]];
    }
    
    //printf("\n");
}

