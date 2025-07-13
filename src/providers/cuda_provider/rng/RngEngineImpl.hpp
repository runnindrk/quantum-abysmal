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

#ifndef QUANTUM_ABYSMAL_SRC_RNG_GPU_ENGINE_HPP
#define QUANTUM_ABYSMAL_SRC_RNG_GPU_ENGINE_HPP

#include "internal/Logger.hpp"
#include "public/Rng.hpp"

#include <curand_kernel.h>

// This will be to a global header file.
#define NUM_THREADS_RNG 256
#define NUM_BLOCKS_RNG 256 

class RngGpuEngine
{
  public:

    static RngGpuEngine& GetInstance();

    RngGpuEngine();
    ~RngGpuEngine();

    std::vector<double> GetRandomVector(unsigned int size);
    std::vector<unsigned int> GetRandomBitsVector(unsigned int size);
    Error SetSeed(unsigned int seed);

    void GetRandomVector(double* gpuBuffer, unsigned int bufferSize);
    void GetRandomBitsVector(unsigned int* gpuBuffer, unsigned int bufferSize);

  private:

    curandStateXORWOW* mDevStates;
    unsigned int mSeed;
};

__global__ void InitCurandXorwow(curandStateXORWOW* state);
__global__ void InitRandomVector(curandStateXORWOW* state, double* buffer, unsigned int bufferSize);
__global__ void InitRandomBitsVector(curandStateXORWOW* state, unsigned int* buffer, unsigned int bufferSize);

#endif