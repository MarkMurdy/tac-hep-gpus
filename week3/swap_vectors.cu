#include <stdio.h>
#include <iostream>


const int DSIZE = 40960;
const int block_size = 256;
const int grid_size = DSIZE/block_size;


__global__ void vector_swap(float *A, float *B, float *C, int vsize) {

    //FIXME:
    // Express the vector index in terms of threads and blocks
    int idx = threadIdx.x + blockDim.x * blockIdx.x;
    // Swap the vector elements - make sure you are not out of range
    if (idx < vsize) {
        C[idx] = A[idx];
        A[idx] = B[idx];
        B[idx] = C[idx];
    }
}


int main() {


    float *h_A, *h_B, *h_C, *d_A, *d_B, *d_C;
    h_A = new float[DSIZE];
    h_B = new float[DSIZE];
    h_C = new float[DSIZE];


    for (int i = 0; i < DSIZE; i++) {
        h_A[i] = rand()/(float)RAND_MAX;
        h_B[i] = rand()/(float)RAND_MAX;
        h_C[i] = 0;
    }
    for (int i = 0; i < 10; i++) {
        std::cout << h_A[i] << " ";
    }
    std::cout << std::endl;
    for (int i = 0; i < 10; i++) {
        std::cout << h_B[i] << " ";
    }
    std::cout << std::endl;


    // Allocate memory for host and device pointers 
    int vector_size = DSIZE*sizeof(float);
    cudaMalloc(&d_A, vector_size);
    cudaMalloc(&d_B, vector_size);
    cudaMalloc(&d_C, vector_size);

    // Copy from host to device
    cudaMemcpy(d_A, h_A, vector_size, cudaMemcpyHostToDevice);
    cudaMemcpy(d_B, h_B, vector_size, cudaMemcpyHostToDevice);
    cudaMemcpy(d_C, h_C, vector_size, cudaMemcpyHostToDevice);

    // Launch the kernel
    vector_swap<<<grid_size, block_size>>>(d_A, d_B, d_C, DSIZE);

    // Copy back to host 
    cudaMemcpy(h_A, d_A, vector_size, cudaMemcpyDeviceToHost);
    cudaMemcpy(h_B, d_B, vector_size, cudaMemcpyDeviceToHost);
    cudaMemcpy(h_C, d_C, vector_size, cudaMemcpyDeviceToHost);

    // Print and check some elements to make sure swapping was successfull
    for (int i = 0; i < 10; i++) {
        std::cout << h_A[i] << " ";
    }
    std::cout << std::endl;
    for (int i = 0; i < 10; i++) {
        std::cout << h_B[i] << " ";
    }
    std::cout << std::endl;

    // Free the memory 
    free(h_A);
    free(h_B);
    free(h_C);
    cudaFree(d_A);
    cudaFree(d_B);
    cudaFree(d_C);

    return 0;
}
