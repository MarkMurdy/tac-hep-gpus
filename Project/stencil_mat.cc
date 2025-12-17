#include <cstdio>

#define N_SIZE 512
#define RADIUS 3

void fill_arr(int arr[N_SIZE][N_SIZE], int val)
{
    for (int i = 0; i < N_SIZE; i++) {
        for (int j = 0; j < N_SIZE; j++) {
	        arr[i][j] = val;
	    }
    }
}

void stencil_2d(int A[N_SIZE][N_SIZE], int out[N_SIZE][N_SIZE])
{
    // fill the stencil
    for (int i = 0; i < N_SIZE - 2*RADIUS; i++) {
        for (int j = 0; j < N_SIZE - 2*RADIUS; j++) {
            int sten = 0;
            for (int k = -RADIUS; k < RADIUS + 1; k++) {
                sten += A[i + RADIUS - k][j + RADIUS];
                sten += A[i + RADIUS][j + RADIUS - k];
            }
            sten -= A[i + RADIUS][j + RADIUS];
	        out[i + RADIUS][j + RADIUS] = sten;
	    }
    }
}

void check_stencil(int out[N_SIZE][N_SIZE])
{
    for (int i = 0; i < N_SIZE; ++i) {
		for (int j = 0; j < N_SIZE; ++j) {

			if (i < RADIUS || i >= N_SIZE - RADIUS) {
				if (out[i][j] != out[0][0]) {
					printf("Mismatch at index [%d,%d], was: %d, should be: %d\n", i,j, out[i][j], out[0][0]);
					return;
				}
			}
			else if (j < RADIUS || j >= N_SIZE - RADIUS) {
				if (out[i][j] != out[0][0]) {
					printf("Mismatch at index [%d,%d], was: %d, should be: %d\n", i,j, out[i][j], out[0][0]);
					return;
				}
			}		 
			else {
				if (out[i][j] != out[0][0]*(1 + 4 * RADIUS)) {
					printf("Mismatch at index [%d,%d], was: %d, should be: %d\n", i,j, out[i][j], out[0][0]*(1 + 4 * RADIUS));
					return;
				}
			}
		}
	}
}

void matmul(int A[N_SIZE][N_SIZE], int B[N_SIZE][N_SIZE], int out[N_SIZE][N_SIZE])
{
    for (int i = 0; i < N_SIZE; ++i) {
		for (int j = 0; j < N_SIZE; ++j) {
            int element = 0;
            for (int k = 0; k < N_SIZE; ++k) {
                element += A[i][k]*B[k][j];
            }
            out[i][j] = element;
        }
    }
}

void check_matmul(int A[N_SIZE][N_SIZE], int B[N_SIZE][N_SIZE], int out[N_SIZE][N_SIZE])
{
    for (int i = 0; i < N_SIZE; ++i) {
		for (int j = 0; j < N_SIZE; ++j) {
            if ((i < RADIUS) | (i >= N_SIZE - RADIUS)) {
                if ((j < RADIUS) | (j >= N_SIZE - RADIUS)) {
                    if (out[i][j] != A[0][0]*B[0][0]*N_SIZE) {
                        printf("Mismatch at index [%d,%d], was: %d, should be: %d\n", i,j, out[i][j], A[0][0]*B[0][0]*N_SIZE);
                    }
                } else {
                    int res = A[0][0]*B[RADIUS][RADIUS]*(N_SIZE-2*RADIUS) + 2*RADIUS*A[0][0]*B[0][0];
                    if (out[i][j] != res) {
                        printf("Mismatch at index [%d,%d], was: %d, should be: %d\n", i,j, out[i][j], res);
                    }
                }
            } else {
                if ((j < RADIUS) | (j >= N_SIZE - RADIUS)) {
                    int res = A[0][0]*B[RADIUS][RADIUS]*(N_SIZE-2*RADIUS) + 2*RADIUS*A[0][0]*B[0][0];
                    if (out[i][j] != res) {
                        printf("Mismatch at index [%d,%d], was: %d, should be: %d\n", i,j, out[i][j], res);
                    }
                } else {
                    int res = A[RADIUS][RADIUS]*B[RADIUS][RADIUS]*(N_SIZE-2*RADIUS) + 2*RADIUS*A[0][0]*B[0][0];
                    if (out[i][j] != res) {
                        printf("Mismatch at index [%d,%d], was: %d, should be: %d\n", i,j, out[i][j], res);
                    }
                }
            }
        }
    }
}

int main()
{
    // make arrays
    int A[N_SIZE][N_SIZE];
    int B[N_SIZE][N_SIZE];
    int stencil_A[N_SIZE][N_SIZE];
    int stencil_B[N_SIZE][N_SIZE];
    fill_arr(A, 1);
    fill_arr(stencil_A, 1);
    fill_arr(B, 2);
    fill_arr(stencil_B, 2);
    int prod[N_SIZE][N_SIZE];

    // check output
    for (int i = 0; i < RADIUS + 2; i++) {
        for (int j = 0; j < RADIUS + 2; j++) {
	        printf("%d ", A[i][j]);
	    }
        printf("\n");
    }
    
    // make stencil output
    stencil_2d(A, stencil_A);
    stencil_2d(B, stencil_B);

    // check output
    printf("\n");
    for (int i = 0; i < RADIUS + 2; i++) {
        for (int j = 0; j < RADIUS + 2; j++) {
	        printf("%d ", stencil_A[i][j]);
	    }
        printf("\n");
    }

    check_stencil(stencil_A);
    check_stencil(stencil_B);

    // do matrix multiplication
    matmul(stencil_A, stencil_B, prod);
    check_matmul(stencil_A, stencil_B, prod);
    printf("\n");
    for (int i = 0; i < RADIUS + 2; i++) {
        for (int j = 0; j < RADIUS + 2; j++) {
	        printf("%d ", stencil_A[i][j]);
	    }
        printf("\n");
    }

    printf("\n");
    for (int i = 0; i < RADIUS + 2; i++) {
        for (int j = 0; j < RADIUS + 2; j++) {
	        printf("%d ", stencil_B[i][j]);
	    }
        printf("\n");
    }

    printf("\n");
    for (int i = 0; i < RADIUS + 2; i++) {
        for (int j = 0; j < RADIUS + 2; j++) {
	        printf("%d ", prod[i][j]);
	    }
        printf("\n");
    }
}
