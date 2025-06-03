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

#include "DensityOfStates2d.hpp"

#include "src/plotting/PlotDensityOfStatesImpl.hpp"
#include "src/storage/StorageEngineImpl.hpp"

#include <chrono>
#include <omp.h>

// ================================================================================================
// Constructors

DensityOfStates2dGpuStandard::DensityOfStates2dGpuStandard()
{
    mLattice = LatticeImpl::GetInstance().GetLattice();
    numOrbitals = mLattice.numberOfOrbitals;
    xSize = mLattice.latticeSize[0];
    ySize = mLattice.latticeSize[1];
    xGhostedSize = xSize + 2;
    yGhostedSize = ySize + 2;
    numberOfGhosts = numOrbitals * (2 * xSize + 2 * ySize + 4);
}

DensityOfStates2dGpuStandard::~DensityOfStates2dGpuStandard()
{
}

// ================================================================================================
// Public

Result<void> DensityOfStates2dGpuStandard::SetDomainDecomposition(std::vector<uint32_t> numDomains)
{
    LOG_ERROR << "Not supported for CUDA Provider.";
    return Result<void>::SetError(NOT_SUPPORTED);
}

Result<void> DensityOfStates2dGpuStandard::SetNumberOfRandomVectors(size_t numVectors)
{
    mNumRandomVectors = numVectors;
    return Result<void>::SetError(SUCCESS);
}

Result<void> DensityOfStates2dGpuStandard::SetNumberOfMoments(size_t order)
{
    mNumOfMoments = order;
    dMomentsSize = order;
    return Result<void>::SetError(SUCCESS);
}

Result<std::vector<double>> DensityOfStates2dGpuStandard::ComputeMoments()
{
    // --------------------------------------------------------------------------------------------
    // Output info.

    LOG_INFO << "Computing DoS from a 2D mLattice on GPU/STANDARD!";
    LOG_INFO << "Number of Lattice Points : " << xSize << "x" << ySize << " = " << xSize * ySize;
    LOG_INFO << "Number of Orbitals : " << numOrbitals;
    LOG_INFO << "Number of Hoppings : " << (uint32_t)mLattice.numberOfHoppings;
    LOG_INFO << "Number of Moments  : " << mNumOfMoments;

    // ---------------------------------------------------------------------------------------------
    // Memory Allocation.

    double* dFirstRed;
    cudaMalloc((void**)&dFirstRed, NUM_THREADS * sizeof(double));
    cudaMemset(dFirstRed, 0, NUM_THREADS * sizeof(double));

    double* dSecondRed;
    cudaMalloc((void**)&dSecondRed, NUM_THREADS * sizeof(double));
    cudaMemset(dSecondRed, 0, NUM_THREADS * sizeof(double));

    double* dA;
    cudaMalloc((void**)&dA, mLattice.hamiltonianSize * sizeof(double));
    cudaMemset(dA, 0, mLattice.hamiltonianSize * sizeof(double));

    double* dB;
    cudaMalloc((void**)&dB, mLattice.hamiltonianSize * sizeof(double));
    cudaMemset(dB, 0, mLattice.hamiltonianSize * sizeof(double));

    cudaMalloc((void**)&dLattice, sizeof(LatticeStructure));
    cudaMemcpy(dLattice, &mLattice, sizeof(LatticeStructure), cudaMemcpyHostToDevice);

    cudaMalloc((void**)&dMoments, dMomentsSize * sizeof(double));
    cudaMemset(dMoments, 0, dMomentsSize * sizeof(double));

    // --------------------------------------------------------------------------------------------
    // Populate the random vector

    RngGpuEngine::GetInstance().GetRandomVector(dA, mLattice.hamiltonianSize);

    // --------------------------------------------------------------------------------------------
    // Compute

    auto start = std::chrono::high_resolution_clock::now();

    {
        Reduce<NUM_THREADS><<<NUM_BLOCKS, NUM_THREADS>>>(dA, dB, *dLattice, dFirstRed, dSecondRed, KpmSparseInit{});
        Reduce<NUM_THREADS><<<1, NUM_THREADS>>>(dFirstRed, dSecondRed, dMoments, 0);
    }

    for (int i = 0; i < mNumOfMoments / 2 - 1; i++)
    {
        Reduce<NUM_THREADS><<<NUM_BLOCKS, NUM_THREADS>>>((i % 2 == 0) ? dA : dB, (i % 2 == 0) ? dB : dA, *dLattice, dFirstRed, dSecondRed, KpmSparse{});
        Reduce<NUM_THREADS><<<1, NUM_THREADS>>>(dFirstRed, dSecondRed, dMoments, i + 1);
    }

    // --------------------------------------------------------------------------------------------
    // Copy to host.

    mMoments.resize(mNumOfMoments);
    cudaMemcpy(mMoments.data(), dMoments, mNumOfMoments * sizeof(double), cudaMemcpyDeviceToHost);

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    LOG_INFO << "Moments computation time : " << elapsed.count() << " seconds.";

    // --------------------------------------------------------------------------------------------
    // Free memory after computation

    // --------------------------------------------------------------------------------------------

    return Result<std::vector<double>>::SetValue(mMoments);
}

