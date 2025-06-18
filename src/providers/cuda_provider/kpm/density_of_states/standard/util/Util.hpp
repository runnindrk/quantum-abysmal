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

#include "src/lattice/LatticeImpl.hpp"
#include "src/math/Math.hpp"

#include <cstdint>
#include <curand_kernel.h>

// ================================================================================================
// General helper functions.

#define NUM_BLOCKS (256)
#define NUM_THREADS (256)

#define ARRAY_IDX(x, y, hx, hy, o) (numOrbitals * (Math::Mod(x + hx, xSize) + Math::Mod(y + hy, ySize) * xSize) + o)

// ================================================================================================
// CUDA KPM reductions helper functions.

// ------------------------------------------------------------------------------------------------
// Warp helper function for maximum thread usage during reductions.

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

// ------------------------------------------------------------------------------------------------
// Reduce KPM function for sparse-matrix operation and reduction of the moments.

template <uint32_t blockSize, typename Op>
__global__ void Reduce(double* a, double* b, LatticeStructure& lattice, double* firstReduction, double* secondReduction, Op opFunc)
{
    __shared__ double sFirstReduceData[blockSize];
    __shared__ double sSecondReduceData[blockSize];

    sFirstReduceData[threadIdx.x] = 0;
    sSecondReduceData[threadIdx.x] = 0;

    uint64_t tid = threadIdx.x + blockIdx.x * blockDim.x;

    __syncthreads();

    while (tid < lattice.numberOfSites)
    {
        opFunc(tid, a, b, lattice, sFirstReduceData, sSecondReduceData);
        tid += blockDim.x * gridDim.x;
    }

    __syncthreads();

    if (blockSize >= 512)
    {
        if (threadIdx.x < 256)
        {
            sFirstReduceData[threadIdx.x] += sFirstReduceData[threadIdx.x + 256];
            sSecondReduceData[threadIdx.x] += sSecondReduceData[threadIdx.x + 256];
        }
        __syncthreads();
    }

    if (blockSize >= 256)
    {
        if (threadIdx.x < 128)
        {
            sFirstReduceData[threadIdx.x] += sFirstReduceData[threadIdx.x + 128];
            sSecondReduceData[threadIdx.x] += sSecondReduceData[threadIdx.x + 128];
        }
        __syncthreads();
    }

    if (blockSize >= 128)
    {
        if (threadIdx.x < 64)
        {
            sFirstReduceData[threadIdx.x] += sFirstReduceData[threadIdx.x + 64];
            sSecondReduceData[threadIdx.x] += sSecondReduceData[threadIdx.x + 64];
        }
        __syncthreads();
    }

    if (threadIdx.x < 32)
    {
        WarpReduce<blockSize>(sFirstReduceData, threadIdx.x);
        WarpReduce<blockSize>(sSecondReduceData, threadIdx.x);
    }

    if (threadIdx.x == 0)
    {
        firstReduction[blockIdx.x] = sFirstReduceData[0] / lattice.hamiltonianSize;
        secondReduction[blockIdx.x] = sSecondReduceData[0] / lattice.hamiltonianSize;
    }
}

// ------------------------------------------------------------------------------------------------
// Reduce KPM function to collapse the blockSize partial sums of the moments into a single number.
// Must be called like Reduce<NUM_THREADS><<<1, NUM_THREADS>>>(...) (NUM_BLOCKS = 1).

template <unsigned int blockSize>
__global__ void Reduce(double* firstPartialReduction, double* secondPartialReduction, double* moments, unsigned int momentIndex)
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

    if (threadIdx.x == 0 && momentIndex == 0)
    {
        moments[2 * momentIndex] = sFirstData[0];
        moments[2 * momentIndex + 1] = sSecondData[0];
    }

    if (threadIdx.x == 0 && momentIndex != 0)
    {
        moments[2 * momentIndex] = 2 * sFirstData[0] - moments[0];
        moments[2 * momentIndex + 1] = 2 * sSecondData[0] - moments[1];
    }
}

// ------------------------------------------------------------------------------------------------
// Kernel for DoS computation.

template <typename T>
__global__ void DensityOfStatesFromMoments(LatticeStructure& lattice, T* mom, uint32_t momSize, std::array<double, 2>* dosPointerArray, uint32_t dosArraySize)
{
    uint64_t tid = threadIdx.x + blockIdx.x * blockDim.x;

    while (tid < dosArraySize)
    {
        double singleEnergy = -0.999 + tid * (2 * 0.999) / (dosArraySize - 1);

        {
            dosPointerArray[tid][1] += 1 * mom[0] * Math::JacksonKernel(0, momSize) * Math::ChebyshevPolynomial(0, singleEnergy);
        }

        for (int j = 1; j < momSize; j++)
        {
            dosPointerArray[tid][1] += 2 * mom[j] * Math::JacksonKernel(j, momSize) * Math::ChebyshevPolynomial(j, singleEnergy);
        }

        dosPointerArray[tid][0] = singleEnergy;
        dosPointerArray[tid][1] *= 1 / (M_PI * (sqrt(1 - singleEnergy * singleEnergy)));

        // Rescaling
        dosPointerArray[tid][0] *= lattice.energyScaling;
        dosPointerArray[tid][0] += lattice.energyShift;

        dosPointerArray[tid][1] /= lattice.energyScaling;
        dosPointerArray[tid][1] += lattice.energyShift;

        tid += blockDim.x * gridDim.x;
    }
}

#endif