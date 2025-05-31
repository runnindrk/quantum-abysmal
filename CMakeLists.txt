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

cmake_minimum_required(VERSION 3.10)
project(Quantum-Abysmal)

option(DEBUG_LOGGING_ENABLED "Enable DEBUG level logging" OFF)
option(INFO_LOGGING_ENABLED "Enable INFO level logging" OFF)
option(WARN_LOGGING_ENABLED "Enable WARN level logging" OFF)
option(ERROR_LOGGING_ENABLED "Enable Result<void> level logging" OFF)

message(STATUS "DEBUG_LOGGING_ENABLED: ${DEBUG_LOGGING_ENABLED}")
message(STATUS "INFO_LOGGING_ENABLED: ${INFO_LOGGING_ENABLED}")
message(STATUS "WARN_LOGGING_ENABLED: ${WARN_LOGGING_ENABLED}")
message(STATUS "ERROR_LOGGING_ENABLED: ${ERROR_LOGGING_ENABLED}")

add_subdirectory(src)
add_subdirectory(tests)