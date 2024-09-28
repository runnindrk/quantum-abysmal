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
#include <random>

Error DensityOfStates2dCpuStandard::SetNumberOfRandomVectors(size_t numVectors)
{
    LOG_INFO << "Hello from DoS 2D on CPU/STANDARD!";
    LOG_INFO << "Number of Orbitals : " << LatticeImpl::GetInstance().GetLattice().numberOfOrbitals;
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
    InitializeKpmVectors();

    for (int i = 1; i < mNumOfMoments / 2; i++)
    {
        if (i % 2 == 0)
        {
            ExecuteKpmVectorUpdate(a, b);
            ComputeMoments(b, a);
        }

        else
        {
            ExecuteKpmVectorUpdate(b, a);
            ComputeMoments(a, b);
        }
    }

    return moments;
}

void DensityOfStates2dCpuStandard::InitializeKpmVectors()
{
    // In development. Trying out ideas, this is perhaps all wrong.

    // This will be substituted by the RNG_ENGINE of the library.
    // For now, it is for tests purpouses.

    LatticeImpl::Hopping currentHop;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<> dist(0.0, 1.0);

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

    UpdateGhosts(a);
    UpdateGhosts(b);

    for (uint64_t y = 1; y < yGhostedSize - 1; y++)
    {
        for (uint64_t x = 1; x < xGhostedSize - 1; x++)
        {
            uint64_t i = x + y * xGhostedSize;
            
            #pragma unroll 4
            for (int j = 0; j < lattice.hoppings.size(); j++)
            {
                currentHop = hoppings[j];
                int64_t newX = x + currentHop.latticeHop[0];
                int64_t newY = y + currentHop.latticeHop[1];

                uint64_t bIndex = numOrbitals * i + currentHop.orbitalHop[0];
                uint64_t aIndex = numOrbitals * (newX + newY * xGhostedSize) + currentHop.orbitalHop[1];

                b[bIndex] += currentHop.hoppingStrength * a[aIndex];
            }
        }
    }

    UpdateGhosts(b);

    double firstMoment{0};
    double secondMoment{0};

    for (uint64_t y = 1; y < yGhostedSize - 1; y++)
    {
        for (uint64_t x = 1; x < xGhostedSize - 1; x++)
        {
            uint64_t i = x + y * xGhostedSize;

            for (int j = 0; j < numOrbitals; j++)
            {
                firstMoment += a[numOrbitals*i + j]*a[numOrbitals*i + j];
                secondMoment += a[numOrbitals*i + j]*b[numOrbitals*i + j];
            }
        }
    }

    moments.push_back(firstMoment);
    moments.push_back(secondMoment);
}

void DensityOfStates2dCpuStandard::ExecuteKpmVectorUpdate(double* a, double* b)
{
    LatticeImpl::Hopping currentHop;
    std::vector<double> accumulation(numOrbitals, 0);

    for (uint64_t y = 1; y < yGhostedSize - 1; y++)
    {
        for (uint64_t x = 1; x < xGhostedSize - 1; x++)
        {
            #pragma unroll 4
            for (int j = 0; j < lattice.hoppings.size(); j++)
            {
                currentHop = hoppings[j];
                int64_t newX = x + currentHop.latticeHop[0];
                int64_t newY = y + currentHop.latticeHop[1];
                uint64_t aIndex = numOrbitals * (newX  + newY * xGhostedSize) + currentHop.orbitalHop[1];

                accumulation[currentHop.orbitalHop[0]] += currentHop.hoppingStrength * b[aIndex];
            }

            #pragma omp simd
            for (int j = 0; j < numOrbitals; j++)
            {
                uint64_t trueIndex = numOrbitals * (x + y * xGhostedSize) + j;
                a[trueIndex] = 2 * accumulation[j] - a[trueIndex];
                accumulation[j] = 0;
            }
        }
    }

    UpdateGhosts(a);
}

void DensityOfStates2dCpuStandard::UpdateGhosts(double* a)
{   
    for (int x = 1; x < xGhostedSize - 1; x++)
    {
        for (int j = 0; j < numOrbitals; j++)
        {
            a[numOrbitals * x + j] = a[numOrbitals * (x + ySize * xGhostedSize) + j];
            a[numOrbitals * (x + (ySize + 1) * xGhostedSize) + j] = a[numOrbitals * (x + xGhostedSize) + j];
        }
    }

    for (int y = 1; y < yGhostedSize; y++)
    {
        for (int j = 0; j < numOrbitals; j++)
        {
            a[numOrbitals * (y * xGhostedSize) + j] = a[numOrbitals * (xSize + y * xGhostedSize) + j];
            a[numOrbitals * (xSize + 1 + y * xGhostedSize) + j] = a[numOrbitals * (1 + y * xGhostedSize) + j];
        }
    }
}

void DensityOfStates2dCpuStandard::ComputeMoments(double *b, double *c)
{
    double firstMoment{0};
    double secondMoment{0};

    for (uint64_t y = 1; y < yGhostedSize - 1; y++)
    {
        for (uint64_t x = 1; x < xGhostedSize - 1; x++)
        {
            uint64_t i = x + y * xGhostedSize;

            for (int j = 0; j < numOrbitals; j++)
            {
                firstMoment += b[numOrbitals*i + j]*b[numOrbitals*i + j];
                secondMoment += c[numOrbitals*i + j]*b[numOrbitals*i + j];
            }
        }
    }

    moments.push_back(2*firstMoment - moments[0]);
    moments.push_back(2*secondMoment - moments[1]);
}