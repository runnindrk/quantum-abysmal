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

#ifndef QUANTUM_ABYSMAL_SRC_RNG_CPU_ENGINE_HPP
#define QUANTUM_ABYSMAL_SRC_RNG_CPU_ENGINE_HPP

#include "internal/Logger.hpp"
#include "public/Rng.hpp"

#include <random>
#include <vector>

class RngCpuEngine
{
    public:

    static RngCpuEngine& GetInstance();

    RngCpuEngine();
    ~RngCpuEngine();

    std::vector<double> GetRandomVector(unsigned int size);
    std::vector<unsigned int> GetRandomBitsVector(unsigned int size);
    Error SetSeed(unsigned int seed);

    void GetRandomVector(double* cpuBuffer, unsigned int bufferSize);
    void GetRandomBitsVector(unsigned int* cpuBuffer, unsigned int bufferSize);

    private:
    
    unsigned int mSeed;
};

#endif