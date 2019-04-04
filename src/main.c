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
#include <math.h>
#include <limits.h>
#include <mkl.h>
#include <float.h>
#include <unistd.h>
#include <string.h>
#include "statistics.h"
#include "matrix.h"
#include "array.h"
#include "helper.h"
#include "error.h"
#include "calculation.h"
#include "binarytree.h"
#include "bli_clock.h"

/* Value to define the CPU frequenz. Can also be defined in the makefile. Always take the frequence divided my 1000 for milliseconds
 *
 */

//CURRENTLY OBSOLETE

#define CPU 3300000

/* Value to define the size of level 3 of the CPU cache. Is needed for the cache scrub
 *
 */

#define CACHESIZE 30000000

/* Value how many matrices are used
 *
 */

#define N 4

/* Value how many iterations a computation should walk through
 *
 */

#define NRUNS 1

#define NCF 2

#define TOP 5

#define BOTTOM 3

/* MODE = 0: All orders
 * MODE = 1: Only TOP and BOTTOM orders
 *
 */

#define MODE 1 

#define SEQ 1

#define BLAS_PARL 0

#define TASK_PARL 0

#define COMB_PARL 0


void specMode(struct node **allTree, double **A, double **copyA, double **interRes, int *sizes, int *copySizes, int **allOrder, int *orderCostFP, int *rankFP, int *orderCostMEM, int *rankMEM, char *statString, char *sizeString, int numOrder, int numCol, char mode) {

    int i,j, indPos, rankPos;
    double timeMeasure = 0.;

    int numRelOrder = NCF*(TOP+BOTTOM);

    int *relIndices = (int*) malloc(numRelOrder*sizeof(int));
    for(i=0;i<numRelOrder;i++)
        relIndices[i] = numOrder+1;

    indPos = 0;

    for(i=0;i<TOP;i++) {

        if(indPos >= numOrder)
            break;

        rankPos = 0;

        while((rankFP[rankPos] != i+1) && (rankPos < numOrder))
            rankPos = rankPos+1;

        if((!contains(relIndices,rankPos,indPos+1)) && (rankPos < numOrder)) {
            relIndices[indPos] = rankPos;
            indPos = indPos+1;
            //printf("Added %d due to rankFP = %d\n\n",rankPos, rankFP[rankPos]);
        }

        rankPos = 0;

        while((rankMEM[rankPos] != i+1) && (rankPos < numOrder))
            rankPos = rankPos+1;

        if((!contains(relIndices,rankPos,indPos+1)) && (rankPos < numOrder)) {
            relIndices[indPos] = rankPos;
            indPos = indPos+1;
            //printf("Added %d due to rankMEM = %d\n\n",rankPos, rankMEM[rankPos]);
        }   

        //printf("I'm at end of %d, indpos is %d\n\n",i,indPos);  

    }

    for(i=0;i<BOTTOM;i++) {

        if(indPos >= numOrder)
            break;

        rankPos = numOrder-1;

        while((rankFP[rankPos] != numOrder-i) && (rankPos >= 0))
            rankPos = rankPos-1;

        if((!contains(relIndices,rankPos,indPos)) && (rankPos >= 0)) {
            relIndices[indPos] = rankPos;
            indPos = indPos+1;
        }

        rankPos = numOrder-1;

        while((rankMEM[rankPos] != numOrder - i) && (rankPos >= 0))
            rankPos = rankPos-1;

        if((!contains(relIndices,rankPos,indPos)) && (rankPos >= 0)) {
            relIndices[indPos] = rankPos;
            indPos = indPos+1;
        }     

    }

    int pos;

    /*for(i=0;i<numRelOrder;i++)
        printf("Pos: %d\n",relIndices[i]);
    printf("\n");*/

    for(i=0;i<numRelOrder;i++) {

        if(relIndices[i] == numOrder+1)
            break;

        pos = relIndices[i];

        for(j=0;j<NRUNS;j++) {

            printf("Setting up the matrices for the intermediate results in iteration %d, run %d\n\n",i,j);

            setupInterMatrices(interRes,allOrder[pos],copySizes,N-1);

            resetCopySizes(sizes,copySizes,N+1);

            printf("Finally calculating the results...\n\n");

            switch(mode) {

                case 'S':
                    timeMeasure = calculateChainSequential(copyA,interRes,copySizes,allTree[pos]);
                    break;

                case'B':
                    timeMeasure = calculateChainSingleParallel(copyA,interRes,copySizes,allTree[pos]);
                    break;
    
                case'T':       
                    timeMeasure = calculateChainTaskParallel(copyA,interRes,copySizes,allTree[pos]);
                    break;
            
                default:
                    printf("Not a valid mode: %s\n\n",mode);
                    break;

            }

            printf("Finished calculating the chain for minimal flops! (%lfs)\n\n", timeMeasure);
            
            resetMatricesCopy(A,copyA,copySizes,N);

            resetCopySizes(sizes,copySizes,N+1);

            resetInterMatrices(interRes,N-1);

            printf("Quickly adding the results to the statistics...\n\n");

            createStatisticString(statString,sizeString,allOrder[pos],orderCostFP[pos],rankFP[pos],orderCostMEM[pos],rankMEM[pos],timeMeasure,N);
           
            addStatisticsToFile(statString,statString,numCol); 

        }

    }

    free(relIndices);

}

