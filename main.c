
#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ---------   Defines    ---------- */

// Variable to print logs 
// 0: logs invisible
// 1: logs visible
#define VERBOSE 0

/* ---------    Structures and Consts    ---------- */


typedef struct 
{
    float* mat;
    int row;
    int col;
} matrix_data;

/* ---------    Function definitions    ---------- */
void print_matrix(matrix_data mat);
matrix_data initialize_matrix(FILE *file_pointer /*Pointer to file containing matrix*/,
                        matrix_data mat /*Pointer to mat which have to be initialized */);



/* ---------    Function declarations    ---------- */


int main (int argc, char *argv[])
{
    int	numtasks,              /* number of tasks in partition */
        taskid,                /* a task identifier */
        numworkers,            /* number of worker tasks */
        source,                /* task id of message source */
        dest,                  /* task id of message destination */
        mtype,                 /* message type */
        rows,                  /* rows of matrix A sent to each worker */
        averow, extra, offset; /* used to determine rows sent to each worker */

    MPI_Status status;

    matrix_data A,
                B;
    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD,&taskid);
    MPI_Comm_size(MPI_COMM_WORLD,&numtasks);
    if (taskid == 0)
    {
        FILE* file_pointer;
        /* ---------    Initialize matrixes    ---------- */
        file_pointer = fopen("A.csv","r");
        A = initialize_matrix(file_pointer, A);
        if(VERBOSE)
        {
            print_matrix(A);
            fprintf(stdout, "rows: %d , cols: %d",A.row,  A.col);
        }
        fclose(file_pointer);
        file_pointer = fopen("B.csv","r");
        B = initialize_matrix(file_pointer, B);
        if(VERBOSE)
        {
            print_matrix(B); 
            fprintf(stdout, "rows: %d , cols: %d",B.row,  B.col);
        }
        fclose(file_pointer);

        fflush(stdout);

        // temporary dealocation
        free(A.mat);
        free(B.mat);
        /* TODO: Check if number of processes is equal of number of columns of first matrix */

    }

MPI_Finalize();
}



matrix_data initialize_matrix(FILE *file_pointer /*Pointer to file containing matrix*/,
                        matrix_data mat /*Pointer to mat which have to be initialized */)
{
    char buffer[1024];
    float n;
    char ch;
    char *line,
         *record;
    int row = 0,
        col = 0;
    while((line = fgets(buffer,sizeof(buffer), file_pointer)) != NULL)
    {
        if(row == 0)
        {
            for(int i = 0; i < strlen(line); i++)
            {  
                if(line[i] == ',')
                {
                    col++;  
                }
            }  
        }
        row++;
    }
    if(VERBOSE)
    {
        fprintf(stdout, "rows: %d , cols: %d \n",row,  col);	
    }

    fseek(file_pointer, 0, SEEK_SET);

    mat.mat = (float *)malloc(sizeof(float) * row * col);

    int k=0;
    int i = 0;
    while((line = fgets(buffer,sizeof(buffer), file_pointer)) != NULL && i++ < row)
    {
        record = strtok(line, ",");
        int j = 0;
        while(record != NULL && j++ < col)
        {
            mat.mat[k] = atof(record);
            record = strtok(NULL,",");
            k++;
        }
    }
    mat.col = col;
    mat.row = row;
    return mat;
}

void print_matrix(matrix_data mat)
{
    int cnt = 0;
    for(int i = 0; i < mat.row; i++) 
    {
        for(int j = 0; j < mat.col; j++)
        {
            fprintf(stdout, "%1.2f\t", mat.mat[cnt]);
            cnt++; 
        }
        fprintf(stdout, "\n");
    }
}