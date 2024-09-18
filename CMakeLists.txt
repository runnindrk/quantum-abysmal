cmake_minimum_required(VERSION 3.10)

project(Quantum-Abysmal VERSION 1.0)

include_directories(${CMAKE_SOURCE_DIR})

set(CMAKE_CUDA_ARCHITECTURES 86)
enable_language(CUDA)

add_library(Quantum-Abysmal STATIC
    src/lattice/LatticeImpl.cu
    src/entrypoint/Entrypoint.cu 
    src/entrypoint/QuantumAbysmalContextImpl.cu
    src/providers/KernelPolynomialMethodFactory.cu

    src/providers/cpu/kpm/density_of_states/DensityOfStatesCpuFactory.cu
    src/providers/cpu/kpm/density_of_states/standard/DensityOfStates1d.cu
    src/providers/cpu/kpm/density_of_states/standard/DensityOfStates2d.cu
    src/providers/cpu/kpm/density_of_states/standard/DensityOfStates3d.cu
)

target_include_directories(Quantum-Abysmal PUBLIC ${CMAKE_SOURCE_DIR}/include)

install(TARGETS Quantum-Abysmal DESTINATION ${CMAKE_SOURCE_DIR}/__lib)

set_target_properties(Quantum-Abysmal PROPERTIES PREFIX "" OUTPUT_NAME "libquantum-abysmal")

add_subdirectory(tests)