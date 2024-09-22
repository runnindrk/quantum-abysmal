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

Error DensityOfStates2dCpuStandard::Compute()
{
    InitializeKpmVectors();

    for (int i = 0; i < mNumOfMoments / 4; i++)
    {
        ExecuteFirstKpmVectorUpdate();
        ExecuteSecondKpmVectorUpdate();
    }

    // PrintVector();
    return SUCCESS;
}

// This will be moved out of here.
__host__ __device__ inline int mod(int a, int b)
{
    int r = a % b;
    return r < 0 ? r + b : r;
}

void DensityOfStates2dCpuStandard::PrintVector()
{
    for (int i = 0; i < lattice.hamiltonianSize; i++)
    {
        LOG_INFO << b[i];
    }

    return;
}

void DensityOfStates2dCpuStandard::InitializeKpmVectors()
{
    // In development. Trying out ideas, this is perhaps all wrong.

    // This will be substituted by the RNG_ENGINE of the library.
    // For now, it is for tests purpouses.

    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<> dist(0.0, 1.0);

    for (uint64_t i = 0; i < lattice.hamiltonianSize; i++)
    {
        a[i] = dist(gen);
        b[i] = 0;
    }

#pragma unroll 4
    for (uint64_t i = 0; i < lattice.numberOfSites; i++)
    {
        for (int j = 0; j < lattice.hoppings.size(); j++)
        {
            uint64_t bIndex = numOrbitals * i + hoppings[j].orbitalHop[0];
            uint64_t aIndex = GetMatrixElement(i, hoppings[j].latticeHop, numOrbitals,
                                               hoppings[j].orbitalHop[1], xSize, ySize);

            b[bIndex] += hoppings[j].hoppingStrength * a[aIndex];
        }
    }

    return;
}

void DensityOfStates2dCpuStandard::ExecuteFirstKpmVectorUpdate()
{
    std::vector<double> result(numOrbitals * lattice.hoppings.size(), 0);

#pragma unroll 4
    for (uint64_t i = 0; i < lattice.numberOfSites; i++)
    {
        std::fill(result.begin(), result.end(), 0.0);

        for (int j = 0; j < lattice.hoppings.size(); j++)
        {
            uint64_t aIndex = numOrbitals * i + hoppings[j].orbitalHop[0];
            uint64_t bIndex = GetMatrixElement(i, hoppings[j].latticeHop, numOrbitals,
                                               hoppings[j].orbitalHop[1], xSize, ySize);

            result[numOrbitals * j + hoppings[j].orbitalHop[0]] +=
                2 * hoppings[j].hoppingStrength * b[bIndex] - a[aIndex];
        }

        for (int j = 0; j < lattice.hoppings.size(); j++)
        {
            a[numOrbitals * i + hoppings[j].orbitalHop[0]] =
                result[numOrbitals * j + hoppings[j].orbitalHop[0]];
        }
    }

    return;
}

void DensityOfStates2dCpuStandard::ExecuteSecondKpmVectorUpdate()
{
    std::vector<double> result(numOrbitals * lattice.hoppings.size(), 0);

    #pragma unroll 4
    for (uint64_t i = 0; i < lattice.numberOfSites; i++)
    {
        std::fill(result.begin(), result.end(), 0.0);

        for (int j = 0; j < lattice.hoppings.size(); j++)
        {
            uint64_t bIndex = numOrbitals * i + hoppings[j].orbitalHop[0];
            uint64_t aIndex = GetMatrixElement(i, hoppings[j].latticeHop, numOrbitals,
                                               hoppings[j].orbitalHop[1], xSize, ySize);

            result[numOrbitals * j + hoppings[j].orbitalHop[0]] +=
                2 * hoppings[j].hoppingStrength * a[aIndex] - b[bIndex];
        }

        for (int j = 0; j < lattice.hoppings.size(); j++)
        {
            b[numOrbitals * i + hoppings[j].orbitalHop[0]] =
                result[numOrbitals * j + hoppings[j].orbitalHop[0]];
        }
    }

    return;
}

inline int DensityOfStates2dCpuStandard::GetMatrixElement(uint64_t& currentLatticePos,
                                                          std::vector<int32_t>& latticeHop,
                                                          uint32_t& numOfOrbitals,
                                                          uint32_t orbitalHop, uint32_t xSize,
                                                          uint32_t ySize)
{
    int elementShift = currentLatticePos;
    int y = currentLatticePos / xSize;
    int x = currentLatticePos % xSize;

    elementShift -= (x - mod(x + latticeHop[0], xSize));
    elementShift -= (y - mod(y + latticeHop[1], ySize)) * xSize;
    elementShift *= numOfOrbitals;
    elementShift += orbitalHop;

    return elementShift;
}