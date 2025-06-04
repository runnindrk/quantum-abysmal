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
project(Quantum-Abysmal-Tests)

enable_testing()

find_package(GTest REQUIRED)

file(GLOB_RECURSE TEST_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/proof_of_concept/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/verify/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/benchmark/*.cpp
)

set(TEST_UTILS_SRC ${CMAKE_CURRENT_SOURCE_DIR}/util/TestUtils.cpp)

include_directories(
    ${CMAKE_SOURCE_DIR}/include/public
    ${CMAKE_CURRENT_SOURCE_DIR}/util
)

foreach(TEST_SRC ${TEST_SOURCES})
    get_filename_component(TEST_NAME ${TEST_SRC} NAME_WE)
    add_executable(${TEST_NAME} ${TEST_SRC} ${TEST_UTILS_SRC})

    target_link_libraries(${TEST_NAME} PRIVATE Quantum-Abysmal GTest::gtest GTest::gtest_main)

    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})

    install(TARGETS ${TEST_NAME} DESTINATION ${CMAKE_SOURCE_DIR}/_bin)
endforeach()