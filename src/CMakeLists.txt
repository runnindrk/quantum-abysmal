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

message(STATUS "PROJECT_ROOT is set to: ${CMAKE_CURRENT_SOURCE_DIR}/")
add_definitions(-DPROJECT_ROOT=\"${CMAKE_CURRENT_SOURCE_DIR}/\")

# =================================================================================================
# Architecture agnostic build

if(APPLE)
    project(Quantum-Abysmal LANGUAGES CXX VERSION 1.0)
else()
    project(Quantum-Abysmal LANGUAGES CXX CUDA VERSION 1.0)
endif()

if(APPLE)
    set(OpenMP_C_FLAGS "-Xpreprocessor -fopenmp -I/opt/homebrew/opt/libomp/include")
    set(OpenMP_C_LIB_NAMES "omp")
    set(OpenMP_omp_LIBRARY /opt/homebrew/opt/libomp/lib/libomp.dylib)

    set(OpenMP_CXX_FLAGS "-Xpreprocessor -fopenmp -I/opt/homebrew/opt/libomp/include")
    set(OpenMP_CXX_LIB_NAMES "omp")
    set(OpenMP_CXX_LIBRARY /opt/homebrew/opt/libomp/lib/libomp.dylib)
endif()

# -------------------------------------------------------------------------------------------------
# Requirements

find_package(OpenMP REQUIRED)
find_package(HDF5 REQUIRED)
find_package(ROOT REQUIRED COMPONENTS RIO Net Tree MathCore Hist Graf Graf3d Gpad ROOTDataFrame Gui)

if(NOT APPLE)
    find_package(CUDAToolkit REQUIRED)
endif()

# -------------------------------------------------------------------------------------------------

if(APPLE)
    set(COMMON_OPT_FLAGS "-O3 -funroll-loops -march=native ${OpenMP_CXX_FLAGS}")
else()
    set(COMMON_OPT_FLAGS "-O3 -funroll-loops -march=native -fopenmp --expt-relaxed-constexpr")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMMON_OPT_FLAGS}")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${COMMON_OPT_FLAGS}")

if(APPLE)
    message(STATUS "CUDA is not enabled on macOS.")
else()
    enable_language(CUDA)
    set(CMAKE_CUDA_ARCHITECTURES 86)
    set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} ${COMMON_OPT_FLAGS}")
endif()

# -------------------------------------------------------------------------------------------------
# Include source files

file(
    GLOB_RECURSE QUANTUM_ABYSMAL_SOURCES

    ${CMAKE_SOURCE_DIR}/include/internal/*.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/entrypoint/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/entrypoint/**/*.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/lattice/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/lattice/**/*.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/math/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/math/**/*.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/plotting/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/plotting/**/*.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/storage/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/storage/**/*.cpp

    # Include provider factories.
    ${CMAKE_CURRENT_SOURCE_DIR}/providers/*.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/providers/cpu_provider/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/providers/cpu_provider/**/*.cpp
)

if(APPLE)
    file(
        GLOB_RECURSE METAL_SOURCES

        ${CMAKE_CURRENT_SOURCE_DIR}/providers/metal_provider/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/providers/metal_provider/**/*.cpp
    )
    list(APPEND QUANTUM_ABYSMAL_SOURCES ${METAL_SOURCES})
else()
    file(
        GLOB_RECURSE CUDA_SOURCES

        ${CMAKE_CURRENT_SOURCE_DIR}/providers/cuda_provider/*.cu
        ${CMAKE_CURRENT_SOURCE_DIR}/providers/cuda_provider/**/*.cu
    )
    list(APPEND QUANTUM_ABYSMAL_SOURCES ${CUDA_SOURCES})
endif()

# -------------------------------------------------------------------------------------------------

if(NOT APPLE)
    set_source_files_properties(${QUANTUM_ABYSMAL_SOURCES} PROPERTIES LANGUAGE CUDA)
endif()

message(STATUS "Found source files:")
foreach(src IN LISTS QUANTUM_ABYSMAL_SOURCES)
  message(STATUS "  ${src}")
endforeach()

add_library(Quantum-Abysmal STATIC ${QUANTUM_ABYSMAL_SOURCES})

target_compile_definitions(
    Quantum-Abysmal PRIVATE
    $<$<BOOL:${DEBUG_LOGGING_ENABLED}>:DEBUG_LOGGING_ENABLED>
    $<$<BOOL:${INFO_LOGGING_ENABLED}>:INFO_LOGGING_ENABLED>
    $<$<BOOL:${WARN_LOGGING_ENABLED}>:WARN_LOGGING_ENABLED>
    $<$<BOOL:${ERROR_LOGGING_ENABLED}>:ERROR_LOGGING_ENABLED>
)

# -------------------------------------------------------------------------------------------------
# Include headers

target_include_directories(Quantum-Abysmal PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${HDF5_INCLUDE_DIRS}
    ${ROOT_INCLUDE_DIRS}
)

# -------------------------------------------------------------------------------------------------
# Link libraries

target_link_libraries(Quantum-Abysmal
    OpenMP::OpenMP_CXX
    ${HDF5_LIBRARIES}
    ${ROOT_LIBRARIES}
)

if(NOT APPLE)
    target_link_libraries(Quantum-Abysmal
        CUDA::cudart
        CUDA::curand
    )
endif()

# -------------------------------------------------------------------------------------------------

set_target_properties(Quantum-Abysmal PROPERTIES PREFIX "" OUTPUT_NAME "libquantum-abysmal")
install(TARGETS Quantum-Abysmal DESTINATION ${CMAKE_SOURCE_DIR}/_lib)