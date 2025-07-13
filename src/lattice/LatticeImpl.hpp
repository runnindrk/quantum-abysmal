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

#include "public/Lattice.hpp"
#include "LatticeEngineImpl.hpp"

class LatticeImpl : public Lattice
{
    public:

    Result<void> AddHopping(std::vector<int32_t> latticeHop, std::array<char, 2> orbitalHop, double hoppingStrength) override;
    Result<void> SetLatticeSize(std::vector<uint32_t> lateralSizes) override;
    Result<void> SetEnergyRange(double minEnergy, double maxEnergy) override;
    Result<void> SetBoundaryType(BoundaryType boundaryType) override;
};

#endif