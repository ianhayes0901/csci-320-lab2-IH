#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include "lab2.h"


int** read_board_from_file(char* filename){
    FILE *fp = NULL;
    sudoku_board = (int**)malloc(sizeof(int*)*ROW_SIZE);
    for(int row = 0; row < ROW_SIZE; row++){
	    sudoku_board[row] = (int*)malloc(sizeof(int)*COL_SIZE);
    }

    fp = fopen(filename,"r");
    for(int i = 0; i < ROW_SIZE; i++) {
        for(int j = 0; j < COL_SIZE; j++) {
            fscanf(fp, "%d%*c", &sudoku_board[i][j]);
        }
    }   

    for(int i = 0; i<ROW_SIZE; i++){
        for(int j = 0; j<COL_SIZE; j++)
        {
            printf("%d ", sudoku_board[i][j]);
        }
        printf("\n");
    }

    fclose(fp);
    return sudoku_board;
}

void *valid_rowcol(void* parameters)
{
    param_struct *prams = (param_struct*) parameters;
    int validation_array[9] = {0};
    int row = prams -> starting_row;
    int col = prams -> starting_col;
    int endrow = prams ->ending_row;
    int endcol = prams ->ending_col;

    if(endrow == row)
    {
        for(int i = 0; i< COL_SIZE; i++)
        {   
            int target = sudoku_board[i][col];
            if (target > 9 || target < 1 || validation_array[target - 1]==1)
            {
                printf("[Column] Problem at Row: %d Column: %d\n", i+1,col+1);
                pthread_exit(NULL);
            }
            else{
                validation_array[target - 1] = 1;
        }   
    } 

    worker_validation[18+col] = 1;
    }

    if(endcol == col)
    {
        for(int i = 0; i< ROW_SIZE; i++)
        {   
            int target = sudoku_board[row][i];
            if (target > 9 || target < 1 || validation_array[target - 1]==1)
            {
                printf("[Row] Problem at Row: %d Column: %d\n", row+1,i+1);
                pthread_exit(NULL);
            }
            else{
                validation_array[target - 1] = 1;
            }
        } 

        worker_validation[9+row] = 1;
    }
    pthread_exit(NULL);
}

void *valid_3x3(void* parameters)
{
    param_struct *prams = (param_struct*) parameters;
    int validation_array[9] = {0};
    int row = prams -> starting_row;
    int col = prams -> starting_col;

    for(int i = row; i < row + 3; i++){
        for(int j = col; j<col + 3; j++)
        {
            int target = sudoku_board[i][j];
            if (target > 9 || target < 1 || validation_array[target - 1]==1)
            {
                printf("[3x3] Problem at Row: %d Column: %d\n", i+1,j+1);
                pthread_exit(NULL);
            }
            else{
                validation_array[target-1] = 1;
            }
        }
    }
    worker_validation[row+col/3]=1;
    pthread_exit(NULL);

}



int is_board_valid(){
    pthread_t* tid;  /* the thread identifiers */
    pthread_attr_t attr;
    param_struct* parameter;
    int threadIndex = 0;
    worker_validation = (int*)malloc(sizeof(int)*NUM_OF_THREADS);
    tid = (pthread_t*)malloc(sizeof(int)*NUM_OF_THREADS);

    for(int i = 0; i < ROW_SIZE; i++)
    {
        for(int j = 0; j < COL_SIZE; j++)
        {
            if(i%3 == 0 && j%3 == 0){
                param_struct *worker3x3 = (param_struct*) malloc(sizeof(param_struct));
                worker3x3->starting_row = i;
                worker3x3->starting_col = j;
                pthread_create(&tid[threadIndex++], NULL, valid_3x3, worker3x3);
            }
            if(i==0){
                param_struct *workerColumn = (param_struct*) malloc(sizeof(param_struct));
                workerColumn->starting_row = i;
                workerColumn->starting_col = j;
                workerColumn->ending_row = i;
                workerColumn->ending_col = j+1;
                pthread_create(&tid[threadIndex++], NULL, valid_rowcol,workerColumn);
            }
            if(j==0){
                param_struct *workerRow = (param_struct*) malloc(sizeof(param_struct));
                workerRow->starting_row = i;
                workerRow->starting_col = j;
                workerRow->ending_col = j;
                workerRow->ending_row = i+1;
                pthread_create(&tid[threadIndex++], NULL, valid_rowcol, workerRow);
            }
            
        }
    }
    
    for(int i = 0; i< NUM_OF_THREADS; i++)
    {
        pthread_join(tid[i], NULL);
    }

    for (int i = 0; i<NUM_OF_THREADS; i++)
    {
        if(worker_validation[i] == 0){
            return 0;
        }

    }
    free(worker_validation);
    free(tid);
    return 1;
    
    // replace this comment with your code
    
}

