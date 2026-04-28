#создаем internal object для тестов
add_library(AssemblerInternal OBJECT
    src/CliOptions.cpp
    src/SourceFile.cpp
    src/Diagnostic.cpp
    src/DiagnosticFormatter.cpp
    src/Lexer.cpp
    src/Parser.cpp
    src/SemanticAnalyzer.cpp
    src/Assembler.cpp
)

target_include_directories(AssemblerInternal PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_link_libraries(AssemblerInternal PUBLIC Core)


file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE HEADER_FILES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")

add_executable(Assembler
    ${SRC_FILES}
    ${HEADER_FILES}
)

target_link_libraries(Assembler PRIVATE Core)

set_target_properties(Assembler PROPERTIES 
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Assembler/bin"
)

