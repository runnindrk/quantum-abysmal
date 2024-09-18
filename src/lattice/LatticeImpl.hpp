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

#ifndef QUANTUM_ABYSMAL_SRC_LATTICE_IMPL_HPP
#define QUANTUM_ABYSMAL_SRC_LATTICE_IMPL_HPP

#include "include/public/Lattice.hpp"

class LatticeImpl : public Lattice
{
public:

    // Singleton to universal access from the same instance.
    static LatticeImpl* GetInstance();

    LatticeImpl();
    ~LatticeImpl();

    // ------------------------------------------------------------------------
    // Override of the Public Interface

    void SetDimension(uint32_t dimension) override;

    // ------------------------------------------------------------------------
    // Internal methods.

    uint32_t GetDimension();

private:

    // ------------------------------------------------------------------------
    // Private methods.


    // ------------------------------------------------------------------------
    // Private member variables.

    static LatticeImpl* mInstance;
    static uint32_t mDimension;
};

#endif