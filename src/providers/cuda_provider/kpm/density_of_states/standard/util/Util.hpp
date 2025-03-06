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

#ifndef QUANTUM_ABYSMAL_SRC_DENSITY_OF_STATES_GPU_UTIL_STANDARD_HPP
#define QUANTUM_ABYSMAL_SRC_DENSITY_OF_STATES_GPU_UTIL_STANDARD_HPP

#include <cstdint>
#include <curand_kernel.h>

// ------------------------------------------------------------------------------------------------

__host__ __device__ inline int mod(int a, int b)
{
   int r = a % b;
   return r < 0 ? r + b : r;
}

// ------------------------------------------------------------------------------------------------
// Kpm reductions helper functions.

template <uint32_t blockSize> __device__ void WarpReduce(volatile double* sdata, uint64_t tid)
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

// Number of block must be = 1.
template <unsigned int blockSize>
__global__ void FinalReduce(double* firstPartialReduction, double* secondPartialReduction,
                            double* moments, unsigned int momentIndex)
{
    __shared__ double sFirstData[blockSize];
    __shared__ double sSecondData[blockSize];

    sFirstData[threadIdx.x] = 0;
    sSecondData[threadIdx.x] = 0;

    uint64_t tid = threadIdx.x + blockIdx.x * blockDim.x;

    __syncthreads();

    while (tid < blockSize)
    {
        sFirstData[threadIdx.x] += firstPartialReduction[tid];
        sSecondData[threadIdx.x] += secondPartialReduction[tid];

        tid += blockDim.x * gridDim.x;
    }

    __syncthreads();

    if (blockSize >= 512)
    {
        if (threadIdx.x < 256)
        {
            sFirstData[threadIdx.x] += sFirstData[threadIdx.x + 256];
            sSecondData[threadIdx.x] += sSecondData[threadIdx.x + 256];
        }

        __syncthreads();
    }

    if (blockSize >= 256)
    {
        if (threadIdx.x < 128)
        {
            sFirstData[threadIdx.x] += sFirstData[threadIdx.x + 128];
            sSecondData[threadIdx.x] += sSecondData[threadIdx.x + 128];
        }

        __syncthreads();
    }

    if (blockSize >= 128)
    {
        if (threadIdx.x < 64)
        {
            sFirstData[threadIdx.x] += sFirstData[threadIdx.x + 64];
            sSecondData[threadIdx.x] += sSecondData[threadIdx.x + 64];
        }

        __syncthreads();
    }

    if (threadIdx.x < 32)
    {
        WarpReduce<blockSize>(sFirstData, threadIdx.x);
        WarpReduce<blockSize>(sSecondData, threadIdx.x);
    }

    if (threadIdx.x == 0 && momentIndex != 0)
    {
        moments[2 * momentIndex] = 2 * sFirstData[0] - moments[0];
        moments[2 * momentIndex + 1] = 2 * sSecondData[0] - moments[1];
    }

    if (threadIdx.x == 0 && momentIndex == 0)
    {
        moments[2 * momentIndex] = sFirstData[0];
        moments[2 * momentIndex + 1] = sSecondData[0];
    }
}

#endif