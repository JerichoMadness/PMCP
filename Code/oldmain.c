#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include <mkl.h>

//Defined function to measure time more accurately

#define get_ticks(var) {                                           \
      unsigned int __a, __d;                                       \
      asm volatile("rdtsc" : "=a" (__a), "=d" (__d));              \
      var = ((unsigned long) __a) | (((unsigned long) __d) << 32); \
   } while(0)

inline uint64_t rdtsc() {
    uint32_t lo, hi;
    __asm__ __volatile__ (
      "xorl %%eax, %%eax\n"
      "cpuid\n"
      "rdtsc\n"
      : "=a" (lo), "=d" (hi)
      :
      : "%ebx", "%ecx");
    return (uint64_t)hi << 32 | lo;
}

//Initialize Matrices with random entries in range (-1,1)


void initializeMatrices(int p[], int n, double **A) {


	/* A is an array of matrices with different sizes
	 * A[i] has the size p[i]*p[i+1] 	 
 	 */

	int i,j;

	//Comput length of each single matrix and allocate memory

	for (i=0; i<n; i++) {

		A[i] = (double*) malloc(p[i]*p[i+1]*sizeof(double));
		
	}

	//Distribute random entries

	for (i=0; i<n; i++) {
		
		for (j=0; j<p[i]*p[i+1]; j++) {
			
			A[i][j] =  (((double) rand() / (double) RAND_MAX));

		}
	}
	
}

/*Method to obtain the concrete matrix order with cost function cf
 *
 * Cost function explanations:
 *
 * 'F' = Minimal Flops
 * 'M' = Least memory usage
 * 'C' = Cache optimal (Keep result matrix in cache)
 *
 * Parameters:
 * 
 * p = matrix sizes
 * n = number of matrices
 * s = matrix which saves the split positions
 * cf = char to indicate which cost function
 * order = array which indicates in which order the matrices are multiplied
 * m = cost function matrix via dynamic programming
 *
 */

*int computeChainOrder(int* p , int n, int** s, char cf) {

	int i,j,k,l;


	//Needed for the cache optimal cost function
	unsigned long addLeft,addRight;
	
	//M[n][n] is our "table to determine number of multiplications 
	//Best result is in m[0][n]
	//q is is used for buffering calculation costs

    unsigned long m[n][n];
	unsigned long q;

	//Cost is zero while multiplying one matrix
	
	for (i=0; i<n; i++)
		m[i][i] = (unsigned long) 0;
		

	//l is chain length
	for (l=2; l<n+1; l++) {
		
		for (i=0; i<n-l+1; i++) {
			
			j = i+l-1;
			m[i][j] = ULONG_MAX;

			if (cf == 'C') {
				//printf("addLeft: %d, addRight: %d\n", addLeft, addRight);
				addLeft = m[i][j-1] + p[i]*p[j]*p[j+1];
				addRight = m[i+1][j] + p[i]*p[i+1]*p[j+1];
				if (addLeft < addRight) {
					m[i][j] = addLeft;
					s[i][j] = i;
				} else {
					m[i][j] = addRight;
					s[i][j] = j-1;
				}

			} else {		 
			
				for (k=i; k<j; k++) {

					switch(cf) {
						case 'F':
						 //q = cost/scalar multiplications
						 q = m[i][k] + m[k+1][j] + ((unsigned long) (p[i]*p[k+1]*p[j+1]));
						 break; 
					
						case 'M':
						 //q = scalar costs of child + left/right neighbour
						 q = m[i][k] + m[k+1][j] + ((unsigned long) (p[i]*p[k+1] + p[k+1]*p[j+1]));
						 break;

						default:
						 printf("Invalid Option");
					}
				
				//Save k in s to remember where to split chain when multiplying
					if (q < m[i][j]) {
					
						m[i][j] = q;
						s[i][j] = k;
					}

				}

			}
		}	
		
	}

    return order;
	
}

/* Algorithm to multiply matrix chain with the correct order
 * With the call s[0,n] you can recursivley ask where to split the chains
 * Abort criteria is calling s[i,i], which returns A[i]
 */

double *dgemmChainDynamic(double **A, int** s, int *p, int i, int j) {

	if (i < j) {

		double *X, *Y, *Z;

		//X has sizes of the left part of the chain
		//Y analogue has the sizes of the right side of the chain

		/**X = malloc(p[i]*p[s[i][j]]*sizeof(double));
		
		X = dgemmChainDynamic(A,s,p,i,s[i][j]);

		Y = malloc((p[s[i][j]])*p[j+1]*sizeof(double));

		Y = dgemmChainDynamic(A,s,p,s[i][j]+1,j);	*/

		Z = malloc(p[i]*p[j+1]*sizeof(double));

		double alpha,beta;

		alpha = 1.0; beta = 0.0;

		int m,n,k;

		m = p[i]; n = p[j+1]; k = p[s[i][j]];

		cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, m, n, k, alpha, X, k, Y, n, beta, Z, n);

		return(Z); 

	} else { 
		return A[i];
	}

}


