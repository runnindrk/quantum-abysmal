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
    LOG_INFO << "Entering RngGpuEngine::GetRandomVector";

    std::vector<double> numbers(size);
    
    double* gpuBuffer;
    cudaMalloc((void**)&gpuBuffer, size * sizeof(double));
    cudaMemset(gpuBuffer, 0, size * sizeof(double));

    LOG_INFO << "Computing random numbers ...";
    GetRandomVector(gpuBuffer, size);
    LOG_INFO << "Computing random numbers done!";

    cudaMemcpy(numbers.data(), gpuBuffer, size * sizeof(double), cudaMemcpyDeviceToHost);
    cudaFree(gpuBuffer);

    LOG_INFO << "Exiting RngGpuEngine::GetRandomVector";
    return numbers;
}

std::vector<unsigned int> RngGpuEngine::GetRandomBitsVector(unsigned int size)
{
    LOG_INFO << "Entering RngGpuEngine::GetRandomBitsVector";

    std::vector<unsigned int> numbers(size);
    
    unsigned int* gpuBuffer;
    cudaMalloc((void**)&gpuBuffer, size * sizeof(unsigned int));
    cudaMemset(gpuBuffer, 0, size * sizeof(unsigned int));

    LOG_INFO << "Computing random bits ...";
    GetRandomBitsVector(gpuBuffer, size);
    LOG_INFO << "Computing random bits done!";

    cudaMemcpy(numbers.data(), gpuBuffer, size * sizeof(unsigned int), cudaMemcpyDeviceToHost);
    cudaFree(gpuBuffer);

    LOG_INFO << "Exiting RngGpuEngine::GetRandomBitsVector";
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

void RngGpuEngine::GetRandomBitsVector(unsigned int* gpuBuffer, unsigned int bufferSize)
{
    InitCurandXorwow<<<NUM_BLOCKS, NUM_THREADS>>>(mDevStates);
    InitRandomBitsVector<<<NUM_BLOCKS, NUM_THREADS>>>(mDevStates, gpuBuffer, bufferSize);
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
        buffer[tid] = curand_uniform_double(&localState);

        state[localTid] = localState;
        tid += blockDim.x * gridDim.x;
    }
}

__global__ void InitRandomBitsVector(curandStateXORWOW* state, unsigned int* buffer, unsigned int bufferSize)
{
    int tid = threadIdx.x + blockIdx.x * blockDim.x;
    curandStateXORWOW localState = state[tid];
    int localTid = tid;

    while (tid < bufferSize)
    {
        buffer[tid] = curand(&localState);

        state[localTid] = localState;
        tid += blockDim.x * gridDim.x;
    }
}