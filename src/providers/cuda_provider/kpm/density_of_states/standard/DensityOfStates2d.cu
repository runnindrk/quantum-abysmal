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

#include "src/math/Math.hpp"
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
    return Result<void>::SetError(SUCCESS);
}

Result<std::vector<double>> DensityOfStates2dGpuStandard::ComputeMoments()
{
    // ------------------------------------------------------------------------
    // Output info.

    LOG_INFO << "Computing DoS from a 2D mLattice on CPU/STANDARD!";
    LOG_INFO << "Number of Lattice Points : " << xSize << "x" << ySize << " = " << xSize * ySize;
    LOG_INFO << "Number of Orbitals : " << numOrbitals;
    LOG_INFO << "Number of Hoppings : " << (uint32_t)mLattice.numberOfHoppings;
    LOG_INFO << "Number of Moments  : " << mNumOfMoments;

    // ---------------------------------------------------------------------------------------------

    const unsigned int numBlocks = 256;
    const unsigned int numThreads = 256;

    // ---------------------------------------------------------------------------------------------
    // Memory Allocation.

    LatticeStructure* dLattice;
    cudaMalloc((void**)&dLattice, sizeof(LatticeStructure));
    cudaMemcpy(dLattice, &mLattice, sizeof(LatticeStructure), cudaMemcpyHostToDevice);

    double* dFirstRed;
    cudaMalloc((void**)&dFirstRed, numThreads * sizeof(double));
    cudaMemset(dFirstRed, 0, numThreads * sizeof(double));

    double* dSecondRed;
    cudaMalloc((void**)&dSecondRed, numThreads * sizeof(double));
    cudaMemset(dSecondRed, 0, numThreads * sizeof(double));

    double* dMoments;
    cudaMalloc((void**)&dMoments, mNumOfMoments * sizeof(double));
    cudaMemset(dMoments, 0, mNumOfMoments * sizeof(double));

    double* dA;
    cudaMalloc((void**)&dA, mLattice.hamiltonianSize * sizeof(double));
    cudaMemset(dA, 0, mLattice.hamiltonianSize * sizeof(double));

    double* dB;
    cudaMalloc((void**)&dB, mLattice.hamiltonianSize * sizeof(double));
    cudaMemset(dB, 0, mLattice.hamiltonianSize * sizeof(double));

    // --------------------------------------------------------------------------------------------
    // Populate the random vector

    RngGpuEngine::GetInstance().GetRandomVector(dA, mLattice.hamiltonianSize);

    // --------------------------------------------------------------------------------------------
    // Compute

    auto start = std::chrono::high_resolution_clock::now();

    {
        Reduce<numThreads><<<numBlocks, numThreads>>>(dA, dB, *dLattice, dFirstRed, dSecondRed, KpmSparseInit{});
        Reduce<numThreads><<<1, numThreads>>>(dFirstRed, dSecondRed, dMoments, 0);
    }

    for (int i = 0; i < mNumOfMoments / 2 - 1; i++)
    {
        Reduce<numThreads><<<numBlocks, numThreads>>>((i % 2 == 0) ? dA : dB, (i % 2 == 0) ? dB : dA, *dLattice, dFirstRed, dSecondRed, KpmSparse{});
        Reduce<numThreads><<<1, numThreads>>>(dFirstRed, dSecondRed, dMoments, i + 1);
    }

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

// Change to a CUDA Kernel
Result<std::vector<double>> DensityOfStates2dGpuStandard::ComputeDoS(uint32_t numPoints)
{
    // Resize to user desired spectral density
    mDoS.resize(numPoints);

    auto start = std::chrono::high_resolution_clock::now();

    // clang-format off
    #pragma omp parallel for
    // clang-format on
    for (int i = 0; i < numPoints; i++)
    {
        double E = -0.999 + i * (2 * 0.999) / (numPoints - 1);

        {
            mDoS[i][1] += 1 * mMoments[0] * Math::JacksonKernel(0, mMoments.size()) * Math::ChebyshevPolynomial(0, E);
        }

        for (int j = 1; j < mMoments.size(); j++)
        {
            mDoS[i][1] += 2 * mMoments[j] * Math::JacksonKernel(j, mMoments.size()) * Math::ChebyshevPolynomial(j, E);
        }

        mDoS[i][1] *= 1 / (M_PI * (sqrt(1 - E * E)));

        mDoS[i][0] = E * mLattice.energyScaling;
        mDoS[i][0] += +mLattice.energyShift;

        mDoS[i][1] /= mLattice.energyScaling;
        mDoS[i][1] += mLattice.energyShift;
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    LOG_INFO << "DoS computation time : " << elapsed.count() << " seconds.";

    return Result<std::vector<double>>::SetError(SUCCESS);
}

Result<void> DensityOfStates2dGpuStandard::Save()
{
    StorageEngine::SaveDoS(mMoments, mDoS);

    return Result<void>::SetError(SUCCESS);
}

Result<void> DensityOfStates2dGpuStandard::PlotDoS()
{
    PlotDensityOfStatesImpl instance;
    instance.Plot(mDoS);

    return Result<void>::SetError(SUCCESS);
}

// ================================================================================================
// CUDA Kernels

__device__ void KpmSparseInit::operator()(uint64_t tid, double* a, double* b, LatticeStructure& lattice, double* sFirstReduceData,
                                          double* sSecondReduceData) const
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

        int64_t newX = mod(x + currentHop.latticeHop[0], xSize);
        int64_t newY = mod(y + currentHop.latticeHop[1], ySize);
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

__device__ void KpmSparse::operator()(uint64_t tid, double* a, double* b, LatticeStructure& lattice, double* sFirstReduceData,
                                      double* sSecondReduceData) const
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

        int64_t newX = mod(x + currentHop.latticeHop[0], xSize);
        int64_t newY = mod(y + currentHop.latticeHop[1], ySize);
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