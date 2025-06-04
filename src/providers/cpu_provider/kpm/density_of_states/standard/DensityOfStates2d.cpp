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

#include "src/providers/cpu_provider/rng/RngImpl.hpp"
#include "src/storage/StorageEngineImpl.hpp"
#include "src/math/Math.hpp"
#include "src/plotting/PlotDensityOfStatesImpl.hpp"
#include "util/Util.hpp"

#include <chrono>
#include <omp.h>

// ================================================================================================
// Constructors

DensityOfStates2dCpuStandard::DensityOfStates2dCpuStandard()
{
    mLattice = LatticeImpl::GetInstance().GetLattice();
    numOrbitals = mLattice.numberOfOrbitals;
    xSize = mLattice.latticeSize[0];
    ySize = mLattice.latticeSize[1];
    xGhostedSize = xSize + 2;
    yGhostedSize = ySize + 2;
    numberOfGhosts = numOrbitals * (2 * xSize + 2 * ySize + 4);
}

DensityOfStates2dCpuStandard::~DensityOfStates2dCpuStandard()
{
}

// ================================================================================================
// Public

Result<void> DensityOfStates2dCpuStandard::SetDomainDecomposition(std::vector<uint32_t> numDomains)
{
    if (numDomains.size() != 2)
    {
        LOG_ERROR << "Invalid dimension!";
        return Result<void>::SetError(RUNTIME_ERROR);
    }

    if (numDomains[0] == 0 || numDomains[1] == 0)
    {
        LOG_ERROR << "Invalid domain decomposition!";
        return Result<void>::SetError(RUNTIME_ERROR);
    }

    if (xSize % numDomains[0] != 0)
    {
        LOG_ERROR << "xSize is not a multiple of xDimension decomposition!";
        return Result<void>::SetError(RUNTIME_ERROR);
    }

    if (ySize % numDomains[1] != 0)
    {
        LOG_ERROR << "ySize is not a multiple of yDimension decomposition!";
        return Result<void>::SetError(RUNTIME_ERROR);
    }

    xDomainDecomposition = numDomains[0];
    yDomainDecomposition = numDomains[1];
    NUM_THREADS = xDomainDecomposition * yDomainDecomposition;
    omp_set_num_threads(NUM_THREADS);

    return Result<void>::SetError(SUCCESS);
}

Result<void> DensityOfStates2dCpuStandard::SetNumberOfRandomVectors(size_t numVectors)
{
    mNumRandomVectors = numVectors;
    return Result<void>::SetError(SUCCESS);
}

Result<void> DensityOfStates2dCpuStandard::SetNumberOfMoments(size_t order)
{
    mNumOfMoments = order;
    return Result<void>::SetError(SUCCESS);
}

Result<std::vector<double>> DensityOfStates2dCpuStandard::ComputeMoments()
{
    // --------------------------------------------------------------------------------------------
    // Output info

    LOG_INFO << "Computing DoS from a 2D mLattice on CPU/STANDARD!";
    LOG_INFO << "Number of Lattice Points : " << xSize << "x" << ySize << " = " << xSize * ySize;
    LOG_INFO << "Number of Orbitals : " << numOrbitals;
    LOG_INFO << "Number of Hoppings : " << (uint32_t)mLattice.numberOfHoppings;
    LOG_INFO << "Number of Moments  : " << mNumOfMoments;

    // --------------------------------------------------------------------------------------------
    // Allocate necessary memory

    uint64_t arraySize = mLattice.hamiltonianSize + numberOfGhosts;

    double* a = (double*)malloc(arraySize * sizeof(double));
    double* b = (double*)malloc(arraySize * sizeof(double));

    memset(a, 0, arraySize * sizeof(double));
    memset(b, 0, arraySize * sizeof(double));

    // --------------------------------------------------------------------------------------------
    // Populate the random vector

    RngCpuEngine::GetInstance().GetRandomVector(a, arraySize);

    // --------------------------------------------------------------------------------------------
    // Compute

    auto start = std::chrono::high_resolution_clock::now();

    {
        InitializeKpmVectors(a, b);
    }

    for (int i = 0; i < mNumOfMoments / 2 - 1; i++)
    {
        ExecuteKpmVectorUpdate((i % 2 == 0) ? a : b, (i % 2 == 0) ? b : a);
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    LOG_INFO << "Moments computation time : " << elapsed.count() << " seconds.";

    // --------------------------------------------------------------------------------------------
    // Free memory after computation

    free(a);
    free(b);

    // --------------------------------------------------------------------------------------------

    return Result<std::vector<double>>::SetValue(mMoments);
}

Result<std::vector<std::array<double, 2>>> DensityOfStates2dCpuStandard::ComputeDoS(uint32_t numPoints)
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
        
        mDoS[i][0] *= mLattice.energyScaling;
        mDoS[i][0] += mLattice.energyShift;

        mDoS[i][1] /= mLattice.energyScaling;
        mDoS[i][1] += mLattice.energyShift;
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    LOG_INFO << "DoS computation time : " << elapsed.count() << " seconds.";

    return Result<std::vector<std::array<double, 2>>>::SetValue(mDoS);
}

