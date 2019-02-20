CC=mpicc

all: 1 2 3 4

1: ring.c
	$(CC) -o 1 -O3 ring.c

2: grid.c
	$(CC) -o 2 -O3 grid.c

3: pcomm.c
	$(CC) -o 3 -O3 pcomm.c

4: transpose.c
	$(CC) -o 4 -O3 transpose.c

clean:
	rm 1 2 3 4
