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
#include "src/lattice/LatticeImpl.hpp"

#include "TApplication.h"
#include "TCanvas.h"
#include "TGraph.h"

#include <cmath>

// NOTE: All of this is provisionary. Only to have the first plots.

double chebyshevPolynomial(int n, double x)
{
    return cos(n * acos(x));
}

double jacksonKernel(int n, int N)
{
    double factor1 = (N - n + 1) * cos(M_PI * n / (N + 1));
    double factor2 = sin(M_PI * n / (N + 1)) * cos(M_PI / (N + 1)) / sin(M_PI / (N + 1));
    return (factor1 + factor2) / (N + 1);
}

Error PlotDensityOfStatesImpl::Plot(std::vector<double> moments)
{
    LOG_INFO << "Hello from DoS plot!";

    int fakeArgc = 0;
    char* fakeArgv[] = {(char*)""};

    TApplication theApp("App", &fakeArgc, fakeArgv);

    LatticeStructure lattice = LatticeImpl::GetInstance().GetLattice();

    double dos[200000];
    double energy[200000];

    for (int i = 0; i < 200000; i++)
    {
        double E = -0.999 + i * (2 * 0.999) / (200000 - 1);

        for (int j = 0; j < moments.size(); j++)
        {
            if (j == 0)
            {
                dos[i] += moments[j];
            }

            else
            {
                dos[i] +=
                    2 * moments[j] * jacksonKernel(j, moments.size()) * chebyshevPolynomial(j, E);
            }
        }

        dos[i] *= 1 / (3.141592 * (sqrt(1 - E * E)));

        energy[i] = E * lattice.energyScaling + lattice.energyShift;
        dos[i] = dos[i] / lattice.energyScaling + lattice.energyShift;
    }

    auto g = new TGraph(200000, energy, dos);
    g->SetTitle("Graph title;X title;Y title");

    TCanvas* c = new TCanvas("c", "Canvas", 800, 600);
    g->Draw("AL");

    theApp.Run();

    return SUCCESS;
}