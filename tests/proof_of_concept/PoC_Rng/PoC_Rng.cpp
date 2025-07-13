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

TEST_F(QuantumAbysmalTest, CustomCudaRng_QualityTest_Crush) 
{
    auto abysmalCtx = GetQuantumAbysmalContext();
    ASSERT_NE(abysmalCtx, nullptr);

    auto rngCtx = abysmalCtx->GetRngMethods()->CreateRngCtx(GPU_STANDARD_IMPL);
    ASSERT_NE(rngCtx, nullptr);

    auto randomNumbers = rngCtx->GetRandomBitsVector(2 << 20);
}