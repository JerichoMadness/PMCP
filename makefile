all:

	gcc -fopenmp -L /opt/intel/mkl/11.3.3.210/mkl/lib/intel64 -lmkl_rt -lpthread -lm -ldl src/main.c -g -o build/out.x
	
