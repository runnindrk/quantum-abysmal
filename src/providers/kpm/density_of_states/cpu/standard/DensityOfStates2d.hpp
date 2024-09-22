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

#ifndef QUANTUM_ABYSMAL_SRC_DENSITY_OF_STATES_2D_CPU_STANDARD_HPP
#define QUANTUM_ABYSMAL_SRC_DENSITY_OF_STATES_2D_CPU_STANDARD_HPP

#include "include/internal/Logger.hpp"
#include "include/public/DensityOfStates.hpp"
#include "src/lattice/LatticeImpl.hpp"

class DensityOfStates2dCpuStandard : public DensityOfStates
{
  public:
    Error SetNumberOfRandomVectors(size_t numVectors) override;
    Error SetNumberOfMoments(size_t order) override;
    Error Compute() override;

  private:
    // ------------------------------------------------------------------------
    // Private methods.

    void InitializeKpmVectors();
    void ExecuteFirstKpmVectorUpdate();
    void ExecuteSecondKpmVectorUpdate();

    int GetMatrixElement(uint64_t& currentLatticePos, std::vector<int32_t>& latticeHop,
                         uint32_t& numOfOrbitals, uint32_t orbitalHop, uint32_t xSize,
                         uint32_t ySize);

    // Helper function for development. To be removed in the future.
    void PrintVector();

    // ------------------------------------------------------------------------
    // Member variables.

    size_t mNumRandomVectors{};
    size_t mNumOfMoments{};
    
    LatticeImpl::Lattice lattice = LatticeImpl::GetInstance().GetLattice();
    uint32_t xSize = lattice.latticeSize[0];
    uint32_t ySize = lattice.latticeSize[1];
    uint32_t numOrbitals = lattice.numberOfOrbitals;
    std::vector<LatticeImpl::Hopping> hoppings = lattice.hoppings;

    double* a = (double*)malloc(lattice.hamiltonianSize * sizeof(double));
    double* b = (double*)malloc(lattice.hamiltonianSize * sizeof(double));

    // Have to think how to handle disorder. (maybe a flag on LatticeMethods)
    // (a flag is wrong, the lattice should allways have the same disorder)
    // (different disorders == different lattices)

    // Domain decomposition is needed for better perfomance.
    // Always max out threads internaly?.
};

#endif