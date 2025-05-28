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

#ifndef QUANTUM_ABYSMAL_SRC_DENSITY_OF_STATES_2D_CPU_STANDARD_HPP
#define QUANTUM_ABYSMAL_SRC_DENSITY_OF_STATES_2D_CPU_STANDARD_HPP

#include "include/internal/Logger.hpp"
#include "include/public/DensityOfStates.hpp"
#include "src/lattice/LatticeImpl.hpp"
#include "src/storage/StorageEngineImpl.hpp"

class DensityOfStates2dCpuStandard : public DensityOfStates
{
  public:
    Error SetNumberOfRandomVectors(size_t numVectors) override;
    Error SetNumberOfMoments(size_t order) override;
    std::vector<double> Compute() override;
    Error Save() const override;

  private:
    // ------------------------------------------------------------------------
    // Private methods.

    void InitializeKpmVectors();
    void ExecuteKpmVectorUpdate(double* a, double* b);
    inline void UpdateGhosts(double* a);

    // ------------------------------------------------------------------------
    // Member variables.

    size_t mNumRandomVectors{};
    size_t mNumOfMoments{};

    LatticeStructure lattice = LatticeImpl::GetInstance().GetLattice();
    uint32_t numOrbitals = lattice.numberOfOrbitals;
    uint32_t xSize = lattice.latticeSize[0];
    uint32_t ySize = lattice.latticeSize[1];
    uint32_t xGhostedSize = xSize + 2;
    uint32_t yGhostedSize = ySize + 2;

    int numberOfGhosts = numOrbitals * (2 * xSize + 2 * ySize + 4);

    double* a = (double*)malloc((lattice.hamiltonianSize + numberOfGhosts) * sizeof(double));
    double* b = (double*)malloc((lattice.hamiltonianSize + numberOfGhosts) * sizeof(double));

    std::vector<double> moments;

    // Have to think how to handle disorder. (maybe a flag on LatticeMethods)
    // (a flag is wrong, the lattice should allways have the same disorder)
    // (different disorders == different lattices)

    // Domain decomposition is needed for better perfomance.
    // Always max out threads internaly?.
};

#endif