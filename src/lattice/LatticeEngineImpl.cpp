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

#include "LatticeEngineImpl.hpp"

LatticeEngineImpl& LatticeEngineImpl::GetInstance()
{
    static LatticeEngineImpl instance;
    return instance;
}

// This will be deprecated in the future.
Result<void> LatticeEngineImpl::AddHopping(std::vector<int32_t> latticeHop, std::array<char, 2> orbitalHop,
                              double hoppingStrength)
{
    LOG_WARN << "Please be sure to call your AddHoppings in sequence!";

    // if (mIsAnyOtherFunctionCalled)
    // {
    //     LOG_ERROR << "inserting AddHopping after calling other functions is not allowed.";
    //     return Result<void>::SetError(FUNCTION_CALL_ORDER_ERROR);
    // }

    if (latticeHop.size() == 0 || latticeHop.size() > 3)
    {
        LOG_ERROR << "Unsupported dimensions.";
        return Result<void>::SetError(DIMENSION_ERROR);
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
        return Result<void>::SetError(RUNTIME_ERROR);
    }

    auto secondOrbital = orbitalMapToInt.find(orbitalHop[1]);
    if (secondOrbital == orbitalMapToInt.end())
    {
        LOG_ERROR << "Map to internal hopping not found!";
        return Result<void>::SetError(RUNTIME_ERROR);
    }

    // ------------------------------------------------------------------------

    Hopping hopping;

    hopping.hoppingStrength = hoppingStrength;    
    hopping.orbitalHop[0] = firstOrbital->second;
    hopping.orbitalHop[1] = secondOrbital->second;

    for (int i = 0; i < latticeHop.size(); i++)
    {
        hopping.latticeHop[i] = latticeHop[i];
    }
    
    // ------------------------------------------------------------------------
    // Conjugate the hopping.

    Hopping conjugate;

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

    return Result<void>::SetError(SUCCESS);
}

Result<void> LatticeEngineImpl::SetLatticeSize(std::vector<uint32_t> lateralSizes)
{
    if (lateralSizes.size() != mLattice.dimension)
    {
        LOG_ERROR << "Wrong set of dimensions.";
        return Result<void>::SetError(DIMENSION_ERROR);
    }

    // Always restart number of sites and hamiltonian size.
    mLattice.numberOfSites = originalLattice.numberOfSites;
    mLattice.hamiltonianSize = originalLattice.hamiltonianSize;

    for (int i = 0; i < lateralSizes.size(); i++)
    {   
        mLattice.latticeSize[i] = lateralSizes[i];
    }

    for (const auto& size : lateralSizes)
    {
        mLattice.numberOfSites *= size;
    }

    mLattice.hamiltonianSize = mLattice.numberOfOrbitals * mLattice.numberOfSites;

    return Result<void>::SetError(SUCCESS);
}

Result<void> LatticeEngineImpl::SetEnergyRange(double minEnergy, double maxEnergy)
{
    // I still need to check for shifts != 0.

    // Always restart hoppingStrength
    for (uint32_t i = 0; i < mLattice.numberOfHoppings; ++i)
    {
        mLattice.hoppings[i].hoppingStrength = originalLattice.hoppings[i].hoppingStrength;
    }

    mLattice.minEnergy = minEnergy;
    mLattice.maxEnergy = maxEnergy;
    mLattice.energyScaling = (maxEnergy - minEnergy) / (2.0 - 0.01);
    mLattice.energyShift = (maxEnergy + minEnergy) / 2.0;

    for (auto& hopping : mLattice.hoppings)
    {
        hopping.hoppingStrength /= mLattice.energyScaling;
    }

    return Result<void>::SetError(SUCCESS);
}

Result<void> LatticeEngineImpl::SetBoundaryType(BoundaryType boundaryType)
{
    mLattice.boundaryType = boundaryType;
    return Result<void>::SetError(SUCCESS);
}

// ----------------------------------------------------------------------------
// Internal methods.

LatticeStructure& LatticeEngineImpl::GetLattice()
{
    return mLattice;
}

// ----------------------------------------------------------------------------
// Private methods.