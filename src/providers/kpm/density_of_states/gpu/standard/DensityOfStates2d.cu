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

#include <chrono>

Error DensityOfStates2dGpuStandard::SetNumberOfRandomVectors(size_t numVectors)
{
    mNumRandomVectors = numVectors;
    return SUCCESS;
}

Error DensityOfStates2dGpuStandard::SetNumberOfMoments(size_t order)
{
    mNumOfMoments = order;
    return SUCCESS;
}

std::vector<double> DensityOfStates2dGpuStandard::Compute()
{
    // ------------------------------------------------------------------------
    // Output info.

    LOG_INFO << "Computing DoS from a 2D lattice on GPU/STANDARD!";
    LOG_INFO << "Number of lattice points : " << mLattice.latticeSize[0] << "x"
             << mLattice.latticeSize[1] << " = " << mLattice.numberOfSites;
    LOG_INFO << "Number of Orbitals : " << mLattice.numberOfOrbitals;
    LOG_INFO << "Number of Hoppings : " << (uint32_t)mLattice.numberOfHoppings;
    LOG_INFO << "Number of Moments  : " << mNumOfMoments;

    // ---------------------------------------------------------------------------------------------
    
    const unsigned int numBlocks = 256;
    const unsigned int numThreads = 256;

    // ---------------------------------------------------------------------------------------------
    // Memory Allocation.

    LatticeStructure* devLattice;
    cudaMalloc((void**)&devLattice, sizeof(LatticeStructure));
    cudaMemcpy(devLattice, &mLattice, sizeof(LatticeStructure), cudaMemcpyHostToDevice);

    double* devFirstReduction;
    cudaMalloc((void**)&devFirstReduction, numThreads * sizeof(double));
    cudaMemset(devFirstReduction, 0, numThreads * sizeof(double));

    double* devSecondReduction;
    cudaMalloc((void**)&devSecondReduction, numThreads * sizeof(double));
    cudaMemset(devSecondReduction, 0, numThreads * sizeof(double));

    double* devMoments;
    cudaMalloc((void**)&devMoments, mNumOfMoments * sizeof(double));
    cudaMemset(devMoments, 0, mNumOfMoments * sizeof(double));

    double* devA;
    cudaMalloc((void**)&devA, mLattice.hamiltonianSize * sizeof(double));
    cudaMemset(devA, 0, mLattice.hamiltonianSize * sizeof(double));

    double* devB;
    cudaMalloc((void**)&devB, mLattice.hamiltonianSize * sizeof(double));
    cudaMemset(devB, 0, mLattice.hamiltonianSize * sizeof(double));

    curandStateXORWOW* devStates;
    cudaMalloc((void**)&devStates, numThreads * numBlocks * sizeof(curandStateXORWOW));

    // ---------------------------------------------------------------------------------------------
    // Kernel execution logic.

    auto start = std::chrono::high_resolution_clock::now();

    InitCurandXorwow<<<numBlocks, numThreads>>>(devStates);
    InitRandomVector<<<numBlocks, numThreads>>>(devStates, devA, mLattice.hamiltonianSize);

    // Initialize
    Reduce<numThreads, true>
        <<<numBlocks, numThreads>>>(devA, devB, *devLattice, devFirstReduction, devSecondReduction);
    FinalReduce<numThreads>
        <<<1, numThreads>>>(devFirstReduction, devSecondReduction, devMoments, 0);

    // Compute
    for (int i = 0; i < mNumOfMoments / 2 - 1; i++)
    {
        Reduce<numThreads, false>
            <<<numBlocks, numThreads>>>((i % 2 == 0) ? devA : devB, (i % 2 == 0) ? devB : devA,
                                        *devLattice, devFirstReduction, devSecondReduction);
        FinalReduce<numThreads>
            <<<1, numThreads>>>(devFirstReduction, devSecondReduction, devMoments, i + 1);
    }

    double* moments = (double*)malloc(mNumOfMoments * sizeof(double));
    cudaMemcpy(moments, devMoments, mNumOfMoments * sizeof(double), cudaMemcpyDeviceToHost);

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    LOG_INFO << "Computation time : " << elapsed.count() << " seconds.";

    // for (int i = 0; i < mNumOfMoments; i++)
    // {
    //     LOG_INFO << moments[i];
    // }

    std::vector<double> momentsToReturn;
    momentsToReturn.assign(moments, moments + mNumOfMoments);
    return momentsToReturn;
}

// -------------------------------------------------------------------------------------------------
// Private

__global__ void InitCurandXorwow(curandStateXORWOW* state)
{
    int tid = threadIdx.x + blockIdx.x * blockDim.x;
    curand_init(clock64(), tid, 0, &state[tid]);
}

__global__ void InitRandomVector(curandStateXORWOW* state, double* buffer, unsigned int bufferSize)
{
    int tid = threadIdx.x + blockIdx.x * blockDim.x;
    curandStateXORWOW localState = state[tid];
    int localTid = tid;

    while (tid < bufferSize)
    {
        buffer[tid] = curand_normal_double(&localState) / sqrt(double(bufferSize));

        state[localTid] = localState;
        tid += blockDim.x * gridDim.x;
    }
}
