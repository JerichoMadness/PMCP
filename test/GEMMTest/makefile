CC = gcc

CFLAGS = -DMKL_ILP64 -m64 -I${MKLROOT}/include

MKLROOT = /opt/intel/mkl/2019.0.117

MKLPARL =  -L${MKLROOT}/lib/intel64 -Wl,--no-as-needed -lmkl_intel_ilp64 -lmkl_intel_thread -lmkl_core -liomp5 -lpthread -lm -ldl

MKLSEQ =  -L${MKLROOT}/lib/intel64 -Wl,--no-as-needed -lmkl_intel_ilp64 -lmkl_sequential -lmkl_core -lpthread -lm -ldl

LIBS = -fopenmp -lmkl_rt 

HEADERS = bli_clock.c main.c

all:
	
	$(CC) $(CFLAGS) $(MKLPARL) $(LIBS) $(HEADERS) -o out.x

