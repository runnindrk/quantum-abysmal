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

#include "../src/entrypoint/QuantumAbysmalContextImpl.hpp"
#include "../src/lattice/LatticeImpl.hpp"
#include "../src/providers/KernelPolynomialMethodFactory.hpp"
#include "../src/providers/LinearAlgebraFactory.hpp"
#include "../src/providers/RngFactory.hpp"
#include "../src/plotting/PlotFactory.hpp"
// #include "../src/storage/StorageImpl.hpp"

Lattice::Uptr QuantumAbysmalContextImpl::GetLatticeMethods()
{
    return std::make_unique<LatticeImpl>();
}

KernelPolynomialMethod::Uptr QuantumAbysmalContextImpl::GetKpmMethods()
{
    return std::make_unique<KernelPolynomialMethodFactory>();
}

LinearAlgebra::Uptr QuantumAbysmalContextImpl::GetLinearAlgebraMethods()
{
    return std::make_unique<LinearAlgebraFactory>();
}

RngMethod::Uptr QuantumAbysmalContextImpl::GetRngMethods()
{
    return std::make_unique<RngFactory>();
}

Plotting::Uptr QuantumAbysmalContextImpl::GetPlotMethods()
{
    return std::make_unique<PlottingFactory>();
}

Storage::Uptr QuantumAbysmalContextImpl::GetStorageMethods()
{
    return nullptr;
}