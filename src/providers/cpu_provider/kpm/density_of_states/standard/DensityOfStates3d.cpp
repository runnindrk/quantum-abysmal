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

#include "DensityOfStates3d.hpp"

Error DensityOfStates3dCpuStandard::SetNumberOfRandomVectors(size_t numVectors)
{
    LOG_INFO << "Hello from DoS 3D on CPU/STANDARD!";
    LOG_INFO << "Number of Orbitals : " << LatticeImpl::GetInstance().GetLattice().numberOfOrbitals;
    return SUCCESS;
}

Error DensityOfStates3dCpuStandard::SetNumberOfMoments(size_t order)
{
    return SUCCESS;
}

std::vector<double> DensityOfStates3dCpuStandard::Compute()
{
    return {};
}

Error DensityOfStates3dCpuStandard::Save() const
{
    return SUCCESS;
}