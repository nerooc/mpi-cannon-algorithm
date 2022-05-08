
SHELL := /bin/bash
EXEC := main

NODES := $(shell echo $$((2*$(shell cat nodes | wc -l))))

## Preparation

## -----------------    Source MPI    -----------------
prepare: 
	source /opt/nfs/config/source_mpich32.sh

## -----------------    Build     -----------------
build: prepare main.c
	mpicc -o main main.c

## -----------------    Create nodes    -----------------
nodes: prepare
	/opt/nfs/config/station206_name_list.sh 1 16 > nodes


## Running

## -----------------    Run on multiple computers    -----------------
run: prepare
	mpiexec -f nodes -n $(NODES) ./main


## -----------------    Run on one computer    -----------------

##        ----------    Run with verbose    ----------
run_one_verb: prepare
	mpiexec -n 16 ./main -v


##        ----------    Run without verbose    ----------
run_one: prepare
	mpiexec -n 16 ./main


## Cleaning

## -----------------    Clean    -----------------
clean:
	rm -f *.o $(EXEC) 