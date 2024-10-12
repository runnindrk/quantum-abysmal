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

#ifndef QUANTUM_ABYSMAL_PUBLIC_KERNEL_POLYNOMIAL_METHOD_HPP
#define QUANTUM_ABYSMAL_PUBLIC_KERNEL_POLYNOMIAL_METHOD_HPP

#include "DensityOfStates.hpp"
#include "LocalDensityOfStates.hpp"
#include "StochasticLocalDensityOfStates.hpp"
#include "QuantumAbysmalTypes.hpp"

#include <memory>

class KernelPolynomialMethod
{
  public:
    using Uptr = std::unique_ptr<KernelPolynomialMethod>;

    /// @brief Get Density of States.
    /// @return A unique pointer to Density of States.
    virtual DensityOfStates::Uptr
    GetDensityOfStates(ProviderImplementation implementation) = 0;

    /// @brief Get Local Density of States.
    /// @return A unique pointer to Local Density of States.
    virtual LocalDensityOfStates::Uptr
    GetLocalDensityOfStates(ProviderImplementation implementation) = 0;

    /// @brief Get Local Density of States.
    /// @return A unique pointer to Local Density of States.
    virtual StochasticLocalDensityOfStates::Uptr GetStochasticLocalDensityOfStates(
        ProviderImplementation implementation) = 0;
};

#endif