cmake_minimum_required(VERSION 3.15)
project(CPU_TASK CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

add_subdirectory(Core)
add_subdirectory(Assembler)
add_subdirectory(Emulator)
add_subdirectory(Tests)

option(TRACING "Enable CPU instruction execution tracing" ON)
if(TRACING)
    add_compile_definitions(TRACING=1)
else()
    add_compile_definitions(TRACING=0)
endif()