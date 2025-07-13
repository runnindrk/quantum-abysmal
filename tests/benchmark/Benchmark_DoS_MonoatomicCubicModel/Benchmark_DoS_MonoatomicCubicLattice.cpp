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
    // Set lattice properties

    auto res13 = latticeCtx->SetLatticeSize({128, 128, 128});
    EXPECT_EQ(res13.ErrorCode, SUCCESS);

    auto res14 = latticeCtx->SetEnergyRange(-6, 6);
    EXPECT_EQ(res14.ErrorCode, SUCCESS);

    auto res15 = latticeCtx->SetBoundaryType(PERIODIC);
    EXPECT_EQ(res15.ErrorCode, SUCCESS);

    // --------------------------------------------------------------------------------------------
    // DoS calculation

    auto kpmCtx = abysmalCtx->GetKpmMethods();
    ASSERT_NE(kpmCtx, nullptr);

    auto dosCtx = kpmCtx->CreateDoSCtx(CPU_STANDARD_IMPL);
    ASSERT_NE(dosCtx, nullptr);

    // Domain Decomposition does not exist in CUDA Provider.
    auto res16 = dosCtx->SetDomainDecomposition({2, 2, 1});
    EXPECT_EQ(res16.ErrorCode, NOT_SUPPORTED);

    auto res17 = dosCtx->SetNumberOfRandomVectors(1);
    EXPECT_EQ(res17.ErrorCode, SUCCESS);

    auto res18 = dosCtx->SetNumberOfMoments(512);
    EXPECT_EQ(res18.ErrorCode, SUCCESS);

    auto res19 = dosCtx->ComputeMoments();
    EXPECT_EQ(res19.ErrorCode, SUCCESS);

    auto res20 = dosCtx->ComputeDoS(2048);
    EXPECT_EQ(res20.ErrorCode, SUCCESS);

    dosCtx->Save();
    
    // --------------------------------------------------------------------------------------------
    // Test output correctness

    
}