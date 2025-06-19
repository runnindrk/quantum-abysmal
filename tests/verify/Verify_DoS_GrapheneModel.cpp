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

// ================================================================================================
// Warning 

// This test will be modified to use SetUp() override and TearDown() override after the codegen update.
// This does not work now becase LatticeImpl is a singleton and cannot be instantiated multiple times.

// ================================================================================================
// Shared variables for test vectors and contexts

std::string testVectorFile;
std::array<uint32_t, 2> testVectorLatticeSize;
size_t testVectorNumOfMoments;
size_t testVectorNumRandomVectors;
std::vector<double> testVectorMomentsAverage;
std::vector<double> testVectorMomentsVariance;
std::vector<std::array<double, 2>> testVectorDoS;

std::shared_ptr<QuantumAbysmalContext> abysmalCtx;
std::shared_ptr<Lattice> latticeCtx;
std::shared_ptr<Plotting> plottingCtx;

// ================================================================================================
// Test fixture for Quantum Abysmal tests

class QuantumAbysmalTest : public ::testing::Test 
{
    public:

    // ============================================================================================
    // Set Up 

    static void SetUpTestSuite()
    {
        // ----------------------------------------------------------------------------------------
        // Read test vector

        testVectorFile = "tests/test_vectors/Verify_DoS_GrapheneModel.hdf5";
        ReadDensityOfStatesData(testVectorFile,
                                testVectorLatticeSize,
                                testVectorNumOfMoments,
                                testVectorNumRandomVectors,
                                testVectorMomentsAverage,
                                testVectorMomentsVariance,
                                testVectorDoS);
        
        // ----------------------------------------------------------------------------------------
        // Set up context and lattice

        // Get context
        abysmalCtx = GetQuantumAbysmalContext();
        ASSERT_NE(abysmalCtx, nullptr);

        latticeCtx = abysmalCtx->GetLatticeMethods();
        ASSERT_NE(latticeCtx, nullptr);

        plottingCtx = abysmalCtx->GetPlotMethods();
        ASSERT_NE(plottingCtx, nullptr);

        // Set Graphene Model lattice hoppings
        auto res1 = latticeCtx->AddHopping({1, 0}, {'A', 'B'}, -1);
        ASSERT_EQ(res1.ErrorCode, SUCCESS);

        auto res2 = latticeCtx->AddHopping({0, 1}, {'A', 'B'}, -1);
        ASSERT_EQ(res2.ErrorCode, SUCCESS);

        auto res3 = latticeCtx->AddHopping({0, 0}, {'A', 'B'}, -1);
        ASSERT_EQ(res3.ErrorCode, SUCCESS);

        // Set lattice properties
        auto res4 = latticeCtx->SetLatticeSize({testVectorLatticeSize[0], testVectorLatticeSize[1]});
        ASSERT_EQ(res4.ErrorCode, SUCCESS);

        auto res5 = latticeCtx->SetEnergyRange(-3, 3);
        ASSERT_EQ(res5.ErrorCode, SUCCESS);

        auto res6 = latticeCtx->SetBoundaryType(PERIODIC);
        ASSERT_EQ(res6.ErrorCode, SUCCESS);
    }

    // ============================================================================================
    // Tear Down 

    static void TearDownTestSuite() 
    {
    }

    // ============================================================================================
    // Helper function to verify computed moments against test vector

    static void VerifyMoments(const std::vector<double>& computedMoments)
    {
        std::vector<std::pair<size_t, double>> outOfBounds;
        for (size_t i = 0; i < computedMoments.size(); ++i) 
        {
            double mean = testVectorMomentsAverage[i];
            double stddev = std::sqrt(testVectorMomentsVariance[i]);
            double z = (stddev > 0) ? (computedMoments[i] - mean) / stddev : 0.0;

            if (std::abs(z) >= 5.0) 
            {
                outOfBounds.emplace_back(i, z);
            }
        }

        if (!outOfBounds.empty()) 
        {
            std::cout << "Moments with z-score out of bounds:\n";
            for (const auto& [idx, z] : outOfBounds) 
            {
                std::cout << "  Moment " << idx << " z-score: " << z << "\n";
            }
            FAIL();
        }
    }
};

// ============================================================================================
// Test for Density of States (DoS) computation on Graphene model on CPU_STANDARD_IMPL

TEST_F(QuantumAbysmalTest, DoS_Graphenemodel_CPU_STANDARD_IMPL) 
{
    auto kpmCtx = abysmalCtx->GetKpmMethods();
    ASSERT_NE(kpmCtx, nullptr);

    auto dosCtx = kpmCtx->CreateDoSCtx(CPU_STANDARD_IMPL);
    ASSERT_NE(dosCtx, nullptr);

    auto res7 = dosCtx->SetDomainDecomposition({2, 2});
    EXPECT_EQ(res7.ErrorCode, SUCCESS);

    auto res8 = dosCtx->SetNumberOfRandomVectors(1);
    EXPECT_EQ(res8.ErrorCode, SUCCESS);

    auto res9 = dosCtx->SetNumberOfMoments(testVectorNumRandomVectors);
    EXPECT_EQ(res9.ErrorCode, SUCCESS);

    auto res10 = dosCtx->ComputeMoments();
    EXPECT_EQ(res10.ErrorCode, SUCCESS);

    auto res11 = dosCtx->ComputeDoS(testVectorDoS.size());
    EXPECT_EQ(res10.ErrorCode, SUCCESS);

    VerifyMoments(res10.Value);
}

// ============================================================================================
// Test for Density of States (DoS) computation on Graphene model on GPU_STANDARD_IMPL

TEST_F(QuantumAbysmalTest, DoS_Graphenemodel_GPU_STANDARD_IMPL) 
{
    auto kpmCtx = abysmalCtx->GetKpmMethods();
    ASSERT_NE(kpmCtx, nullptr);

    auto dosCtx = kpmCtx->CreateDoSCtx(GPU_STANDARD_IMPL);
    ASSERT_NE(dosCtx, nullptr);

    // Domain Decomposition does not exist in GPU Provider.
    auto res7 = dosCtx->SetDomainDecomposition({2, 2});
    EXPECT_EQ(res7.ErrorCode, NOT_SUPPORTED);

    auto res8 = dosCtx->SetNumberOfRandomVectors(1);
    EXPECT_EQ(res8.ErrorCode, SUCCESS);

    auto res9 = dosCtx->SetNumberOfMoments(testVectorNumRandomVectors);
    EXPECT_EQ(res9.ErrorCode, SUCCESS);

    auto res10 = dosCtx->ComputeMoments();
    EXPECT_EQ(res10.ErrorCode, SUCCESS);

    auto res11 = dosCtx->ComputeDoS(testVectorDoS.size());
    EXPECT_EQ(res10.ErrorCode, SUCCESS);

    VerifyMoments(res10.Value);
}
