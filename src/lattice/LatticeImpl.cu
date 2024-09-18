//============================================================================
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
//============================================================================

#include "LatticeImpl.hpp"

LatticeImpl* LatticeImpl::mInstance = nullptr;
uint32_t LatticeImpl::mDimension = 0; 

LatticeImpl::LatticeImpl()
{
    mInstance = this;
};

LatticeImpl::~LatticeImpl()
{
    mInstance = nullptr;
};

LatticeImpl* LatticeImpl::GetInstance()
{
    return mInstance;
}

void LatticeImpl::SetDimension(uint32_t dimension)
{
    mDimension = dimension;
}

uint32_t LatticeImpl::GetDimension()
{
    return mDimension;
};