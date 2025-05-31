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

#ifndef QUANTUM_ABYSMAL_SRC_DENSITY_OF_STATES_1D_CPU_STANDARD_HPP
#define QUANTUM_ABYSMAL_SRC_DENSITY_OF_STATES_1D_CPU_STANDARD_HPP

#include "include/internal/Logger.hpp"
#include "include/public/DensityOfStates.hpp"
#include "src/lattice/LatticeImpl.hpp"

class DensityOfStates1dCpuStandard : public DensityOfStates
{
    public:

    Result<void> SetDomainDecomposition(std::vector<uint32_t> numDomains) override;
    Result<void> SetNumberOfRandomVectors(size_t numVectors) override;
    Result<void> SetNumberOfMoments(size_t order) override;
    Result<std::vector<double>> ComputeMoments() override;
    Result<std::vector<double>> ComputeDoS(uint32_t numPoints) override;
    Result<void> Save() override;
    Result<void> PlotDoS() override;

  private:

};

#endif