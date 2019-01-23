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

int factorial(int n) {

    if (n==1)
        return n;

    return n * factorial(n-1);

}

void printArray(int *a, int n) {

    int i;
    int fac = factorial(n);

    int **allOrders = (*int) malloc(fac*sizeof(int));
    for(i=0;i<fac;i++)
        allOrders[i] = (int) malloc(2*n*sizeof(int));
    
    for(i=0;i<n;i++)
        printf("%d",a[i]);

    printf("\n");

}

void swap(int *x, int *y) {

    int temp; 
    temp = *x; 
    *x = *y; 
    *y = temp; 

}

void permute(int *array,int i,int length) { 

  if (length == i){
     printArray(array,length);
     return;
  }

  int j = i;
  for (j = i; j < length; j++) { 
     swap(array+i,array+j);
     permute(array,i+1,length);
     swap(array+i,array+j);
  }

  return;
}
 

int main() {

    int i;

    int n; 
    n = 4;

    int *order = malloc((n-1)*sizeof(int));

    for (i=0;i<n-1;i++)
        order[i] = i;

    permute(order, 0, n-1);

}
