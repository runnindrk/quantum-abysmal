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

#ifndef QUANTUM_ABYSMAL_SRC_MATH_HPP
#define QUANTUM_ABYSMAL_SRC_MATH_HPP

#include "include/public/ErrorHandler.hpp"
#include "include/public/QuantumAbysmalTypes.hpp"

#ifndef __APPLE__
#include <curand_kernel.h>
#endif 

class Math
{
    public:
    
    // --------------------------------------------------------------------------------------------
    // Define math helper functions for both host and device. Ensuring build correctness for CUDA and C++

    #ifndef __APPLE__
    __host__ __device__
    #endif
    static inline double ChebyshevPolynomial(int n, double x)
    {
        return cos(n * acos(x));
    }

    #ifndef __APPLE__
    __host__ __device__
    #endif 
    static inline double JacksonKernel(int n, int N)
    {
        double factor1 = (N - n + 1) * cos(M_PI * n / (N + 1));
        double factor2 = sin(M_PI * n / (N + 1)) * cos(M_PI / (N + 1)) / sin(M_PI / (N + 1));
        return (factor1 + factor2) / (N + 1);
    }

    #ifndef __APPLE__
    __host__ __device__
    #endif
    static inline int Mod(int a, int b)
    {
        int r = a % b;
        return r < 0 ? r + b : r;
    }
};

#endif