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

#ifndef QUANTUM_ABYSMAL_SRC_PLOT_DOS_HPP
#define QUANTUM_ABYSMAL_SRC_PLOT_DOS_HPP

#include "public/ErrorHandler.hpp"
#include "internal/Logger.hpp"

#include <vector>

class PlotDensityOfStatesImpl
{
    public:
    
    Result<void> Plot(std::vector<std::array<double, 2>> DensityOfStates);
};

#endif