#include <stdio.h>
#include <algorithm>


using namespace std;

#define N 512
#define RADIUS 3
#define BLOCK_SIZE 32


__global__ void stencil_2d(int *in, int *out) {

	__shared__ int temp[BLOCK_SIZE + 2 * RADIUS][BLOCK_SIZE + 2 * RADIUS];
	int gindex_x = threadIdx.x + blockIdx.x*blockDim.x;
	int lindex_x = threadIdx.x + RADIUS;
	int gindex_y = threadIdx.y + blockIdx.y*blockDim.y;
	int lindex_y = threadIdx.y + RADIUS;

	// Read input elements into shared memory
	int size = N + 2 * RADIUS;
	temp[lindex_x][lindex_y] = in[gindex_x*size + gindex_y];

	if (threadIdx.x < RADIUS) {
		temp[lindex_x - RADIUS][lindex_y] = in[(gindex_x - RADIUS)*size + gindex_y];
		temp[lindex_x + BLOCK_SIZE][lindex_y] = in[(gindex_x + BLOCK_SIZE)*size + gindex_y];
	}

	if (threadIdx.y < RADIUS ) {
		temp[lindex_x][lindex_y - RADIUS] = in[gindex_x*size + gindex_y - RADIUS];
		temp[lindex_x][lindex_y + BLOCK_SIZE] = in[gindex_x*size + gindex_y + BLOCK_SIZE];
	}
	__syncthreads();

	// Apply the stencil
	int result = 0;
	for (int offset = -RADIUS; offset <= RADIUS; offset++){
		result += temp[lindex_x + offset][lindex_y];
		result += temp[lindex_x][lindex_y + offset];
	}
	result -= temp[lindex_x][lindex_y];

	// Store the result
	out[gindex_y+size*gindex_x] = result;
}


void fill_ints(int *x, int n) {
   // Store the result
   // https://en.cppreference.com/w/cpp/algorithm/fill_n
   fill_n(x, n, 1);
}


void check_stencil(int *out) {
	// Error Checking
	for (int i = 0; i < N + 2 * RADIUS; ++i) {
		for (int j = 0; j < N + 2 * RADIUS; ++j) {

			if (i < RADIUS || i >= N + RADIUS) {
				if (out[j+i*(N + 2 * RADIUS)] != 1) {
					printf("Mismatch in stencil at index [%d,%d], was: %d, should be: %d\n", i,j, out[j+i*(N + 2 * RADIUS)], 1);
					return;
				}
			}
			else if (j < RADIUS || j >= N + RADIUS) {
				if (out[j+i*(N + 2 * RADIUS)] != out[0]) {
					printf("Mismatch in stencil at index [%d,%d], was: %d, should be: %d\n", i,j, out[j+i*(N + 2 * RADIUS)], out[0]);
					return;
				}
			}		 
			else {
				if (out[j+i*(N + 2 * RADIUS)] != out[0]*(1 + 4 * RADIUS)) {
					printf("Mismatch in stencil at index [%d,%d], was: %d, should be: %d\n", i,j, out[j+i*(N + 2 * RADIUS)], out[0]*(1 + 4*RADIUS));
					return;
				}
			}
		}
	}
}

