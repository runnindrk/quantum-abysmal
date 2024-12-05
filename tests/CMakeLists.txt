cmake_minimum_required(VERSION 3.10)
project(Quantum-Abysmal-PoC)

add_executable(Quantum-Abysmal-PoC-DoS
${CMAKE_CURRENT_SOURCE_DIR}/standard/PoC_DoS.cu
)

target_link_libraries(Quantum-Abysmal-PoC-DoS PRIVATE Quantum-Abysmal)
target_include_directories(Quantum-Abysmal-PoC-DoS PRIVATE ${CMAKE_SOURCE_DIR}/include/public)

add_executable(Quantum-Abysmal-PoC-Rng
${CMAKE_CURRENT_SOURCE_DIR}/standard/PoC_Rng.cu
)

target_link_libraries(Quantum-Abysmal-PoC-Rng PRIVATE Quantum-Abysmal)
target_include_directories(Quantum-Abysmal-PoC-Rng PRIVATE ${CMAKE_SOURCE_DIR}/include/public)

install(
TARGETS 
Quantum-Abysmal-PoC-DoS 
Quantum-Abysmal-PoC-Rng

DESTINATION 
${CMAKE_SOURCE_DIR}/__bin
)