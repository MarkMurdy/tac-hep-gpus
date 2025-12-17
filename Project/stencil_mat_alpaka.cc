#include <iostream>
#include <vector>

#include <alpaka/alpaka.hpp>
#include "config.h"
#include "WorkDiv.hpp"

#define N 512
#define RADIUS 3
#define BLOCK_SIZE 32

struct stencil_2d {
    template <typename TAcc, typename T>
    ALPAKA_FN_ACC void operator()(
        TAcc const& acc, T const* __restrict__ in, T* __restrict__ out) const 
    {
        // Apply the stencil
        auto globalThreadId = alpaka::getIdx<alpaka::Grid, alpaka::Threads>(acc);
        auto globalThreadExtent = alpaka::getWorkDiv<alpaka::Grid, alpaka::Threads>(acc);
        auto threadElementId = alpaka::mapIdx<1u>(globalThreadId, globalThreadExtent);

        int length = N + 2*RADIUS;
        auto index = threadElementId[0];
        auto idx = index % length;
        auto idy = index / length;

        if ((idx >= RADIUS) && (idx < length - RADIUS) && (idy >= RADIUS) && (idy < length - RADIUS)) {
            int result = 0;
            for (int offset = -RADIUS; offset <= RADIUS; offset++){
                result += in[(idy + offset)*length + idx];
                result += in[idy*length + idx + offset];
            }
            result -= in[idy*length + idx];
            out[idy*length + idx] = result;
        }
    }
};

