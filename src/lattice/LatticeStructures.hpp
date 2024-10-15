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

#include "include/public/QuantumAbysmalTypes.hpp"

// -------------------------------------------------------------------------------------------------
// Host Structures

typedef struct
{
    std::vector<int32_t> latticeHop;
    std::array<uint32_t, 2> orbitalHop;
    double hoppingStrength;

} HostHopping;

typedef struct
{
    uint32_t dimension{};
    uint32_t numberOfOrbitals{};
    uint64_t numberOfSites{1};
    uint64_t hamiltonianSize{1};
    std::vector<uint32_t> latticeSize{};
    std::vector<HostHopping> hoppings{};

    double minEnergy{};
    double maxEnergy{};
    double energyScaling{};
    double energyShift{};

    BoundaryType boundaryType;

} HostLattice;

// -------------------------------------------------------------------------------------------------
// Device Structures

typedef struct
{
    int32_t* latticeHop;
    uint32_t orbitalHop[2];
    double hoppingStrength;

} DeviceHopping;

typedef struct
{
    uint32_t dimension{};
    uint32_t numberOfOrbitals{};
    uint64_t numberOfSites{1};
    uint64_t hamiltonianSize{1};
    uint32_t* latticeSize{};
    DeviceHopping* hoppings;

    double minEnergy{};
    double maxEnergy{};
    double energyScaling{};
    double energyShift{};

    BoundaryType boundaryType;

} DeviceLattice;