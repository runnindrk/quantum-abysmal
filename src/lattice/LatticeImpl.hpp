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

#ifndef QUANTUM_ABYSMAL_SRC_LATTICE_IMPL_HPP
#define QUANTUM_ABYSMAL_SRC_LATTICE_IMPL_HPP

#include "include/internal/Logger.hpp"
#include "include/public/Lattice.hpp"
#include "codegen/Codegen_LatticeStructure.hpp"
#include "util/Conversions.hpp"

#include <unordered_set>
#include <array>

class LatticeImpl : public Lattice
{
    public:

    // Singleton for thread safety access
    static LatticeImpl& GetInstance();

    Result<void> AddHopping(std::vector<int32_t> latticeHop, std::array<char, 2> orbitalHop,
                    double hoppingStrength) override;
    Result<void> SetLatticeSize(std::vector<uint32_t> lateralSizes) override;
    Result<void> SetEnergyRange(double minEnergy, double maxEnergy) override;
    Result<void> SetBoundaryType(BoundaryType boundaryType) override;

    // ------------------------------------------------------------------------
    // Internal methods.

    LatticeStructure& GetLattice();

    private:
    
    // ------------------------------------------------------------------------
    // Private methods.

    // ------------------------------------------------------------------------
    // Private member variables.

    std::unordered_set<char> mOrbitalsSet;
    
    // A copy of the original lattice structure to reset values.
    static LatticeStructure originalLattice;

    // The current lattice structure.
    static LatticeStructure mLattice;
};

#endif