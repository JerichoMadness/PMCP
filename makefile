CC = gcc

CFLAGS = -DMKL_ILP64 -m64 -I${MKLROOT}/include

MKLROOT = /opt/intel/mkl/2019.0.117

MKLPARL =  -L${MKLROOT}/lib/intel64 -Wl,--no-as-needed -lmkl_intel_ilp64 -lmkl_intel_thread -lmkl_core -liomp5 -lpthread -lm -ldl

MKLSEQ =  -L${MKLROOT}/lib/intel64 -Wl,--no-as-needed -lmkl_intel_ilp64 -lmkl_sequential -lmkl_core -lpthread -lm -ldl

LIBS = -fopenmp -lmkl_rt 

HEADERS = src/main.c src/statistics.c src/array.c src/matrix.c src/error.c src/helper.c src/bli_clock.c src/calculation.c src/binarytree.c src/modes.c 

OUT = -I src/ -o build/out.x

all:
	
	$(CC) $(CFLAGS) $(MKLPARL) $(LIBS) $(HEADERS) $(OUT)

