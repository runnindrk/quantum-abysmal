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
#include "../include/internal/Logger.hpp"
#include "../util/TestUtils.hpp"

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
    // Read test vector

    std::string testVectorFile = "tests/test_vectors/Verify_DoS_GrapheneModel.hdf5";
    std::array<uint32_t, 2> testVectorLatticeSize;
    std::vector<double> testVectorMoments;
    std::vector<std::array<double, 2>> testVectorDoS;

    ReadDensityOfStatesSData(testVectorFile, testVectorLatticeSize, testVectorMoments, testVectorDoS);
    
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

    auto res4 = latticeCtx->SetLatticeSize({testVectorLatticeSize[0], testVectorLatticeSize[1]});
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

    auto res9 = dosCtx->SetNumberOfMoments(testVectorMoments.size());
    EXPECT_EQ(res9.ErrorCode, SUCCESS);

    auto res10 = dosCtx->ComputeMoments();
    EXPECT_EQ(res10.ErrorCode, SUCCESS);

    auto res11 = dosCtx->ComputeDoS(testVectorDoS.size());
    EXPECT_EQ(res10.ErrorCode, SUCCESS);

    // --------------------------------------------------------------------------------------------
    // Verify output

    auto stdMoments = StandardDeviation(res10.Value, testVectorMoments);
    auto stdDoS = StandardDeviation(res11.Value, testVectorDoS);

    std::cout << "stdMoments : " << stdMoments << std::endl;
    std::cout << "stdDoS : " << stdDoS.first << " " << stdDoS.second << std::endl;

    
}