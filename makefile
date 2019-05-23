CC = gcc

MKLROOT =  /opt/intel/mkl/2019.0.117

CFLAGS = -O0 -Wall -m64 -I${MKLROOT}/include -fopenmp -Isrc -std=c99 -g 

MKLPARL =  -L${MKLROOT}/lib/intel64 -Wl,--no-as-needed -lmkl_intel_lp64 -lmkl_intel_thread -lmkl_core -liomp5 -lpthread -lm -ldl
MKLSEQ  =  -L${MKLROOT}/lib/intel64 -Wl,--no-as-needed -lmkl_intel_lp64 -lmkl_sequential -lmkl_core -lpthread -lm -ldl

HEADERS = src/main.c src/statistics.c src/array.c src/matrix.c src/error.c src/helper.c src/bli_clock.c src/calculation.c src/binarytree.c src/modes.c 

OUT = build/out.x

all:
	$(CC) $(CFLAGS) $(MKLPARL) $(LIBS) $(HEADERS) -o $(OUT)

