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

#ifndef QUANTUM_ABYSMAL_SRC_KERNEL_POLYNOMIAL_METHOD_FACTORY_HPP
#define QUANTUM_ABYSMAL_SRC_KERNEL_POLYNOMIAL_METHOD_FACTORY_HPP

#include "include/public/KernelPolynomialMethod.hpp"

#include "kpm/density_of_states/cpu/standard/DensityOfStates1d.hpp"
#include "kpm/density_of_states/cpu/standard/DensityOfStates2d.hpp"
#include "kpm/density_of_states/cpu/standard/DensityOfStates3d.hpp"
#include "kpm/density_of_states/gpu/standard/DensityOfStates1d.hpp"
#include "kpm/density_of_states/gpu/standard/DensityOfStates2d.hpp"
#include "kpm/density_of_states/gpu/standard/DensityOfStates3d.hpp"

#include "src/lattice/LatticeImpl.hpp"

class KernelPolynomialMethodFactory : public KernelPolynomialMethod
{
  public:
    DensityOfStates::Uptr CreateDoSCtx(ProviderImplementation implementation) override;

    LocalDensityOfStates::Uptr CreateLDoSCtx(ProviderImplementation implementation) override;

    StochasticLocalDensityOfStates::Uptr
    CreateStochasticLDoSCtx(ProviderImplementation implementation) override;
};

#endif