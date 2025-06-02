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

#ifndef QUANTUM_ABYSMAL_SRC_DENSITY_OF_STATES_2D_GPU_STANDARD_HPP
#define QUANTUM_ABYSMAL_SRC_DENSITY_OF_STATES_2D_GPU_STANDARD_HPP

#include "include/internal/Logger.hpp"
#include "include/public/DensityOfStates.hpp"
#include "src/lattice/LatticeImpl.hpp"
#include "src/providers/cuda_provider/rng/RngEngineImpl.hpp"
#include "util/Util.hpp"

#include <curand_kernel.h>

class DensityOfStates2dGpuStandard : public DensityOfStates
{
    public:

    DensityOfStates2dGpuStandard();
    ~DensityOfStates2dGpuStandard();

    Result<void> SetDomainDecomposition(std::vector<uint32_t> numDomains) override;
    Result<void> SetNumberOfRandomVectors(size_t numVectors) override;
    Result<void> SetNumberOfMoments(size_t order) override;
    Result<std::vector<double>> ComputeMoments() override;
    Result<std::vector<double>> ComputeDoS(uint32_t numPoints) override;
    Result<void> Save() override;
    Result<void> PlotDoS() override;

    // ============================================================================================
    // CUDA Device Functions

    struct KpmSparseInit
    {
        __device__ void operator()(uint64_t tid, double* a, double* b, LatticeStructure& lattice, double* sFirstReduceData,
                                   double* sSecondReduceData) const;
    };

    struct KpmSparse
    {
        __device__ void operator()(uint64_t tid, double* a, double* b, LatticeStructure& lattice, double* sFirstReduceData,
                                   double* sSecondReduceData) const;
    };

    private:

    // ============================================================================================
    // Private methods.

    // ============================================================================================
    // Member variables.

    // Kpm variables.
    size_t mNumRandomVectors{};
    size_t mNumOfMoments{};
    std::vector<double> mMoments;
    std::vector<std::array<double, 2>> mDoS;

    // Lattice variables.
    LatticeStructure mLattice{};
    uint32_t numOrbitals{};
    uint32_t xSize{};
    uint32_t ySize{};
    uint32_t xGhostedSize{};
    uint32_t yGhostedSize{};
    uint32_t numberOfGhosts{};

    // CUDA pointers.
    LatticeStructure* dLattice;
    double* dMoments;
    uint32_t dMomentsSize;
};

#endif