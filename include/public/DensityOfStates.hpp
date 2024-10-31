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

#ifndef QUANTUM_ABYSMAL_PUBLIC_DENSITY_OF_STATES_HPP
#define QUANTUM_ABYSMAL_PUBLIC_DENSITY_OF_STATES_HPP

#include "ErrorHandler.hpp"
#include "QuantumAbysmalTypes.hpp"

#include <memory>
#include <vector>

class DensityOfStates
{
  public:
    using Uptr = std::unique_ptr<DensityOfStates>;

    /// @brief Set the number of random vectors to use for the DoS computation.
    /// @param numVectors The number of random vectors.
    virtual Error SetNumberOfRandomVectors(size_t numVectors) = 0;

    /// @brief Set the number of polynomials for the KPM.
    /// @param order The polynomial order.
    virtual Error SetNumberOfMoments(size_t order) = 0;

    /// @brief Perform the DoS moments computation.
    virtual std::vector<double> Compute() = 0;

    /// @brief Retrieve the computed moments.
    /// @return A vector containing the moments values.
    // virtual std::vector<double> GetMoments() const = 0;

    /// @brief Retrieve the computed DoS.
    /// @return A vector containing the DoS values.
    // virtual std::vector<double> CreateDoSCtx(size_t numPoints) const = 0;

    /// @brief Save the computed moments.
    // virtual Error SaveMoments() const = 0;

    /// @brief Save the computed DoS.
    // virtual Error SaveDoS() const = 0;

    /// @brief Plot the computed DoS.
    // virtual Error PlotDoS() const = 0;

    virtual ~DensityOfStates() = default;
};

#endif