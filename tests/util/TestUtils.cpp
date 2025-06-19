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

void ReadDensityOfStatesData(const std::string& filename,
                             std::array<uint32_t, 2>& latticeSize,
                             size_t& numOfMoments,
                             size_t& numRandomVectors,
                             std::vector<double>& momentsAverage,
                             std::vector<double>& momentsVariance,
                             std::vector<std::array<double, 2>>& densityOfStates)
{
    // Open the HDF5 file
    hid_t fileId = H5Fopen(filename.c_str(), H5F_ACC_RDONLY, H5P_DEFAULT);
    if (fileId < 0)
    {
        // Error. Handle later.
        return;
    }

    // Read NumOfMoments
    {
        hid_t dataset = H5Dopen(fileId, "NumOfMoments", H5P_DEFAULT);
        if (dataset >= 0)
        {
            uint64_t nMom;
            H5Dread(dataset, H5T_NATIVE_UINT64, H5S_ALL, H5S_ALL, H5P_DEFAULT, &nMom);
            numOfMoments = static_cast<size_t>(nMom);
            H5Dclose(dataset);
        }
        else
        {
            numOfMoments = 0;
        }
    }

    // Read NumRandomVectors
    {
        hid_t dataset = H5Dopen(fileId, "NumRandomVectors", H5P_DEFAULT);
        if (dataset >= 0)
        {
            uint64_t nRand;
            H5Dread(dataset, H5T_NATIVE_UINT64, H5S_ALL, H5S_ALL, H5P_DEFAULT, &nRand);
            numRandomVectors = static_cast<size_t>(nRand);
            H5Dclose(dataset);
        }
        else
        {
            numRandomVectors = 0;
        }
    }

    // Read MomentsAverage
    {
        hid_t dataset = H5Dopen(fileId, "MomentsAverage", H5P_DEFAULT);
        if (dataset >= 0)
        {
            hid_t space = H5Dget_space(dataset);
            hsize_t dim;
            H5Sget_simple_extent_dims(space, &dim, nullptr);
            momentsAverage.resize(dim);
            H5Dread(dataset, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, momentsAverage.data());
            H5Sclose(space);
            H5Dclose(dataset);
        }
        else
        {
            momentsAverage.clear();
        }
    }

    // Read MomentsVariance
    {
        hid_t dataset = H5Dopen(fileId, "MomentsVariance", H5P_DEFAULT);
        if (dataset >= 0)
        {
            hid_t space = H5Dget_space(dataset);
            hsize_t dim;
            H5Sget_simple_extent_dims(space, &dim, nullptr);
            momentsVariance.resize(dim);
            H5Dread(dataset, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, momentsVariance.data());
            H5Sclose(space);
            H5Dclose(dataset);
        }
        else
        {
            momentsVariance.clear();
        }
    }

    // Read DensityOfStates
    {
        hid_t dataset = H5Dopen(fileId, "DensityOfStates", H5P_DEFAULT);
        if (dataset >= 0)
        {
            hid_t space = H5Dget_space(dataset);
            hsize_t dims[2];
            H5Sget_simple_extent_dims(space, dims, nullptr);
            std::vector<double> dosFlat(dims[0] * 2);
            H5Dread(dataset, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, dosFlat.data());
            densityOfStates.resize(dims[0]);
            for (size_t i = 0; i < dims[0]; ++i)
            {
                densityOfStates[i] = {dosFlat[2 * i], dosFlat[2 * i + 1]};
            }
            H5Sclose(space);
            H5Dclose(dataset);
        }
        else
        {
            densityOfStates.clear();
        }
    }

    // Read LatticeSize
    {
        hid_t dataset = H5Dopen(fileId, "LatticeSize", H5P_DEFAULT);
        if (dataset >= 0)
        {
            double latticeSizeDouble[2];
            H5Dread(dataset, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, latticeSizeDouble);
            latticeSize[0] = static_cast<uint32_t>(latticeSizeDouble[0]);
            latticeSize[1] = static_cast<uint32_t>(latticeSizeDouble[1]);
            H5Dclose(dataset);
        }
        else
        {
            latticeSize = {0, 0};
        }
    }

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
