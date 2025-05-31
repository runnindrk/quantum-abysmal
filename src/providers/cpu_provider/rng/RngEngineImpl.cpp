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

#include "RngEngineImpl.hpp"

// -------------------------------------------------------------------------------------------------
// Constructors

RngCpuEngine& RngCpuEngine::GetInstance()
{
    static RngCpuEngine instance;
    return instance;
}

RngCpuEngine::RngCpuEngine()
{
}

RngCpuEngine::~RngCpuEngine()
{
}

// -------------------------------------------------------------------------------------------------
// Interface

std::vector<double> RngCpuEngine::GetRandomVector(unsigned int size)
{
    std::vector<double> numbers(size);
    GetRandomVector(numbers.data(), numbers.size());

    return numbers;
}

std::vector<unsigned int> RngCpuEngine::GetRandomBitsVector(unsigned int size)
{
    return {};
}

Error RngCpuEngine::SetSeed(unsigned int seed)
{
    return SUCCESS;
}

// -------------------------------------------------------------------------------------------------
// Internal public functions.

void RngCpuEngine::GetRandomVector(double* cpuBuffer, unsigned int bufferSize)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<> dist(0.0, pow(2.0 / 3.0, 1.0 / 4.0));

    for (int i = 0; i < bufferSize; i++)
    {
        cpuBuffer[i] = dist(gen);
    }
}

void RngCpuEngine::GetRandomBitsVector(unsigned int* cpuBuffer, unsigned int bufferSize)
{
    
}