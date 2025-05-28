# =================================================================================================
# Copyright (c) 2024, runnindrk
#
# This file is part of Quantum Abysmal.
#
# Quantum Abysmal is free software: you can redistribute it and/or modify it
# as needed, with the intent of making it freely available to everyone.
#
# This project is in its early stages and is provided without any warranties,
# expressed or implied, including but not limited to the warranties of
# merchantability, fitness for a particular purpose, or non-infringement.
#
# Use it at your own risk, and feel free to contribute as the project evolves!
# =================================================================================================

# =================================================================================================
# General

# =================================================================================================
# Dependencies

include(FetchContent)

# -------------------------------------------------------------------------------------------------
# Google Test Library

# Configure Google Test options
set(BUILD_SHARED_LIBS OFF)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.17.0
)

FetchContent_MakeAvailable(googletest)

# =================================================================================================
# Architecture agnostic test build

cmake_minimum_required(VERSION 3.10)
project(Quantum-Abysmal-PoC)

add_executable(Quantum-Abysmal-PoC-DoS
${CMAKE_CURRENT_SOURCE_DIR}/standard/PoC_DoS.cpp
)

target_link_libraries(Quantum-Abysmal-PoC-DoS PRIVATE Quantum-Abysmal gtest gtest_main)
target_include_directories(Quantum-Abysmal-PoC-DoS PRIVATE ${CMAKE_SOURCE_DIR}/include/public)

add_executable(Quantum-Abysmal-PoC-Rng
${CMAKE_CURRENT_SOURCE_DIR}/standard/PoC_Rng.cpp
)

target_link_libraries(Quantum-Abysmal-PoC-Rng PRIVATE Quantum-Abysmal gtest gtest_main)
target_include_directories(Quantum-Abysmal-PoC-Rng PRIVATE ${CMAKE_SOURCE_DIR}/include/public)

install(
TARGETS 
Quantum-Abysmal-PoC-DoS 
Quantum-Abysmal-PoC-Rng

DESTINATION 
${CMAKE_SOURCE_DIR}/_bin
)