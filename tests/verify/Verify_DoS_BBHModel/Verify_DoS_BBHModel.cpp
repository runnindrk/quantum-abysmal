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

#include "public/Entrypoint.hpp"

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

TEST_F(QuantumAbysmalTest, DoS_BBHModel_GPU_STANDARD_IMPL) 
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

    double delta = 0;
    double gamma = -0.5;
    
    // Delta hoppings
    auto res1 = latticeCtx->AddHopping({0, 0}, {'A', 'A'}, delta);
    EXPECT_EQ(res1.ErrorCode, SUCCESS);

    auto res2 = latticeCtx->AddHopping({0, 0}, {'B', 'B'}, delta);
    EXPECT_EQ(res2.ErrorCode, SUCCESS);

    auto res3 = latticeCtx->AddHopping({0, 0}, {'C', 'D'}, -delta);
    EXPECT_EQ(res3.ErrorCode, SUCCESS);

    auto res4 = latticeCtx->AddHopping({0, 0}, {'D', 'D'}, -delta);
    EXPECT_EQ(res4.ErrorCode, SUCCESS);

    // Main hoppings
    auto res5 = latticeCtx->AddHopping({1, 0}, {'C', 'A'}, 1);
    EXPECT_EQ(res5.ErrorCode, SUCCESS);

    auto res6 = latticeCtx->AddHopping({1, 0}, {'B', 'D'}, 1);
    EXPECT_EQ(res6.ErrorCode, SUCCESS);

    auto res7 = latticeCtx->AddHopping({0, 1}, {'D', 'A'}, 1);
    EXPECT_EQ(res7.ErrorCode, SUCCESS);

    auto res8 = latticeCtx->AddHopping({0, 1}, {'B', 'C'}, -1);
    EXPECT_EQ(res8.ErrorCode, SUCCESS);

    // Gamma hoppings
    auto res9 = latticeCtx->AddHopping({0, 0}, {'C', 'A'}, gamma);
    EXPECT_EQ(res9.ErrorCode, SUCCESS);

    auto res10 = latticeCtx->AddHopping({0, 0}, {'B', 'D'}, gamma);
    EXPECT_EQ(res10.ErrorCode, SUCCESS);

    auto res11 = latticeCtx->AddHopping({0, 0}, {'D', 'A'}, gamma);
    EXPECT_EQ(res11.ErrorCode, SUCCESS);

    auto res12 = latticeCtx->AddHopping({0, 0}, {'B', 'C'}, -gamma);
    EXPECT_EQ(res12.ErrorCode, SUCCESS);

    // --------------------------------------------------------------------------------------------
    // Set lattice properties

    auto res13 = latticeCtx->SetLatticeSize({2048, 2048});
    EXPECT_EQ(res13.ErrorCode, SUCCESS);

    auto res14 = latticeCtx->SetEnergyRange(-3, 3);
    EXPECT_EQ(res14.ErrorCode, SUCCESS);

    auto res15 = latticeCtx->SetBoundaryType(PERIODIC);
    EXPECT_EQ(res15.ErrorCode, SUCCESS);

    // --------------------------------------------------------------------------------------------
    // DoS calculation

    auto kpmCtx = abysmalCtx->GetKpmMethods();
    ASSERT_NE(kpmCtx, nullptr);

    auto dosCtx = kpmCtx->CreateDoSCtx(GPU_STANDARD_IMPL);
    ASSERT_NE(dosCtx, nullptr);

    // Domain Decomposition does not exist in CUDA Provider.
    auto res16 = dosCtx->SetDomainDecomposition({2, 2});
    EXPECT_EQ(res16.ErrorCode, NOT_SUPPORTED);

    auto res17 = dosCtx->SetNumberOfRandomVectors(1);
    EXPECT_EQ(res17.ErrorCode, SUCCESS);

    auto res18 = dosCtx->SetNumberOfMoments(512);
    EXPECT_EQ(res18.ErrorCode, SUCCESS);

    auto res19 = dosCtx->ComputeMoments();
    EXPECT_EQ(res19.ErrorCode, SUCCESS);

    auto res20 = dosCtx->ComputeDoS(2048);
    EXPECT_EQ(res20.ErrorCode, SUCCESS);
}