Result<std::vector<std::array<double, 2>>> DensityOfStates2dGpuStandard::ComputeDoS(uint32_t numPoints)
{
    // ---------------------------------------------------------------------------------------------
    // Memory Allocation.

    std::array<double, 2>* dosPointerArray;
    cudaMalloc((void**)&dosPointerArray, numPoints * sizeof(std::array<double, 2>));
    cudaMemset(dosPointerArray, 0, numPoints * sizeof(std::array<double, 2>));

    // --------------------------------------------------------------------------------------------
    // Compute.

    auto start = std::chrono::high_resolution_clock::now();

    DensityOfStatesFromMoments<<<NUM_BLOCKS, NUM_THREADS>>>(*dLattice, dMoments, dMomentsSize, dosPointerArray, numPoints);

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    LOG_INFO << "DoS computation time : " << elapsed.count() << " seconds.";

    // --------------------------------------------------------------------------------------------
    // Copy to host.

    mDoS.resize(numPoints);
    cudaMemcpy(mDoS.data(), dosPointerArray, numPoints * sizeof(std::array<double, 2>), cudaMemcpyDeviceToHost);

    return Result<std::vector<std::array<double, 2>>>::SetValue(mDoS);
}

Result<void> DensityOfStates2dGpuStandard::Save()
{
    StorageEngine::SaveDoS(mMoments, mDoS);

    return Result<void>::SetError(SUCCESS);
}

Result<void> DensityOfStates2dGpuStandard::PlotDoS()
{
    // change to static
    PlotDensityOfStatesImpl instance;
    instance.Plot(mDoS);

    return Result<void>::SetError(SUCCESS);
}

// ================================================================================================
// CUDA Device Functions

__device__ void DensityOfStates2dGpuStandard::KpmSparseInit::operator()(uint64_t tid, double* a, double* b, LatticeStructure& lattice,
                                                                        double* sFirstReduceData, double* sSecondReduceData) const
{
    Hopping currentHop;
    int numOrbitals = lattice.numberOfOrbitals;
    int xSize = lattice.latticeSize[0];
    int ySize = lattice.latticeSize[1];
    int y = tid / xSize;
    int x = tid % xSize;

    for (int j = 0; j < lattice.numberOfHoppings; j++)
    {
        currentHop = lattice.hoppings[j];

        int64_t newX = Math::Mod(x + currentHop.latticeHop[0], xSize);
        int64_t newY = Math::Mod(y + currentHop.latticeHop[1], ySize);
        uint64_t bIndex = numOrbitals * tid + currentHop.orbitalHop[0];
        uint64_t aIndex = numOrbitals * (newX + newY * xSize) + currentHop.orbitalHop[1];

        b[bIndex] += currentHop.hoppingStrength * a[aIndex];
    }

    for (int j = 0; j < numOrbitals; j++)
    {
        uint64_t trueIndex = numOrbitals * (x + y * xSize) + j;

        sFirstReduceData[threadIdx.x] += a[trueIndex] * a[trueIndex];
        sSecondReduceData[threadIdx.x] += b[trueIndex] * a[trueIndex];
    }
};

__device__ void DensityOfStates2dGpuStandard::KpmSparse::operator()(uint64_t tid, double* a, double* b, LatticeStructure& lattice,
                                                                    double* sFirstReduceData, double* sSecondReduceData) const
{
    Hopping currentHop;
    int numOrbitals = lattice.numberOfOrbitals;
    int xSize = lattice.latticeSize[0];
    int ySize = lattice.latticeSize[1];
    int y = tid / xSize;
    int x = tid % xSize;
    double temp[4] = {};

    for (int j = 0; j < lattice.numberOfHoppings; j++)
    {
        currentHop = lattice.hoppings[j];

        int64_t newX = Math::Mod(x + currentHop.latticeHop[0], xSize);
        int64_t newY = Math::Mod(y + currentHop.latticeHop[1], ySize);
        int64_t newPos = newX + newY * xSize;
        int64_t newIndex = numOrbitals * newPos + currentHop.orbitalHop[1];

        temp[currentHop.orbitalHop[0]] += currentHop.hoppingStrength * b[newIndex];
    }

    for (int j = 0; j < numOrbitals; j++)
    {
        uint64_t trueIndex = numOrbitals * (x + y * xSize) + j;

        a[trueIndex] = 2 * temp[j] - a[trueIndex];

        sFirstReduceData[threadIdx.x] += b[trueIndex] * b[trueIndex];
        sSecondReduceData[threadIdx.x] += a[trueIndex] * b[trueIndex];

        temp[j] = 0;
    }
};