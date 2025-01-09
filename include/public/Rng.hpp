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

#ifndef QUANTUM_ABYSMAL_PUBLIC_RNG_HPP
#define QUANTUM_ABYSMAL_PUBLIC_RNG_HPP

#include "ErrorHandler.hpp"
#include "QuantumAbysmalTypes.hpp"

class RandomNumberGenerator
{
  public:
    using Uptr = std::unique_ptr<RandomNumberGenerator>;

    /// @brief Get a random number.
    /// @param implementation Implementation to draw the random number.
    /// @return A random number.
    // virtual void GetRandomNumber() = 0;

    /// @brief Get a random vector.
    /// @param size Size of the random vector.
    /// @return A random number.
    virtual std::vector<double> GetRandomVector(unsigned int size) = 0;

    /// @brief Get a random vector of bits.
    /// @param size Size of the random vector.
    /// @return A random number.
    virtual std::vector<unsigned int> GetRandomBitsVector(unsigned int size) = 0;

    /// @brief Set the seed for RNG.
    /// @param seed Seed.
    virtual void SetSeed(unsigned int seed) = 0;

    virtual ~RandomNumberGenerator() = default;
};

#endif