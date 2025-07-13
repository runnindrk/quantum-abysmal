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

#ifndef QUANTUM_ABYSMAL_SRC_DENSITY_OF_STATES_CPU_UTIL_STANDARD_HPP
#define QUANTUM_ABYSMAL_SRC_DENSITY_OF_STATES_CPU_UTIL_STANDARD_HPP

// #define ARRAY_IDX_1D(x, hx, o) (numOrbitals * ((x + hx)) + o)
// #define ARRAY_IDX_2D(x, y, hx, hy, o) (numOrbitals * ((x + hx) + (y + hy) * xGhostedSize) + o)
// #define ARRAY_IDX_3D(x, y, z, hx, hy, hz, o) (numOrbitals * ((x + hx) + (y + hy) * xGhostedSize + (z + hz) * xGhostedSize * yGhostedSize) + o)

#define ARRAY_IDX_1D(x, hx, o) (numOrbitals * (Math::Mod(x + hx, xSize)) + o)
#define ARRAY_IDX_2D(x, y, hx, hy, o) (numOrbitals * (Math::Mod(x + hx, xSize) + Math::Mod(y + hy, ySize) * xSize) + o)
#define ARRAY_IDX_3D(x, y, z, hx, hy, hz, o) (numOrbitals * (Math::Mod(x + hx, xSize) + Math::Mod(y + hy, ySize) * xSize + Math::Mod(z + hz, zSize) * xSize * ySize) + o)

#endif