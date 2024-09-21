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

#ifndef QUANTUM_ABYSMAL_SRC_LATTICE_IMPL_HPP
#define QUANTUM_ABYSMAL_SRC_LATTICE_IMPL_HPP

#include "include/internal/Logger.hpp"
#include "include/public/Lattice.hpp"
#include "util/Conversions.hpp"

#include <unordered_set>

class LatticeImpl : public Lattice
{
  public:
    // Singleton for thread safety access
    static LatticeImpl& GetInstance();

    // ------------------------------------------------------------------------
    // Lattice Struct

    typedef struct
    {
        std::vector<int32_t> latticeHop;
        std::array<uint32_t, 2> orbitalHop;
        double hoppingStrength;

    } Hopping;

    typedef struct
    {
        uint32_t dimension{};
        uint32_t numberOfOrbitals{};
        uint64_t numberOfSites{1};
        uint64_t hamiltonianSize{1};
        std::vector<uint32_t> latticeSize{};
        std::vector<Hopping> hoppings{};

        double minEnergy{};
        double maxEnergy{};
        double energyScaling{};
        double energyShift{};

        BoundaryType boundaryType;

    } Lattice;

    // ------------------------------------------------------------------------
    // Override of the Public Interface

    Error AddHopping(std::vector<int32_t> latticeHop, std::array<char, 2> orbitalHop,
                    double hoppingStrength) override;
    Error SetLatticeSize(std::vector<uint32_t> lateralSizes) override;
    Error SetEnergyRange(double minEnergy, double maxEnergy) override;
    Error SetBoundaryType(BoundaryType boundaryType) override;

    // ------------------------------------------------------------------------
    // Internal methods.

    Lattice GetLattice();
    Error PrintLatticeInformation();

  private:
    // ------------------------------------------------------------------------
    // Private methods.

    // ------------------------------------------------------------------------
    // Private member variables.

    bool mIsAddHoppingCalled{false};
    bool mIsAnyOtherFunctionCalled{false};
    std::unordered_set<char> mOrbitalsSet;
    static Lattice mLattice;
};

#endif