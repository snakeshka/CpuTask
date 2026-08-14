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

file(GLOB ASM_FILES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/../AsmTests/*.s")

foreach(ASM_FILE ${ASM_FILES})
    get_filename_component(TEST_NAME "${ASM_FILE}" NAME_WE)

    add_test(
        NAME "AsmE2E_${TEST_NAME}"
        COMMAND ${CMAKE_COMMAND}
            -DASSEMBLER=$<TARGET_FILE:Assembler>
            -DEMULATOR=$<TARGET_FILE:Emulator>
            -DASM_FILE=${ASM_FILE}
            -DWORK_DIR=${CMAKE_CURRENT_BINARY_DIR}/AsmE2E_${TEST_NAME}
            -P ${CMAKE_CURRENT_SOURCE_DIR}/RunAsmTest.cmake
    )
endforeach()