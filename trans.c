/*************************************************************
*name: 马致远
*loginID:ics517021910070
...
*/
/* 
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */ 
#include <stdio.h>
#include "cachelab.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. 
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])//32 sets, one line per set, one block 32 bytes(8 int)
{
	if (M == 32 && N == 32) //(8*8)one block can hold one line
	{
		int i=0, j=0, k=0, a1=0, a2=0, a3=0, a4=0, a5=0, a6=0, a7=0, a8=0;
		for (i = 0; i < N; i += 8) 
		{
			for (j = 0; j < M; j += 8) 
			{
				for (k = i; k < i + 8; k++) //(8*8)one block can hold one line
				{
					a1 = A[k][j];
					a2 = A[k][j + 1];
					a3 = A[k][j + 2];
					a4 = A[k][j + 3];
					a5 = A[k][j + 4];
					a6 = A[k][j + 5];
					a7 = A[k][j + 6];
					a8 = A[k][j + 7];
					B[j][k] = a1;
					B[j + 1][k] = a2;
					B[j + 2][k] = a3;
					B[j + 3][k] = a4;
					B[j + 4][k] = a5;
					B[j + 5][k] = a6;
					B[j + 6][k] = a7;
					B[j + 7][k] = a8;
					
				}
			}
		}
    }
	if (M == 64 && N == 64) //(8*8)one block can hold one line
	{
		int i=0, j=0, k=0, l=0, a1=0, a2=0, a3=0, a4=0, a5=0, a6=0, a7=0, a8=0;
		for (i = 0; i < N; i += 8) 
		{
			for (j = 0; j < M; j += 8) 
			{
				for (k = i; k < i + 4; k++) //put the upper half of A 8*8 to the upper half of B 8*8
				{
					a1 = A[k][j];
					a2 = A[k][j + 1];
					a3 = A[k][j + 2];
					a4 = A[k][j + 3];
					a5 = A[k][j + 4];
					a6 = A[k][j + 5];
					a7 = A[k][j + 6];
					a8 = A[k][j + 7];

					B[j][k] = a1;
					B[j + 1][k] = a2;
					B[j + 2][k] = a3;
					B[j + 3][k] = a4;
					B[j][k + 4] = a5;//just make sense by putting the result to the top right corner 0f the 8*8 block temporaily
					B[j + 1][k + 4] = a6;
					B[j + 2][k + 4] = a7;
					B[j + 3][k + 4] = a8;
				}
				for (l = j; l < j + 4; l++) 
				{
					a5 = A[i + 4][l];//take one col(len=4) from the left down corner of A
					a6 = A[i + 5][l];
					a7 = A[i + 6][l];
					a8 = A[i + 7][l];

					a1 = B[l][i + 4];// take one row(len=4) form the top right corner of B  ( which we lend the space previously)
					a2 = B[l][i + 5];
					a3 = B[l][i + 6];
					a4 = B[l][i + 7];

					B[l][i + 4] = a5;// put the col of A to the row of B 
					B[l][i + 5] = a6;
					B[l][i + 6] = a7;
					B[l][i + 7] = a8;
					
					B[l+4][i] = a1;//take the row from the top right corner of B to the right place( bottom left corner of B)
					B[l+4][i + 1] = a2;
					B[l+4][i + 2] = a3;
					B[l+4][i + 3] = a4;
					
					// up until than , only the bottom right corner is unfinished
					B[l+4][i + 4] = A[i + 4][l+4];
					B[l+4][i + 5] = A[i + 5][l+4];
					B[l+4][i + 6] = A[i + 6][l+4];
					B[l+4][i + 7] = A[i + 7][l+4];
                }
			}
		}
	}
	if (M == 61 && N == 67)
    {
		int i=0, j=0, k=0, l=0, t=0;
		int size = 22;//just make some try about block size
		for (i = 0; i < N; i += size)
		{
			for (j = 0;  j < M; j += size) 
			{
				for (k = i; k < i + size && k < N; k++) 
				{
					for (l = j; l < j + size && l < M; l++) 
					{
						t = A[k][l];
						B[l][k] = t;
					}
				}
			}
		}   
    }
}

/* 
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started. 
 */ 

/* 
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }    

}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc); 

    /* Register any additional transpose functions */
    registerTransFunction(trans, trans_desc); 

}

/* 
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}

