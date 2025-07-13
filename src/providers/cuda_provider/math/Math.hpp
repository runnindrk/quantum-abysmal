// ================================================================================================
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
// ================================================================================================

#ifndef QUANTUM_ABYSMAL_SRC_MATH_GPU_PROVIDER_HPP
#define QUANTUM_ABYSMAL_SRC_MATH_GPU_PROVIDER_HPP

#include "public/ErrorHandler.hpp"
#include "public/QuantumAbysmalTypes.hpp"

#include <curand_kernel.h>

namespace Math 
{

__device__ inline double ChebyshevPolynomial(int n, double x)
{
    return cos(n * acos(x));
}

__device__ inline double JacksonKernel(int n, int N)
{
    double factor1 = (N - n + 1) * cos(M_PI * n / (N + 1));
    double factor2 = sin(M_PI * n / (N + 1)) * cos(M_PI / (N + 1)) / sin(M_PI / (N + 1));
    return (factor1 + factor2) / (N + 1);
}

__device__ inline int Mod(int a, int b)
{
    int r = a % b;
    return r < 0 ? r + b : r;
}

__global__ inline void SatisticsAccumulator(double* newDataArray, 
                                            double* averageDataArray, 
                                            double* varianceDataArray, 
                                            uint64_t size, 
                                            uint64_t numSamples)
{
    uint64_t tid = threadIdx.x + blockIdx.x * blockDim.x;

    while (tid < size)
    {
        // Save the old average to compute the new variance.
        double oldAverage = averageDataArray[tid];
        
        averageDataArray[tid] += (newDataArray[tid] - oldAverage) / (numSamples + 1.0);
        varianceDataArray[tid] += ((newDataArray[tid] - oldAverage) * (newDataArray[tid] - averageDataArray[tid]) - varianceDataArray[tid]) / (numSamples + 1.0);
        
        tid += blockDim.x * gridDim.x;
    }
}

} // namespace Math

#endif