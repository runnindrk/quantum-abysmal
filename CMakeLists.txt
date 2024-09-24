cmake_minimum_required(VERSION 3.10)

project(Quantum-Abysmal VERSION 1.0)

include_directories(${CMAKE_SOURCE_DIR})

set(CMAKE_CUDA_ARCHITECTURES 86)
enable_language(CUDA)

set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -O3 -funroll-loops -fopenmp -march=native")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -funroll-loops -fopenmp -march=native")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -funroll-loops -fopenmp -march=native")

add_library(Quantum-Abysmal STATIC
    include/internal/Logger.cu
    include/internal/ErrorHandler.cu
    src/entrypoint/Entrypoint.cu 
    src/entrypoint/QuantumAbysmalContextImpl.cu
    src/lattice/util/Conversions.cu
    src/lattice/LatticeImpl.cu
    src/providers/LinearAlgebraFactory.cu
    src/providers/KernelPolynomialMethodFactory.cu
    src/providers/kpm/density_of_states/cpu/standard/DensityOfStates1d.cu
    src/providers/kpm/density_of_states/cpu/standard/DensityOfStates2d.cu
    src/providers/kpm/density_of_states/cpu/standard/DensityOfStates3d.cu
)

target_include_directories(Quantum-Abysmal PUBLIC ${CMAKE_SOURCE_DIR}/include)

install(TARGETS Quantum-Abysmal DESTINATION ${CMAKE_SOURCE_DIR}/__lib)

set_target_properties(Quantum-Abysmal PROPERTIES PREFIX "" OUTPUT_NAME "libquantum-abysmal")

add_subdirectory(tests)