__global__ void matmul(int *A, int *B, int *out) {
	// Initialize shared memory for row of A, column of B
	__shared__ int tempA[BLOCK_SIZE][BLOCK_SIZE];
	__shared__ int tempB[BLOCK_SIZE][BLOCK_SIZE];

	// Store row and column into array
	int idx = threadIdx.x + blockIdx.x*BLOCK_SIZE;
	int idy = threadIdx.y + blockIdx.y*BLOCK_SIZE ;
	int nTiles = (N+2*RADIUS + BLOCK_SIZE-1) / BLOCK_SIZE;
	int sum = 0;
	for (int i = 0; i < nTiles; i++) {
		int a_col = i * BLOCK_SIZE + threadIdx.x;
		if ((idy < N + 2*RADIUS) && (a_col < N + 2*RADIUS)) {
			tempA[threadIdx.y][threadIdx.x] = A[(N + 2*RADIUS)*idy + a_col];
		} else {
			tempA[threadIdx.y][threadIdx.x] = 0;
		}

		int b_row = i * BLOCK_SIZE + threadIdx.y;
		if ((idx < N + 2*RADIUS) && (b_row < N + 2*RADIUS)) {
			tempB[threadIdx.y][threadIdx.x] = B[(N + 2*RADIUS)*b_row + idx];
		} else {
			tempB[threadIdx.y][threadIdx.x] = 0;
		}
		__syncthreads();

		for (int j = 0; j < BLOCK_SIZE; j++) {
			sum += tempA[threadIdx.y][j]*tempB[j][threadIdx.x];
		}
		__syncthreads();
	}

	if ((idy < N + 2*RADIUS) && (idx < N + 2*RADIUS)) {
		out[idy*(N+2*RADIUS) + idx] = sum;
	}
}

void check_matmul(int *A, int *B, int *out) {
    for (int i = 0; i < N; ++i) {
		for (int j = 0; j < N; ++j) {
            if ((i < RADIUS) || (i >= N + RADIUS)) {
                if ((j < RADIUS) || (j >= N + RADIUS)) {
                    if (out[(N + 2*RADIUS)*i + j] != A[0]*B[0]*(N + 2*RADIUS)) {
                        printf("Mismatch in matmul at index [%d,%d], was: %d, should be: %d\n", i,j, out[i*(N + 2*RADIUS) + j], A[0]*B[0]*(N + 2*RADIUS));
						return;
                    }
                } else {
                    int res = A[0]*B[(N + 2*RADIUS)*RADIUS + RADIUS]*N + 2*RADIUS*A[0]*B[0];
                    if (out[(N + 2*RADIUS)*i + j] != res) {
                        printf("Mismatch in matmul at index [%d,%d], was: %d, should be: %d\n", i,j, out[i*(N + 2*RADIUS) + j], res);
						return;
                    }
                }
            } else {
                if ((j < RADIUS) || (j >= N + RADIUS)) {
                    int res = A[0]*B[(N + 2*RADIUS)*RADIUS + RADIUS]*N + 2*RADIUS*A[0]*B[0];
                    if (out[(N + 2*RADIUS)*i + j] != res) {
                        printf("Mismatch in matmul at index [%d,%d], was: %d, should be: %d\n", i,j, out[(N + 2*RADIUS)*i + j], res);
						return;
                    }
                } else {
                    int res = A[(N + 2*RADIUS)*RADIUS + RADIUS]*B[(N + 2*RADIUS)*RADIUS + RADIUS]*N + 2*RADIUS*A[0]*B[0];
                    if (out[(N + 2*RADIUS)*i + j] != res) {
                        printf("Mismatch in matmul at index [%d,%d], was: %d, should be: %d\n", i,j, out[(N + 2*RADIUS)*i + j], res);
						return;
                    }
                }
            }
        }
    }
}

