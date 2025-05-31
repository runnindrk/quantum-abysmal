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
    // --------------------------------------------------------------------------------------------
    // Get context

    auto abysmalCtx = GetQuantumAbysmalContext();
    ASSERT_NE(abysmalCtx, nullptr);

    auto latticeCtx = abysmalCtx->GetLatticeMethods();
    ASSERT_NE(latticeCtx, nullptr);

    auto plottingCtx = abysmalCtx->GetPlotMethods();
    ASSERT_NE(plottingCtx, nullptr);

    // --------------------------------------------------------------------------------------------
    // Set Graphene Model lattice hoppings

    auto res1 = latticeCtx->AddHopping({1, 0}, {'A', 'B'}, -1);
    EXPECT_EQ(res1.ErrorCode, SUCCESS);

    auto res2 = latticeCtx->AddHopping({0, 1}, {'A', 'B'}, -1);
    EXPECT_EQ(res2.ErrorCode, SUCCESS);

    auto res3 = latticeCtx->AddHopping({0, 0}, {'A', 'B'}, -1);
    EXPECT_EQ(res3.ErrorCode, SUCCESS);

    // --------------------------------------------------------------------------------------------
    // Set lattice properties

    auto res4 = latticeCtx->SetLatticeSize({2048, 2048});
    EXPECT_EQ(res4.ErrorCode, SUCCESS);

    auto res5 = latticeCtx->SetEnergyRange(-3, 3);
    EXPECT_EQ(res5.ErrorCode, SUCCESS);

    auto res6 = latticeCtx->SetBoundaryType(PERIODIC);
    EXPECT_EQ(res6.ErrorCode, SUCCESS);

    // --------------------------------------------------------------------------------------------
    // DoS calculation

    auto kpmCtx = abysmalCtx->GetKpmMethods();
    ASSERT_NE(kpmCtx, nullptr);

    auto dosCtx = kpmCtx->CreateDoSCtx(CPU_STANDARD_IMPL);
    ASSERT_NE(dosCtx, nullptr);

    auto res7 = dosCtx->SetDomainDecomposition({2, 2});
    EXPECT_EQ(res7.ErrorCode, SUCCESS);

    auto res8 = dosCtx->SetNumberOfRandomVectors(1);
    EXPECT_EQ(res8.ErrorCode, SUCCESS);

    auto res9 = dosCtx->SetNumberOfMoments(512);
    EXPECT_EQ(res9.ErrorCode, SUCCESS);

    auto res10 = dosCtx->ComputeMoments();
    EXPECT_EQ(res10.ErrorCode, SUCCESS);

    auto res11 = dosCtx->ComputeDoS(200000);
    EXPECT_EQ(res10.ErrorCode, SUCCESS);

    auto res12 = dosCtx->Save();
    EXPECT_EQ(res12.ErrorCode, SUCCESS);

    auto res13 = dosCtx->PlotDoS();
    EXPECT_EQ(res13.ErrorCode, SUCCESS);
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
    EXPECT_NO_THROW(dosCtx->ComputeMoments());
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
    EXPECT_NO_THROW(dosCtx->ComputeMoments());
    EXPECT_NO_THROW(dosCtx->Save());
}