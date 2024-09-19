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

#ifndef QUANTUM_ABYSMAL_PUBLIC_LATTICE_HPP
#define QUANTUM_ABYSMAL_PUBLIC_LATTICE_HPP

#include "QuantumAbysmalTypes.hpp"

#include <memory>

// This can go deeper, having several methods for different type of lattices.
// (Like KPM has several methods and then a factory)
// For now, it is the simplest.

// What the methods could be.
// - Normal lattice with set hoppings.
// - Control over hoppings & local energies with function pointers
// - Sparse matrix control.

class Lattice
{
public:
    using Uptr = std::unique_ptr<Lattice>;

    /// @brief Set the dimension of the lattice.
    /// @param dimension The dimension of the lattice (e.g., 1D, 2D, 3D).
    virtual void SetDimension(uint32_t dimension) = 0;

    /// @brief Set the size of the lattice in each dimension.
    /// @param sizes A vector containing the size of the lattice in each dimension.
    // virtual void SetSize(std::vector<uint32_t> sizes) = 0;

    /// @brief Set the range of the spectrum for resizing.
    /// @param minEnergy The minimum energy.
    /// @param maxEnergy The maximum energy.
    // virtual void SetEnergyRange(double minEnergy, double maxEnergy) = 0;

    /// @brief Set the boundary type of the lattice.
    /// @param boundaryType The boundary type (PERIODIC, TWISTED, OPEN).
    // virtual void SetBoundary(BoundaryType boundaryType) = 0;

    virtual ~Lattice() = default;
};

#endif