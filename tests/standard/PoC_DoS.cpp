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

#include "../include/public/Entrypoint.hpp"
#include <gtest/gtest.h>

class QuantumAbysmalTest : public ::testing::Test 
{
protected:

    void SetUp() override 
    {
    }

    void TearDown() override 
    {
    }
};

TEST_F(QuantumAbysmalTest, GrapheneModel_DensityOfStates) 
{
    // Get context
    auto abysmalCtx = GetQuantumAbysmalContext();
    ASSERT_NE(abysmalCtx, nullptr);

    auto latticeCtx = abysmalCtx->GetLatticeMethods();
    ASSERT_NE(latticeCtx, nullptr);

    // Graphene model hoppings
    EXPECT_NO_THROW(latticeCtx->AddHopping({1, 0}, {'A', 'B'}, -1));
    EXPECT_NO_THROW(latticeCtx->AddHopping({0, 1}, {'A', 'B'}, -1));
    EXPECT_NO_THROW(latticeCtx->AddHopping({0, 0}, {'A', 'B'}, -1));

    // Set lattice properties
    EXPECT_NO_THROW(latticeCtx->SetLatticeSize({1024, 1024}));
    EXPECT_NO_THROW(latticeCtx->SetEnergyRange(-3, 3));
    EXPECT_NO_THROW(latticeCtx->SetBoundaryType(PERIODIC));

    // DoS calculation
    auto kpmCtx = abysmalCtx->GetKpmMethods();
    ASSERT_NE(kpmCtx, nullptr);

    auto dosCtx = kpmCtx->CreateDoSCtx(CPU_STANDARD_IMPL);
    ASSERT_NE(dosCtx, nullptr);

    EXPECT_NO_THROW(dosCtx->SetNumberOfRandomVectors(1));
    EXPECT_NO_THROW(dosCtx->SetNumberOfMoments(256));
    EXPECT_NO_THROW(dosCtx->Compute());
    EXPECT_NO_THROW(dosCtx->Save());
}

TEST_F(QuantumAbysmalTest, DISABLED_BBHModel_DensityOfStates) 
{
    // Get context
    auto abysmalCtx = GetQuantumAbysmalContext();
    ASSERT_NE(abysmalCtx, nullptr);

    auto latticeCtx = abysmalCtx->GetLatticeMethods();
    ASSERT_NE(latticeCtx, nullptr);

    double delta = 0;
    double gamma = -0.5;

    // Delta hoppings
    EXPECT_NO_THROW(latticeCtx->AddHopping({0, 0}, {'A', 'A'}, delta));
    EXPECT_NO_THROW(latticeCtx->AddHopping({0, 0}, {'B', 'B'}, delta));
    EXPECT_NO_THROW(latticeCtx->AddHopping({0, 0}, {'C', 'D'}, -delta));
    EXPECT_NO_THROW(latticeCtx->AddHopping({0, 0}, {'D', 'D'}, -delta));

    // Main hoppings
    EXPECT_NO_THROW(latticeCtx->AddHopping({1, 0}, {'C', 'A'}, 1));
    EXPECT_NO_THROW(latticeCtx->AddHopping({1, 0}, {'B', 'D'}, 1));
    EXPECT_NO_THROW(latticeCtx->AddHopping({0, 1}, {'D', 'A'}, 1));
    EXPECT_NO_THROW(latticeCtx->AddHopping({0, 1}, {'B', 'C'}, -1));

    // Gamma hoppings
    EXPECT_NO_THROW(latticeCtx->AddHopping({0, 0}, {'C', 'A'}, gamma));
    EXPECT_NO_THROW(latticeCtx->AddHopping({0, 0}, {'B', 'D'}, gamma));
    EXPECT_NO_THROW(latticeCtx->AddHopping({0, 0}, {'D', 'A'}, gamma));
    EXPECT_NO_THROW(latticeCtx->AddHopping({0, 0}, {'B', 'C'}, -gamma));

    // Set lattice properties
    EXPECT_NO_THROW(latticeCtx->SetLatticeSize({1024, 1024}));
    EXPECT_NO_THROW(latticeCtx->SetEnergyRange(-3, 3));
    EXPECT_NO_THROW(latticeCtx->SetBoundaryType(PERIODIC));

    // DoS calculation
    auto kpmCtx = abysmalCtx->GetKpmMethods();
    ASSERT_NE(kpmCtx, nullptr);

    auto dosCtx = kpmCtx->CreateDoSCtx(CPU_STANDARD_IMPL);
    ASSERT_NE(dosCtx, nullptr);

    EXPECT_NO_THROW(dosCtx->SetNumberOfRandomVectors(1));
    EXPECT_NO_THROW(dosCtx->SetNumberOfMoments(256));
    EXPECT_NO_THROW(dosCtx->Compute());
    EXPECT_NO_THROW(dosCtx->Save());
}

TEST_F(QuantumAbysmalTest, DISABLED_OneDTightBindingModel_DensityOfStates) 
{
    // Get context
    auto abysmalCtx = GetQuantumAbysmalContext();
    ASSERT_NE(abysmalCtx, nullptr);

    auto latticeCtx = abysmalCtx->GetLatticeMethods();
    ASSERT_NE(latticeCtx, nullptr);

    // 1D tight-binding model hopping
    EXPECT_NO_THROW(latticeCtx->AddHopping({1}, {'A', 'A'}, -1));

    // Set lattice properties
    EXPECT_NO_THROW(latticeCtx->SetLatticeSize({1048576}));
    EXPECT_NO_THROW(latticeCtx->SetEnergyRange(-2, 2));
    EXPECT_NO_THROW(latticeCtx->SetBoundaryType(PERIODIC));

    // DoS calculation
    auto kpmCtx = abysmalCtx->GetKpmMethods();
    ASSERT_NE(kpmCtx, nullptr);

    auto dosCtx = kpmCtx->CreateDoSCtx(CPU_STANDARD_IMPL);
    ASSERT_NE(dosCtx, nullptr);

    EXPECT_NO_THROW(dosCtx->SetNumberOfRandomVectors(1));
    EXPECT_NO_THROW(dosCtx->SetNumberOfMoments(256));
    EXPECT_NO_THROW(dosCtx->Compute());
    EXPECT_NO_THROW(dosCtx->Save());
}