void allMode(struct node **allTree, double **A, double **copyA, double **interRes, int *sizes, int *copySizes, int **allOrder, int *orderCostFP, int *rankFP, int *orderCostMEM, int *rankMEM, char *statString, char *sizeString, int numCol, int numOrder, char mode) {

    int i,j;

    for(i=0;i<numOrder;i++)
        printf("%d and %d\n",rankFP[i],rankMEM[i]);
    printf("\n");

    double timeMeasure = 0.;

    for(i=0;i<numOrder;i++) {

        for(j=0;j<NRUNS;j++) {

            printf("Setting up the matrices for the intermediate results in iteration %d, run %d\n\n",i,j);

            setupInterMatrices(interRes,allOrder[i],copySizes,N-1);

            resetCopySizes(sizes,copySizes,N+1);

            printf("Finally calculating the results...\n\n");

            switch(mode) {

                case 'S':
                    timeMeasure = calculateChainSequential(copyA,interRes,copySizes,allTree[i]);
                    break;

                case'B':
                    timeMeasure = calculateChainSingleParallel(copyA,interRes,copySizes,allTree[i]);
                    break;
    
                case'T':       
                    timeMeasure = calculateChainTaskParallel(copyA,interRes,copySizes,allTree[i]);
                    break;
            
                default:
                    printf("Not a valid mode: %s\n\n",mode);
                    break;

            }

            printf("Finished calculating the chain for minimal flops! (%lfs)\n\n", timeMeasure);
            
            resetMatricesCopy(A,copyA,copySizes,N);

            resetCopySizes(sizes,copySizes,N+1);

            resetInterMatrices(interRes,N-1);

            printf("Quickly adding the results to the statistics...\n\n");

            createStatisticString(statString,sizeString,allOrder[i],orderCostFP[i],rankFP[i],orderCostMEM[i],rankMEM[i],timeMeasure,N);
           
            addStatisticsToFile(statString,statString,numCol); 

        }

    }

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
 	* sizes = Size of matrices
 	* copySizes = This copy is needed for the functions which evaluate the matrix chain and therefore 
 	* Never pass over sizes as an argument, instead use copySizes!
 	*
 	* sizeMin,sizeMax = min & max matrix sizes
 	*
 	* cost (CURRENTLY OBSOLETE) = Matrix used to compute optimal costs via dynamic programming
 	* 
 	* split (CURRENTLY OBSOLETE) = Matrix which saves the split position of computing
 	* a matrix chain (e.g. split[0,2] = 1, the computation is
 	* splitted in position 1
 	*
 	* fac = fac(N-1) -> Number of possible multiplication orders
 	* 
 	* allOrders = Matrix where all possible orders are saved. Save the cost of this order at the end of each array
 	*
 	* orderCost[FP/MEM] = Array that saves the according cost of that multiplicationOrder according to the cost function
 	* rank[FP/MEM] = Rank of according cost (After being sorted)
 	*
 	* statString = String which will include all values needed for the statistics. Will be printed into output file
 	* sizeString = Redundant part of statString which is the part of all matrix sizes
 	*
 	* numCol = Number of columns in our statistic file. Used to catch errors if there aren't enough elements in statString
 	*
 	* i,j indices
 	*
 	*
 	**/

    int i,j;

    double **A;
    double **copyA;
    A = (double **) mkl_malloc(N*sizeof(double*),64);
    copyA = (double **) mkl_malloc(N*sizeof(double*),64);

    double **interRes;
    interRes = (double **) mkl_malloc((N-1)*sizeof(double*),64);
	
    if ((A == NULL) || (interRes == NULL)) {
	
		printf("Error! Memory for A or interRes not allocated!");
		exit(0);

	}

	int sizes[N+1];
    int copySizes[N+1];

    int sizeMin;
    int sizeMax;    


    //Check number of arguments and set matrix sizes accordingly
    if (argc == 1) {
        sizeMin = 10;
        sizeMax = 1000;

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

    int numOrder;
    numOrder  = factorial(N-1);

    int removed = 0;
    

    int **allOrder;
    allOrder = (int**) malloc(numOrder*sizeof(int*));
    for(i=0;i<numOrder;i++)
        allOrder[i] = (int*) malloc((2*(N-1))*sizeof(int));

    int *orderCostFP;
    orderCostFP = (int*) malloc(numOrder*sizeof(int));
    for(i=0;i<numOrder;i++)
        orderCostFP[i] = 0;

    int *rankFP;
    rankFP = (int*) malloc(numOrder*sizeof(int));

    int *orderCostMEM;
    orderCostMEM = (int*) malloc(numOrder*sizeof(int));
    for(i=0;i<numOrder;i++)
        orderCostMEM[i] = 0;

    int *rankMEM;
    rankMEM = (int*) malloc(numOrder*sizeof(int));

    //TODO Try to find a better value for those chars?
    char *statString;
    statString = (char*) malloc(10*N+512*sizeof(char));    

    char *sizeString;
    sizeString = (char*) malloc(10*N*sizeof(char));

    int numCol;

    struct node **allTree;
    allTree = (struct node**) malloc(numOrder*sizeof(struct node*));
    for(i=0;i<numOrder;i++)
        allTree[i] = NULL;
 
    /**
     *
     * END OF VARIABLES */ 

        
/**********  SETUP SECTION **************/

    srand(time(NULL));

    printf("Creating matrix sizes...\n\n");   

    setMatrixSizes(sizes,copySizes,N,sizeMin,sizeMax);

	printf("The matrix sizes are:\n");
	for (i=0; i<N; i++) {
		printf("size[%d]: [%dx%d]\n", i, copySizes[i], copySizes[i+1]);
	}


    printf("\nAllocation memory for matrices...\n\n");

	setupMatrices(A,copyA,copySizes,N);

    printf("Creating matrices with random values...\n\n");

    initializeMatrices(A,copyA,copySizes,N);

    printf("Creating statistics file...\n\n");

    numCol = createStatisticsFile(N);

    createSizeString(sizeString,copySizes,N);

    printf("Creating all permutation possibilities...\n\n");

    getAllOrders(allOrder,N-1);

    printf("Converting all orders so they are readable...\n\n");

    convertOrders(allOrder,N-1);

    printf("Creating traversable trees...\n\n");

    for(i=0;i<numOrder;i++) {
        allTree[i] = createTree(allTree[i],allOrder[i], N-1);  
        //printf("Created tree %d!\n\n",i);
    }

    printf("Removing duplicate trees and multiplication orders...\n\n");

    removed = removeDuplicates(allOrder, allTree, numOrder, N-1);    

    printf("Removed %d duplicate(s) out of %d tree(s)!\n\n",removed, numOrder);

    numOrder = numOrder-removed;

    printf("Now computing the costs for each pair (order,cost function)...\n\n");

    normalizeSizes(sizes,copySizes,sizeMin,sizeMax,N);

    computeChainCosts(allOrder,orderCostFP,copySizes,N,numOrder,'F');

    /*for(i=0;i<numOrder;i++)
        printf("%d\n",orderCostFP[i]);
    printf("\n");*/

    resetCopySizes(sizes,copySizes,N+1);
 
    normalizeSizes(sizes,copySizes,sizeMin,sizeMax,N);
  
    computeChainCosts(allOrder,orderCostMEM,copySizes,N,numOrder,'M');

    /*for(i=0;i<numOrder;i++)
        printf("%d\n",orderCostMEM[i]);
    printf("\n");*/

    resetCopySizes(sizes,copySizes,N+1);

    printf("Now ranking each cost\n\n");

    rankElements(orderCostFP,rankFP,numOrder);

    rankElements(orderCostMEM,rankMEM,numOrder);

    /*for(i=0;i<numOrder;i++)
        printf("%d and %d\n",rankFP[i],rankMEM[i]);
    printf("\n");*/


	printf("Now the evaluation results... \n\n");

	//Now time to evaluate results with the different cost functions. 
	//After each cost function rewrite copyA to continue with next matrix


/********** TIMING SECTION **************/

    if(MODE == 0) {

        if(SEQ)
            allMode(allTree,A, copyA, interRes, sizes, copySizes, allOrder, orderCostFP, rankFP, orderCostMEM, rankMEM, statString, sizeString,  numCol, numOrder, 'S');

        if(BLAS_PARL)
             allMode(allTree,A, copyA, interRes, sizes, copySizes, allOrder, orderCostFP, rankFP, orderCostMEM, rankMEM, statString, sizeString, numCol, numOrder, 'B');
 
        if(TASK_PARL)      
             allMode(allTree,A, copyA, interRes, sizes, copySizes, allOrder, orderCostFP, rankFP, orderCostMEM, rankMEM, statString, sizeString, numCol, numOrder, 'T');

        if(COMB_PARL)
             allMode(allTree,A, copyA, interRes, sizes, copySizes, allOrder, orderCostFP, rankFP, orderCostMEM, rankMEM, statString, sizeString, numCol, numOrder, 'C');


    } else if (MODE == 1) {

        if(SEQ)
            specMode(allTree,A, copyA, interRes, sizes, copySizes, allOrder, orderCostFP, rankFP, orderCostMEM, rankMEM, statString, sizeString, numOrder, numCol, 'S');

        if(BLAS_PARL)
             specMode(allTree,A, copyA, interRes, sizes, copySizes, allOrder, orderCostFP, rankFP, orderCostMEM, rankMEM, statString, sizeString, numOrder, numCol, 'B');
 
        if(TASK_PARL)      
             specMode(allTree,A, copyA, interRes, sizes, copySizes, allOrder, orderCostFP, rankFP, orderCostMEM, rankMEM, statString, sizeString, numOrder, numCol, 'T');

        if(COMB_PARL)
             specMode(allTree,A, copyA, interRes, sizes, copySizes, allOrder, orderCostFP, rankFP, orderCostMEM, rankMEM, statString, sizeString, numOrder, numCol, 'C');

    } else {

        printf("ERROR! Invalid mode %d",MODE);

    }
    
    printf("The chains have been calculated! Now a quick cleanup...\n\n");

	for (i=0; i<N; i++) {
    	mkl_free(A[i]);
    }

    mkl_free(A);
    mkl_free(interRes); 
    mkl_free(copyA);

    for(i=0;i<numOrder;i++) {
        free(allOrder[i]);
        destroyTree(allTree[i]);
    }

    free(allOrder);
    free(allTree);
    free(orderCostFP);
    free(rankFP);
    free(orderCostMEM);
    free(rankMEM);
    free(statString);
    free(sizeString);

	printf("Done! \n\n\n");

	return(0);

}
