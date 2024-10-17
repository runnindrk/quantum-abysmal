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
#include "util/Util.hpp"

#include <chrono>
#include <omp.h>
#include <random>

Error DensityOfStates2dCpuStandard::SetNumberOfRandomVectors(size_t numVectors)
{
    mNumRandomVectors = numVectors;
    return SUCCESS;
}

Error DensityOfStates2dCpuStandard::SetNumberOfMoments(size_t order)
{
    mNumOfMoments = order;
    return SUCCESS;
}

std::vector<double> DensityOfStates2dCpuStandard::Compute()
{
    // ------------------------------------------------------------------------
    // Output info.

    LOG_INFO << "Computing DoS from a 2D lattice on CPU/STANDARD!";
    LOG_INFO << "Number of lattice points : " << lattice.latticeSize[0] << "x"
             << lattice.latticeSize[1] << " = " << lattice.numberOfSites;
    LOG_INFO << "Number of Orbitals : " << lattice.numberOfOrbitals;
    LOG_INFO << "Number of Hoppings : " << (uint32_t)lattice.numberOfHoppings;
    LOG_INFO << "Number of Moments  : " << mNumOfMoments;

    // ------------------------------------------------------------------------

    auto start = std::chrono::high_resolution_clock::now();

    InitializeKpmVectors();

    for (int i = 0; i < mNumOfMoments / 2 - 1; i++)
    {
        ExecuteKpmVectorUpdate((i % 2 == 0) ? a : b, (i % 2 == 0) ? b : a);
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    LOG_INFO << "Computation time : " << elapsed.count() << " seconds.";

    // ------------------------------------------------------------------------

    return moments;
}

// ----------------------------------------------------------------------------
// Private

void DensityOfStates2dCpuStandard::InitializeKpmVectors()
{
    // In development. Trying out ideas, this is perhaps all wrong.

    // This will be substituted by the RNG_ENGINE of the library.
    // For now, it is for tests purpouses.

    std::random_device rd;
    std::mt19937 gen(rd());

    // The distribution width comes from theory.
    // Optimal error comes from the fourth moment = 2.
    std::normal_distribution<> dist(0.0, pow(2.0 / 3.0, 1.0 / 4.0));

    double randomVectorNorm{};

    for (uint64_t y = 1; y < yGhostedSize - 1; y++)
    {
        for (uint64_t x = 1; x < xGhostedSize - 1; x++)
        {
            uint64_t i = x + y * xGhostedSize;

            for (int j = 0; j < numOrbitals; j++)
            {
                uint64_t trueIndex = numOrbitals * i + j;
                double randomNumber = dist(gen);

                randomVectorNorm += randomNumber * randomNumber;
                a[trueIndex] = randomNumber;
            }
        }
    }

    for (uint64_t y = 1; y < yGhostedSize - 1; y++)
    {
        for (uint64_t x = 1; x < xGhostedSize - 1; x++)
        {
            uint64_t i = x + y * xGhostedSize;

            for (int j = 0; j < numOrbitals; j++)
            {
                uint64_t trueIndex = numOrbitals * i + j;

                a[trueIndex] /= sqrt(randomVectorNorm);
                b[trueIndex] = 0;
            }
        }
    }

    HostHopping currentHop;
    double firstMoment{0};
    double secondMoment{0};
    UpdateGhosts(a);
    UpdateGhosts(b);

    for (uint64_t y = 1; y < yGhostedSize - 1; y++)
    {
        for (uint64_t x = 1; x < xGhostedSize - 1; x++)
        {
            uint64_t i = x + y * xGhostedSize;

            // clang-format off
            #pragma unroll 4
            // clang-format on
            for (int j = 0; j < lattice.numberOfHoppings; j++)
            {
                currentHop = lattice.hoppings[j];
                int64_t newX = x + currentHop.latticeHop[0];
                int64_t newY = y + currentHop.latticeHop[1];

                uint64_t bIndex = numOrbitals * i + currentHop.orbitalHop[0];
                uint64_t aIndex =
                    numOrbitals * (newX + newY * xGhostedSize) + currentHop.orbitalHop[1];

                b[bIndex] += currentHop.hoppingStrength * a[aIndex];
            }

            for (int j = 0; j < numOrbitals; j++)
            {
                firstMoment += a[numOrbitals * i + j] * a[numOrbitals * i + j];
                secondMoment += b[numOrbitals * i + j] * a[numOrbitals * i + j];
            }
        }
    }

    moments.push_back(firstMoment);
    moments.push_back(secondMoment);
    UpdateGhosts(b);
}

void DensityOfStates2dCpuStandard::ExecuteKpmVectorUpdate(double* a, double* b)
{
    double firstMoment{0};
    double secondMoment{0};

    int numThreads = maxOutNumThreads(omp_get_num_procs());
    int numBlockPerDim = static_cast<int>(sqrt(numThreads));

    omp_set_num_threads(numThreads);

    // clang-format off
    #pragma omp parallel reduction(+:firstMoment, secondMoment)
    // clang-format on
    {
        int threadId = omp_get_thread_num();

        int xBlock = threadId / numBlockPerDim;
        int yBlock = threadId % numBlockPerDim;

        uint64_t xInitIdx = xBlock * (xSize / numBlockPerDim) + 1;
        uint64_t xEndIdx = xInitIdx + (xSize / numBlockPerDim);
        uint64_t yInitIdx = yBlock * (ySize / numBlockPerDim) + 1;
        uint64_t yEndIdx = yInitIdx + (ySize / numBlockPerDim);

        // printf("Thread %d\n", thread_id, " xBlock ", xBlock, " yBlock", yBlock);

        HostHopping currentHop;
        double temp[2] = {};

        for (uint64_t y = yInitIdx; y < yEndIdx; y++)
        {
            for (uint64_t x = xInitIdx; x < xEndIdx; x++)
            {
                // clang-format off
                #pragma unroll 4
                // clang-format on
                for (int j = 0; j < lattice.numberOfHoppings; j++)
                {
                    currentHop = lattice.hoppings[j];
                    
                    int64_t newX = x + currentHop.latticeHop[0];
                    int64_t newY = y + currentHop.latticeHop[1];
                    int64_t newPos = newX + newY * xGhostedSize;
                    int64_t newIndex = numOrbitals * newPos + currentHop.orbitalHop[1];

                    temp[currentHop.orbitalHop[0]] += currentHop.hoppingStrength * b[newIndex];
                }

                // clang-format off
                #pragma unroll 2
                // clang-format on
                for (int j = 0; j < numOrbitals; j++)
                {
                    uint64_t trueIndex = numOrbitals * (x + y * xGhostedSize) + j;

                    a[trueIndex] = 2 * temp[j] - a[trueIndex];

                    firstMoment += b[trueIndex] * b[trueIndex];
                    secondMoment += a[trueIndex] * b[trueIndex];

                    temp[j] = 0;
                }
            }
        }
    }

    // Trick to halve the number of vector updates.
    moments.push_back(2 * firstMoment - moments[0]);
    moments.push_back(2 * secondMoment - moments[1]);

    UpdateGhosts(a);
}

inline void DensityOfStates2dCpuStandard::UpdateGhosts(double* a)
{
    for (int x = 1; x < xGhostedSize - 1; x++)
    {
        for (int j = 0; j < numOrbitals; j++)
        {
            a[numOrbitals * x + j] = a[numOrbitals * (x + ySize * xGhostedSize) + j];
            a[numOrbitals * (x + (ySize + 1) * xGhostedSize) + j] =
                a[numOrbitals * (x + xGhostedSize) + j];
        }
    }

    for (int y = 1; y < yGhostedSize; y++)
    {
        for (int j = 0; j < numOrbitals; j++)
        {
            a[numOrbitals * (y * xGhostedSize) + j] =
                a[numOrbitals * (xSize + y * xGhostedSize) + j];
            a[numOrbitals * (xSize + 1 + y * xGhostedSize) + j] =
                a[numOrbitals * (1 + y * xGhostedSize) + j];
        }
    }
}