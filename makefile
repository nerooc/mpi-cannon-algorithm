
SHELL := /bin/bash
EXEC := main nodes C.csv

## NODES := $(shell echo $$((2*$(shell cat nodes | wc -l))))
NODES := 16

## Preparation

## -----------------    Build     -----------------
build: main.c
	source /opt/nfs/config/source_mpich32.sh && mpicc -o main main.c

## -----------------    Valgrind     -----------------
build_valgrind: main.c
	source /opt/nfs/config/source_mpich32.sh && mpicc  -Wall -ggdb3 -O0 -o main main.c

## -----------------    Create nodes    -----------------
nodes:
	/opt/nfs/config/station206_name_list.sh 1 16 > nodes


## Running

## -----------------    Run on multiple computers    -----------------
run:
	source /opt/nfs/config/source_mpich32.sh && mpiexec -f nodes -n $(NODES) ./main


## -----------------    Run on one computer    -----------------

##        ----------    Run with verbose    ----------
run_one_verb:
	source /opt/nfs/config/source_mpich32.sh && mpiexec -n 16 ./main -v


##        ----------    Run without verbose    ----------
run_one:
	source /opt/nfs/config/source_mpich32.sh && mpiexec -n 1  gdb ./main : -n 15 ./main

##        ----------    Run without verbose    ----------
run_one_valgrind:
	source /opt/nfs/config/source_mpich32.sh && valgrind  --leak-check=full \
         --show-leak-kinds=all \
         --track-origins=yes \
         --verbose \
         --log-file=valgrind-out.txt \
		  mpiexec -n 16 ./main


## Cleaning

## -----------------    Clean    -----------------
clean:
	rm -f *.o $(EXEC) 