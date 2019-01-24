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
#include <unistd.h>
#include "statistics.h"

int factorial (int n) {

    if(n==1)
        return n;
    else
        return n*factorial(n-1);

}

/* Assembly function used for the timings.
 *
 * Get the number of clock ticks and divide it by the CPU frequence
 *
 * Arguments: -
 *
 * Return argument:
 *
 * Ticks (unsigned long)
 *
 */

#define get_ticks(var) {                                           \
      unsigned int __a, __d;                                       \
      asm volatile("rdtsc" : "=a" (__a), "=d" (__d));              \
      var = ((unsigned long) __a) | (((unsigned long) __d) << 32); \
   } while(0)

/* Enum for error handling
 *
 * Further errors can be added!
 *
 */

enum _error {
    
    E_SUCCESS = 0,
    E_NULL_POINTER = 1,
    E_WRONG_ARGUMENT_NUMBER = 2,
    E_INVALID_INPUT = 3,
    E_FILE_NOT_FOUND = 4,
    E_UNKNOWN_ERROR = 5    

};

typedef enum _error error_t;


/* Datatype needed to provide readable error messages
 *
 * Consists of a code number and an error message (As a kind of perror alternative)
 *
 */

struct _errordesc {
    int  code;
    char *message;
} errordesc[] = {
    { E_SUCCESS, "No error" },
    { E_NULL_POINTER, "Working with a null pointer" },
    { E_WRONG_ARGUMENT_NUMBER, "Wrong number of arguments provided" },
    { E_INVALID_INPUT, "Invalid input" },
    { E_FILE_NOT_FOUND, "File not found" },
    { E_UNKNOWN_ERROR, "Unknown error!"}
};

/* Function to print out the error message to the user
 *
 * Arguments:
 *
 * errnum = Error number
 *
 * Return argument:
 *
 * Error Message according to the error number
 *
 */


const char *errorString(error_t errnum) {

    int i;
    int size = sizeof(errordesc) / sizeof(struct _errordesc);

    //Fetch error message
    for(i=0;i<size;i++) 
        if(errordesc[i].code == errnum)
            return errordesc[i].message;

    //If error not provided return unknown error
    return errordesc[size-i].message;

}

/* Function to extract the size of the cache level
 * You intend to have all the matrices fit in the resulting cache level
 * This function useses the bashscript "cachesize.sh"
 *
 * Arguments:
 *
 * clevel = cache level which size you want to find out
 *
 */

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

    //Use buffer to extract the entire contents of our output file

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
    
    } else {

        printf("No file there!\n\n");

    }

    system("rm lvl.csv"); 

    int csize;

    //Convert to integer value

    if (buffer) {
        csize = atoi(buffer);
        printf("%d\n\n",csize);
    }

    return csize;

} 

/* Function to set randomized matrix sizes between min and max
 *
 * Arguments:
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
 * Arguments:
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
 * Arguments:
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


/* Function which is needed to free the cache of all data, due to the occasion of matrices still being in the cache while doing consecutive runs
 * A simple matrix addition is performed where each matrix is as large as the entire cache
 *
 * Arguments: -
 *
 */

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

    //Print a value to prevent optimizing this part of the code

    printf("%lf",C[0]);

    free(A);
    free(B);
    free(C);

}

/* Short function needed to swap to elements of an array
 *
 * Parameters:
 *
 * *x = First element
 * *y = Second element
 *
 */

void swap(int *x, int *y) {

    int temp; 
    temp = *x; 
    *x = *y; 
    *y = temp; 

}

/* Function to save all possible chain order permutations
 *
 * Parameters:
 *
 * permChain = Integer array that is permutated
 * int i = Current position in integer array
 * int n = number of elements in permChain
 * buffer = char array where all orders are saved
 *
 */

void permute(int **allOrder, int *permChain, int n) { 

    int tmp[n];

    int i,j,fac,pos;
    fac = factorial(n);
    pos = 0;

    for(i=0;i<n;i++) {
        tmp[i] = 0;
        allOrder[pos][2*i] = permChain[i];
        allOrder[pos][(2*i)+1] = permChain[i]+1;
    }

    pos = pos+1;
    i=0;

    while(i<n) {
        
        if(tmp[i] < i) {
   
            if((i%2) == 0)
                swap(permChain+0,permChain+i);
            else
                swap(permChain+tmp[i],permChain+i);
            
            for(j=0;j<n;j++) {
                allOrder[pos][2*j] = permChain[j];
                allOrder[pos][(2*j)+1] = permChain[j]+1;
            }

            
            tmp[i] = tmp[i]+1;
            pos = pos+1;
            i=0;
    
            
            
        } else {
            tmp[i] = 0;
            i = i+1;
        }
    }

}


