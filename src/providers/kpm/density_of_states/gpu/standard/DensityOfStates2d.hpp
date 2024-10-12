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

#ifndef QUANTUM_ABYSMAL_SRC_DENSITY_OF_STATES_2D_GPU_STANDARD_HPP
#define QUANTUM_ABYSMAL_SRC_DENSITY_OF_STATES_2D_GPU_STANDARD_HPP

#include "include/internal/Logger.hpp"
#include "include/public/DensityOfStates.hpp"
#include "src/lattice/LatticeImpl.hpp"

#include <curand_kernel.h>

class DensityOfStates2dGpuStandard : public DensityOfStates
{
  public:
    Error SetNumberOfRandomVectors(size_t numVectors) override;
    Error SetNumberOfMoments(size_t order) override;
    std::vector<double> Compute() override;

  private:
    // ------------------------------------------------------------------------
    // Private methods.

    // ------------------------------------------------------------------------
    // Member variables.
};

// Random number will be substituted for an RNG ENGINE in the future.
// This is just to kickstart the development.
__global__ void InitCurandXorwow(curandStateXORWOW *state);

__global__ void InitRandomVector(curandStateXORWOW *state, double *buffer, unsigned int bufferSize);

#endif