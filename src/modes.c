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

#define NRUNS 50

#define NCF 2

#define TOP 5

#define BOTTOM 3

#define NUM_THREADS 8

void specMode(struct node **allTree, double **A, double **copyA, double **interRes, int *sizes, int *copySizes, int **allOrder, double *orderCostFP, int *rankFP, double *orderCostMEM, int *rankMEM, char *statString, char *sizeString, int numOrder, int numCol, char mode) {

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
                    timeMeasure = calculateChainSequential(copyA,interRes,copySizes,allTree[pos],'S',NUM_THREADS);
                    break;

                case'B':
                    timeMeasure = calculateChainSequential(copyA,interRes,copySizes,allTree[pos],'B',NUM_THREADS);
                    break;
    
                case'T':       
                    timeMeasure = calculateChainTaskParallel(copyA,interRes,copySizes,allTree[pos],'T',NUM_THREADS);
                    break;
 
                case'C':       
                    timeMeasure = calculateChainTaskParallel(copyA,interRes,copySizes,allTree[pos],'C',NUM_THREADS);
                    break;
            
                default:
                    printf("Not a valid mode: %c\n\n",mode);
                    break;

            }

            printf("Finished calculating the chain in %lfs\n\n", timeMeasure);
            
            resetMatricesCopy(A,copyA,copySizes,N);

            resetCopySizes(sizes,copySizes,N+1);

            resetInterMatrices(interRes,N-1);

            printf("Quickly adding the results to the statistics...\n\n");

            createStatisticString(statString,sizeString,allOrder[pos],orderCostFP[pos],rankFP[pos],orderCostMEM[pos],rankMEM[pos],timeMeasure,N,mode);
           
            addStatisticsToFile(statString,statString,numCol); 

        }

    }

    free(relIndices);

}

void allMode(struct node **allTree, double **A, double **copyA, double **interRes, int *sizes, int *copySizes, int **allOrder, double *orderCostFP, int *rankFP, double *orderCostMEM, int *rankMEM, char *statString, char *sizeString, int numCol, int numOrder, char mode) {

    int i,j;

    double timeMeasure = 0.;

    for(i=0;i<numOrder;i++) {

        for(j=0;j<NRUNS;j++) {

            printf("Setting up the matrices for the intermediate results in iteration %d, run %d\n\n",i,j);

            setupInterMatrices(interRes,allOrder[i],copySizes,N-1);

            resetCopySizes(sizes,copySizes,N+1);

            printf("Finally calculating the results...\n\n");

            switch(mode) {

                case 'S':
                    timeMeasure = calculateChainSequential(copyA,interRes,copySizes,allTree[i],'S',NUM_THREADS);
                    break;

                case'B':
                    timeMeasure = calculateChainSequential(copyA,interRes,copySizes,allTree[i],'B',NUM_THREADS);
                    break;
    
                case'T':       
                    timeMeasure = calculateChainTaskParallel(copyA,interRes,copySizes,allTree[i],'T',NUM_THREADS);
                    break;
            
                case'C':       
                    timeMeasure = calculateChainTaskParallel(copyA,interRes,copySizes,allTree[i],'C',NUM_THREADS);
                    break;
 
                default:
                    printf("Not a valid mode: %c\n\n",mode);
                    break;

            }

            printf("Finished calculating the chain in %lfs\n\n", timeMeasure);
            
            resetMatricesCopy(A,copyA,copySizes,N);

            resetCopySizes(sizes,copySizes,N+1);

            resetInterMatrices(interRes,N-1);

            printf("Quickly adding the results to the statistics...\n\n");

            createStatisticString(statString,sizeString,allOrder[i],orderCostFP[i],rankFP[i],orderCostMEM[i],rankMEM[i],timeMeasure,N,mode);
          
            addStatisticsToFile(statString,statString,numCol); 

        }

    }

}


