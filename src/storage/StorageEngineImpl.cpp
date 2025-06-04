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

#include "StorageEngineImpl.hpp"

#include <H5Cpp.h>
#include <ctime>
#include <filesystem>
#include <iomanip>
#include <string>

StorageEngine::StorageEngine()
{
}

StorageEngine::~StorageEngine()
{
}

void StorageEngine::SaveDoS(LatticeStructure& mLattice, std::vector<double>& moments, std::vector<std::array<double, 2>>& densityOfStates)
{
    // Ensure "data" directory exists
    const std::filesystem::path dir{"data"};
    if (!std::filesystem::exists(dir))
    {
        std::filesystem::create_directories(dir);
    }

    // Get current time and format it for filename
    std::time_t t = std::time(nullptr);
    std::tm tm = *std::localtime(&t);

    std::ostringstream filenameStream;
    filenameStream << "data/DoS-" << std::put_time(&tm, "%Y%m%d-%H%M%S") << ".hdf5";
    std::string filename = filenameStream.str();

    // Create or overwrite the HDF5 file with the timestamped filename
    hid_t fileId = H5Fcreate(filename.c_str(), H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
    if (fileId < 0)
    {
        // Failed to create file
        return;
    }

    // Save moments as 1D dataset
    hsize_t momentsDim = moments.size();
    hid_t momentsSpace = H5Screate_simple(1, &momentsDim, nullptr);
    hid_t momentsDataset = H5Dcreate(fileId, "Moments", H5T_IEEE_F64LE, momentsSpace, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    H5Dwrite(momentsDataset, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, moments.data());
    H5Dclose(momentsDataset);
    H5Sclose(momentsSpace);

    // Flatten densityOfStates to contiguous array
    std::vector<double> dosFlat;
    dosFlat.reserve(densityOfStates.size() * 2);
    for (const auto& pair : densityOfStates)
    {
        dosFlat.push_back(pair[0]);
        dosFlat.push_back(pair[1]);
    }

    // Save densityOfStates as 2D dataset [N, 2]
    hsize_t dosDims[2] = {densityOfStates.size(), 2};
    hid_t dosSpace = H5Screate_simple(2, dosDims, nullptr);
    hid_t dosDataset = H5Dcreate(fileId, "DensityOfStates", H5T_IEEE_F64LE, dosSpace, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    H5Dwrite(dosDataset, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, dosFlat.data());
    H5Dclose(dosDataset);
    H5Sclose(dosSpace);

    // Save lattice size as 2D shape [xSize, ySize]
    double latticeSizeXY[2] = {
        static_cast<double>(mLattice.latticeSize[0]),
        static_cast<double>(mLattice.latticeSize[1])
    };
    hsize_t latticeSizeDim = 2;
    hid_t latticeSizeSpace = H5Screate_simple(1, &latticeSizeDim, nullptr);
    hid_t latticeSizeDataset = H5Dcreate(fileId, "LatticeSize", H5T_IEEE_F64LE, latticeSizeSpace, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    H5Dwrite(latticeSizeDataset, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, latticeSizeXY);
    H5Dclose(latticeSizeDataset);
    H5Sclose(latticeSizeSpace);

    // Close file
    herr_t status = H5Fclose(fileId);
    if (status < 0)
    {
        // Handle file close error if needed
    }
}

void StorageEngine::SaveLDoS()
{
}
