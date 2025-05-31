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

#ifndef QUANTUM_ABYSMAL_SRC_RNG_CPU_STANDARD_HPP
#define QUANTUM_ABYSMAL_SRC_RNG_CPU_STANDARD_HPP

#include "include/public/Rng.hpp"
#include "RngEngineImpl.hpp"

class RngCpuStandard : public RandomNumberGenerator
{
  public:
    Result<std::vector<double>> GetRandomVector(unsigned int size) override;
    Result<std::vector<unsigned int>> GetRandomBitsVector(unsigned int size) override;
    Result<void> SetSeed(unsigned int seed) override;
};

#endif