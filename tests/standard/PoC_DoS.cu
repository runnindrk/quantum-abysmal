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
    std::cout << "Quantum Abysmal PoC-DoS.\n\n";

    // ------------------------------------------------------------------------
    // Context.

    auto abysmalCtx = GetQuantumAbysmalContext();

    // ------------------------------------------------------------------------
    // Lattice construction.

    auto latticeCtx = abysmalCtx->GetLatticeMethods();

    // Graphene Model
    // latticeCtx->AddHopping({1, 0}, {'A', 'B'}, -1);
    // latticeCtx->AddHopping({0, 1}, {'A', 'B'}, -1);
    // latticeCtx->AddHopping({0, 0}, {'A', 'B'}, -1);

    // BBH Model
    // double delta = 0;
    // double gamma = -0.5;

    // latticeCtx->AddHopping({0, 0}, {'A', 'A'}, delta);
    // latticeCtx->AddHopping({0, 0}, {'B', 'B'}, delta);
    // latticeCtx->AddHopping({0, 0}, {'C', 'D'}, -delta);
    // latticeCtx->AddHopping({0, 0}, {'D', 'D'}, -delta);

    // latticeCtx->AddHopping({1, 0}, {'C', 'A'}, 1);
    // latticeCtx->AddHopping({1, 0}, {'B', 'D'}, 1);
    // latticeCtx->AddHopping({0, 1}, {'D', 'A'}, 1);
    // latticeCtx->AddHopping({0, 1}, {'B', 'C'}, -1);

    // latticeCtx->AddHopping({0, 0}, {'C', 'A'}, gamma);
    // latticeCtx->AddHopping({0, 0}, {'B', 'D'}, gamma);
    // latticeCtx->AddHopping({0, 0}, {'D', 'A'}, gamma);
    // latticeCtx->AddHopping({0, 0}, {'B', 'C'}, -gamma);

    // latticeCtx->SetLatticeSize({4096, 4096});
    // latticeCtx->SetEnergyRange(-3, 3);
    // latticeCtx->SetBoundaryType(PERIODIC);

    // 1D TB Model
    latticeCtx->AddHopping({1}, {'A', 'A'}, -1);

    latticeCtx->SetLatticeSize({4194304});
    latticeCtx->SetEnergyRange(-2, 2);
    latticeCtx->SetBoundaryType(PERIODIC);

    // ------------------------------------------------------------------------
    // Density of States.

    auto kpmCtx = abysmalCtx->GetKpmMethods();
    auto dosCtx = kpmCtx->CreateDoSCtx(GPU_STANDARD_IMPL);

    dosCtx->SetNumberOfRandomVectors(1);
    dosCtx->SetNumberOfMoments(131072);
    std::vector<double> moments = dosCtx->Compute();

    // for (int i = 0; i < moments.size(); i++)
    // {
    //    std::cout << moments[i] << std::endl;
    // }

    // ------------------------------------------------------------------------
    // Plot DoS

    // auto plottingCtx = abysmalCtx->GetPlotMethods();
    // plottingCtx->PlotDensityOfStates(moments);

    // ------------------------------------------------------------------------

    return 0;
}