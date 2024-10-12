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

#include "DensityOfStates2d.hpp"

#include <chrono>

Error DensityOfStates2dGpuStandard::SetNumberOfRandomVectors(size_t numVectors)
{
    // mNumRandomVectors = numVectors;
    return SUCCESS;
}

Error DensityOfStates2dGpuStandard::SetNumberOfMoments(size_t order)
{
    // mNumOfMoments = order;
    return SUCCESS;
}

std::vector<double> DensityOfStates2dGpuStandard::Compute()
{
    LOG_INFO << "Computing from GPU KPM 2D";

    unsigned int numBlocks = 256;
    unsigned int numThreads = 256;
    unsigned int numRandomNumbers = 1 << 20;

    double* devRandomNumbers;
    cudaMalloc((void**)&devRandomNumbers, numRandomNumbers * sizeof(double));

    curandStateXORWOW* devStates;
    cudaMalloc((void**)&devStates, numThreads * numBlocks * sizeof(curandStateXORWOW));

    InitCurandXorwow<<<numBlocks, numThreads>>>(devStates);
    InitRandomVector<<<numBlocks, numThreads>>>(devStates, devRandomNumbers, numRandomNumbers);

    double* randomNumbers = (double*)malloc(numRandomNumbers * sizeof(double));
    cudaMemcpy(randomNumbers, devRandomNumbers, numRandomNumbers * sizeof(double),
               cudaMemcpyDeviceToHost);

    for (int i = 0; i < numRandomNumbers; i++)
    {
        LOG_INFO << randomNumbers[i];
    }

    return {};
}

// ----------------------------------------------------------------------------
// Private

__global__ void InitCurandXorwow(curandStateXORWOW* state)
{
    int tid = threadIdx.x + blockIdx.x * blockDim.x;
    curand_init(clock64(), tid, 0, &state[tid]);
}

__global__ void InitRandomVector(curandStateXORWOW* state, double* buffer, unsigned int bufferSize)
{
    int tid = threadIdx.x + blockIdx.x * blockDim.x;
    curandStateXORWOW localState = state[tid];
    int localTid = tid;

    while (tid < bufferSize)
    {
        buffer[tid] = curand_normal_double(&localState);

        state[localTid] = localState;
        tid += blockDim.x * gridDim.x;
    }
}

// ----------------------------------------------------------------------------
// Optimized reduction routine

template <unsigned int blockSize> 
__device__ void WarpReduce(volatile int* sdata, unsigned int tid)
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

template <unsigned int blockSize> 
__global__ void Reduce(int* g_idata, int* g_odata, unsigned int n)
{
    extern __shared__ int sdata[];
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
        WarpReduce(sdata, tid);
        
    if (tid == 0)
        g_odata[blockIdx.x] = sdata[0];
}
