/* The main goal for this file is to calculate a matrix chain product.
 * The optimal chain is computed by different cost functions
 * You are supposed to compare the calculation times of each cost function
 *
 * Check the main function to see how everything exactly works!
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include <mkl.h>
#include <float.h>

int factorial(int n) {

    if (n == 0)
        return 1;
    return (n*factorial(n-1));

}

/*Function to set randomized matrix sizes between min and max
 *
 * Parameters:
 *
 * sizes = Array with matrix sizes
 * copySizes = A copy of sizes
 * n = Number of matrices
 * min = Smallest matrix size
 * max = Largest matrix size
 *
 */

void setMatrixSizes(int *sizes, int *copySizes, int n, int min, int max) {
    
    int i;

	for (i=0; i<n+1; i++) {
		sizes[i] = (rand() % (max-min)) + min;
        copySizes[i] = sizes[i];
	}

    if (sizes == NULL) {
        printf("Error! Sizes Array not allocated!");
        exit(0);
    }

}

/* Function to reset the changes of copySizes
 *
 * Parameters:
 * sizes = Array with matrix sizes
 * copySizes = A copy of sizes
 * n = Number of matrices
 *
 */

void resetCopySizes(int *sizes, int *copySizes, int n) {

    int i;
    
    for (i=0;i<n+1;i++)
        copySizes[i] = sizes[i];

}

/* To prevent over-/underflow during cost computation normalize the values by factor (sizeMax/sizeMin)/sizeMax
 *
 * Parameters:
 *
 * sizes = Array with matrix sizes
 * sizeMin/Max = Smallest/largest matrix sizes used to compute factorization factor
 * n = Number of matrices
 *
 */

void normalizeSizes(int *sizes, int *normSizes, int sizeMin, int sizeMax, int n) {

    int i;

    for (i=0;i<n+1;i++)
        normSizes[i] = ((sizeMax/sizeMin)*sizes[i])/sizeMax;

}     

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
 * Parameters:
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
            interRes[i] = (double*) mkl_malloc(sizes[i]*sizes[i+1]*sizeof(double),64);

    }

}


/* Set random (double) values in all matrices between 0 and 1
 * 
 * Parameters:
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

int getCacheSize(int clevel) {

    switch(clevel) {

        case 1:
        system("cachesize.sh 1");
        break;

        case 2:
        system("cachesize.sh 2");
        break;

        case 3:
        system("cachesize.sh 3");
        break;

        default:
        printf("Invalid case. Case is %d\n\n",clevel);

    }

    char *buffer;
    long length;
    FILE *f = fopen("lvl.csv", "r");

    if (f) {
        fseek(f,0,SEEK_END);
        length = ftell(f);
        fseek(f,0,SEEK_SET);
        buffer = malloc(length);
        
        if(buffer)
            fread(buffer,1,length,f);
        
        fclose(f);
    
    } 

    int csize;

    if (buffer) {
        csize = atoi(buffer);
        printf("%d\n\n",csize);
    }

    return csize;
 
}

void cache_scrub() {

    int csize = getCacheSize(3);

    int n = (int) sqrt(csize);

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
        
        C[i] = A[i] + B[i];
    }

    printf("%lf",C[0]);

    free(A);
    free(B);
    free(C);

}

/*Method to obtain the optimal chain order with a standard dynamic programming algorithm
 *
 * Cost function explanations:
 *
 * 'F' = Minimal Flops
 * 'M' = Least memory usage
 * 'C' = Cache optimal (Keep result matrix in cache) - Start chain with least memory usage
 *
 * Parameters:
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
						 q = cost[i][k] + cost[k+1][j] + ((double) (normSizes[i]*normSizes[k+1] + normSizes[k+1]*normSizes[j+1]));
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

/* Recursive part of printing chain. Chain [i,j] is then referenced as chain part [j]
 *
 * Parameters:
 *
 * split = Result matrix from DP which gives a split position for each chain
 * order = Array where the multiplication order is saved
 * left/right = Left/right matrix of the given chain
 * pos = Current position of the next empty entry of order. Therefore, pos should be 2*(n-1) in the end
 *
 */

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

