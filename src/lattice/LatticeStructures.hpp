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

#include "include/public/QuantumAbysmalTypes.hpp"

struct Hopping
{
    int8_t latticeHop[3];
    uint8_t orbitalHop[2];
    double hoppingStrength{};
};

struct LatticeStructure
{
    uint32_t dimension{};
    uint32_t numberOfOrbitals{};
    uint32_t latticeSize[3] = {};
    uint64_t numberOfSites{1};
    uint64_t hamiltonianSize{1};

    Hopping hoppings[128] = {};
    uint8_t numberOfHoppings{};

    double minEnergy{};
    double maxEnergy{};
    double energyScaling{};
    double energyShift{};

    BoundaryType boundaryType{};

    // Add necessary ghost information.
};

/*
struct CudaLattice
{
    double* data;
    LatticeStructure latticeStructure;

    __host__ CudaLattice(LatticeStructure structure)
    {
        latticeStructure = structure;
        cudaMalloc(&data, latticeStructure.hamiltonianSize * sizeof(double));
    }

    __host__ ~CudaLattice()
    {
        cudaFree(data);
    }

    __host__ CudaLattice(CudaLattice&& other) noexcept : data(other.data)
    {
        other.data = nullptr;
    }

    __host__ CudaLattice& operator=(CudaLattice&& other) noexcept
    {
        if (this != &other)
        {
            cudaFree(data);

            data = other.data;
            latticeStructure = other.latticeStructure;

            other.data = nullptr;
        }

        return *this;
    }

    CudaLattice(const Lattice&) = delete;
    CudaLattice& operator=(const CudaLattice&) = delete;

    // Overload operator() for 1D access (device & host)
    __device__ double& operator()(uint64_t x, uint64_t o)
    {
        return data[latticeStructure.numberOfOrbitals * x + o];
    }

    // Overload operator() for 2D access (device & host)
    __device__ double& operator()(uint64_t x, uint64_t y, uint64_t o)
    {
        return data[latticeStructure.numberOfOrbitals * (x + latticeStructure.latticeSize[0] * y) +
                    o];
    }

    // Overload operator() for 3D access (device & host)
    __device__ double& operator()(uint64_t x, uint64_t y, uint64_t z, uint64_t o)
    {
        return data[latticeStructure.numberOfOrbitals *
                        (x + latticeStructure.latticeSize[0] * y +
                         latticeStructure.latticeSize[0] * latticeStructure.latticeSize[1] * z) +
                    o];
    }

    __host__ void copyFromHost(const double* hostData)
    {
        cudaMemcpy(data, hostData, latticeStructure.hamiltonianSize * sizeof(double),
                   cudaMemcpyHostToDevice);
    }

    __host__ void copyToHost(double* hostData) const
    {
        cudaMemcpy(hostData, data, latticeStructure.hamiltonianSize * sizeof(double),
                   cudaMemcpyDeviceToHost);
    }
};
*/