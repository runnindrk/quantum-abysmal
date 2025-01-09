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

#include "RngImpl.hpp"

RngCpuStandard::RngCpuStandard()
{
}

std::vector<double> RngCpuStandard::GetRandomVector(unsigned int size)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<> dist(0.0, pow(2.0 / 3.0, 1.0 / 4.0));

    std::vector<double> numbers(size);

    for (auto& num : numbers)
    {
        num = dist(gen);
    }

    return numbers;
}

std::vector<unsigned int> RngCpuStandard::GetRandomBitsVector(unsigned int size)
{
    return {};
}

void RngCpuStandard::SetSeed(unsigned int seed)
{
    
}
