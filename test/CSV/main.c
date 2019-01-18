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

int main() {

    printf("Creating .csv file!\n\n");

    FILE *fp;
    int i,j;
   
    char *fname = "results.csv";

    if (access(fname,F_OK) != -1) {

        fp = fopen(fname,"a");

        for(i=0;i<4;i++) {
            fprintf(fp,"\n%d",i);
            for(j=0;j<5;j++)
                fprintf(fp,",%d",j*j);

        }

        fclose(fp);

    } else {

        printf("%s\n\n",fname);
    
        fp = fopen(fname,"w+");

        fprintf(fp,"Number,Size0,Size1,Size2,Size3,Size4");

        fclose(fp);

    }

    printf("Done!\n\n");       

}
