CC = icc
LD = icc
CFLAGS = -g -xhost -ipo -O3 -std=gnu99 -qopenmp -static-intel -ansi-alias -qopt-assume-safe-padding -qopt-report=5 -qopt-malloc-options=4 #-qopt-streaming-stores=always

#CC = gcc
#LD = gcc
#CFLAGS = -g -flto -march=native -mtune=native -O3 -fopenmp -fopenmp-simd -fstrict-aliasing -std=gnu99

LDFLAGS = $(CFLAGS)

.PHONY:  default all clean

default: all ;

all: poly_rw.x poly_ro.x ;

poly_rw.o: poly_driver.o

poly_ro.o: poly_driver.o

poly_driver.o: util.o poly_driver.h

util.o: util.h

clean:
	rm -f *.x *.o

%.x: %.o util.o
	$(LD)    $< -o $@ $(LDFLAGS) util.o poly_driver.o
	touch $@

%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)
	touch $@
