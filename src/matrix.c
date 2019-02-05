#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <math.h>
#include <mkl.h>
#include <limits.h>
#include <float.h>
#include <string.h>

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
        double *pointer;
        int length = sizes[i]*sizes[i+1];
        pointer = mkl_realloc(copyA[i],length);
        if (pointer == NULL)
            printf("Error, pointer is NULL\n\n");
        copyA[i] = pointer;
        memcpy(copyA[i],A[i],length);
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

void setupMatrices(double **A, double **copyA, double **interRes, int *sizes, int n) {

    int i;

	for (i=0; i<n; i++) { 
		A[i] = (double*) mkl_malloc(sizes[i]*sizes[i+1]*sizeof(double),64);
        copyA[i] =  (double*) mkl_malloc(sizes[i]*sizes[i+1]*sizeof(double),64);
        
        if(i<n-1)
            //Malloc matrices for consecutive multiplication as default
            interRes[i] = (double*) mkl_malloc(sizes[0]*sizes[i+2]*sizeof(double),64);

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

	for (i=0; i<n; i++) 
		for (j=0; j<sizes[i]*sizes[i+1]; j++)
			A[i][j] =  (((double) rand() / (double) RAND_MAX));

    for (i=0; i<n; i++) 
        memcpy(copyA[i],A[i],sizes[i]*sizes[i+1]);

}

/* Function to extract all permutations of the multiplication order (UNCONVERTED)
 *
 * Parameters:
 *
 * allOrder = Matrix where all the permutated orders are saved
 * n = Number of multiplications needed to compute the chain product (!= number of matrices
 *
 */

void getAllOrders(int **allOrder, int n) {

    int i,j,fac;
    fac = factorial(n);

    int *permChain;
    permChain = (int*) malloc((n)*sizeof(int));

    for(i=0;i<n;i++)
        permChain[i] = i;

    permute(allOrder,permChain,n);

    /*for(i=0;i<fac;i++) {
        printf("[ ");
        for(j=0;j<n;j++) {
            printf("(%d,%d)",allOrder[i][2*j],allOrder[i][(2*j)+1]);
        }
        printf(" ]\n");
    }*/

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

    int i,j,k,fac,contained,val;
    fac = factorial(n-1);

    for(i=0;i<fac;i++) {
        
        for(j=1;j<n-1;j++) {    
       
            val = allOrder[i][2*j+1];
            contained = contains(allOrder[i],val,2*j+1);

            if(contained == 1) {
                while(contained == 1) {
                    val = val+1;
                    contained = contains(allOrder[i],val,2*j+1);
                }
                allOrder[i][2*j+1] = val-1;
            } else
                allOrder[i][2*j+1] = val;
        }

    }

    /*for(i=0;i<fac;i++) {
        printf("[ ");
        for(j=0;j<n-1;j++) {
            printf("(%d,%d)",allOrder[i][2*j],allOrder[i][(2*j)+1]);
        }
        printf(" ]\n");
    }*/

}

void sortChainCosts(int **allOrder, int *orderCost, int n) {

    int i;

    // Base case 
    if (n == 1) 
        return; 
    
    // One pass of bubble sort. After this pass, the largest element is moved (or bubbled) to end. 
    for (i=0; i<n-1; i++)
        if (orderCost[i] > orderCost[i+1]) { 
            printf("What?\n"); 
            swap(orderCost[i], orderCost[i+1]); 
            int *pointer = malloc(2*(n-1)*sizeof(int));
            pointer = allOrder[i];
            allOrder[i] = allOrder[i+1];
            allOrder[i+1] = pointer;
            free(pointer);
        }
                                                     
    // Largest element is fixed, 
    // recur for remaining array 
    sortChainCosts(allOrder, orderCost, n-1); 

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

void computeChainCosts(int **allOrder, int *orderCost, int *normSizes, int n, int fac, char cf) {

    int i,j,m,k,l;

    int *copySizes;
    copySizes = (int*) malloc((n+1)*sizeof(int));

    memcpy(copySizes,normSizes,(n+1)*sizeof(int));

    for(i=0;i<fac;i++) {
        
        switch(cf) {
         
            case'F':

                for(j=0;j<n-1;j++) {

                    m = copySizes[allOrder[i][2*j]];
                    k = copySizes[allOrder[i][2*j]+1];
                    l = copySizes[allOrder[i][2*j+1]+1];

                    //printf("m:%d , k:%d ,l:%d\n",m,k,l);                   
 
                    orderCost[i] = orderCost[i] + m*k*l;
                    
                    copySizes[allOrder[i][2*j+1]] = m;
                
                }
                
                break;
            
            case'M':
            
                for(j=0;j<n-1;j++) {

                    m = copySizes[allOrder[i][2*j]];
                    l = copySizes[allOrder[i][2*j+1]+1];
                
                    orderCost[i] = orderCost[i] + m*l;
                    
                    copySizes[allOrder[i][2*j+1]] = m;

                }

                break;

            default:
                printf("Wrong parameter!");

        }

        resetCopySizes(normSizes,copySizes,n);
    
    }

    /*for(i=0;i<fac;i++)
        printf("The normed cost is: %d\n",orderCost[i]);*/

    free(copySizes);

}

/*Method to obtain the optimal chain order with a standard dynamic programming algorithm
 *
 * Cost function explanations:
 *
 * 'F' = Minimal Flops
 * 'M' = Least memory usage
 * 'C' = Cache optimal (Keep result matrix in cache) - Start chain with most memory usage
 *
 * INFO: Cost function 'C' is currently left out of the computations
 *
 * Arguments:
 * 
 * split = Matrix which saves the split positions
 * cost = Matrix which saves the costs of each split
 * normSizes = Array with normalized matrix sizes
 * n = Number of Matrices
 * fac = Length of split/cost ((n-1)!)
 * cf = Char to indicate which cost function
 *
 */

void computeChainOrder(int **split, double **cost, int *normSizes, int n, char cf) {

	int i,j,k,l;


	//Needed for the cache optimal cost function
	double addLeft,addRight;
	
	//q is is used for buffering calculation costs

	double q;

	//Cost is zero while multiplying one matrix
	
	for (i=0; i<n; i++) {
    	cost[i][i] = 0.;
        split[i][i] = i;
    }
		

	//l is chain length
	for (l=2; l<n+1; l++) {
		
		for (i=0; i<n-l+1; i++) {
			
			j = i+l-1;
			cost[i][j] = DBL_MAX;

			if (cf == 'C') {
				addLeft = cost[i][j-1] + normSizes[i]*normSizes[j]*normSizes[j+1];
				addRight = cost[i+1][j] + normSizes[i]*normSizes[i+1]*normSizes[j+1];
				if (addLeft < addRight) {
					cost[i][j] = addLeft;
					split[i][j] = i;
				} else {
					cost[i][j] = addRight;
					split[i][j] = j-1;
				}

			} else {		 
			
				for (k=i; k<j; k++) {

					switch(cf) {
						case 'F':
						 //q = cost/scalar multiplications
						 q = cost[i][k] + cost[k+1][j] + ((double) (normSizes[i]*normSizes[k+1]*normSizes[j+1]));
						 break; 
					
						case 'M':
						 //q = scalar costs of child + left/right neighbour
						 q = cost[i][k] + cost[k+1][j] + ((double) (normSizes[i]*normSizes[j+1]));
						 break;

						default:
						 printf("Invalid Cost Function!\n\n");
					}
				
				//Save k in s to remember where to split chain when multiplying
					if (q < cost[i][j]) {
					
						cost[i][j] = q;
						split[i][j] = k;
					}
                
				}

                //printf("Cost for %d,%d: %f\n",i,j,cost[i][j]);
                //printf("Split for %d,%d: %d\n\n",i,j,split[i][j]);
            
			}
        
        }	
		
	}

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

    for(i=0;i<n-1;i++) {
        double *pointer;
     	pointer = (double*) mkl_realloc(interRes[i],sizes[order[2*i]]*sizes[order[2*i+1]+1]*sizeof(double));
        interRes[i] = pointer;
        sizes[order[2*i+1]] = sizes[order[2*i]];
        //printf("The intermatrix %d has the size %dx%d\n",i,sizes[order[2*i]],sizes[order[2*i+1]+1]);
    }
    
    //printf("\n");
}

