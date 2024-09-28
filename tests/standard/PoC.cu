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

#include "../include/public/Entrypoint.hpp"

#include <iostream>
#include <vector>

int main()
{
    std::cout << "Quantum Abysmal PoC.\n\n";

    // ------------------------------------------------------------------------
    // Context.

    auto abysmalCtx = GetQuantumAbysmalContext();

    // ------------------------------------------------------------------------
    // Lattice construction.
    
    auto latticeCtx = abysmalCtx->GetLatticeMethods();

    latticeCtx->AddHopping({1, 0}, {'A', 'B'}, -1);
    latticeCtx->AddHopping({0, 1}, {'A', 'B'}, -1);
    latticeCtx->AddHopping({0, 0}, {'A', 'B'}, -1);

    latticeCtx->SetLatticeSize({2048, 2048});
    latticeCtx->SetEnergyRange(-3.1, 3.1);
    latticeCtx->SetBoundaryType(PERIODIC);

    // ------------------------------------------------------------------------
    // Density of States.

    auto kpmCtx = abysmalCtx->GetKpmMethods();
    auto dosCtx = kpmCtx->GetDensityOfStates();

    dosCtx->SetNumberOfRandomVectors(1);
    dosCtx->SetNumberOfMoments(512);
    std::vector<double> moments = dosCtx->Compute();

    // ------------------------------------------------------------------------
    // Print moments

    // std::cout << "Number of moments = " << moments.size() << std::endl;
    
    // for (int i = 0; i < moments.size(); i++)
    // {
    //     std::cout << moments[i] << std::endl;
    // }

    // ------------------------------------------------------------------------
    // Plot DoS

    auto plottingCtx = abysmalCtx->GetPlotMethods();

    plottingCtx->PlotDensityOfStates(moments);
    
    // ------------------------------------------------------------------------

    return 0;
}