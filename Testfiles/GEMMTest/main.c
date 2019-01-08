#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include <mkl.h>
#include <float.h>

//Function to set randomized matrix sizes between min and max

void setMatrixSizes(int *sizes, int n, int min, int max) {
    
    int i;

	for (i=0; i<n+1; i++) {
		sizes[i] = (rand() % (max-min)) + min;
	}

    if (sizes == NULL) {
        printf("Error! Sizes Array not allocated!");
        exit(0);
    }

}


//Function to allocate matrices (as an array of matrices
// and cost matrix with given the given sizes

void setupMatrices(double **A, int *sizes, int n) {

    int i;

	for (i=0; i<n; i++) 
		A[i] = (double*) mkl_malloc(sizes[i]*sizes[i+1]*sizeof(double),64);

    if ((A == NULL)) {
	
		printf("Error! Memory not allocated!");
		exit(0);

	}

}


//Set random (double) values in all matrices

void initializeMatrices (double **A, int *sizes, int n) {

    int i,j;

	for (i=0; i<n; i++) 
		for (j=0; j<sizes[i]*sizes[i+1]; j++)
			A[i][j] =  (((double) rand() / (double) RAND_MAX));

}


/*Method to obtain the concrete cost matrix order with cost function cf
 *
 * Cost function explanations:
 *
 * 'F' = Minimal Flops
 * 'M' = Least memory usage
 * 'C' = Cache optimal (Keep result matrix in cache)
 *
 * Parameters:
 * 
 * sizes = matrix sizes
 * n = number of matrices
 * split = matrix which saves the split positions
 * cost = matrix which saves the costs of each split
 * cf = char to indicate which cost function
 */

void computeChainOrder(int **split, double **cost, int *sizes, int sizeMax, int n, char cf) {

	int i,j,k,l;
    int copySizes[n+1];

    for (i=0;i<n+1;i++) 
        copySizes[i] = 10*sizes[i]/sizeMax;


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
				addLeft = cost[i][j-1] + copySizes[i]*copySizes[j]*copySizes[j+1];
				addRight = cost[i+1][j] + copySizes[i]*copySizes[i+1]*copySizes[j+1];
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
						 q = cost[i][k] + cost[k+1][j] + ((double) (copySizes[i]*copySizes[k+1]*copySizes[j+1]));
						 break; 
					
						case 'M':
						 //q = scalar costs of child + left/right neighbour
						 q = cost[i][k] + cost[k+1][j] + ((double) (copySizes[i]*copySizes[k+1] + copySizes[k+1]*copySizes[j+1]));
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

//Recursive part of printing chain. Chain [i,j] is then referenced as chain part [j]

int getRecursiveChain(int **split, int *order,  int left, int right, int pos) {

    //printf("Doing the chain with %d,%d\n", i,j);

	if (left < right) {

		int k;
        
		k = split[left][right];
       
		pos = getRecursiveChain(split,order, left, k,  pos);

		pos = getRecursiveChain(split,order, k+1, right, pos);

        //printf("left is %d, right is %d\n",left,right);
        //printf("split position k is %d\n",k);
        //printf("pos is %d\n",pos);
        
        order[pos] = k;

        order[pos+1] = right;

        pos = pos+2;

        return pos;

	}

    return pos;

}

//Iterative function to call recursive function with error handling

void getChainOrder(int **split, int *order, int n) {

    int pos = 0;
    
    pos = getRecursiveChain(split,order,0,n-1,pos);

    if(pos != 2*(n-1))
        printf("Error in creating the order array! Pos is %d instead of %d! \n\n", pos, 2*n);

}



void setupInterMatrices(double **interRes, int *order, int *sizes, int n) {

    int i;
    int copySizes[2*n];

    for(i=0;i<2*n;i++)
        copySizes[i] = sizes[i];
    
    for(i=0;i<n-1;i++) {
     	interRes[i] = (double*) malloc(copySizes[order[2*i]]*copySizes[order[2*i+1]+1]*sizeof(double));
        copySizes[order[2*i+1]] = copySizes[order[2*i]];
        printf("The intermatrix %d has the size %dx%d\n",i,copySizes[order[2*i]],copySizes[order[2*i+1]+1]);
    }
    
    printf("\n");
}

void calculateChain(double **A, double **interRes, int *order, int *sizes, int j)  {

    double wtime_start, wtime_end, wtime_sum;
    wtime_sum = 0.;

    int i,q;

    int posX,posY;

    double alpha,beta;
    alpha = 1.0;
    beta = 0.0;
    int m,n,k;

    for(i=0;i<j-1;i++) {

        printf("Still working at the start of it %d\n\n", i);

        posX = order[2*i];
        posY = order[2*i+1];

        m = sizes[posX];
        printf("m is %d\n",m);
        k = sizes[posX+1];
        printf("k is %d\n",k);
        n = sizes[posY+1];
        printf("n is %d\n\n",n);

        //wtime_start = omp_get_wtime();

        cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, m, n, k, alpha, A[posX], k, A[posY], n, beta, interRes[i], n);

        printf("Finished computing\n\n");

        //wtime_end = omp_get_wtime();

        //wtime_sum = wtime_sum + (wtime_end - wtime_start);

        if (i != j-2) {
            mkl_free(A[posY]);
            A[posY] = (double*) mkl_malloc(m*n*sizeof(double),64);
            A[posY] = interRes[i];
            sizes[posY] = m;
        }

        printf("Still working at the end of it %d\n\n", i);    

    }

    //printf("Results: %f\n\n",(double) (wtime_sum)); 
    
}


