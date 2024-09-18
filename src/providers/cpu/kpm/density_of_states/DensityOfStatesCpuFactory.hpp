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

#ifndef QUANTUM_ABYSMAL_SRC_DENSITY_OF_STATES_CPU_FACTORY_HPP
#define QUANTUM_ABYSMAL_SRC_DENSITY_OF_STATES_CPU_FACTORY_HPP

#include "include/public/DensityOfStates.hpp"
#include "include/public/QuantumAbysmalTypes.hpp"

#include "src/lattice/LatticeImpl.hpp"
#include "standard/DensityOfStates1d.hpp"
#include "standard/DensityOfStates2d.hpp"
#include "standard/DensityOfStates3d.hpp"

// #include "dummy/DensityOfStates1d.hpp"
// #include "dummy/DensityOfStates2d.hpp"
// #include "dummy/DensityOfStates3d.hpp"

class DensityOfStatesCpuFactory
{
public:
    static std::unique_ptr<DensityOfStates> GetInstance(ProviderImplementation implementation);
};

#endif