Result<void> DensityOfStates2dCpuStandard::Save()
{
    StorageEngine::SaveDoS(mLattice, mMoments, mDoS);

    return Result<void>::SetError(SUCCESS);
}

Result<void> DensityOfStates2dCpuStandard::PlotDoS()
{
    PlotDensityOfStatesImpl instance;
    instance.Plot(mDoS);

    return Result<void>::SetError(SUCCESS);
}

// ================================================================================================
// Private

// ------------------------------------------------------------------------------------------------
// This is the HPC part. Careful tuning needs to explored.

void DensityOfStates2dCpuStandard::InitializeKpmVectors(double* a, double* b)
{
    double firstMoment{0};
    double secondMoment{0};

    // Update ghosts before computation
    UpdateGhosts(a);
    UpdateGhosts(b);

    // clang-format off
    #pragma omp parallel reduction(+:firstMoment, secondMoment)
    // clang-format on
    {
        uint32_t threadId = omp_get_thread_num();
        uint32_t xBlock = threadId / xDomainDecomposition;
        uint32_t yBlock = threadId % yDomainDecomposition;

        uint64_t xInitIdx = xBlock * (xSize / xDomainDecomposition) + 1;
        uint64_t xEndIdx = xInitIdx + (xSize / xDomainDecomposition);
        uint64_t yInitIdx = yBlock * (ySize / yDomainDecomposition) + 1;
        uint64_t yEndIdx = yInitIdx + (ySize / yDomainDecomposition);

        for (uint64_t y = yInitIdx; y < yEndIdx; y++)
        {
            for (uint64_t x = xInitIdx; x < xEndIdx; x++)
            {
                for (int j = 0; j < mLattice.numberOfHoppings; j++)
                {
                    Hopping currentHop = mLattice.hoppings[j];
                    int64_t bIdx = ARRAY_IDX(x, y, 0, 0, currentHop.orbitalHop[0]);
                    int64_t aIdx = ARRAY_IDX(x, y, currentHop.latticeHop[0], currentHop.latticeHop[1], currentHop.orbitalHop[1]);

                    b[bIdx] += currentHop.hoppingStrength * a[aIdx];
                }

                for (int j = 0; j < numOrbitals; j++)
                {
                    int64_t idx = ARRAY_IDX(x, y, 0, 0, j);

                    firstMoment += a[idx] * a[idx];
                    secondMoment += b[idx] * a[idx];
                }
            }
        }
    }

    // --------------------------------------------------------------------------------------------
    // 

    mMoments.push_back(firstMoment / mLattice.hamiltonianSize);
    mMoments.push_back(secondMoment / mLattice.hamiltonianSize);

    UpdateGhosts(b);
}

