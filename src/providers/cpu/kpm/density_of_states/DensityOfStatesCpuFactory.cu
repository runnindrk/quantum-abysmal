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

#include "DensityOfStatesCpuFactory.hpp"

std::unique_ptr<DensityOfStates> DensityOfStatesCpuFactory::GetInstance(ProviderImplementation implementation)
{
    switch (LatticeImpl::GetInstance()->GetDimension())
    {

    case 1:
    {
        switch (implementation)
        {
        case CPU_STANDARD_IMPL:
            return std::make_unique<DensityOfStates1dCpuStandard>();
        case CPU_DUMMY_IMPL:
            return nullptr;
        default:
            return nullptr;
        }
    }

    case 2:
    {
        switch (implementation)
        {
        case CPU_STANDARD_IMPL:
            return std::make_unique<DensityOfStates2dCpuStandard>();
        case CPU_DUMMY_IMPL:
            return nullptr;
        default:
            return nullptr;
        }
    }

    case 3:
    {
        switch (implementation)
        {
        case CPU_STANDARD_IMPL:
            return std::make_unique<DensityOfStates3dCpuStandard>();
        case CPU_DUMMY_IMPL:
            return nullptr;
        default:
            return nullptr;
        }
    }

    default:
    {
        return nullptr;
    }

    }
}