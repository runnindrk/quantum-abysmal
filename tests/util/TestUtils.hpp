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

#include <cstdint>
#include <string>
#include <vector>
#include <array>

void ReadDensityOfStatesData(const std::string& filename,
                             std::array<uint32_t, 2>& latticeSize,
                             size_t& numOfMoments,
                             size_t& numRandomVectors,
                             std::vector<double>& momentsAverage,
                             std::vector<double>& momentsVariance,
                             std::vector<std::array<double, 2>>& densityOfStates);

double StandardDeviation(std::vector<double> testData, std::vector<double> groundData);
std::pair<double, double> StandardDeviation(std::vector<std::array<double, 2>> testData, std::vector<std::array<double, 2>> groundData);