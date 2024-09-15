cmake_minimum_required(VERSION 3.10)

project(Quantum-Abysmal VERSION 1.0)

include_directories(${CMAKE_SOURCE_DIR})

set(CMAKE_CUDA_ARCHITECTURES 86)
enable_language(CUDA)

add_library(Quantum-Abysmal STATIC
    src/lattice/LatticeImpl.cu
)

target_include_directories(Quantum-Abysmal PUBLIC ${CMAKE_SOURCE_DIR}/include)

install(TARGETS Quantum-Abysmal DESTINATION ${CMAKE_SOURCE_DIR}/bin/lib)

# set_target_properties(Quantum-Abysmal PROPERTIES
#     OUTPUT_NAME "Quantum-Abysmal" 
# )

add_subdirectory(tests)