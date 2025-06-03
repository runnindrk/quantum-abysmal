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

#include "include/public/DensityOfStates.hpp"
#include "src/lattice/LatticeImpl.hpp"

class DensityOfStates2dCpuStandard : public DensityOfStates
{
    // --------------------------------------------------------------------------------------------
    //

    public:

    DensityOfStates2dCpuStandard();
    ~DensityOfStates2dCpuStandard();

    Result<void> SetDomainDecomposition(std::vector<uint32_t> numDomains) override;
    Result<void> SetNumberOfRandomVectors(size_t numVectors) override;
    Result<void> SetNumberOfMoments(size_t order) override;
    Result<std::vector<double>> ComputeMoments() override;
    Result<std::vector<std::array<double, 2>>> ComputeDoS(uint32_t numPoints) override;
    Result<void> Save() override;
    Result<void> PlotDoS() override;

    // --------------------------------------------------------------------------------------------
    // Private

    private:

    void InitializeKpmVectors(double* a, double* b);
    void ExecuteKpmVectorUpdate(double* a, double* b);
    inline void UpdateGhosts(double* a);

    // --------------------------------------------------------------------------------------------
    // Member variables

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

    // Parallelization variables. 
    uint32_t xDomainDecomposition{1};
    uint32_t yDomainDecomposition{1};
    uint32_t NUM_THREADS{};

    // Have to think how to handle disorder. (maybe a flag on LatticeMethods)
    // (a flag is wrong, the lattice should allways have the same disorder)
    // (different disorders == different lattices)
};

#endif