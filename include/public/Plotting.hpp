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

#ifndef QUANTUM_ABYSMAL_PUBLIC_PLOTTING_HPP
#define QUANTUM_ABYSMAL_PUBLIC_PLOTTING_HPP

#include "ErrorHandler.hpp"
#include "QuantumAbysmalTypes.hpp"

#include <memory>
#include <vector>

class Plotting
{
  public:
    using Uptr = std::unique_ptr<Plotting>;

    // In the distant future, this will change because there are several methods for the DoS
    /// @brief Plot the Density of States.
    /// @param moments Kernel Polynomial Method moments.
    virtual Error PlotDensityOfStates(std::vector<double> moments) = 0;

    virtual ~Plotting() = default;
};

#endif