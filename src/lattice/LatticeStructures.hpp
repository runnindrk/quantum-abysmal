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

struct Hopping
{
    int8_t latticeHop[3];
    uint8_t orbitalHop[2];
    double hoppingStrength{};
};

struct LatticeStructure
{
    uint32_t dimension{};
    uint32_t numberOfOrbitals{};
    uint32_t latticeSize[3] = {};
    uint64_t numberOfSites{1};
    uint64_t hamiltonianSize{1};
    
    Hopping hoppings[128] = {};
    uint8_t numberOfHoppings{};

    double minEnergy{};
    double maxEnergy{};
    double energyScaling{};
    double energyShift{};

    BoundaryType boundaryType{};
};