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

#include "PlotDensityOfStatesImpl.hpp"

#include <TROOT.h>
#include "TApplication.h"
#include "TCanvas.h"
#include "TGraph.h"

#include <cmath>

Error PlotDensityOfStatesImpl::Plot(std::vector<double> moments)
{
    LOG_INFO << "Hello from DoS plot!";

    int fakeArgc = 0;
    char* fakeArgv[] = {(char*)""};

    TApplication theApp("App", &fakeArgc, fakeArgv);

    double x[100], y[100];
    int n = 20;

    for (int i = 0; i < n; i++)
    {
        x[i] = i * 0.1;
        y[i] = 10 * sin(x[i] + 0.2);
    }

    auto g = new TGraph(n, x, y);
    g->SetTitle("Graph title;X title;Y title");

    TCanvas *c = new TCanvas("c", "Canvas", 800, 600);
    g->Draw("AC*");

    theApp.Run();

    return SUCCESS;
}