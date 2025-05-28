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

#include "public/ErrorHandler.hpp"
#include "Logger.hpp"

void GetErrorString(Error err)
{
    switch (err)
    {
        case FUNCTION_CALL_ORDER_ERROR:
            LOG_ERROR << "Function call order error.";
            break;
        case RUNTIME_ERROR:
            LOG_ERROR << "Runtime error.";
            break;
        case SUCCESS:
            LOG_INFO << "Success.";
            break;
        default:
            LOG_ERROR << "Unknown error.";
            break;
    }
}
