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

// NOTE: This class is internally read only. No need for locks and mutex (I think).
// TODO: Error Handling.

LatticeImpl::Lattice LatticeImpl::mLattice;

LatticeImpl& LatticeImpl::GetInstance()
{
    static LatticeImpl instance;
    return instance;
}

void LatticeImpl::AddHopping(std::vector<int32_t> latticeHop, std::array<char, 2> orbitalHop,
                             double hoppingStrength)
{
    for (const auto& orbital : orbitalHop)
    {
        if (mOrbitalsSet.find(orbital) == mOrbitalsSet.end())
        {
            mLattice.numberOfOrbitals += 1;
            mOrbitalsSet.insert(orbital);
        }
    }

    // ------------------------------------------------------------------------
    // Convert hopping to internal representation.

    auto firstOrbital = orbitalMapToInt.find(orbitalHop[0]);
    if (firstOrbital == orbitalMapToInt.end())
    {
        LOG_ERROR << "Map to internal hopping not found!";
        return;
    }

    auto secondOrbital = orbitalMapToInt.find(orbitalHop[1]);
    if (secondOrbital == orbitalMapToInt.end())
    {
        LOG_ERROR << "Map to internal hopping not found!";
        return;
    }

    Hopping hopping;
    hopping.hoppingStrength = hoppingStrength;
    hopping.latticeHop = latticeHop;
    hopping.orbitalHop = std::array<uint32_t, 2>{firstOrbital->second, secondOrbital->second};

    // ------------------------------------------------------------------------
    // Conjugate the hopping.

    std::vector<int32_t> positionConjugate;
    for (const auto& value : latticeHop)
    {
        positionConjugate.push_back(-value);
    }

    Hopping conjugate;
    conjugate.hoppingStrength = hoppingStrength;
    conjugate.latticeHop = positionConjugate;
    conjugate.orbitalHop = std::array<uint32_t, 2>{secondOrbital->second, firstOrbital->second};

    // ------------------------------------------------------------------------

    mLattice.hoppings.push_back(hopping);
    mLattice.hoppings.push_back(conjugate);
}

void LatticeImpl::SetLatticeSize(std::vector<uint32_t> lateralSizes)
{
    if (lateralSizes.size() == 0 || lateralSizes.size() > 3)
    {
        LOG_ERROR << "Unsupported dimensions.";
        return;
    }

    mLattice.dimension = lateralSizes.size();
    mLattice.latticeSize = lateralSizes;

    for (const auto& size : lateralSizes)
    {
        mLattice.numberOfSites *= size;
    }

    mLattice.hamiltonianSize = mLattice.numberOfOrbitals * mLattice.numberOfSites;
}

void LatticeImpl::SetEnergyRange(double minEnergy, double maxEnergy)
{
    // I still need to check for shifts != 0.

    mLattice.minEnergy = minEnergy;
    mLattice.maxEnergy = maxEnergy;
    mLattice.energyShift = (maxEnergy - minEnergy) / 2.0;
    mLattice.energyScaling = (maxEnergy + minEnergy) / 2.0;

    for (auto& hopping : mLattice.hoppings)
    {
        hopping.hoppingStrength /= mLattice.energyScaling;
    }
}

void LatticeImpl::SetBoundaryType(BoundaryType boundaryType)
{
    mLattice.boundaryType = boundaryType;
}

// ----------------------------------------------------------------------------
// Internal methods.

LatticeImpl::Lattice LatticeImpl::GetLattice()
{
    return mLattice;
}

// ----------------------------------------------------------------------------
// Private methods.