cmake_minimum_required(VERSION 3.10)
project(Quantum-Abysmal-PoC)

add_executable(Quantum-Abysmal-PoC
${CMAKE_CURRENT_SOURCE_DIR}/standard/PoC.cu
)

target_link_libraries(Quantum-Abysmal-PoC PRIVATE Quantum-Abysmal)
target_include_directories(Quantum-Abysmal-PoC PRIVATE ${CMAKE_SOURCE_DIR}/include/public)

install(TARGETS Quantum-Abysmal-PoC
        DESTINATION ${CMAKE_SOURCE_DIR}/bin
)