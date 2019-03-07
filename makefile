
all:
	gcc -fopenmp -L /opt/intel/mkl/11.3.3.210/mkl/lib/intel64 -lmkl_rt -lpthread -lm -ldl src/main.c src/statistics.c src/array.c src/matrix.c src/error.c src/helper.c src/bli_clock.c src/calculation.c src/binarytree.c -g -I src/ -o build/out.x
	
