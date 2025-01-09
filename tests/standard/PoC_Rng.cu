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

extern "C" 
{
#include "TestU01.h"
}

// -------------------------------------------------------------------------------------------------
// We shall test the quality of the RNG

int batchSize = 2 << 20;
int batchCount = 0;
std::vector<unsigned int> batchRandomNumbers(batchSize);

// Constant Context
auto abysmalCtx = GetQuantumAbysmalContext();
auto rngCtx = abysmalCtx->GetRngMethods()->CreateRngCtx(GPU_STANDARD_IMPL);

unsigned int CustomRng()
{
    if (batchCount % batchSize == 0)
    {
        batchRandomNumbers = rngCtx->GetRandomBitsVector(batchSize);
        batchCount = 0;
    }

    batchCount += 1;
    return batchRandomNumbers[batchCount - 1];
}

// -------------------------------------------------------------------------------------------------

int main()
{
    unif01_Gen* gen = unif01_CreateExternGenBits("Custom CUDA RNG", CustomRng);
    
    // bbattery_SmallCrush(gen);
    bbattery_Crush(gen);
    // bbattery_BigCrush(gen);
    
    unif01_DeleteExternGenBits(gen);

    return 0;
}