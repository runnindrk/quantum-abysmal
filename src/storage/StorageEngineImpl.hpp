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

#ifndef QUANTUM_ABYSMAL_SRC_STORAGE_ENGINE_HPP
#define QUANTUM_ABYSMAL_SRC_STORAGE_ENGINE_HPP

#include "internal/Logger.hpp"
#include "lattice/LatticeEngineImpl.hpp"

class StorageEngine
{
  public:

    StorageEngine();
    ~StorageEngine();

    static void SaveDoS(LatticeStructure& mLattice,
                            size_t numOfMoments,
                            size_t numRandomVectors,
                            std::vector<double>& momentsAverage,
                            std::vector<double>& momentsVariance,
                            std::vector<std::array<double, 2>>& densityOfStates);
    
    static void SaveLDoS();

  private:
  
};

#endif