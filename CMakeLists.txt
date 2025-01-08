cmake_minimum_required(VERSION 3.10)

project(Quantum-Abysmal VERSION 1.0)

list(APPEND CMAKE_PREFIX_PATH "/snap/root-framework/936/usr/local/cmake")

enable_language(CUDA)
set(CMAKE_CUDA_ARCHITECTURES 86)

set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -O3 -funroll-loops -fopenmp -march=native")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -funroll-loops -fopenmp -march=native")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -funroll-loops -fopenmp -march=native")

option(DEBUG_LOGGING_ENABLED "Enable DEBUG level logging" OFF)
option(INFO_LOGGING_ENABLED "Enable INFO level logging" OFF)
option(WARN_LOGGING_ENABLED "Enable WARN level logging" OFF)
option(ERROR_LOGGING_ENABLED "Enable ERROR level logging" OFF)

add_library(Quantum-Abysmal STATIC
    
    include/internal/Logger.cu
    include/internal/ErrorHandler.cu
    
    src/entrypoint/Entrypoint.cu 
    src/entrypoint/QuantumAbysmalContextImpl.cu
    
    src/lattice/util/Conversions.cu
    src/lattice/LatticeImpl.cu
    
    src/providers/KernelPolynomialMethodFactory.cu
    src/providers/LinearAlgebraFactory.cu
    src/providers/RngFactory.cu
    src/plotting/PlotFactory.cu
    
    src/plotting/PlotDensityOfStatesImpl.cu
    
    src/providers/cpu_provider/kpm/density_of_states/standard/DensityOfStates1d.cu
    src/providers/cpu_provider/kpm/density_of_states/standard/DensityOfStates2d.cu
    src/providers/cpu_provider/kpm/density_of_states/standard/DensityOfStates3d.cu
    src/providers/cpu_provider/kpm/density_of_states/standard/util/Util.cu

    src/providers/cuda_provider/kpm/density_of_states/standard/DensityOfStates2d.cu

    src/providers/cpu_provider/rng/RngImpl.cu
    
    src/providers/cuda_provider/rng/RngEngineImpl.cu
    src/providers/cuda_provider/rng/RngImpl.cu
)

target_compile_definitions(Quantum-Abysmal PRIVATE
    $<$<BOOL:${DEBUG_LOGGING_ENABLED}>:DEBUG_LOGGING_ENABLED>
    $<$<BOOL:${INFO_LOGGING_ENABLED}>:INFO_LOGGING_ENABLED>
    $<$<BOOL:${WARN_LOGGING_ENABLED}>:WARN_LOGGING_ENABLED>
    $<$<BOOL:${ERROR_LOGGING_ENABLED}>:ERROR_LOGGING_ENABLED>
)

target_include_directories(Quantum-Abysmal PUBLIC ${CMAKE_SOURCE_DIR}/include)

find_package(OpenMP REQUIRED)
find_package(CUDAToolkit REQUIRED)
find_package(ROOT REQUIRED COMPONENTS Graf Graf3d Gpad Hist Core)

include_directories(${ROOT_INCLUDE_DIRS})
include_directories(${CMAKE_SOURCE_DIR})

target_link_libraries(Quantum-Abysmal OpenMP::OpenMP_CXX ${ROOT_LIBRARIES} CUDA::cudart CUDA::curand)

message(STATUS "CUDA include directories: ${CUDAToolkit_INCLUDE_DIRS}")
message(STATUS "DEBUG_LOGGING_ENABLED: ${DEBUG_LOGGING_ENABLED}")
message(STATUS "INFO_LOGGING_ENABLED: ${INFO_LOGGING_ENABLED}")
message(STATUS "WARN_LOGGING_ENABLED: ${WARN_LOGGING_ENABLED}")
message(STATUS "ERROR_LOGGING_ENABLED: ${ERROR_LOGGING_ENABLED}")

install(TARGETS Quantum-Abysmal DESTINATION ${CMAKE_SOURCE_DIR}/__lib)

set_target_properties(Quantum-Abysmal PROPERTIES PREFIX "" OUTPUT_NAME "libquantum-abysmal")

add_subdirectory(tests)