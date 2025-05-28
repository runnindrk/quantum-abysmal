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
# Architecture agnostic test build

cmake_minimum_required(VERSION 3.10)
project(Quantum-Abysmal-PoC)

find_package(GTest REQUIRED)

add_executable(Quantum-Abysmal-PoC-DoS
${CMAKE_CURRENT_SOURCE_DIR}/standard/PoC_DoS.cpp
)

target_link_libraries(Quantum-Abysmal-PoC-DoS PRIVATE Quantum-Abysmal GTest::gtest GTest::gtest_main)
target_include_directories(Quantum-Abysmal-PoC-DoS PRIVATE ${CMAKE_SOURCE_DIR}/include/public)

add_executable(Quantum-Abysmal-PoC-Rng
${CMAKE_CURRENT_SOURCE_DIR}/standard/PoC_Rng.cpp
)

target_link_libraries(Quantum-Abysmal-PoC-Rng PRIVATE Quantum-Abysmal GTest::gtest GTest::gtest_main)
target_include_directories(Quantum-Abysmal-PoC-Rng PRIVATE ${CMAKE_SOURCE_DIR}/include/public)

install(
TARGETS 
Quantum-Abysmal-PoC-DoS 
Quantum-Abysmal-PoC-Rng

DESTINATION 
${CMAKE_SOURCE_DIR}/_bin
)