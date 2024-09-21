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

Error LatticeImpl::AddHopping(std::vector<int32_t> latticeHop, std::array<char, 2> orbitalHop,
                              double hoppingStrength)
{
    LOG_WARN << "Please be sure to call your AddHoppings in sequence!";

    if (mIsAnyOtherFunctionCalled)
    {
        LOG_ERROR << "inserting AddHopping after calling other functions is not allowed.";
        return FUNCTION_CALL_ORDER_ERROR;
    }

    if (latticeHop.size() == 0 || latticeHop.size() > 3)
    {
        LOG_ERROR << "Unsupported dimensions.";
        return DIMENSION_ERROR;
    }

    mLattice.dimension = latticeHop.size();

    // ------------------------------------------------------------------------
    // Check orbitals.

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
        return RUNTIME_ERROR;
    }

    auto secondOrbital = orbitalMapToInt.find(orbitalHop[1]);
    if (secondOrbital == orbitalMapToInt.end())
    {
        LOG_ERROR << "Map to internal hopping not found!";
        return RUNTIME_ERROR;
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

    mIsAddHoppingCalled = true;

    return SUCCESS;
}

Error LatticeImpl::SetLatticeSize(std::vector<uint32_t> lateralSizes)
{
    if (!mIsAddHoppingCalled)
    {
        LOG_ERROR << "AddHopping was not called. Cannot determine dimensions.";
        return FUNCTION_CALL_ORDER_ERROR;
    }

    if (lateralSizes.size() != mLattice.dimension)
    {
        LOG_ERROR << "Wrong set of dimensions.";
        return DIMENSION_ERROR;
    }

    mLattice.latticeSize = lateralSizes;

    for (const auto& size : lateralSizes)
    {
        mLattice.numberOfSites *= size;
    }

    mLattice.hamiltonianSize = mLattice.numberOfOrbitals * mLattice.numberOfSites;
    mIsAnyOtherFunctionCalled = true;

    return SUCCESS;
}

Error LatticeImpl::SetEnergyRange(double minEnergy, double maxEnergy)
{
    if (!mIsAddHoppingCalled)
    {
        LOG_ERROR << "AddHopping was not called. Cannot resize hoppings.";
        return FUNCTION_CALL_ORDER_ERROR;
    }

    // I still need to check for shifts != 0.

    mLattice.minEnergy = minEnergy;
    mLattice.maxEnergy = maxEnergy;
    mLattice.energyShift = (maxEnergy - minEnergy) / 2.0;
    mLattice.energyScaling = (maxEnergy + minEnergy) / 2.0;

    for (auto& hopping : mLattice.hoppings)
    {
        hopping.hoppingStrength /= mLattice.energyScaling;
    }

    mIsAnyOtherFunctionCalled = true;

    return SUCCESS;
}

Error LatticeImpl::SetBoundaryType(BoundaryType boundaryType)
{
    mLattice.boundaryType = boundaryType;
    return SUCCESS;
}

// ----------------------------------------------------------------------------
// Internal methods.

LatticeImpl::Lattice LatticeImpl::GetLattice()
{
    return mLattice;
}

Error LatticeImpl::PrintLatticeInformation()
{
    return SUCCESS;
}

// ----------------------------------------------------------------------------
// Private methods.