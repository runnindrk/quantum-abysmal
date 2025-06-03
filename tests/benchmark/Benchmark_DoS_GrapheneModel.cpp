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
#include "TestVectors.hpp"

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

TEST_F(QuantumAbysmalTest, DoS_Graphenemodel_GPU_STANDARD_IMPL) 
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

    auto dosCtx = kpmCtx->CreateDoSCtx(GPU_STANDARD_IMPL);
    ASSERT_NE(dosCtx, nullptr);

    // Domain Decomposition does not exist in CUDA Provider.
    auto res7 = dosCtx->SetDomainDecomposition({2, 2});
    EXPECT_EQ(res7.ErrorCode, NOT_SUPPORTED);

    auto res8 = dosCtx->SetNumberOfRandomVectors(1);
    EXPECT_EQ(res8.ErrorCode, SUCCESS);

    auto res9 = dosCtx->SetNumberOfMoments(DOS_NUM_MOMENTS);
    EXPECT_EQ(res9.ErrorCode, SUCCESS);

    auto res10 = dosCtx->ComputeMoments();
    EXPECT_EQ(res10.ErrorCode, SUCCESS);

    auto res11 = dosCtx->ComputeDoS(DOS_NUM_POINTS);
    EXPECT_EQ(res10.ErrorCode, SUCCESS);

    // --------------------------------------------------------------------------------------------
    // Test output correctness

    
}