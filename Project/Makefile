CUDA_BASE   ?= /usr/local/cuda
BOOST_BASE  = /afs/hep.wisc.edu/home/abocci/public/boost
ALPAKA_BASE ?= $(HOME)/public/alpaka
#MDSPAN_BASE ?= $(HOME)/public/mdspan


CXX  := g++
NVCC := $(CUDA_BASE)/bin/nvcc

CXXFLAGS       := -std=c++20 -O2 -g -I$(MDSPAN_BASE)/include -I$(ALPAKA_BASE)/include# -DALPAKA_HAS_STD_ATOMIC_REF -DALPAKA_USE_MDSPAN
CXX_HOST_FLAGS := -pthread
CUDA_ARCH      := sm_75
CXX_CUDA_FLAGS := -arch=$(CUDA_ARCH) -Wno-deprecated-gpu-targets -Wno-deprecated-declarations --extended-lambda --expt-relaxed-constexpr


all:
	g++ -std=c++20 stencil_mat.cc -o stencil_mat_cpu
	nvcc stencil_mat.cu -o stencil_mat_cuda
	nvcc stencil_mat_optimized.cu -o stencil_mat_optimized_cuda
	$(NVCC) -x cu -ccbin $(CXX) $(CXXFLAGS) $(CXX_CUDA_FLAGS) -Xcompiler '$(CXX_HOST_FLAGS)' stencil_mat_alpaka.cc -DALPAKA_ACC_GPU_CUDA_ENABLED -o stencil_mat_alpaka
