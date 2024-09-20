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

class LatticeImpl : public Lattice
{
  public:
    // Singleton for thread safety access
    static LatticeImpl& GetInstance();

    // ------------------------------------------------------------------------
    // Lattice Struct

    typedef struct
    {
        uint32_t dimension{};
        uint32_t numberOfOrbitals{};
        uint64_t numberOfSites{};
        uint64_t hamiltonianSize{};
        std::vector<uint32_t> latticeSize{};
        std::vector<Hopping> hoppings{};

        double minEnergy{};
        double maxEnergy{};
        double energyScaling{};
        double energyShift{};

    } Lattice;

    // ------------------------------------------------------------------------
    // Override of the Public Interface

    void AddHopping(Hopping hopping) override;
    void SetLatticeSize(std::vector<uint32_t> lateralSizes) override;
    void SetEnergyRange(double minEnergy, double maxEnergy) override;
    void SetBoundaryType(BoundaryType boundaryType) override;

    // ------------------------------------------------------------------------
    // Internal methods.

    Lattice GetLattice();

  private:
    // ------------------------------------------------------------------------
    // Private methods.

    void FinalizeLattice();

    // ------------------------------------------------------------------------
    // Private member variables.

    static Lattice lattice;
};

#endif