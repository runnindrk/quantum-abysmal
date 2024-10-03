cmake_minimum_required(VERSION 3.10)

project(Quantum-Abysmal VERSION 1.0)

list(APPEND CMAKE_PREFIX_PATH "/snap/root-framework/936/usr/local/cmake")
find_package(ROOT REQUIRED COMPONENTS Graf Graf3d Gpad Hist Core)

include_directories(${ROOT_INCLUDE_DIRS})
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
    src/plotting/PlotFactory.cu
    src/plotting/PlotDensityOfStatesImpl.cu
    src/providers/kpm/density_of_states/cpu/standard/DensityOfStates1d.cu
    src/providers/kpm/density_of_states/cpu/standard/DensityOfStates2d.cu
    src/providers/kpm/density_of_states/cpu/standard/DensityOfStates3d.cu

    src/providers/kpm/density_of_states/cpu/standard/util/Util.cu
)

target_include_directories(Quantum-Abysmal PUBLIC ${CMAKE_SOURCE_DIR}/include)

find_package(OpenMP REQUIRED)
if(OpenMP_CXX_FOUND)
    target_link_libraries(Quantum-Abysmal OpenMP::OpenMP_CXX ${ROOT_LIBRARIES})
endif()

install(TARGETS Quantum-Abysmal DESTINATION ${CMAKE_SOURCE_DIR}/__lib)

set_target_properties(Quantum-Abysmal PROPERTIES PREFIX "" OUTPUT_NAME "libquantum-abysmal")

add_subdirectory(tests)