//Function to multiply matrices with a specific order

double *multiplyMatrices(double **A, int **s, int *p, int n, double *Res) {
	
	//Recursive function
	Res = dgemmChainDynamic(A,s,p,0,n-1);

	return Res;

}


//Recursive part of printing chain. Chain [i,j] is then referenced as chain part [j]

void printRecursiveChain(int **s, int i, int j, int *order, int pos) {

	if (i < j) {

		int k;

		k = s[i][j];

		printRecursiveChain(s, i, k, order, pos-2);

		printRecursiveChain(s, k+1, j, order, pos-2);

		printf("(%d, %d)", k, j);

        order[pos-2] = k;

        order[pos-1] = j;

	}

}


//Function to print chain with recursive call

int *printChain(int **s, int i, int j, int *order) {

	printf("The chain is the following: [ "); 

	printRecursiveChain(s, i, j, order, 2*j);

	printf(" ]\n\n");

    //Now print order array
    
    printf("The order array is the following: [ ");

    int k;

    for(k=0; k<j; k++) {
        printf("(%d, %d)", order[2*k], order[2*k+1]);
    }

	printf(" ]\n\n");

    return order;

}

//Function to evaluate results in terms of the amount of time spent

/** Parameters
 *
 * A = Matrix array
 * cost = cost matrix 
 * size = matrix size array
 * n = number of matrices
 * cf = evaluation method
 *
 */

void evaluateResults(double **A, int **cost, int *size, int n, char cf)  {

	//Get the order of the multiplication of matrices

    int order[2*n]; 

	order = computeChainOrder(size,n,cost,cf);

	//Start computation time
	
	double wtime_start, wtime_end, wtime_sum;

	int i,its;

	its = 1000;

	wtime_sum = 0.0;

	//At first lets do this in sequential
	
	printf("Sequential results:\n\n");

	/*for (i=0; i<1000; i++) {
		
		double *Res = (double *) malloc(p[0]*p[n]*sizeof(double));
		
		wtime_start = omp_get_wtime();		

		multiplyMatrices(A,s,p,n,Res);
		
		wtime_end = omp_get_wtime();

		wtime_sum += (wtime_end - wtime_start);
			
		free(Res);
		Res = NULL;

	}*/

	//Print the execution time of multiplications

	//printf("The wall time that is needed for cost function %c at %d iterations: %2.7f\n", cf, its, (double) (wtime_sum));

}

int main() {

	srand(time(NULL));

	//Initial matrix where each matrix is saved and the copy used for each evaluation method
	double **A, **copyA;
	int i,j,n;

	//Time variables
	unsigned long ticksStart, ticksEnd, ticksSum;
	unsigned long ticksFlops;

	//Matrix to save costs of matrix chain multiplications
	int **cost;

	/**START OF IMPORTANT VALUES
 	*
 	* n = Number of matrices in the chain
 	* size[n+1] = Size of matrices
 	**/

	n = 4; 
	int size[n+1];

	for (i=0; i<n+1; i++) {
		size[i] = (rand() % 90) + 10;
	}

	//OR set array yourself
	
	//int size[11] = {100,90,80,70,60,50,60,70,80,90,100};

	/**
	*
	*END OF IMPORTANT VALUES **/

	//Allocate pointer array of size n for all matrices	
	
    A = (double **) malloc(n*sizeof(double*));
    copyA = (double **) malloc(n*sizeof(double*));

	//Allocate matrix cost for dynamic programming results
	
    cost = (int **) malloc(n*sizeof(int*));

	for (i=0; i<n; i++) 
		cost[i] = (int*) malloc(n*sizeof(int)); 

	if ((A == NULL) || (copyA == NULL) || (cost == NULL)) {
	
		printf("Error! Memory not allocated!");
		exit(0);

	}

	//Initialize matrices and 	

	initializeMatrices(size,n,A);

	if ((A == NULL) || (copyA == NULL)) {
	
		printf("Error! Matrix memory not allocated!");
		exit(0);

	}

	printf("Lets start!\n\n");

	//Print matrix size coefficients in size[]
	
	printf("The matrix sizes are:\n");
	for (i=0; i<n; i++) {
		printf("size[%d]: [%dx%d]\n", i, size[i], size[i+1]);
	}

	printf("\n\nNow the evaluation results: \n\n");


	//Now time to evaluate results with the different cost functions. 
	//After each cost function rewrite copyA to continue with next matrix
	
	printf("Results for minimum Flops:\n\n");	

	evaluateResults(copyA,cost,size,n,'F');

	printf("\n\nResults for minimal Memory usage:\n\n");

	evaluateResults(A,cost,size,n,'M');

	printf("\n\nResults for optimale cache usage:\n\n");

	evaluateResults(A,cost,size,n,'C');

	for (i=0; i<n; i++) free(A[i]);
	free(A);
	for (i=0; i<n; i++) free(cost[i]);	
	free(cost);

	printf("\nDone! \n\n\n");

	return(0);

}
