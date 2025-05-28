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

#ifndef QUANTUM_ABYSMAL_SRC_DENSITY_OF_STATES_1D_GPU_STANDARD_HPP
#define QUANTUM_ABYSMAL_SRC_DENSITY_OF_STATES_1D_GPU_STANDARD_HPP

#include "include/internal/Logger.hpp"
#include "include/public/DensityOfStates.hpp"
#include "src/lattice/LatticeImpl.hpp"
#include "src/providers/cuda_provider/rng/RngEngineImpl.hpp"
#include "util/Util.hpp"

#include <curand_kernel.h>

class DensityOfStates1dGpuStandard : public DensityOfStates
{
  public:
    Error SetNumberOfRandomVectors(size_t numVectors) override;
    Error SetNumberOfMoments(size_t order) override;
    std::vector<double> Compute() override;
    Error Save() const override;

    virtual ~DensityOfStates1dGpuStandard() override;

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

__forceinline__ __device__ void
KpmSparseMatrixInitializer(uint64_t local_tid, double* aHaloRegion,
                           double* bHaloRegion, LatticeStructure& lattice, double* sFirstReduceData,
                           double* sSecondReduceData)
{
    Hopping currentHop;
    int numOrbitals = lattice.numberOfOrbitals;

    // clang-format off
    #pragma unroll 4
    // clang-format on
    for (int j = 0; j < lattice.numberOfHoppings; j++)
    {
        currentHop = lattice.hoppings[j];

        int64_t newX = local_tid + currentHop.latticeHop[0];
        uint64_t bIndex = numOrbitals * local_tid + currentHop.orbitalHop[0];
        uint64_t aIndex = numOrbitals * newX + currentHop.orbitalHop[1];

        bHaloRegion[bIndex] += currentHop.hoppingStrength * aHaloRegion[aIndex];
    }

    // clang-format off
    #pragma unroll 2
    // clang-format on
    for (int j = 0; j < numOrbitals; j++)
    {
        uint64_t trueIndex = numOrbitals * local_tid + j;

        sFirstReduceData[threadIdx.x] += aHaloRegion[trueIndex] * aHaloRegion[trueIndex];
        sSecondReduceData[threadIdx.x] += bHaloRegion[trueIndex] * aHaloRegion[trueIndex];
    }
}

__forceinline__ __device__ void
KpmSparseMatrixOperation(uint64_t local_tid, double* aHaloRegion,
                         double* bHaloRegion, LatticeStructure& lattice, double* sFirstReduceData,
                         double* sSecondReduceData)
{
    Hopping currentHop;
    int numOrbitals = lattice.numberOfOrbitals;
    double temp[4] = {};

    // clang-format off
    #pragma unroll 4
    // clang-format on
    for (int j = 0; j < lattice.numberOfHoppings; j++)
    {
        currentHop = lattice.hoppings[j];

        int64_t newX = local_tid + currentHop.latticeHop[0];
        int64_t newIndex = numOrbitals * newX + currentHop.orbitalHop[1];

        temp[currentHop.orbitalHop[0]] += currentHop.hoppingStrength * bHaloRegion[newIndex];
    }

    // clang-format off
    #pragma unroll 2
    // clang-format on
    for (int j = 0; j < numOrbitals; j++)
    {
        uint64_t haloIndex = numOrbitals * local_tid + j;

        aHaloRegion[haloIndex] = 2 * temp[j] - aHaloRegion[haloIndex];

        sFirstReduceData[threadIdx.x] += bHaloRegion[haloIndex] * bHaloRegion[haloIndex];
        sSecondReduceData[threadIdx.x] += aHaloRegion[haloIndex] * bHaloRegion[haloIndex];

        temp[j] = 0;
    }
}

template <uint32_t blockSize, bool initializer>
__global__ void Reduce(double* a, double* b, LatticeStructure& lattice, double* firstReduction,
                       double* secondReduction)
{
    __shared__ double aHaloRegion[blockSize + 2];
    __shared__ double bHaloRegion[blockSize + 2];

    __shared__ double sFirstReduceData[blockSize];
    __shared__ double sSecondReduceData[blockSize];

    uint64_t numOrbitals = lattice.numberOfOrbitals;
    uint64_t tid = threadIdx.x + blockIdx.x * blockDim.x;
    uint64_t local_tid = threadIdx.x;

    sFirstReduceData[local_tid] = 0;
    sSecondReduceData[local_tid] = 0;

    __syncthreads();

    while (tid < lattice.numberOfSites)
    {
        // ----------------------------------------------------------------------------------------
        // Halo region loading.

        // Load main data into shared memory
        if (true)
        {
            aHaloRegion[local_tid + 1] = a[tid];
            bHaloRegion[local_tid + 1] = b[tid];
        }

        // Load left halo (only first thread in the block loads it)
        if (local_tid == 0)
        {
            aHaloRegion[0] = a[mod(tid - 1, lattice.latticeSize[0])];
            bHaloRegion[0] = b[mod(tid - 1, lattice.latticeSize[0])];
        }

        // Load right halo (only last thread in the block loads it)
        if (local_tid == blockSize - 1) 
        {
            aHaloRegion[blockSize + 1] = a[mod(tid + 1, lattice.latticeSize[0])];
            bHaloRegion[blockSize + 1] = b[mod(tid + 1, lattice.latticeSize[0])];
        }

        __syncthreads();

        // ----------------------------------------------------------------------------------------
        // Kpm Sparse Matrix Operation

        
        if (initializer)
        {
            KpmSparseMatrixInitializer(local_tid + 1, aHaloRegion, bHaloRegion, lattice,
                                       sFirstReduceData, sSecondReduceData);

            for (int j = 0; j < numOrbitals; j++)
            {
                b[numOrbitals * tid + j] = bHaloRegion[numOrbitals * (local_tid + 1) + j];
            }
        }

        else
        {
            KpmSparseMatrixOperation(local_tid + 1, aHaloRegion, bHaloRegion, lattice,
                                     sFirstReduceData, sSecondReduceData);
            
            for (int j = 0; j < numOrbitals; j++)
            {
                a[numOrbitals * tid + j] = aHaloRegion[numOrbitals * (local_tid + 1) + j];
            }
        }
        
        tid += blockDim.x * gridDim.x;
    }

    // --------------------------------------------------------------------------------------------

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

#endif