void getAllOrders(int **allOrder, int n) {

    int i,j,fac;
    fac = factorial(n);

    int *permChain;
    permChain = (int*) malloc((n)*sizeof(int));

    for(i=0;i<n;i++)
        permChain[i] = i;

    permute(allOrder,permChain,n);

    printf("Somethings wrong?\n");

    for(i=0;i<fac;i++) {
        printf("[ ");
        for(j=0;j<n;j++) {
            printf("(%d,%d)",allOrder[i][2*j],allOrder[i][(2*j)+1]);
        }
        printf(" ]\n");
    }

    free(permChain);

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

/* Recursive part of printing chain. Chain [i,j] is then referenced as chain part [j]
 *
 * Arguments:
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
 * Arguments:
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

/* Chain order is set to consecutive multiplication order 
 *
 * Arguments:
 *
 * order - Array where multiplication order is saved
 * n = Number of matrices
 */

void setConsecutiveOrder(int *order, int n) {

    int i;

    for(i=0;i<n-1;i++) {
        order[2*i] = i;
        order[2*i+1] = i+1;
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

/* Function to calculate the matrix chain and measure the time needed
 *
 * Arguments:
 *
 * A = Matrix Array
 * interRes = Matrix Array for the intermediate results
 * order = Multiplication order
 * sizes = Matrix sizes
 * j = Number of matrices
 *
 */

void calculateChain(double **A, double **interRes, int *order, int *sizes, int j)  {

    unsigned long ticksB4, ticksAfter, ticksSum;
    ticksSum = 0;

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

        get_ticks(ticksB4);
        
        if((A[posX] == NULL) || (A[posY] == NULL) || (interRes[i]) == NULL)
            printf("Error! One of the matrices is empty!");

        cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, m, n, k, alpha, A[posX], k, A[posY], n, beta, interRes[i], n);

        //printf("Finished computing\n\n");

        get_ticks(ticksAfter);

        //printf("Intermediate time result: %ld\n",(wtime_end - wtime_start));

        ticksSum = ticksSum + (ticksAfter - ticksB4);

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

    printf("Results: %lu\n\n",ticksSum/2100000000); 
    
}


int main(int argc, char *argv[]) {

	srand(time(NULL));

	/**START OF VARIABLES
 	*
 	* A = Matrix array
 	* coypA = copy of A
 	* Never pass over A as an argument, only copyA!
 	*
 	* interRes = Matrix array for the intermediate results
 	* With the order and sizes the memory will be allocated beforehand
 	* 
 	* n = Size of matrix array (Default 4)
 	* 
 	* sizes[n+1] = Size of matrices
 	* copySizes[n+1] = This copy is needed for the functions which evaluate the matrix chain and therefore 
 	* Never pass over sizes as an argument, instead use copySizes!
 	*
 	*
 	* sizeMin,sizeMax = min & max matrix sizes
 	* 
 	* cost = Matrix used to compute optimal costs via dynamic programming
 	* 
 	* split = Matrix which saves the split position of computing
 	* a matrix chain (e.g. split[0,2] = 1, the computation is
 	* splitted in position 1
 	* 
 	* optOrder = Array where the optimal multiplication order according to a cost function is saved
 	* 
 	* allOrders = Matrix where all possible orders are saved. Save the cost of this order at the end of each array
 	*
 	* i,j indices
 	*
 	*
 	**/

    int i,j;

    int n;
    //TODO set as variable!
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

    int sizeMin;
    int sizeMax;    


    //Check number of arguments and set matrix sizes accordingly
    if (argc == 1) {
        sizeMin = 10;
        sizeMax = 5000;
    } else if (argc == 2) {
        sizeMin = 10;
        char *c;
        int lvl = strtol(argv[2], &c, 10);
        sizeMax = getCacheSize(lvl);
    } else if (argc == 3) {
        char *c;
        long conv = strtol(argv[1], &c, 10);
        sizeMin = conv;
        conv = strtol(argv[2], &c, 10);
        sizeMax = conv;
    } else {
        printf("Error. Use the maximum of two arguments!");
    }

    double **cost;
    cost = (double**) malloc(n*sizeof(double*));
    for (i=0;i<n;i++)
        cost[i] = (double*) malloc((n-i)*sizeof(double));    

    int **split;
    split = (int**) malloc(n*sizeof(int*));
    for (i=0;i<n;i++)
        split[i] = (int*) malloc((n-i)*sizeof(int));

    int optOrder[2*(n-1)];

    int fac;
    fac  = factorial(n-1);

    int **allOrder;
    allOrder = (int**) malloc(n*sizeof(int*));
    for(i=0;i<fac;i++)
        allOrder[i] = (int*) malloc((2*(n-1))*sizeof(int));

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


    printf("\nAllocation memory for matrices...\n\n");

	setupMatrices(A,copyA,interRes,copySizes,n);

    printf("Creating matrices with random values...\n\n");

    initializeMatrices(A,copyA,copySizes,n);

    printf("Creating statistics file...\n\n");

    //createStatisticsFile(n);

    printf("Now creaiting all permutation possibilities...\n\n");

    getAllOrders(allOrder,n-1);

	printf("Now the evaluation results... \n\n");


	//Now time to evaluate results with the different cost functions. 
	//After each cost function rewrite copyA to continue with next matrix
	
/**********  MINIMAL FLOPS SECTION **************/

	printf("Procedure for minimal flops...\n\n");

    printf("Computing costs minimal flops...\n\n");	

 
    normalizeSizes(sizes,copySizes,sizeMin,sizeMax,n);
  
    computeChainOrder(split,cost,copySizes,n,'F');

    resetCopySizes(sizes,copySizes,n);

    printf("Computing the best multiplication order...\n\n");

	getChainOrder(split, optOrder, n);
    
    printf("The order array is the following: [ ");

    for(i=0; i<n-1; i++) {
        printf("(%d, %d)", optOrder[2*i], optOrder[2*i+1]);
    }

	printf(" ]\n\n");

    printf("Setting up the matrices for the intermediate results...\n\n");

    setupInterMatrices(interRes,optOrder,copySizes,n);

    resetCopySizes(sizes,copySizes,n);

    printf("Finally calculating the results...\n\n");

	calculateChain(copyA,interRes,optOrder,copySizes,n);

    printf("Finished calculating the chain for minimal flops \n\n");

/********** MINIMAL MEMORY USAGE SECTION **************/

    printf("At first lets clean up a few things from the last calculation...\n\n");
    
    resetCopySizes(sizes,copySizes,n);
    
    resetMatricesCopy(A,copyA,copySizes,n);    

	printf("Procedure for minimum minimal memory usage...\n\n");

    printf("Computing costs for minimal memory usage...\n\n");	

    normalizeSizes(sizes,copySizes,sizeMin,sizeMax,n);

    computeChainOrder(split,cost,copySizes,n,'M');

    resetCopySizes(sizes,copySizes,n);

    printf("Computing the best multiplication order...\n\n");

	getChainOrder(split, optOrder, n);
    
    printf("The order array is the following: [ ");

    for(i=0; i<n-1; i++) {
        printf("(%d, %d)", optOrder[2*i], optOrder[2*i+1]);
    }

	printf(" ]\n\n");

    printf("Setting up the matrices for the intermediate results...\n\n");

    setupInterMatrices(interRes,optOrder,copySizes,n);

    resetCopySizes(sizes,copySizes,n);

    printf("Finally calculating the results...\n\n");

	calculateChain(copyA,interRes,optOrder,copySizes,n);

    printf("Finished calculating the chain for minimal memory usage!\n\n");

/********** CONSECUTIVE ORDER SECTION **************/

    printf("At first lets clean up a few things from the last calculation...\n\n");
    
    resetCopySizes(sizes,copySizes,n);
    
    resetMatricesCopy(A,copyA,copySizes,n);    

	printf("Procedure for consecutve order...\n\n");

    setConsecutiveOrder(optOrder,n);

    printf("The order array is the following: [ ");

    for(i=0; i<n-1; i++) {
        printf("(%d, %d)", optOrder[2*i], optOrder[2*i+1]);
    }

	printf(" ]\n\n");

    printf("Setting up the matrices for the intermediate results...\n\n");

    setupInterMatrices(interRes,optOrder,copySizes,n);

    resetCopySizes(sizes,copySizes,n);

    printf("Finally calculating the results...\n\n");

	calculateChain(copyA,interRes,optOrder,copySizes,n);

    printf("Finished calculating the consecutive chain!\n\n");


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