/* Iterative function to call recursive function with error handling
 *
 * Parameters:
 * split = Result matrix from DP which gives a split position for each chain
 * order = Array where the multiplication order is saved
 * n = Number of matrices
 *
 */

void getChainOrder(int **split, int *order, int n) {

    int pos = 0;
    
    pos = getRecursiveChain(split,order,0,n-1,pos);

    if(pos != 2*(n-1))
        printf("Error in creating the order array! Pos is %d instead of %d! \n\n", pos, 2*n);

}

/* Function for allocating memory for intermediate matrix results
 *
 * Parameters:
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

/* Function to calculate the matrix chain and measure the time needed
 *
 * Parameters:
 *
 * A = Matrix Array
 * interRes = Matrix Array for the intermediate results
 * order = Multiplication order
 * sizes = Matrix sizes
 * j = Number of matrices
 *
 */

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

        //printf("Still working at the start of it %d\n\n", i);

        posX = order[2*i];
        posY = order[2*i+1];

        printf("Using matrices %d and %d\n\n",posX,posY);

        m = sizes[posX];
        k = sizes[posX+1];
        n = sizes[posY+1];
        
        /*printf("m is %d\n",m);
        printf("k is %d\n",k);
        printf("n is %d\n\n",n);*/

        wtime_start = omp_get_wtime();
        
        if((A[posX] == NULL) || (A[posY] == NULL) || (interRes[i]) == NULL)
            printf("Error! One of the matrices is empty!");

        cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, m, n, k, alpha, A[posX], k, A[posY], n, beta, interRes[i], n);

        //printf("Finished computing\n\n");

        wtime_end = omp_get_wtime();

        //printf("Intermediate time result: %ld\n",(wtime_end - wtime_start));

        wtime_sum = wtime_sum + (wtime_end - wtime_start);

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

    printf("Results: %f\n\n",wtime_sum); 
    
}


