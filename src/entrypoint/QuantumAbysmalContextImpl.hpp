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

#ifndef QUANTUM_ABYSMAL_SRC_CONTEXT_IMPL_HPP
#define QUANTUM_ABYSMAL_SRC_CONTEXT_IMPL_HPP

#include "public/QuantumAbysmalContext.hpp"

class QuantumAbysmalContextImpl : public QuantumAbysmalContext
{
public:
    using Uptr = std::unique_ptr<QuantumAbysmalContextImpl>;

    /// @brief Get Lattice methods.
    /// @return A unique pointer to Lattice.
    Lattice::Uptr GetLatticeMethods() override;

    /// @brief Get Kernel Polynomial Method methods.
    /// @return A unique pointer to KernalPolynomialMethod.
    KernelPolynomialMethod::Uptr GetKpmMethods() override;
    
    /// @brief Get Linear Algebra methods.
    /// @return A unique pointer to LinearAlgebra.
    LinearAlgebra::Uptr GetLinearAlgebraMethods() override;

    /// @brief Get Random Number Generator methods.
    /// @return A unique pointer to RandomNumberGenerator.
    RngMethod::Uptr GetRngMethods() override;

    /// @brief Get Plots methods.
    /// @return A unique pointer to Plotting.
    Plotting::Uptr GetPlotMethods() override;

    /// @brief Get Storage methods.
    /// @return A unique pointer to Storage.
    Storage::Uptr GetStorageMethods() override;

    virtual ~QuantumAbysmalContextImpl() = default;
};

#endif