void DensityOfStates2dCpuStandard::ExecuteKpmVectorUpdate(double* a, double* b)
{
    double firstMoment{0};
    double secondMoment{0};

    // clang-format off
    #pragma omp parallel reduction(+:firstMoment, secondMoment)
    // clang-format on
    {
        uint32_t threadId = omp_get_thread_num();
        uint32_t xBlock = threadId / xDomainDecomposition;
        uint32_t yBlock = threadId % yDomainDecomposition;

        uint64_t xInitIdx = xBlock * (xSize / xDomainDecomposition) + 1;
        uint64_t xEndIdx = xInitIdx + (xSize / xDomainDecomposition);
        uint64_t yInitIdx = yBlock * (ySize / yDomainDecomposition) + 1;
        uint64_t yEndIdx = yInitIdx + (ySize / yDomainDecomposition);

        for (uint64_t y = yInitIdx; y < yEndIdx; y++)
        {
            for (uint64_t x = xInitIdx; x < xEndIdx; x++)
            {
                double temp[4] = {};

                // ------------------------------------------------------------------------------------
                // Perfomance test of codegen for Graphene model

                // temp[0] += mLattice.hoppings[0].hoppingStrength * b[ARRAY_IDX(x, y, 1, 0, 1)];
                // temp[0] += mLattice.hoppings[1].hoppingStrength * b[ARRAY_IDX(x, y, 0, 1, 1)];
                // temp[0] += mLattice.hoppings[2].hoppingStrength * b[ARRAY_IDX(x, y, 0, 0, 1)];

                // temp[1] += mLattice.hoppings[3].hoppingStrength * b[ARRAY_IDX(x, y, -1, 0, 0)];
                // temp[1] += mLattice.hoppings[4].hoppingStrength * b[ARRAY_IDX(x, y, 0, -1, 0)];
                // temp[1] += mLattice.hoppings[5].hoppingStrength * b[ARRAY_IDX(x, y, 0, 0, 0)];

                // a[ARRAY_IDX(x, y, 0, 0, 0)] = 2 * temp[0] - a[ARRAY_IDX(x, y, 0, 0, 0)];
                // a[ARRAY_IDX(x, y, 0, 0, 1)] = 2 * temp[1] - a[ARRAY_IDX(x, y, 0, 0, 1)];

                // firstMoment += b[ARRAY_IDX(x, y, 0, 0, 0)] * b[ARRAY_IDX(x, y, 0, 0, 0)];
                // firstMoment += b[ARRAY_IDX(x, y, 0, 0, 1)] * b[ARRAY_IDX(x, y, 0, 0, 1)];

                // secondMoment += a[ARRAY_IDX(x, y, 0, 0, 0)] * b[ARRAY_IDX(x, y, 0, 0, 0)];
                // secondMoment += a[ARRAY_IDX(x, y, 0, 0, 1)] * b[ARRAY_IDX(x, y, 0, 0, 1)];

                // Observed reduction of 60%! Codegen/Callback will be remarkable.
                // This is full loop unrolling.

                // --------------------------------------------------------------------------------

                for (int j = 0; j < mLattice.numberOfHoppings; j++)
                {
                    Hopping currentHop = mLattice.hoppings[j];
                    int64_t idx = ARRAY_IDX(x, y, currentHop.latticeHop[0], currentHop.latticeHop[1], currentHop.orbitalHop[1]);

                    temp[currentHop.orbitalHop[0]] += currentHop.hoppingStrength * b[idx];
                }

                for (int j = 0; j < numOrbitals; j++)
                {
                    int64_t idx = ARRAY_IDX(x, y, 0, 0, j);

                    a[idx] = 2 * temp[j] - a[idx];

                    firstMoment += b[idx] * b[idx];
                    secondMoment += a[idx] * b[idx];
                }
            }
        }
    }

    // --------------------------------------------------------------------------------------------
    // Trick to halve the number of vector updates.

    mMoments.push_back(2 * (firstMoment / mLattice.hamiltonianSize) - mMoments[0]);
    mMoments.push_back(2 * (secondMoment / mLattice.hamiltonianSize) - mMoments[1]);

    UpdateGhosts(a);
}

inline void DensityOfStates2dCpuStandard::UpdateGhosts(double* a)
{
    // --------------------------------------------------------------------------------------------
    // Edges.

    for (int x = 1; x < xGhostedSize - 1; x++)
    {
        for (int j = 0; j < numOrbitals; j++)
        {
            a[ARRAY_IDX(x, 0, 0, 0, j)] = a[ARRAY_IDX(x, ySize, 0, 0, j)];
            a[ARRAY_IDX(x, ySize, 0, 1, j)] = a[ARRAY_IDX(x, 0, 0, 1, j)];
        }
    }

    for (int y = 1; y < yGhostedSize - 1; y++)
    {
        for (int j = 0; j < numOrbitals; j++)
        {
            a[ARRAY_IDX(0, y, 0, 0, j)] = a[ARRAY_IDX(xSize, y, 0, 0, j)];
            a[ARRAY_IDX(xSize, y, 1, 0, j)] = a[ARRAY_IDX(0, y, 1, 0, j)];
        }
    }

    // --------------------------------------------------------------------------------------------
    // Vertexes.

    for (int j = 0; j < numOrbitals; j++)
    {
        a[ARRAY_IDX(0, 0, 0, 0, j)] = a[ARRAY_IDX(xSize, ySize, 0, 0, j)];
        a[ARRAY_IDX(xSize, ySize, 1, 1, j)] = a[ARRAY_IDX(0, 0, 1, 1, j)];
        a[ARRAY_IDX(xSize, 0, 1, 0, j)] = a[ARRAY_IDX(0, ySize, 1, 0, j)];
        a[ARRAY_IDX(0, ySize, 0, 1, j)] = a[ARRAY_IDX(xSize, 0, 0, 1, j)];
    }
}