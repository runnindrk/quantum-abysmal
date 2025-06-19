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

#include "PlotDensityOfStatesImpl.hpp"
#include "src/lattice/LatticeImpl.hpp"

#include "TApplication.h"
#include "TCanvas.h"
#include "TGraph.h"

// ------------------------------------------------------------------------------------------------
// NOTE: All of this is provisionary. Only to have the first plots.

Result<void> PlotDensityOfStatesImpl::Plot(std::vector<std::array<double, 2>> DensityOfStates)
{
    LOG_INFO << "Hello from DoS plot!";

    int fakeArgc = 0;
    char* fakeArgv[] = {(char*)""};
    TApplication theApp("App", &fakeArgc, fakeArgv);

    // Extract energies and dos into separate vectors
    std::vector<double> energies;
    std::vector<double> dosValues;
    energies.reserve(DensityOfStates.size());
    dosValues.reserve(DensityOfStates.size());

    for (const auto& pair : DensityOfStates)
    {
        energies.push_back(pair[0]);
        dosValues.push_back(pair[1]);
    }

    auto g = new TGraph(DensityOfStates.size(), energies.data(), dosValues.data());
    g->SetTitle("Graph title;Energy;Density of States");

    TCanvas* c = new TCanvas("c", "Canvas", 800, 600);
    g->Draw("AL");

    theApp.Run(kTRUE);

    return Result<void>::SetError(SUCCESS);
}