int main(void) {

	int *A, *B, *outA, *outB, *C; // host copies of A, B, C
	int *d_A, *d_B, *d_outA, *d_outB, *d_C; // device copies of A, B, C

	// Alloc space for host copies and setup values
	int size = (N + 2*RADIUS)*(N + 2*RADIUS) * sizeof(int);
	A = (int *)malloc(size); fill_ints(A, (N + 2*RADIUS)*(N + 2*RADIUS));
	outA = (int *)malloc(size); fill_ints(outA, (N + 2*RADIUS)*(N + 2*RADIUS));
	B = (int *)malloc(size); fill_ints(B, (N + 2*RADIUS)*(N + 2*RADIUS));
	outB = (int *)malloc(size); fill_ints(outB, (N + 2*RADIUS)*(N + 2*RADIUS));
	C = (int *)malloc(size); fill_ints(C, (N + 2*RADIUS)*(N + 2*RADIUS));


	// Alloc space for device copies
	cudaMalloc((void **)&d_A, size);
	cudaMalloc((void **)&d_outA, size);
	cudaMalloc((void **)&d_B, size);
	cudaMalloc((void **)&d_outB, size);
	cudaMalloc((void **)&d_C, size);

	// Launch stencil_2d() kernel on GPU
	int gridSize = (N + BLOCK_SIZE-1)/BLOCK_SIZE;
	dim3 gridS(gridSize, gridSize);
	dim3 blockS(BLOCK_SIZE, BLOCK_SIZE);

    // Create two kernels for stenciling the two matrices
    cudaStream_t stream1, stream2;
    cudaStreamCreate(&stream1);
    cudaStreamCreate(&stream2);

    // Copy to device to streams asynchronously
	cudaMemcpyAsync(d_A, A, size, cudaMemcpyHostToDevice, stream1);
	cudaMemcpyAsync(d_outA, outA, size, cudaMemcpyHostToDevice, stream1);
	cudaMemcpyAsync(d_B, B, size, cudaMemcpyHostToDevice, stream2);
	cudaMemcpyAsync(d_outB, outB, size, cudaMemcpyHostToDevice, stream2);

	// Launch the kernel 
	// Properly set memory address for first element on which the stencil will be applied
	stencil_2d<<<gridS,blockS,0,stream1>>>(d_A + RADIUS*(N + 2*RADIUS) + RADIUS , d_outA + RADIUS*(N + 2*RADIUS) + RADIUS);
	stencil_2d<<<gridS,blockS,0,stream2>>>(d_B + RADIUS*(N + 2*RADIUS) + RADIUS , d_outB + RADIUS*(N + 2*RADIUS) + RADIUS);

	// Copy result back to host
	cudaMemcpyAsync(A, d_A, size, cudaMemcpyDeviceToHost, stream1);
	cudaMemcpyAsync(outA, d_outA, size, cudaMemcpyDeviceToHost, stream1);
	cudaMemcpyAsync(B, d_B, size, cudaMemcpyDeviceToHost, stream2);
	cudaMemcpyAsync(outB, d_outB, size, cudaMemcpyDeviceToHost, stream2);

    cudaStreamSynchronize(stream1);
    cudaStreamSynchronize(stream2);

	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 10; j++) {
			printf("%d ", outA[(N+2*RADIUS)*i + j]);
		}
		printf("\n");
	}

	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 10; j++) {
			printf("%d ", outB[(N+2*RADIUS)*i + j]);
		}
		printf("\n");
	}

	// Error checking
	check_stencil(outA);
	check_stencil(outB);

	// Matrix multiplication
	int gridSizeM = (N + 2*RADIUS + (BLOCK_SIZE-1))/(BLOCK_SIZE);
	dim3 gridM(gridSizeM, gridSizeM);
	dim3 blockM(BLOCK_SIZE, BLOCK_SIZE);
	cudaMemcpy(d_C, C, size, cudaMemcpyHostToDevice);
	matmul<<<gridM, blockM>>>(d_outA, d_outB, d_C);
	cudaMemcpy(C, d_C, size, cudaMemcpyDeviceToHost);
	check_matmul(outA, outB, C);

	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 10; j++) {
			printf("%d ", C[(N+2*RADIUS)*i + j]);
		}
		printf("\n");
	}

	// Cleanup
	free(A);
	free(outA);
	cudaFree(d_A);
	cudaFree(d_outA);
	free(B);
	free(outB);
	cudaFree(d_B);
	cudaFree(d_outB);
	free(C);
	cudaFree(d_C);
	printf("Success!\n");

    cudaStreamDestroy(stream1);
    cudaStreamDestroy(stream2);

	return 0;
}