int main() {

	srand(time(NULL));

	/**START OF VARIABLES
 	*
 	* A = Matrix array
 	*
 	* interRes = Matrix array for the intermediate results
 	* With the order and sizes the memory will be allocated beforehand
 	* 
 	* n = Size of matrix array (Default 4)
 	* TODO Request number of matrices and sizes?
 	* 
 	* sizes[n+1] = Size of matrices
 	* 
 	* 
 	* sizeMin,sizeMax = min & max matrix sizes
 	* 
 	* cost = Matrix which saves all multiplication costs
 	* 
 	* split = Matrix which saves the split position of computing
 	* a matrix chain (e.g. split[0,2] = 1, the computation is
 	* splitted in position 1
 	* 
 	* order = Array where the multiplication order is saved
 	*
 	* orderCost = Array where the costs of the multiplication is saved
 	*
 	* i,j indices
 	*
 	* ticks* = time variables
 	**/

    int i,j;

    int n;
    n = 4;

    double **A;
    A = (double **) mkl_malloc(n*sizeof(double*),64);

    double **interRes;
    interRes = (double **) mkl_malloc(n*sizeof(double*),64);
	
	int sizes[n+1];

    int sizeMin = 1000;
    int sizeMax = 5000;

    double **cost;
    cost = (double**) malloc(n*sizeof(double*));
    for (i=0;i<n;i++)
        cost[i] = (double*) malloc((n-i)*sizeof(double));    

    int **split;
    split = (int**) malloc(n*sizeof(int*));
    for (i=0;i<n;i++)
        split[i] = (int*) malloc((n-i)*sizeof(int));

    int order[2*(n-1)];
    unsigned long orderCost[n];

    
	unsigned long ticksStart, ticksEnd, ticksSum;
	unsigned long ticksFlops;

    /**
     *
     * END OF VARIABLES */ 

    srand(time(NULL));

    printf("Creating matrix sizes...\n\n");   

    setMatrixSizes(sizes,n,sizeMin,sizeMax);

	printf("The matrix sizes are:\n");
	for (i=0; i<n; i++) {
		printf("size[%d]: [%dx%d]\n", i, sizes[i], sizes[i+1]);
	}


    printf("\n\nAllocation memory for matrices...\n\n");

	setupMatrices(A,sizes,n);

    printf("Creating matrices with random values...\n\n");

    initializeMatrices(A,sizes,n);

	printf("Now the evaluation results! \n\n");


	//Now time to evaluate results with the different cost functions. 
	//After each cost function rewrite copyA to continue with next matrix
	
	printf("Procedure for minimum Flops!\n\n");

    printf("Computing costs for minimal flops...\n\n");	

    computeChainOrder(split,cost,sizes,sizeMax,n,'F');

    printf("Computing the best multiplication order...\n\n");

	getChainOrder(split, order, n);
    
    printf("The order array is the following: [ ");

    for(i=0; i<n-1; i++) {
        printf("(%d, %d)", order[2*i], order[2*i+1]);
    }

	printf(" ]\n\n");

    printf("Setting up the matrices for the intermediate results...\n\n");

    setupInterMatrices(interRes,order,sizes,n);

    printf("Finally calculating the results...\n\n");

	calculateChain(A,interRes,order,sizes,n);

	/*for (i=0; i<n; i++) {
    	mkl_free(A[i]);
        mkl_free(interRes[i]);
        free(cost[i]);
        free(split[i]);
    }

    mkl_free(A);
    mkl_free(interRes);   
    free(cost);
    free(split);*/

	printf("\nDone! \n\n\n");

	return(0);

}
