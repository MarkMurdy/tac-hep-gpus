#include <stdio.h>
#include <iostream>


const int DSIZE_X = 256;
const int DSIZE_Y = 256;

__global__ void add_matrix(const float *A, const float *B, float *C, int size_x, int size_y)
{
    //FIXME:
    // Express in terms of threads and blocks
    int idx = blockDim.x*blockIdx.x + threadIdx.x;
    int idy = blockDim.y*blockIdx.y + threadIdx.y;
    // Add the two matrices - make sure you are not out of range
    if (idx <  size_x && idy < size_y )
        C[idx*size_y + idy] = A[idx*size_y + idy] + B[idx*size_y + idy];

}

void print_matrix(const float *mat, int y_size, int x_max, int y_max)
{
    for (int i = 0; i < x_max; i++) {
        for (int j = 0; j < y_max; j++) {
            std::cout << mat[i*y_size + j] << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

int main()
{

    // Create and allocate memory for host and device pointers 
    float *h_A, *h_B, *h_C, *d_A, *d_B, *d_C;
    h_A = new float[DSIZE_X*DSIZE_Y];
    h_B = new float[DSIZE_X*DSIZE_Y];
    h_C = new float[DSIZE_X*DSIZE_Y];
    int matrix_size = sizeof(float)*DSIZE_X*DSIZE_Y;
    cudaMalloc(&d_A, matrix_size);
    cudaMalloc(&d_B, matrix_size);
    cudaMalloc(&d_C, matrix_size);

    // Fill in the matrices
    // FIXME
    for (int i = 0; i < DSIZE_X; i++) {
        for (int j = 0; j < DSIZE_Y; j++) {
            h_A[i*DSIZE_Y + j] = rand()/(float)RAND_MAX;
            h_B[i*DSIZE_Y + j] = rand()/(float)RAND_MAX;
            h_C[i*DSIZE_Y +j] = 0;
        }
    }

    // Copy from host to device
    cudaMemcpy(d_A, h_A, matrix_size, cudaMemcpyHostToDevice);
    cudaMemcpy(d_B, h_B, matrix_size, cudaMemcpyHostToDevice);
    cudaMemcpy(d_C, h_C, matrix_size, cudaMemcpyHostToDevice);

    // Launch the kernel
    // dim3 is a built in CUDA type that allows you to define the block 
    // size and grid size in more than 1 dimentions
    // Syntax : dim3(Nx,Ny,Nz)
    dim3 blockSize(16, 16);
    dim3 gridSize(16, 16);
    
    add_matrix<<<gridSize, blockSize>>>(d_A, d_B, d_C, DSIZE_X, DSIZE_Y);

    // Copy back to host 
    cudaMemcpy(h_A, d_A, matrix_size, cudaMemcpyDeviceToHost);
    cudaMemcpy(h_B, d_B, matrix_size, cudaMemcpyDeviceToHost);
    cudaMemcpy(h_C, d_C, matrix_size, cudaMemcpyDeviceToHost);

    // Print and check some elements to make the addition was succesfull
    print_matrix(h_A, DSIZE_Y, 2, 5);
    print_matrix(h_B, DSIZE_Y, 2, 5);
    print_matrix(h_C, DSIZE_Y, 2, 5);

    // Free the memory
    free(h_A);
    free(h_B);
    free(h_C);
    cudaFree(d_A);
    cudaFree(d_B);
    cudaFree(d_C);

    return 0;
}