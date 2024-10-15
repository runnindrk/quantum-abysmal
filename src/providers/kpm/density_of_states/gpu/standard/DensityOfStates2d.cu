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

    const unsigned int numBlocks = 256;
    const unsigned int numThreads = 256;
    unsigned int numRandomNumbers = 1 << 22;

    double* devFinalReduction;
    cudaMalloc((void**)&devFinalReduction, sizeof(double));

    double* devReduction;
    cudaMalloc((void**)&devReduction, numThreads * sizeof(double));

    double* devRandomNumbers;
    cudaMalloc((void**)&devRandomNumbers, numRandomNumbers * sizeof(double));

    curandStateXORWOW* devStates;
    cudaMalloc((void**)&devStates, numThreads * numBlocks * sizeof(curandStateXORWOW));

    InitCurandXorwow<<<numBlocks, numThreads>>>(devStates);
    InitRandomVector<<<numBlocks, numThreads>>>(devStates, devRandomNumbers, numRandomNumbers);
    // Reduce<numThreads><<<numBlocks, numThreads>>>(devRandomNumbers, devReduction, numRandomNumbers);
    // FinalReduce<numThreads><<<1, numThreads>>>(devReduction, devFinalReduction, numThreads);

    // double* randomNumbers = (double*)malloc(numRandomNumbers * sizeof(double));
    // cudaMemcpy(randomNumbers, devRandomNumbers, numRandomNumbers * sizeof(double), cudaMemcpyDeviceToHost);

    // for (int i = 0; i < numRandomNumbers; i++)
    // {
    //     LOG_INFO << randomNumbers[i];
    // }

    double* firstReduction = (double*)malloc(numThreads * sizeof(double));
    cudaMemcpy(firstReduction, devReduction, numThreads * sizeof(double), cudaMemcpyDeviceToHost);

    for (int i = 0; i < numThreads; i++)
    {
        LOG_INFO << firstReduction[i];
    }

    double reduction;
    cudaMemcpy(&reduction, devFinalReduction, sizeof(double), cudaMemcpyDeviceToHost);

    LOG_INFO << "Final reduction is : " << reduction;

    return {};
}

// -------------------------------------------------------------------------------------------------
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
        buffer[tid] = 1;

        state[localTid] = localState;
        tid += blockDim.x * gridDim.x;
    }
}
