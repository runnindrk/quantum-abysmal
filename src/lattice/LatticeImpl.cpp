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

#include "LatticeImpl.hpp"

// -------------------------------------------------------------------------------------------------
// Interface.

Result<void> LatticeImpl::AddHopping(std::vector<int32_t> latticeHop, std::array<char, 2> orbitalHop, double hoppingStrength)
{
    return Result<void>::SetError(LatticeEngineImpl::GetInstance().AddHopping(latticeHop, orbitalHop, hoppingStrength).ErrorCode);
}

Result<void> LatticeImpl::SetLatticeSize(std::vector<uint32_t> lateralSizes)
{
    return Result<void>::SetError(LatticeEngineImpl::GetInstance().SetLatticeSize(lateralSizes).ErrorCode);
}

Result<void> LatticeImpl::SetEnergyRange(double minEnergy, double maxEnergy)
{
    return Result<void>::SetError(LatticeEngineImpl::GetInstance().SetEnergyRange(minEnergy, maxEnergy).ErrorCode);
}

Result<void> LatticeImpl::SetBoundaryType(BoundaryType boundaryType)
{
    return Result<void>::SetError(LatticeEngineImpl::GetInstance().SetBoundaryType(boundaryType).ErrorCode);
}
