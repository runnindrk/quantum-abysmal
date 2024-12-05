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
    std::cout << "Quantum Abysmal PoC-Rng.\n\n";

    // ------------------------------------------------------------------------
    // Context.

    auto abysmalCtx = GetQuantumAbysmalContext();

    // ------------------------------------------------------------------------
    // Random Number Generation.

    auto rngCtx = abysmalCtx->GetRngMethods()->CreateRngCtx(CPU_STANDARD_IMPL);
    auto res = rngCtx->GetRandomVector(16384);

    for (int i = 0; i < res.size(); i++)
    {
        std::cout << res[i] << std::endl;
    }

    return 0;
}