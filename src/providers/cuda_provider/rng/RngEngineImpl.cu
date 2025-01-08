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

#include "RngEngineImpl.hpp"

RngGpuEngine& RngGpuEngine::GetInstance()
{
    static RngGpuEngine instance;
    return instance;
}

RngGpuEngine::RngGpuEngine()
{
    cudaMalloc((void**)&mDevStates, NUM_THREADS * NUM_BLOCKS * sizeof(curandStateXORWOW));
}

RngGpuEngine::~RngGpuEngine()
{
    cudaFree(mDevStates);
    mDevStates = nullptr;
}

// -------------------------------------------------------------------------------------------------
// Interface.

std::vector<double> RngGpuEngine::GetRandomVector(unsigned int size)
{
    LOG_INFO << "Entering GetRandomVector.";

    std::vector<double> numbers(size);
    
    double* gpuBuffer;
    cudaMalloc((void**)&gpuBuffer, size * sizeof(double));
    cudaMemset(gpuBuffer, 0, size * sizeof(double));

    GetRandomVector(gpuBuffer, size);

    cudaMemcpy(numbers.data(), gpuBuffer, size * sizeof(double), cudaMemcpyDeviceToHost);
    cudaFree(gpuBuffer);

    return numbers;
}

void RngGpuEngine::SetSeed(unsigned int seed)
{
    mSeed = seed;
}

// -------------------------------------------------------------------------------------------------
// Internal public functions.

void RngGpuEngine::GetRandomVector(double* gpuBuffer, unsigned int bufferSize)
{
    InitCurandXorwow<<<NUM_BLOCKS, NUM_THREADS>>>(mDevStates);
    InitRandomVector<<<NUM_BLOCKS, NUM_THREADS>>>(mDevStates, gpuBuffer, bufferSize);
}

// -------------------------------------------------------------------------------------------------
// CUDA functions.

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
