
SHELL := /bin/bash
EXEC := main

NODES := 2 * cat nodes | wc -l

## -----------------    Build     -----------------
build: main.c
	source /opt/nfs/config/source_mpich32.sh && mpicc -o main main.c

## -----------------    Run on multiple computers    -----------------
run: 
	source /opt/nfs/config/source_mpich32.sh && mpiexec -f nodes -n $(NODES) ./main


## -----------------    Run on one computer    -----------------

##        ----------    Run with verbose    ----------
run_one_verb: 
	source /opt/nfs/config/source_mpich32.sh && mpiexec -n 16 ./main -v


##        ----------    Run without verbose    ----------
run_one: 
	source /opt/nfs/config/source_mpich32.sh && mpiexec -n 16 ./main

## -----------------    Clean    -----------------
clean:
	rm -f *.o $(EXEC) 