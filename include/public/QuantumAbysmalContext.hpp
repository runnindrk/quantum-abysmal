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

#ifndef QUANTUM_ABYSMAL_PUBLIC_CONTEXT_HPP
#define QUANTUM_ABYSMAL_PUBLIC_CONTEXT_HPP

#include "Lattice.hpp"
#include "KernelPolynomialMethod.hpp"
#include "LinearAlgebra.hpp"
#include "RngMethod.hpp"
#include "Plotting.hpp"
#include "Storage.hpp"

class QuantumAbysmalContext
{
public:
    using Uptr = std::unique_ptr<QuantumAbysmalContext>;

    /// @brief Get Lattice methods.
    /// @return A unique pointer to Lattice.
    virtual Lattice::Uptr GetLatticeMethods() = 0;

    /// @brief Get Kernel Polynomial Method methods.
    /// @return A unique pointer to KernalPolynomialMethod.
    virtual KernelPolynomialMethod::Uptr GetKpmMethods() = 0;

    /// @brief Get Linear Algebra methods.
    /// @return A unique pointer to LinearAlgebra.
    virtual LinearAlgebra::Uptr GetLinearAlgebraMethods() = 0;

    /// @brief Get Random Number Generator methods.
    /// @return A unique pointer to RandomNumberGenerator.
    virtual RngMethod::Uptr GetRngMethods() = 0;

    /// @brief Get Plots methods.
    /// @return A unique pointer to Plotting.
    virtual Plotting::Uptr GetPlotMethods() = 0;

    /// @brief Get Storage methods.
    /// @return A unique pointer to Storage.
    virtual Storage::Uptr GetStorageMethods() = 0;

    virtual ~QuantumAbysmalContext() = default;

};

#endif