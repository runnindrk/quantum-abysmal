//============================================================================
// Copyright (c) 2024, runnindrk
//
// This file is part of Quantum Abysmal.
//
// Quantum Abysmal is free software: you can redistribute it and/or modify it
// as needed, with the intent of making it freely available to everyone.
//
// This project is in its early stages and is provided without any warranties,
// expressed or implied, including but not limited to the warranties of
// merchantability, fitness for a particular purpose, or non-infringement.
//
// Use it at your own risk, and feel free to contribute as the project evolves!
//============================================================================

#ifndef QUANTUM_ABYSMAL_SRC_DENSITY_OF_STATES_2D_GPU_STANDARD_HPP
#define QUANTUM_ABYSMAL_SRC_DENSITY_OF_STATES_2D_GPU_STANDARD_HPP

#include "include/internal/Logger.hpp"
#include "include/public/DensityOfStates.hpp"
#include "src/lattice/LatticeImpl.hpp"

#include <curand_kernel.h>

class DensityOfStates2dGpuStandard : public DensityOfStates
{
  public:
    Error SetNumberOfRandomVectors(size_t numVectors) override;
    Error SetNumberOfMoments(size_t order) override;
    std::vector<double> Compute() override;

  private:
    // ------------------------------------------------------------------------
    // Private methods.

    // ------------------------------------------------------------------------
    // Member variables.
};

// ----------------------------------------------------------------------------
// CUDA Routines

// Random number will be substituted for an RNG ENGINE in the future.
// This is just to kickstart the development.
__global__ void InitCurandXorwow(curandStateXORWOW* state);

__global__ void InitRandomVector(curandStateXORWOW* state, double* buffer, unsigned int bufferSize);

// ----------------------------------------------------------------------------
// Optimized reduction routine

// template <unsigned int numberOfHoppings>
// struct CudaLattice
// {
//     1;

// };

// __forceinline__ __device__ void KpmSparseMatrixOperation()
// {
//     int a = 1;
//     a += 1;
// }

template <uint32_t blockSize> __device__ void WarpReduce(volatile double* sdata, unsigned int tid)
{
    if (blockSize >= 64)
        sdata[tid] += sdata[tid + 32];
    if (blockSize >= 32)
        sdata[tid] += sdata[tid + 16];
    if (blockSize >= 16)
        sdata[tid] += sdata[tid + 8];
    if (blockSize >= 8)
        sdata[tid] += sdata[tid + 4];
    if (blockSize >= 4)
        sdata[tid] += sdata[tid + 2];
    if (blockSize >= 2)
        sdata[tid] += sdata[tid + 1];
}

template <uint32_t blockSize>
__global__ void Reduce(double* a, double* b, DeviceLattice& lattice,
                       double* firstReduction, double* secondReduction)
{
    __shared__ DeviceLattice sLattice;
    __shared__ double sFirstReduceData[blockSize];
    __shared__ double sSecondReduceData[blockSize];

    sLattice = lattice;
    sFirstReduceData[threadIdx.x] = 0;
    sSecondReduceData[threadIdx.x] = 0;

    unsigned int tid = threadIdx.x + blockIdx.x * blockDim.x;

    while (tid < sLattice.numberOfSites)
    {
        sFirstReduceData[threadIdx.x] += a[tid];
        tid += blockDim.x * gridDim.x;
    }

    __syncthreads();

    if (blockSize >= 512)
    {
        if (threadIdx.x < 256)
        {
            sFirstReduceData[threadIdx.x] += sFirstReduceData[threadIdx.x + 256];
        }

        __syncthreads();
    }

    if (blockSize >= 256)
    {
        if (threadIdx.x < 128)
        {
            sFirstReduceData[threadIdx.x] += sFirstReduceData[threadIdx.x + 128];
        }

        __syncthreads();
    }

    if (blockSize >= 128)
    {
        if (threadIdx.x < 64)
        {
            sFirstReduceData[threadIdx.x] += sFirstReduceData[threadIdx.x + 64];
        }

        __syncthreads();
    }

    if (threadIdx.x < 32)
        WarpReduce<blockSize>(sFirstReduceData, threadIdx.x);

    if (threadIdx.x == 0)
        firstReduction[blockIdx.x] = sFirstReduceData[0];
}

template <unsigned int blockSize>
__global__ void FinalReduce(double* g_idata, double* g_odata, unsigned int n)
{
    __shared__ double sdata[blockSize];
    unsigned int tid = threadIdx.x;
    unsigned int i = tid + blockIdx.x * (blockSize * 2);
    unsigned int gridSize = blockSize * 2 * gridDim.x;
    sdata[tid] = 0;

    while (i < n)
    {
        sdata[tid] += g_idata[i] + g_idata[i + blockSize];
        i += gridSize;
    }

    __syncthreads();

    if (blockSize >= 512)
    {
        if (tid < 256)
        {
            sdata[tid] += sdata[tid + 256];
        }

        __syncthreads();
    }

    if (blockSize >= 256)
    {
        if (tid < 128)
        {
            sdata[tid] += sdata[tid + 128];
        }

        __syncthreads();
    }

    if (blockSize >= 128)
    {
        if (tid < 64)
        {
            sdata[tid] += sdata[tid + 64];
        }

        __syncthreads();
    }

    if (tid < 32)
        WarpReduce<blockSize>(sdata, tid);

    if (tid == 0)
        g_odata[blockIdx.x] = sdata[0];
}

#endif