#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

/* Function to create the redundant part of the string (matrix sizes) that is added to 
 * the statistics file
 *
 * Arguments:
 *
 * sizeString = String to save the results
 * sizes = Matrix sizes
 * n = Number of matrices
 *
 */

void createSizeString (char *sizeString, int *sizes, int n) {

    int i;

    char *tmp;
    tmp = malloc(50*sizeof(char));

    sprintf(sizeString,"%d",sizes[0]);

    for(i=1;i<n+1;i++) {
        sprintf(tmp,",%d",sizes[i]);
        strcat(sizeString,tmp);
    }

    printf("Sizestring: %s\n\n",sizeString);

    free(tmp);
}

/* Function to create the entire string for the statistics file with all relevant information
 *
 * Arguments:
 *
 * statString = Resulting string to be added to the statistic file
 * sizeString = Redundant string with all matrix sizes
 * order = The multiplication order
 * costFP = Multiplication cost according to floating-point cost function with that order
 * rankFP = Rank for that cost
 * costMEM = Memory Usage cost function accordingly
 * rankMEM = According rank
 * timeMeasure = Measured time in milliseconds
 * int n = Number of matrices
 *
 */

void createStatisticString(char *statString, char *sizeString, int *order, int costFP, int rankFP, int costMEM, int rankMEM, double timeMeasure, int n) {
    
    int i;

    char *orderString;
    orderString = malloc(20*n*sizeof(char));
    char *tmp;
    tmp = malloc(512*sizeof(char));

    //Start with empty string
    sprintf(statString,"");

    //Append sizes
    strcat(statString,sizeString);

    //Create temporary order string
    sprintf(orderString,"");

    for(i=0;i<n-1;i++) {
        sprintf(tmp,"(%dx%d)",order[2*i],order[2*i+1]);
        strcat(orderString,tmp);
    }

    printf("Orderstring with %d iterations: %s\n\n",n,orderString);

    //Append multiplication order
    sprintf(tmp,",[");
    strcat(statString,tmp);

    strcat(statString,orderString);

    sprintf(tmp,"],");
    strcat(statString,tmp);
 
    //Append rest of the values
    sprintf(tmp,"%d,%d,%d,%d,%lf",costFP,rankFP,costMEM,rankMEM,timeMeasure);

    strcat(statString,tmp);
 
    free(orderString);
    free(tmp);

}



/* Function to create our standard statistics file. Only the heads of the columns are written into the file currently
 *
 * Arguments:
 *
 * n = Number of matrices.
 *
 * Return argument:
 *
 * Number of columns the file has, so the string can be examined on number of elements seperated by a comma
 * Number of columns = #matrix sizes(n+1) + currently five more columns needed
 *
 */

int createStatisticsFile(int n) {

    FILE *fp;
    int i;   

    char *fname = "results.csv";

    //If file exists save the old file

    if (access(fname,F_OK) != -1)
        system("cd scripts ; renameStatisticsFile.sh");

    printf("Creating new statistics file %s\n\n",fname);
    
    fp = fopen(fname,"w+");

    //First argument doesn't need a comma
    fprintf(fp,"Size1");

    for(i=1;i<n+1;i++)
        fprintf(fp,",Size%d",i+1);

    fprintf(fp,",Order,FP Cost,FP Rank,Memory Cost,Memory Rank,Time");

    fclose(fp);

    printf("Finished creating new statistics file!\n\n");
    
    int numCol = n+1+6;

    return numCol;

}

/* Function to add a line of statistics to the statistics file
 *
 * Arguments:
 *
 * buffer = String needed to be appended to the statistics file
 * cpyBuffer = Copy which is needed to be processed for validation
 * numCol = number of columns in the file for error checks
 *
 */

void addStatisticsToFile(char *buffer, char *cpyBuffer, int numCol) {

    FILE *fp;
    int i;   

    char *fname = "results.csv";

    //If file doesn't exist create it

    //if (access(fname,F_OK) == -1)
    //    createStatisticsFile(numCol-6);    
        //TODO Find out a better way how to manage the error here

    //Number of commata = numRows that should be in the buffer
    int commata = 0;

    for(; *cpyBuffer; cpyBuffer++)
        if (*cpyBuffer == ',')
            commata = commata + 1;


    fp = fopen(fname,"a");

    if (commata == numCol-1) {
  
        fprintf(fp,"\n%s",buffer); 
        printf("Finished updating statistics file!\n\n");

    } else {

        fprintf(fp,"\nWrong number of elements in this row: %d", commata+1);
        printf("Wrong number of elements in this row: %d\n", commata+1);

    }   

    fclose(fp);    

}

