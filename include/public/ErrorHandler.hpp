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

#ifndef QUANTUM_ABYSMAL_PUBLIC_ERRORHANDLER_HPP
#define QUANTUM_ABYSMAL_PUBLIC_ERRORHANDLER_HPP

// ------------------------------------------------------------------------------------------------
// Errors

typedef enum
{
    DIMENSION_ERROR,
    FUNCTION_CALL_ORDER_ERROR,
    RUNTIME_ERROR,
    UNKNOWN_ERROR,
    NOT_SUPPORTED,
    SUCCESS

} Error;

// ------------------------------------------------------------------------------------------------
// Result type

// Generic version
template <typename T>
struct Result 
{
    T Value;
    Error ErrorCode;

    static Result<T> SetError(Error error) 
    {
        return { T{}, error };
    }

    static Result<T> SetValue(T value) 
    {
        return { value, SUCCESS };
    }
};


// Specialization for void
template <>
struct Result<void> 
{
    Error ErrorCode;

    static Result<void> SetError(Error error) 
    {
        return { error };
    }
};

// ------------------------------------------------------------------------------------------------
// String output of errors.

void GetErrorString(Error err);

#endif