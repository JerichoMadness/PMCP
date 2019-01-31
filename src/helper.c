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

/* Helper function to get the factorial of a number n
 *
 * Return value:
 *
 * n = Number
 *
 */

int factorial (int n) {

    if(n==1)
        return n;
    else
        return n*factorial(n-1);

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

