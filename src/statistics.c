#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>


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
    fprintf(fp,"Size 1");

    for(i=1;i<n;i++)
        fprintf(fp,",Size %d",i+1);

    fprintf(fp,",Cache Size,Time Floating Point,Time Least Memory Usage,Time Consecutive");

    fclose(fp);

    printf("Finished creating new statistics file!\n\n");
    
    int numCol = n+1+5;

    return numCol;

}

/* Function to add a line of statistics to the statistics file
 *
 * Arguments:
 *
 * buffer = String needed to be appended to the statistics file
 * numCol = number of columns in the file for error checks
 *
 */

void addStatisticsToFile(char *buffer, int numCol) {

    FILE *fp;
    int i;   

    char *fname = "results.csv";

    //If file doesn't exist create it

    if (access(fname,F_OK) != -1)
        //TODO Find out a better way how to manage the error here
        createStatisticsFile(numCol-6);    
    
    //Number of commata that should be in the buffer
    int commata = 0;

    for(; *buffer; buffer++)
        if (*buffer == ',')
            commata = commata + 1;


    fp = fopen(fname,"w+");

    if (commata == numCol) {
   
        fprintf(fp,"buffer");
        printf("Finished updating statistics file!\n\n");

    } else {

        fprintf(fp,"Wrong number of elements in this row: %d", commata);
        printf("Wrong number of elements in this row: %d", commata);

    }   

     fclose(fp);
    
}

