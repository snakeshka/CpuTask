if(NOT DEFINED ASSEMBLER)
    message(FATAL_ERROR "ASSEMBLER is not defined")
endif()

if(NOT DEFINED EMULATOR)
    message(FATAL_ERROR "EMULATOR is not defined")
endif()

if(NOT DEFINED WORK_DIR)
    message(FATAL_ERROR "WORK_DIR is not defined")
endif()

file(REMOVE_RECURSE "${WORK_DIR}")
file(MAKE_DIRECTORY "${WORK_DIR}")

set(SOURCE_FILE "${WORK_DIR}/program.s")
set(OBJECT_FILE "${WORK_DIR}/program.o")

file(WRITE "${SOURCE_FILE}"
"LI R0 0x1
LI R1 0x8000
ADD R1 R1 R0
")

execute_process(
    COMMAND "${ASSEMBLER}" "${SOURCE_FILE}" "${OBJECT_FILE}"
    RESULT_VARIABLE ASSEMBLER_RESULT
    OUTPUT_VARIABLE ASSEMBLER_STDOUT
    ERROR_VARIABLE ASSEMBLER_STDERR
)

if(NOT ASSEMBLER_RESULT EQUAL 0)
    message(FATAL_ERROR
        "Assembler failed\n"
        "Command: ${ASSEMBLER} ${SOURCE_FILE} ${OBJECT_FILE}\n"
        "stdout:\n${ASSEMBLER_STDOUT}\n"
        "stderr:\n${ASSEMBLER_STDERR}\n"
    )
endif()

if(NOT EXISTS "${OBJECT_FILE}")
    message(FATAL_ERROR "Assembler did not create output file: ${OBJECT_FILE}")
endif()

file(READ "${OBJECT_FILE}" OBJECT_HEX HEX)

set(EXPECTED_OBJECT_HEX "000001000001008002010100")

if(NOT OBJECT_HEX STREQUAL EXPECTED_OBJECT_HEX)
    message(FATAL_ERROR
        "Unexpected object file bytes\n"
        "Expected: ${EXPECTED_OBJECT_HEX}\n"
        "Actual:   ${OBJECT_HEX}\n"
    )
endif()

execute_process(
    COMMAND "${EMULATOR}" "${OBJECT_FILE}"
    RESULT_VARIABLE EMULATOR_RESULT
    OUTPUT_VARIABLE EMULATOR_STDOUT
    ERROR_VARIABLE EMULATOR_STDERR
)

if(NOT EMULATOR_RESULT EQUAL 0)
    message(FATAL_ERROR
        "Emulator failed\n"
        "Command: ${EMULATOR} ${OBJECT_FILE}\n"
        "stdout:\n${EMULATOR_STDOUT}\n"
        "stderr:\n${EMULATOR_STDERR}\n"
    )
endif()

string(REPLACE "\r\n" "\n" EMULATOR_STDOUT "${EMULATOR_STDOUT}")

set(EXPECTED_OUTPUT
"R0=0x00000001
R1=0x00008001
R2=0x00000000
R3=0x00000000
R4=0x00000000
R5=0x00000000
R6=0x00000000
R7=0x00000000
R8=0x00000000
R9=0x00000000
R10=0x00000000
R11=0x00000000
R12=0x00000000
R13=0x00000000
R14=0x00000000
R15=0x00000000
")

if(NOT EMULATOR_STDOUT STREQUAL EXPECTED_OUTPUT)
    message(FATAL_ERROR
        "Unexpected emulator output\n"
        "Expected:\n${EXPECTED_OUTPUT}\n"
        "Actual:\n${EMULATOR_STDOUT}\n"
    )
endif()