int main(int argc, char *argv[]) {

	srand(time(NULL));

	/**START OF VARIABLES
 	*
 	* A = Matrix array
 	* coypA = copy of A
 	*
 	* interRes = Matrix array for the intermediate results
 	* With the order and sizes the memory will be allocated beforehand
 	* 
 	* n = Size of matrix array (Default 4)
 	* TODO Request number of matrices and sizes?
 	* 
 	* sizes[n+1] = Size of matrices
 	* copySizes[n+1] = This copy is needed for the functions which evaluate the matrix chain and therefore 
 	* Never pass over sizes as a parameter, instead use copySizes!
 	*
 	* normSizes[n+1] = Array with the sizes normalized by a factor. Used for calculations to prevent overflows
 	*
 	* sizeMin,sizeMax = min & max matrix sizes
 	* 
 	* cost = Matrix used to compute optimal costs via dynamic programming
 	* 
 	* split = Matrix which saves the split position of computing
 	* a matrix chain (e.g. split[0,2] = 1, the computation is
 	* splitted in position 1
 	* 
 	* order = Array where the multiplication order is saved
 	*
 	* i,j indices
 	*
 	* ticks* = time variables
 	**/

    int i,j;

    int n;
    n = 4;

    double **A;
    double **copyA;
    A = (double **) mkl_malloc(n*sizeof(double*),64);
    copyA = (double **) mkl_malloc(n*sizeof(double*),64);

    double **interRes;
    interRes = (double **) mkl_malloc(n*sizeof(double*),64);
	
    if ((A == NULL) || (interRes == NULL)) {
	
		printf("Error! Memory for A or interRes not allocated!");
		exit(0);

	}

	int sizes[n+1];
    int copySizes[n+1];
    int normSizes[n+1];

    int sizeMin;
    int sizeMax;    

    if (argc == 1) {
        sizeMin = 1000;
        sizeMax = 5000;
    } else if (argc == 3) {
        char *c;
        long conv = strtol(argv[1], &c, 10);
        sizeMin = conv;
        conv = strtol(argv[2], &c, 10);
        sizeMax = conv;
    } else {
        printf("Error. Use either zero or two arguments!");
    }

    double **cost;
    cost = (double**) malloc(n*sizeof(double*));
    for (i=0;i<n;i++)
        cost[i] = (double*) malloc((n-i)*sizeof(double));    

    int **split;
    split = (int**) malloc(n*sizeof(int*));
    for (i=0;i<n;i++)
        split[i] = (int*) malloc((n-i)*sizeof(int));

    int order[2*(n-1)];

    /**
     *
     * END OF VARIABLES */ 

        
/**********  SETUP SECTION **************/

    srand(time(NULL));

    printf("Creating matrix sizes...\n\n");   

    setMatrixSizes(sizes,copySizes,n,sizeMin,sizeMax);

	printf("The matrix sizes are:\n");
	for (i=0; i<n; i++) {
		printf("size[%d]: [%dx%d]\n", i, sizes[i], sizes[i+1]);
	}

    normalizeSizes(copySizes,normSizes,sizeMin,sizeMax,n);

    printf("\nAllocation memory for matrices...\n\n");

	setupMatrices(A,copyA,interRes,copySizes,n);

    printf("Creating matrices with random values...\n\n");

    initializeMatrices(A,copyA,copySizes,n);

	printf("Now the evaluation results... \n\n");

	//Now time to evaluate results with the different cost functions. 
	//After each cost function rewrite copyA to continue with next matrix
	
/**********  MINIMAL FLOPS SECTION **************/

	printf("Procedure for minimal flops...\n\n");

    printf("Computing costs minimal flops...\n\n");	

    computeChainOrder(split,cost,copySizes,n,'F');

    resetCopySizes(sizes,copySizes,n);

    printf("Computing the best multiplication order...\n\n");

	getChainOrder(split, order, n);
    
    printf("The order array is the following: [ ");

    for(i=0; i<n-1; i++) {
        printf("(%d, %d)", order[2*i], order[2*i+1]);
    }

	printf(" ]\n\n");

    printf("Setting up the matrices for the intermediate results...\n\n");

    setupInterMatrices(interRes,order,copySizes,n);

    resetCopySizes(sizes,copySizes,n);

    printf("Finally calculating the results...\n\n");

	calculateChain(copyA,interRes,order,copySizes,n);

    printf("Finished calculating the chain for minimal flops \n\n");

/********** MINIMAL MEMORY USAGE SECTION **************/

    printf("At first lets clean up a few things from the last calculation...\n\n");
    
    resetCopySizes(sizes,copySizes,n);
    
    resetMatricesCopy(A,copyA,copySizes,n);    

	printf("Procedure for minimum minimal memory usage...\n\n");

    printf("Computing costs for minimal memory usage...\n\n");	

    computeChainOrder(split,cost,copySizes,n,'M');

    resetCopySizes(sizes,copySizes,n);

    printf("Computing the best multiplication order...\n\n");

	getChainOrder(split, order, n);
    
    printf("The order array is the following: [ ");

    for(i=0; i<n-1; i++) {
        printf("(%d, %d)", order[2*i], order[2*i+1]);
    }

	printf(" ]\n\n");

    printf("Setting up the matrices for the intermediate results...\n\n");

    setupInterMatrices(interRes,order,copySizes,n);

    resetCopySizes(sizes,copySizes,n);

    printf("Finally calculating the results...\n\n");

	calculateChain(copyA,interRes,order,copySizes,n);

    printf("Finished calculating the chain for minimal memory usage!\n\n");

/********** CACHE OPTIMAL  SECTION **************/


    printf("The chains have been calculated! Now a quick cleanup...\n\n");

	for (i=0; i<n; i++) {
    	mkl_free(A[i]);
        mkl_free(interRes[i]);
        //free(cost[i]);
        //free(split[i]);
    }

    mkl_free(A);
    mkl_free(interRes);  

    //free(cost);
    //free(split);

	printf("Done! \n\n\n");

	return(0);

}