void check_stencil(alpaka::BufCpu<int, Dim1D, uint32_t> out) {
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

struct matmul {
    template <typename TAcc, typename T>
    ALPAKA_FN_ACC void operator()(
        TAcc const& acc, T const* __restrict__ A, T const* __restrict__ B, T* __restrict__ C) const
    {
        // Multiply matrices
        auto globalThreadId = alpaka::getIdx<alpaka::Grid, alpaka::Threads>(acc);
        auto globalThreadExtent = alpaka::getWorkDiv<alpaka::Grid, alpaka::Threads>(acc);
        auto threadElementId = alpaka::mapIdx<1u>(globalThreadId, globalThreadExtent);

        auto length = (N + 2*RADIUS);
        auto index = threadElementId[0];
        auto idx = index % length;
        auto idy = index / length;

        int sum = 0;
        for (int i = 0; i < length; i++) {
			sum += A[idy*length + i]*B[i*length + idx];
		}
        C[idy*length + idx] = sum;
    }
};

void check_matmul(alpaka::BufCpu<int, Dim1D, uint32_t> A, alpaka::BufCpu<int, Dim1D, uint32_t> B, alpaka::BufCpu<int, Dim1D, uint32_t> out) {
    uint32_t length = N + 2*RADIUS;
    for (int i = 0; i < length; ++i) {
		for (int j = 0; j < length; ++j) {
            if ((i < RADIUS) || (i >= N + RADIUS)) {
                if ((j < RADIUS) || (j >= N + RADIUS)) {
                    if (out[length*i + j] != A[0]*B[0]*length) {
                        printf("Mismatch in matmul at index [%d,%d], was: %d, should be: %d\n", i,j, out[i*length + j], A[0]*B[0]*length);
						return;
                    }
                } else {
                    int res = A[0]*B[length*RADIUS + RADIUS]*N + 2*RADIUS*A[0]*B[0];
                    if (out[length*i + j] != res) {
                        printf("Mismatch in matmul at index [%d,%d], was: %d, should be: %d\n", i,j, out[i*length + j], res);
						return;
                    }
                }
            } else {
                if ((j < RADIUS) || (j >= N + RADIUS)) {
                    int res = A[0]*B[length*RADIUS + RADIUS]*N + 2*RADIUS*A[0]*B[0];
                    if (out[length*i + j] != res) {
                        printf("Mismatch in matmul at index [%d,%d], was: %d, should be: %d\n", i,j, out[length*i + j], res);
						return;
                    }
                } else {
                    int res = A[length*RADIUS + RADIUS]*B[length*RADIUS + RADIUS]*N + 2*RADIUS*A[0]*B[0];
                    if (out[length*i + j] != res) {
                        printf("Mismatch in matmul at index [%d,%d], was: %d, should be: %d\n", i,j, out[length*i + j], res);
						return;
                    }
                }
            }
        }
    }
}

int main() {
    // get host
    HostPlatform host_platform;
    Host host = alpaka::getDevByIdx(host_platform, 0u);
    std::cout << "Using " << alpaka::getName(host) << '\n';

    // get GPU device
    Platform platform;
    Device device = alpaka::getDevByIdx(platform, 0u);
    std::cout << "Using " << alpaka::getName(device) << '\n';

    // host copies of A, B, C
    constexpr Vec1D size = {(N + 2*RADIUS) * (N + 2*RADIUS)};
    auto A_h = alpaka::allocMappedBuf<int, uint32_t>(host, platform, size);
    auto outA_h = alpaka::allocMappedBuf<int, uint32_t>(host, platform, size);
    auto B_h = alpaka::allocMappedBuf<int, uint32_t>(host, platform, size);
    auto outB_h = alpaka::allocMappedBuf<int, uint32_t>(host, platform, size);
    auto C_h = alpaka::allocMappedBuf<int, uint32_t>(host, platform, size);

    // fill input buffers with data
    uint32_t fsize = size.prod();
    for (uint32_t i = 0; i < fsize; i++) {
        A_h[i] = 1;
        B_h[i] = 1;
        outA_h[i] = 1;
        outB_h[i] = 1;
        C_h[i] = 1;
    }

    // start asynchronous queue
    auto queue = Queue{device};

    // allocate device memory
    auto A_d = alpaka::allocAsyncBuf<int, uint32_t>(queue, size);
    auto outA_d = alpaka::allocAsyncBuf<int, uint32_t>(queue, size);
    auto B_d = alpaka::allocAsyncBuf<int, uint32_t>(queue, size);
    auto outB_d = alpaka::allocAsyncBuf<int, uint32_t>(queue, size);
    auto C_d = alpaka::allocAsyncBuf<int, uint32_t>(queue, size);

    // copy host values into device
    alpaka::memcpy(queue, A_d, A_h);
    alpaka::memcpy(queue, outA_d, outA_h);
    alpaka::memcpy(queue, B_d, B_h);
    alpaka::memcpy(queue, outB_d, outB_h);
    alpaka::memcpy(queue, C_d, C_h);

    // set work division
    uint32_t length = (N + 2*RADIUS);
	int grid_size = (length*length + BLOCK_SIZE - 1) / BLOCK_SIZE;
    auto div = makeWorkDiv<Acc1D>(grid_size, BLOCK_SIZE);
    std::cout << "Running stencil_2d kernel with scalar indices with a grid of ";
    std::cout << alpaka::getWorkDiv<alpaka::Grid, alpaka::Blocks>(div);
    std::cout << " blocks x " << alpaka::getWorkDiv<alpaka::Block, alpaka::Threads>(div) << " threads x " << alpaka::getWorkDiv<alpaka::Thread, alpaka::Elems>(div);
    std::cout << " elements" << std::endl;
    alpaka::exec<Acc1D>(queue, div, stencil_2d{}, A_d.data(), outA_d.data());
    alpaka::exec<Acc1D>(queue, div, stencil_2d{}, B_d.data(), outB_d.data());

    // copy device memory back to host
    alpaka::memcpy(queue, outA_h, outA_d);
    alpaka::memcpy(queue, outB_h, outB_d);

    // wait for stencil to complete
    alpaka::wait(queue);

    // check stencil
    check_stencil(outA_h);
	check_stencil(outB_h);

    // perform matrix multiplication
    std::cout << "Running matmul kernel with scalar indices with a grid of ";
    std::cout << alpaka::getWorkDiv<alpaka::Grid, alpaka::Blocks>(div);
    std::cout << " blocks x " << alpaka::getWorkDiv<alpaka::Block, alpaka::Threads>(div) << " threads x " << alpaka::getWorkDiv<alpaka::Thread, alpaka::Elems>(div);
    std::cout << " elements" << std::endl;
    alpaka::exec<Acc1D>(queue, div, matmul{}, outA_d.data(), outB_d.data(), C_d.data());

    // copy output matrix to host
    alpaka::memcpy(queue, C_h, C_d);

    alpaka::wait(queue);

    check_matmul(outA_h, outB_h, C_h);

    for (uint32_t i = 0; i < 10; i++) {
        for (uint32_t j = 0; j < 10; j++) {
            printf("%d ", outA_h[i*length + j]);
        }
        printf("\n");
    }

    for (uint32_t i = 0; i < 10; i++) {
        for (uint32_t j = 0; j < 10; j++) {
            printf("%d ", outB_h[i*length + j]);
        }
        printf("\n");
    }

    for (uint32_t i = 0; i < 10; i++) {
        for (uint32_t j = 0; j < 10; j++) {
            printf("%d ", C_h[i*length + j]);
        }
        printf("\n");
    }

}