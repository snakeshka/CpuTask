#e2e тест
add_test(
    NAME E2EFullFlow
    COMMAND ${CMAKE_COMMAND}
        -DASSEMBLER=$<TARGET_FILE:Assembler>
        -DEMULATOR=$<TARGET_FILE:Emulator>
        -DWORK_DIR=${CMAKE_CURRENT_BINARY_DIR}/E2EFullFlow
        -P ${CMAKE_CURRENT_SOURCE_DIR}/FullFlowTest.cmake
)

add_test(
    NAME E2ETrace
    COMMAND ${CMAKE_COMMAND}
        -DASSEMBLER=$<TARGET_FILE:Assembler>
        -DEMULATOR=$<TARGET_FILE:Emulator>
        -DWORK_DIR=${CMAKE_CURRENT_BINARY_DIR}/E2ETrace
        -P ${CMAKE_CURRENT_SOURCE_DIR}/TraceTest.cmake
)