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

#include "LatticeImpl.hpp"

// This is internally read only. No need for locks and mutex (I think).

LatticeImpl::Lattice LatticeImpl::lattice;

// LatticeImpl::LatticeImpl()
// {
//
// };

// LatticeImpl::~LatticeImpl()
// {
//
// };

LatticeImpl& LatticeImpl::GetInstance()
{
    static LatticeImpl instance;
    return instance;
}

void LatticeImpl::SetDimension(uint32_t dimension)
{
    lattice.dimension = dimension;
}

void LatticeImpl::SetSize(std::vector<uint32_t> lateralSizes)
{
    if (lateralSizes.size() != lattice.dimension)
    {
        LOG_ERROR << "Wrong dimensions!";
        return;
    }

    lattice.latticeSize = lateralSizes;
}

void LatticeImpl::SetNumberOfOrbitals(uint32_t dimension)
{
    1;
}

void LatticeImpl::SetEnergyRange(double minEnergy, double maxEnergy)
{
    1;
}

// ----------------------------------------------------------------------------
// Internal methods.

LatticeImpl::Lattice LatticeImpl::GetLattice()
{
    return lattice;
}

// ------------------------------------------------------------------------
// Private methods.

void LatticeImpl::FinalizeLattice()
{
    1;
}