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

#include "StorageEngineImpl.hpp"
#include <H5Cpp.h>

StorageEngine::StorageEngine()
{
}

StorageEngine::~StorageEngine()
{
}

void StorageEngine::SaveDoS()
{
    hid_t fileId = H5Fcreate("data/DoS.hdf5", H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
    if (fileId < 0) 
    {
        return;
    }

    herr_t status = H5Fclose(fileId);
    if (status < 0) 
    {
        // File close failed; optionally log this
    }
}

void StorageEngine::SaveLDoS()
{
}
