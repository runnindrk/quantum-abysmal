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

HostLattice LatticeImpl::mLattice;

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

    // ------------------------------------------------------------------------

    HostHopping hopping;

    hopping.hoppingStrength = hoppingStrength;    
    hopping.orbitalHop[0] = firstOrbital->second;
    hopping.orbitalHop[1] = secondOrbital->second;

    for (int i = 0; i < latticeHop.size(); i++)
    {
        hopping.latticeHop[i] = latticeHop[i];
    }
    
    // ------------------------------------------------------------------------
    // Conjugate the hopping.

    HostHopping conjugate;

    conjugate.hoppingStrength = hoppingStrength;    
    conjugate.orbitalHop[0] = secondOrbital->second;
    conjugate.orbitalHop[1] = firstOrbital->second;

    for (int i = 0; i < latticeHop.size(); i++)
    {
        conjugate.latticeHop[i] = - latticeHop[i];
    }

    // ------------------------------------------------------------------------
    
    mLattice.hoppings[mLattice.numberOfHoppings] = hopping;
    mLattice.hoppings[mLattice.numberOfHoppings + 1] = conjugate;
    mLattice.numberOfHoppings += 2;

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

    for (int i = 0; i < lateralSizes.size(); i++)
    {   
        mLattice.latticeSize[i] = lateralSizes[i];
    }

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
    mLattice.energyScaling = (maxEnergy - minEnergy) / (2.0 - 0.01);
    mLattice.energyShift = (maxEnergy + minEnergy) / 2.0;

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

HostLattice LatticeImpl::GetLattice()
{
    return mLattice;
}

Error LatticeImpl::PrintLatticeInformation()
{
    return SUCCESS;
}

// ----------------------------------------------------------------------------
// Private methods.