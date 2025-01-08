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

    size_t mNumRandomVectors{};
    size_t mNumOfMoments{};

    LatticeStructure mLattice = LatticeImpl::GetInstance().GetLattice();
};

// ----------------------------------------------------------------------------
// CUDA Routines

__host__ __device__ inline int mod(int a, int b)
{
    int r = a % b;
    return r < 0 ? r + b : r;
}

__forceinline__ __device__ void KpmSparseMatrixInitializer(uint64_t tid, double* a, double* b,
                                                           LatticeStructure& lattice,
                                                           double* sFirstReduceData,
                                                           double* sSecondReduceData)
{
    Hopping currentHop;
    int numOrbitals = lattice.numberOfOrbitals;
    int xSize = lattice.latticeSize[0];
    int ySize = lattice.latticeSize[1];
    int y = tid / xSize;
    int x = tid % xSize;

    // clang-format off
    #pragma unroll 4
    // clang-format on
    for (int j = 0; j < lattice.numberOfHoppings; j++)
    {
        currentHop = lattice.hoppings[j];

        int64_t newX = mod(x + currentHop.latticeHop[0], xSize);
        int64_t newY = mod(y + currentHop.latticeHop[1], ySize);
        uint64_t bIndex = numOrbitals * tid + currentHop.orbitalHop[0];
        uint64_t aIndex = numOrbitals * (newX + newY * xSize) + currentHop.orbitalHop[1];

        b[bIndex] += currentHop.hoppingStrength * a[aIndex];
    }

    // clang-format off
    #pragma unroll 2
    // clang-format on
    for (int j = 0; j < numOrbitals; j++)
    {
        uint64_t trueIndex = numOrbitals * (x + y * xSize) + j;

        sFirstReduceData[threadIdx.x] += a[trueIndex] * a[trueIndex];
        sSecondReduceData[threadIdx.x] += b[trueIndex] * a[trueIndex];
    }
}

__forceinline__ __device__ void KpmSparseMatrixOperation(uint64_t tid, double* a, double* b,
                                                         LatticeStructure& lattice,
                                                         double* sFirstReduceData,
                                                         double* sSecondReduceData)
{
    Hopping currentHop;
    int numOrbitals = lattice.numberOfOrbitals;
    int xSize = lattice.latticeSize[0];
    int ySize = lattice.latticeSize[1];
    int y = tid / xSize;
    int x = tid % xSize;
    double temp[4] = {};

    // clang-format off
    #pragma unroll 4
    // clang-format on
    for (int j = 0; j < lattice.numberOfHoppings; j++)
    {
        currentHop = lattice.hoppings[j];

        int64_t newX = mod(x + currentHop.latticeHop[0], xSize);
        int64_t newY = mod(y + currentHop.latticeHop[1], ySize);
        int64_t newPos = newX + newY * xSize;
        int64_t newIndex = numOrbitals * newPos + currentHop.orbitalHop[1];

        temp[currentHop.orbitalHop[0]] += currentHop.hoppingStrength * b[newIndex];
    }

    // clang-format off
    #pragma unroll 2
    // clang-format on
    for (int j = 0; j < numOrbitals; j++)
    {
        uint64_t trueIndex = numOrbitals * (x + y * xSize) + j;

        a[trueIndex] = 2 * temp[j] - a[trueIndex];

        sFirstReduceData[threadIdx.x] += b[trueIndex] * b[trueIndex];
        sSecondReduceData[threadIdx.x] += a[trueIndex] * b[trueIndex];

        temp[j] = 0;
    }
}

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

template <uint32_t blockSize, bool initializer>
__global__ void Reduce(double* a, double* b, LatticeStructure& lattice, double* firstReduction,
                       double* secondReduction)
{
    __shared__ LatticeStructure sLattice;
    __shared__ double sFirstReduceData[blockSize];
    __shared__ double sSecondReduceData[blockSize];

    sLattice = lattice;
    sFirstReduceData[threadIdx.x] = 0;
    sSecondReduceData[threadIdx.x] = 0;
    
    uint64_t tid = threadIdx.x + blockIdx.x * blockDim.x;

    __syncthreads();

    while (tid < sLattice.numberOfSites)
    {
        if (initializer)
            KpmSparseMatrixInitializer(tid, a, b, sLattice, sFirstReduceData, sSecondReduceData);

        else
            KpmSparseMatrixOperation(tid, a, b, sLattice, sFirstReduceData, sSecondReduceData);

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
        firstReduction[blockIdx.x] = sFirstReduceData[0] / sLattice.hamiltonianSize;
        secondReduction[blockIdx.x] = sSecondReduceData[0] / sLattice.hamiltonianSize;
    }
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