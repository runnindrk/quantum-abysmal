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

#include "TestUtils.hpp"

#include <H5Cpp.h>
#include <cmath>

// ================================================================================================
// Read test vectors util functions

void ReadDensityOfStatesSData(const std::string& filename, std::array<uint32_t, 2>& latticeSize, std::vector<double>& moments,
                              std::vector<std::array<double, 2>>& densityOfStates)
{
    // --------------------------------------------------------------------------------------------
    // Open the HDF5 file

    hid_t fileId = H5Fopen(filename.c_str(), H5F_ACC_RDONLY, H5P_DEFAULT);
    if (fileId < 0)
    {
        // Error. Handle later.
    }

    // --------------------------------------------------------------------------------------------
    // Read Moments Dataset

    hid_t momentsDataset = H5Dopen(fileId, "Moments", H5P_DEFAULT);
    hid_t momentsSpace = H5Dget_space(momentsDataset);
    hsize_t momentsDim;
    H5Sget_simple_extent_dims(momentsSpace, &momentsDim, nullptr);

    moments.resize(momentsDim);
    H5Dread(momentsDataset, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, moments.data());

    H5Sclose(momentsSpace);
    H5Dclose(momentsDataset);

    // --------------------------------------------------------------------------------------------
    // Read DensityOfStates Dataset

    hid_t dosDataset = H5Dopen(fileId, "DensityOfStates", H5P_DEFAULT);
    hid_t dosSpace = H5Dget_space(dosDataset);
    hsize_t dosDims[2];
    H5Sget_simple_extent_dims(dosSpace, dosDims, nullptr);

    std::vector<double> dosFlat(dosDims[0] * 2);
    H5Dread(dosDataset, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, dosFlat.data());

    densityOfStates.resize(dosDims[0]);
    for (size_t i = 0; i < dosDims[0]; ++i)
    {
        densityOfStates[i] = {dosFlat[2 * i], dosFlat[2 * i + 1]};
    }

    H5Sclose(dosSpace);
    H5Dclose(dosDataset);

    // --------------------------------------------------------------------------------------------
    // Read LatticeSize Dataset

    hid_t latticeSizeDataset = H5Dopen(fileId, "LatticeSize", H5P_DEFAULT);
    if (latticeSizeDataset >= 0)
    {
        double latticeSizeDouble[2];
        H5Dread(latticeSizeDataset, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, latticeSizeDouble);
        latticeSize[0] = static_cast<uint32_t>(latticeSizeDouble[0]);
        latticeSize[1] = static_cast<uint32_t>(latticeSizeDouble[1]);
        H5Dclose(latticeSizeDataset);
    }

    else
    {
        latticeSize = {0, 0}; // Default fallback if dataset missing
    }

    // --------------------------------------------------------------------------------------------
    // Close file

    herr_t status = H5Fclose(fileId);
    if (status < 0)
    {
        // Handle file close error if needed
    }
}

// ================================================================================================
// Statistical util functions

double StandardDeviation(std::vector<double> testData, std::vector<double> groundData)
{
    if (testData.size() != groundData.size() || testData.empty())
    {
        // Error. Handle it later.
    }

    double mean = 0.0;
    double variance = 0.0;
    size_t n = testData.size();
    std::vector<double> diff(n);

    for (size_t i = 0; i < n; ++i)
    {
        diff[i] = testData[i] - groundData[i];
    }

    for (double d : diff)
    {
        mean += d;
    }

    mean /= n;

    for (double d : diff)
    {
        variance += (d - mean) * (d - mean);
    }

    variance /= n;

    return std::sqrt(variance);
}

#include <array>
#include <cmath>
#include <stdexcept>
#include <vector>

std::pair<double, double> StandardDeviation(std::vector<std::array<double, 2>> testData, std::vector<std::array<double, 2>> groundData)
{
    if (testData.size() != groundData.size() || testData.empty())
    {
        // Error. Handle it later.
    }

    size_t n = testData.size();
    std::vector<double> diffX(n);
    std::vector<double> diffY(n);

    for (size_t i = 0; i < n; ++i)
    {
        diffX[i] = testData[i][0] - groundData[i][0];
        diffY[i] = testData[i][1] - groundData[i][1];
    }

    auto computeStd = [](const std::vector<double>& diff) -> double
    {
        double mean = 0.0;
        double variance = 0.0;
        
        for (double d : diff)
        {
            mean += d;
        }

        mean /= diff.size();

        for (double d : diff)
        {
            variance += (d - mean) * (d - mean);
        }
        variance /= diff.size();

        return std::sqrt(variance);
    };

    double stdX = computeStd(diffX);
    double stdY = computeStd(diffY);

    return {stdX, stdY};
}
