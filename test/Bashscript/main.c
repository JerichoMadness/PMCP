#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include <mkl.h>
#include <float.h>


int main(int argc, char *argv[]) {

    //Part where you perform the bash script to get the cash size

    srand(time(NULL));

    int clevel;

    if(argc == 1)
        clevel = 3;
    else if (argc == 2)
        clevel = atoi(argv[1]);
    else
        printf("Too many arguments!");

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

    int n;

    if (buffer) {
        n = atoi(buffer);
        printf("%d\n\n",n);
    }
    
    n = (int) sqrt(n);

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

    return(1);

}
