
SHELL := /bin/bash
EXEC := main

## NODES := $(shell echo $$((2*$(shell cat nodes | wc -l))))
NODES := 16

## Preparation

## -----------------    Build     -----------------
build: main.c
	source /opt/nfs/config/source_mpich32.sh && mpicc -o main main.c

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
	source /opt/nfs/config/source_mpich32.sh && mpiexec -n 16 ./main


## Cleaning

## -----------------    Clean    -----------------
clean:
	rm -f *.o $(EXEC) 