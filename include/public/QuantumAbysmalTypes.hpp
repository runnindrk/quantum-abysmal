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

#ifndef QUANTUM_ABYSMAL_PUBLIC_TYPES_HPP
#define QUANTUM_ABYSMAL_PUBLIC_TYPES_HPP

#include <array>
#include <cstdint>
#include <vector>

typedef enum
{
    CPU_STANDARD_IMPL,
    GPU_STANDARD_IMPL,
    CPU_DUMMY_IMPL,
    GPU_DUMMY_IMPL

} ProviderImplementation;

typedef enum
{
    PERIODIC,
    TWISTED,
    OPEN

} BoundaryType;

#endif