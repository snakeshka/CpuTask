# Проверяем переданные переменные
if(NOT DEFINED ASSEMBLER OR NOT DEFINED EMULATOR OR NOT DEFINED ASM_FILE OR NOT DEFINED WORK_DIR)
    message(FATAL_ERROR "Missing required variables for RunAsmTest.cmake")
endif()

file(REMOVE_RECURSE "${WORK_DIR}")
file(MAKE_DIRECTORY "${WORK_DIR}")

set(OBJECT_FILE "${WORK_DIR}/program.o")

# Ассемблирование .s файла
execute_process(
    COMMAND "${ASSEMBLER}" "${ASM_FILE}" "${OBJECT_FILE}"
    RESULT_VARIABLE ASM_RES
    OUTPUT_VARIABLE ASM_OUT
    ERROR_VARIABLE ASM_ERR
)

if(NOT ASM_RES EQUAL 0)
    message(FATAL_ERROR "Assembler failed for ${ASM_FILE}:\n${ASM_ERR}\n${ASM_OUT}")
endif()

# Запуск в эмуляторе
execute_process(
    COMMAND "${EMULATOR}" "${OBJECT_FILE}"
    RESULT_VARIABLE EMU_RES
    OUTPUT_VARIABLE EMU_OUT
    ERROR_VARIABLE EMU_ERR
)

if(NOT EMU_RES EQUAL 0)
    message(FATAL_ERROR "Emulator failed for ${ASM_FILE} with exit code ${EMU_RES}:\n${EMU_ERR}\n${EMU_OUT}")
endif()