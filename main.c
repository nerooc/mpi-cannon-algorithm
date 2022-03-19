#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* ---------   Defines    ---------- */

// Variable to print logs 
// 0: logs invisible
// 1: logs visible
#define VERBOSE 0

/* ---------    Structures and Consts    ---------- */

/*
 * Structure:  matrix_data 
 * --------------------
 * holds matrix values
 *
 *  mat: pointer to values of matrix object
 *  row: numbers of rows in matrix object
 *  col: numbers of columns in matrix object
 */
typedef struct 
{
    float* mat;
    int row;
    int col;
} matrix_data;

/* ---------    Function definitions    ---------- */

/*
 * Function:  print_matrix 
 * --------------------
 * prints out given matrix:
 *
 *  mat: Matrix to print out
 *
 *  returns: void
 */
void print_matrix(matrix_data mat);

/*
 * Function: initialize_matrix
 * ----------------------------
 *   One dimentional matrix created from csv file (comma delivered).
 *
 *   file_pointer: Pointer to *.csv file from which will be imported matrix
 *   matrix_data: Pointer to matrix which have to be filled.
 *
 *   returns: Void
 */
void initialize_matrix(FILE *file_pointer /*Pointer to file containing matrix*/,
                        matrix_data *mat /*Pointer to mat which have to be initialized */);


/* ---------    Function declarations    ---------- */


int main (int argc, char *argv[])
{
    int	numtasks,
        id,
        source,
        dest,
        row;
    float a=0,b=0, c=0;
    MPI_Status status;

    matrix_data A,
                B,
                C;

    FILE* file_pointer;

    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD,& id);
    MPI_Comm_size(MPI_COMM_WORLD,&numtasks);


    /* ---------    Master task    ---------- */
    if(id == 0)
    {
        /* ---------    Initialize matrixes    ---------- */
        file_pointer = fopen("A.csv","r");
        initialize_matrix(file_pointer, &A);
        if(VERBOSE)
        {
            print_matrix(A);
            fprintf(stdout, "rows: %d , cols: %d",A.row,  A.col);
        }
        fclose(file_pointer);
        file_pointer = fopen("B.csv","r");
        initialize_matrix(file_pointer, &B);
        if(VERBOSE)
        {
            print_matrix(B); 
            fprintf(stdout, "rows: %d , cols: %d",B.row,  B.col);
        }
        fclose(file_pointer);

        fflush(stdout);
        row = A.row;
    }
    
    MPI_Bcast(&row,1,MPI_INT,0,MPI_COMM_WORLD);
    
    int periods[]={1,1}; //both vertical and horizontal movement; 
    int dims[]={row,row};
    int coords[2]; /* 2 Dimension topology so 2 coordinates */
    int right=0, left=0, down=0, up=0;    // neighbor ranks


    /* ---------    Initialize squared communication topology     ---------- */
    MPI_Comm comm_2d;
    MPI_Cart_create(MPI_COMM_WORLD, 2, dims,periods, 1, &comm_2d);
    MPI_Scatter(A.mat, 1, MPI_FLOAT, &a, 1, MPI_FLOAT, 0, comm_2d);
    MPI_Scatter(B.mat, 1, MPI_FLOAT, &b, 1, MPI_FLOAT, 0, comm_2d);

    /* ---------    redefine id in new topology     ---------- */
    MPI_Comm_rank(comm_2d, &numtasks);  
    MPI_Cart_coords(comm_2d, numtasks, 2, coords);

    MPI_Cart_shift(comm_2d, 1, coords[0], &left,&right);
    MPI_Cart_shift(comm_2d, 0, coords[1], &up,&down);
    MPI_Sendrecv_replace(&a, 1, MPI_FLOAT, left, 0, right, 0, comm_2d, MPI_STATUS_IGNORE);
    MPI_Sendrecv_replace(&b, 1, MPI_FLOAT, up, 0, down, 0, comm_2d, MPI_STATUS_IGNORE);
    c += a * b;
    for(int i = 1; i < row; i++)
    {
        MPI_Cart_shift(comm_2d, 1, 1, &left, &right);
        MPI_Cart_shift(comm_2d, 0, 1, &up, &down);
        MPI_Sendrecv_replace(&a, 1, MPI_FLOAT, left, 0, right, 0, comm_2d, MPI_STATUS_IGNORE);
        MPI_Sendrecv_replace(&b, 1, MPI_FLOAT, up, 0, down, 0, comm_2d, MPI_STATUS_IGNORE);
        c += a * b;
    }
    C.mat = (float*)calloc(sizeof(float), A.row * B.col);
    C.row = A.row;
    C.col = B.col;
    MPI_Gather(&c, 1, MPI_FLOAT, C.mat, 1, MPI_FLOAT, 0, comm_2d);

    if( id == 0)
    {
        print_matrix(C);
    }
    // TODO: Add saving matrix to csv
    MPI_Comm_free(&comm_2d); 
    free(C.mat);
    free(A.mat);
    free(B.mat);
    MPI_Finalize();
}

void initialize_matrix(FILE *file_pointer /*Pointer to file containing matrix*/,
                        matrix_data *mat /*Pointer to mat which have to be initialized */)
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

    mat->mat = (float *)malloc(sizeof(float) * row * col);

    int k=0;
    int i = 0;
    while((line = fgets(buffer,sizeof(buffer), file_pointer)) != NULL && i++ < row)
    {
        record = strtok(line, ",");
        int j = 0;
        while(record != NULL && j++ < col)
        {
            mat->mat[k] = atof(record);
            record = strtok(NULL,",");
            k++;
        }
    }
    mat->col = col;
    mat->row = row;
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