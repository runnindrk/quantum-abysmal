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
    
    // Delta hoppings
    auto res1 = latticeCtx->AddHopping({1}, {'A', 'A'}, -1);
    EXPECT_EQ(res1.ErrorCode, SUCCESS);

    // --------------------------------------------------------------------------------------------
    // Set lattice properties

    auto res2 = latticeCtx->SetLatticeSize({2048, 2048});
    EXPECT_EQ(res2.ErrorCode, SUCCESS);

    auto res3 = latticeCtx->SetEnergyRange(-3, 3);
    EXPECT_EQ(res3.ErrorCode, SUCCESS);

    auto res4 = latticeCtx->SetBoundaryType(PERIODIC);
    EXPECT_EQ(res4.ErrorCode, SUCCESS);

    // --------------------------------------------------------------------------------------------
    // DoS calculation

    auto kpmCtx = abysmalCtx->GetKpmMethods();
    ASSERT_NE(kpmCtx, nullptr);

    auto dosCtx = kpmCtx->CreateDoSCtx(GPU_STANDARD_IMPL);
    ASSERT_NE(dosCtx, nullptr);

    // Domain Decomposition does not exist in CUDA Provider.
    auto res5 = dosCtx->SetDomainDecomposition({2, 2});
    EXPECT_EQ(res5.ErrorCode, NOT_SUPPORTED);

    auto res6 = dosCtx->SetNumberOfRandomVectors(1);
    EXPECT_EQ(res6.ErrorCode, SUCCESS);

    auto res7 = dosCtx->SetNumberOfMoments(512);
    EXPECT_EQ(res7.ErrorCode, SUCCESS);

    auto res8 = dosCtx->ComputeMoments();
    EXPECT_EQ(res8.ErrorCode, SUCCESS);

    auto res9 = dosCtx->ComputeDoS(2048);
    EXPECT_EQ(res9.ErrorCode, SUCCESS);
}