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

#ifndef QUANTUM_ABYSMAL_PUBLIC_DENSITY_OF_STATES_HPP
#define QUANTUM_ABYSMAL_PUBLIC_DENSITY_OF_STATES_HPP

#include "ErrorHandler.hpp"
#include "QuantumAbysmalTypes.hpp"

class DensityOfStates
{
    public:

    using Uptr = std::unique_ptr<DensityOfStates>;

    /// @brief Set the number of random vectors to use for the DoS computation.
    /// @param numVectors The number of random vectors.
    virtual Result<void> SetDomainDecomposition(std::vector<uint32_t> numDomains) = 0;

    /// @brief Set the number of random vectors to use for the DoS computation.
    /// @param numVectors The number of random vectors.
    virtual Result<void> SetNumberOfRandomVectors(size_t numVectors) = 0;

    /// @brief Set the number of polynomials for the KPM.
    /// @param order The polynomial order.
    virtual Result<void> SetNumberOfMoments(size_t order) = 0;

    /// @brief Perform the DoS moments computation.
    virtual Result<std::vector<double>> ComputeMoments() = 0;

    /// @brief Perform the DoS moments computation.
    virtual Result<std::vector<double>> ComputeDoS(uint32_t numPoints) = 0;

    /// @brief Save the computed DoS.
    virtual Result<void> Save() = 0;

    /// @brief Plot the computed DoS.
    virtual Result<void> PlotDoS() = 0;

    virtual ~DensityOfStates() = default;
};

#endif